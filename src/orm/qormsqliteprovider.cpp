/*
 * Copyright (C) 2020-2024 Dmitriy Purgin <dpurgin@gmail.com>
 * Copyright (C) 2019-2024 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2019-2024 sequality software engineering e.U. <office@sequality.at>
 *
 * This file is part of QtOrm library.
 *
 * QtOrm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QtOrm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with QtOrm.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "qormsqliteprovider.h"

#include "qormclassproperty.h"
#include "qormentityinstancecache.h"
#include "qormerror.h"
#include "qormfilter.h"
#include "qormfilterexpression.h"
#include "qormmetadatacache.h"
#include "qormorder.h"
#include "qormpropertymapping.h"
#include "qormquery.h"
#include "qormqueryresult.h"
#include "qormrelation.h"
#include "qormsqliteconfiguration.h"

#include "qormglobal_p.h"
#include "qormsqlitestatementgenerator_p.h"

#include <QtCore/qdebug.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qobject.h>
#include <QtCore/qscopeguard.h>
#include <QtCore/quuid.h>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlerror.h>
#include <QtSql/qsqlfield.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlrecord.h>

QT_BEGIN_NAMESPACE

class QOrmSqliteProviderPrivate
{
    Q_DECLARE_PUBLIC(QOrmSqliteProvider)

    explicit QOrmSqliteProviderPrivate(const QOrmSqliteConfiguration& configuration,
                                       QOrmSqliteProvider* parent)
        : q_ptr{parent}
        , m_sqlConfiguration{configuration}
    {
        detectSqliteCapabilities();
    }

    QOrmSqliteProvider* q_ptr{nullptr};
    QSqlDatabase m_database{QSqlDatabase::addDatabase("QSQLITE", "QtOrm")};
    QOrmSqliteConfiguration m_sqlConfiguration;
    QSet<QString> m_schemaSyncCache;
    int m_transactionCounter{0};
    QOrmSqliteStatementGenerator m_statementGenerator;
    QOrmSqliteProvider::SqliteCapabilities m_capabilities{QOrmSqliteProvider::NoCapabilities};

    Q_REQUIRED_RESULT
    QString toSqlType(QVariant::Type type);
    [[nodiscard]] bool canConvertFromSqliteToQProperty(QVariant::Type fromSqlType,
                                                       QVariant::Type toQPropertyType);

    Q_REQUIRED_RESULT
    QOrmError lastDatabaseError() const;

    Q_REQUIRED_RESULT
    QSqlQuery prepareAndExecute(const QString& statement, const QVariantMap& parameters);

    Q_REQUIRED_RESULT
    QOrmPrivate::Expected<QObject*, QOrmError> makeEntityInstance(
        const QOrmMetadata& entityMetadata,
        const QSqlRecord& record,
        QOrmEntityInstanceCache& entityInstanceCache);
    QOrmError fillEntityInstance(const QOrmMetadata& entityMetadata,
                                 QObject* entityInstance,
                                 const QSqlRecord& record,
                                 QOrmEntityInstanceCache& entityInstanceCache,
                                 const QFlags<QOrm::QueryFlags>& queryFlags);

    QOrmError ensureSchemaSynchronized(const QOrmRelation& entityMetadata);
    QOrmError recreateSchema(const QOrmRelation& entityMetadata);
    QOrmError updateSchema(const QOrmRelation& entityMetadata);
    QOrmError validateSchema(const QOrmRelation& entityMetadata);
    QOrmError appendSchema(const QOrmRelation& entityMetadata);

    QOrmQueryResult<QObject> read(const QOrmQuery& query,
                                  QOrmEntityInstanceCache& entityInstanceCache);
    QOrmQueryResult<QObject> merge(const QOrmQuery& query);
    QOrmQueryResult<QObject> remove(const QOrmQuery& query,
                                    QOrmEntityInstanceCache& entityInstanceCache);

    [[nodiscard]] bool foreignKeysEnabled();
    [[nodiscard]] QOrmError setForeignKeysEnabled(bool enabled);
    [[nodiscard]] QOrmError checkForeignKeys();
    void detectSqliteCapabilities();
};

// Returns whether the data type stored in the database column is compatible with its QProperty
// counterpart. Most of the time, QVariant::canConvert() is enough but there are special cases like
// Date and Time datatype in SQLite. This datatype is stored in SQLite as Text, Real or Integer, and
// QVariant::canConvert() reports a false negative.
//
// See https://www.sqlite.org/datatype3.html
bool QOrmSqliteProviderPrivate::canConvertFromSqliteToQProperty(QVariant::Type fromSqlType,
                                                                QVariant::Type toQPropertyType)
{
    if ((fromSqlType == QVariant::Double || //
         fromSqlType == QVariant::Int ||    //
         fromSqlType == QVariant::String) &&
        toQPropertyType == QVariant::DateTime)
    {
        return true;
    }
    // If target type is QVariant, assume it is convertible.
    else if (toQPropertyType == 41)
    {
        return true;
    }

    return QVariant{fromSqlType}.canConvert(toQPropertyType);
}

QOrmError QOrmSqliteProviderPrivate::lastDatabaseError() const
{
    return QOrmError{QOrm::ErrorType::Provider, m_database.lastError().text()};
}

QSqlQuery QOrmSqliteProviderPrivate::prepareAndExecute(const QString& statement,
                                                       const QVariantMap& parameters = {})
{
    QSqlQuery query{m_database};

    if (m_sqlConfiguration.verbose())
        qCDebug(qtorm).noquote() << "Executing:" << statement;

    if (!query.prepare(statement))
        return query;

    if (!parameters.isEmpty())
    {
        if (m_sqlConfiguration.verbose())
            qCDebug(qtorm) << "Bound parameters:" << parameters;

        for (auto it = parameters.begin(); it != parameters.end(); ++it)
            query.bindValue(it.key(), it.value());
    }

    query.exec();

    return query;
}

QOrmPrivate::Expected<QObject*, QOrmError> QOrmSqliteProviderPrivate::makeEntityInstance(
    const QOrmMetadata& entityMetadata,
    const QSqlRecord& record,
    QOrmEntityInstanceCache& entityInstanceCache)
{
    QObject* entityInstance = entityMetadata.qMetaObject().newInstance();
    Q_ASSERT(entityInstance != nullptr);

    // assign object ID and put into cache to be able to resolve cyclic references
    Q_ASSERT(entityMetadata.objectIdMapping() != nullptr);
    if (!QOrmPrivate::setPropertyValue(entityInstance,
                                       entityMetadata.objectIdMapping()->classPropertyName(),
                                       record.value(
                                           entityMetadata.objectIdMapping()->tableFieldName())))
    {
        Q_ORM_UNEXPECTED_STATE;
    }

    entityInstanceCache.insert(entityMetadata, entityInstance);

    // fill the rest of the properties
    QOrmError fillError = fillEntityInstance(
        entityMetadata, entityInstance, record, entityInstanceCache, QOrm::QueryFlags::None);

    if (fillError != QOrm::ErrorType::None)
        return QOrmPrivate::makeUnexpected(fillError);

    entityInstanceCache.finalize(entityMetadata, entityInstance);

    return entityInstance;
}

QOrmError QOrmSqliteProviderPrivate::fillEntityInstance(
    const QOrmMetadata& entityMetadata,
    QObject* entityInstance,
    const QSqlRecord& record,
    QOrmEntityInstanceCache& entityInstanceCache,
    const QFlags<QOrm::QueryFlags>& queryFlags)
{
    for (const QOrmPropertyMapping& mapping : entityMetadata.propertyMappings())
    {
        // if this property is a reference, retrieve referenced entity instances and assign
        if (mapping.isReference())
        {
            QOrmRelation referencedRelation{*mapping.referencedEntity()};

            QOrmError syncError = ensureSchemaSynchronized(referencedRelation);
            if (syncError != QOrm::ErrorType::None)
                return syncError;

            // transient references are one-to-many references
            if (mapping.isTransient())
            {
                const QOrmPropertyMapping* backReference = QOrmPrivate::backReference(mapping);
                Q_ASSERT(backReference != nullptr);
                Q_ASSERT(entityMetadata.objectIdMapping() != nullptr);

                // read all entity instances referring to the current record
                //                QVariant objectId =
                //                    record.value(entityMetadata.objectIdMapping()->tableFieldName());

                QOrmFilter filter{*backReference == entityInstance};

                QOrmQuery query{QOrm::Operation::Read,
                                referencedRelation,
                                *mapping.referencedEntity(),
                                filter,
                                {},
                                {},
                                queryFlags};

                QOrmQueryResult<QObject> result = read(query, entityInstanceCache);

                // error during read: return this error and do not continue
                if (result.error().type() != QOrm::ErrorType::None)
                {
                    return result.error();
                }

                // dispatch according to declared property type
                QVariant propertyValue;

                if (mapping.dataTypeName().startsWith("QVector<", Qt::CaseInsensitive))
                {
                    propertyValue = QVariant::fromValue(result.toVector());
                }
                else if (mapping.dataTypeName().startsWith("QSet<", Qt::CaseInsensitive))
                {
                    propertyValue = QVariant::fromValue(result.toSet());
                }
                else
                    Q_ORM_UNEXPECTED_STATE;

                Q_ASSERT(propertyValue.isValid() && !propertyValue.isNull());
                if (!QOrmPrivate::setPropertyValue(entityInstance,
                                                   mapping.classPropertyName(),
                                                   propertyValue))
                {
                    Q_ORM_UNEXPECTED_STATE;
                }
            }
            // non-transient references are many-to-one references
            else
            {
                Q_ASSERT(mapping.referencedEntity() != nullptr);

                // try to retrieve the referenced instance from the cache.
                QVariant referencedObjectId = record.value(mapping.tableFieldName());

                if (referencedObjectId.isNull())
                    continue;

                QObject* referencedEntityInstance =
                    entityInstanceCache.get(*mapping.referencedEntity(), referencedObjectId);

                // referenced instance is in cache: check that it wasn't modified and assign to the
                // corresponding property
                if (referencedEntityInstance != nullptr)
                {
                    if (entityInstanceCache.isModified(referencedEntityInstance) &&
                        !queryFlags.testFlag(QOrm::QueryFlags::OverwriteCachedInstances))
                    {
                        Q_ORM_UNEXPECTED_STATE;
                    }

                    if (!QOrmPrivate::setPropertyValue(entityInstance,
                                                       mapping.classPropertyName(),
                                                       QVariant::fromValue(
                                                           referencedEntityInstance)))
                    {
                        Q_ORM_UNEXPECTED_STATE;
                    }
                }
                // referenced instance is not in cache: retrieve it from the database by ID
                else
                {
                    QOrmFilter filter{*mapping.referencedEntity()->objectIdMapping() ==
                                      referencedObjectId};

                    QOrmQuery query{QOrm::Operation::Read,
                                    referencedRelation,
                                    *mapping.referencedEntity(),
                                    filter,
                                    {},
                                    {},
                                    queryFlags};

                    QOrmQueryResult<QObject> result = read(query, entityInstanceCache);

                    // error during read: return this error and do not continue
                    if (result.error().type() != QOrm::ErrorType::None)
                    {
                        return result.error();
                    }

                    // sanity check: when selecting by object id, only one instance should be
                    // returned
                    if (result.toVector().size() != 1)
                    {
                        qCCritical(qtorm) << "Database inconsistency detected: a row in table"
                                          << mapping.enclosingEntity().tableName() << "references"
                                          << mapping.referencedEntity()->tableName() << "in column"
                                          << mapping.tableFieldName()
                                          << "using a non-existing object ID" << referencedObjectId
                                          << ", or there are multiple rows in the referenced table "
                                             "with this value";

                        Q_ORM_UNEXPECTED_STATE;
                    }

                    if (!QOrmPrivate::setPropertyValue(entityInstance,
                                                       mapping.classPropertyName(),
                                                       QVariant::fromValue(
                                                           result.toVector().front())))
                    {
                        Q_ORM_UNEXPECTED_STATE;
                    }
                }
            }
        }
        // just a value: set the property value
        else if (!mapping.isTransient())
        {
            bool isNull = record.isNull(mapping.tableFieldName());
            QVariant propertyValue = isNull ? QVariant{} : record.value(mapping.tableFieldName());

            if (!QOrmPrivate::setPropertyValue(entityInstance,
                                               mapping.classPropertyName(),
                                               propertyValue))
            {
                qFatal("Unable to setPropertyValue() for %s <-> %s",
                       qPrintable(mapping.classPropertyName()),
                       qPrintable(mapping.tableFieldName()));
            }
        }
    }

    return QOrmError{QOrm::ErrorType::None, {}};
}

QOrmError QOrmSqliteProviderPrivate::ensureSchemaSynchronized(const QOrmRelation& relation)
{
    switch (relation.type())
    {
        case QOrm::RelationType::Mapping:
        {
            Q_ASSERT(relation.mapping() != nullptr);

            if (m_schemaSyncCache.contains(relation.mapping()->className()))
                return {QOrm::ErrorType::None, ""};

            QOrmError error{QOrm::ErrorType::None, {}};

            QOrmSqliteConfiguration::SchemaMode effectiveSchemaMode =
                m_sqlConfiguration.schemaMode();

            if (relation.mapping()->userMetadata().contains(QOrm::Keyword::Schema))
            {
                static QMap<QString, QOrmSqliteConfiguration::SchemaMode> schemaModes{
                    {"recreate", QOrmSqliteConfiguration::SchemaMode::Recreate},
                    {"update", QOrmSqliteConfiguration::SchemaMode::Update},
                    {"validate", QOrmSqliteConfiguration::SchemaMode::Validate},
                    {"bypass", QOrmSqliteConfiguration::SchemaMode::Bypass},
                    {"append", QOrmSqliteConfiguration::SchemaMode::Append}};

                QString schemaModeValue =
                    relation.mapping()->userMetadata().value(QOrm::Keyword::Schema).toString();

                if (!schemaModes.contains(schemaModeValue))
                {
                    qFatal("QtOrm: Unsupported schema mode in %s: Q_ORM_CLASS(SCHEMA %s)",
                           qPrintable(relation.mapping()->className()),
                           qPrintable(schemaModeValue));
                }
                else
                {
                    effectiveSchemaMode = schemaModes.value(schemaModeValue);
                }
            }

            switch (effectiveSchemaMode)
            {
                case QOrmSqliteConfiguration::SchemaMode::Recreate:
                    error = recreateSchema(relation);
                    break;

                case QOrmSqliteConfiguration::SchemaMode::Update:
                    error = updateSchema(relation);
                    break;

                case QOrmSqliteConfiguration::SchemaMode::Validate:
                    error = validateSchema(relation);
                    break;

                case QOrmSqliteConfiguration::SchemaMode::Bypass:
                    // no error
                    break;

                case QOrmSqliteConfiguration::SchemaMode::Append:
                    error = appendSchema(relation);
                    break;
            }

            if (error.type() == QOrm::ErrorType::None)
            {
                m_schemaSyncCache.insert(relation.mapping()->className());

                for (const QOrmPropertyMapping& propertyMapping :
                     relation.mapping()->propertyMappings())
                {
                    if (propertyMapping.isReference())
                    {
                        QOrmError referencedError = ensureSchemaSynchronized(
                            QOrmRelation{*propertyMapping.referencedEntity()});

                        if (referencedError.type() != QOrm::ErrorType::None)
                            return referencedError;
                    }
                }
            }

            return error;
        }

        case QOrm::RelationType::Query:
            Q_ASSERT(relation.query() != nullptr);
            return ensureSchemaSynchronized(relation.query()->relation());
    }

    Q_ORM_UNEXPECTED_STATE;
}

QOrmError QOrmSqliteProviderPrivate::recreateSchema(const QOrmRelation& relation)
{
    Q_ASSERT(m_database.isOpen());
    Q_ASSERT(relation.type() == QOrm::RelationType::Mapping);
    Q_ASSERT(relation.mapping() != nullptr);

    if (m_database.tables().contains(relation.mapping()->tableName()))
    {
        QString statement = m_statementGenerator.generateDropTableStatement(*relation.mapping());

        QSqlQuery query = prepareAndExecute(statement);

        if (query.lastError().type() != QSqlError::NoError)
            return QOrmError{QOrm::ErrorType::UnsynchronizedSchema, query.lastError().text()};
    }

    QString statement = m_statementGenerator.generateCreateTableStatement(*relation.mapping());

    QSqlQuery query = prepareAndExecute(statement);

    if (query.lastError().type() != QSqlError::NoError)
        return QOrmError{QOrm::ErrorType::UnsynchronizedSchema, query.lastError().text()};

    return QOrmError{QOrm::ErrorType::None, ""};
}

QOrmError QOrmSqliteProviderPrivate::updateSchema(const QOrmRelation& relation)
{
    Q_ASSERT(m_database.isOpen());
    Q_ASSERT(relation.type() == QOrm::RelationType::Mapping);
    Q_ASSERT(relation.mapping() != nullptr);

    Q_Q(QOrmSqliteProvider);

    // Create table if it does not exist.
    if (!m_database.tables().contains(relation.mapping()->tableName()))
    {
        q->beginTransaction();

        QString statement = m_statementGenerator.generateCreateTableStatement(*relation.mapping());
        QSqlQuery query = prepareAndExecute(statement);

        if (query.lastError().type() != QSqlError::NoError)
        {
            q->rollbackTransaction();
            return QOrmError{QOrm::ErrorType::UnsynchronizedSchema, query.lastError().text()};
        }

        q->commitTransaction();
    }
    // If the table exists, check if an update is needed. An update is needed if not all columns
    // appear in both the database and the entity metadata, or if their data types are not
    // compatible.
    else
    {
        bool updateNeeded = false;
        QSqlRecord record = m_database.record(relation.mapping()->tableName());

        // Check if all table columns are mapped by non-transient class properties, and there data
        // types are compatible.
        for (int i = 0; i < record.count() && !updateNeeded; ++i)
        {
            QSqlField field = record.field(i);

            const QOrmPropertyMapping* mapping =
                relation.mapping()->tableFieldMapping(field.name());

            if (mapping == nullptr)
            {
                qCDebug(qtorm).noquote().nospace()
                    << "updating table " << relation.mapping()->tableName() << ": field "
                    << field.name() << " has no mapping in entity "
                    << relation.mapping()->className();
                updateNeeded = true;
            }
            else if (mapping->isTransient())
            {
                qCDebug(qtorm).noquote().nospace()
                    << "updating table " << relation.mapping()->tableName() << ": field "
                    << field.name() << " is mapped to a transient property "
                    << relation.mapping()->className() << "::" << mapping->classPropertyName();
                updateNeeded = true;
            }
            else if (mapping->referencedEntity() != nullptr)
            {
                Q_ASSERT(mapping->referencedEntity()->objectIdMapping() != nullptr);
                auto referencedType = mapping->referencedEntity()->objectIdMapping()->dataType();

                if (!canConvertFromSqliteToQProperty(field.type(), referencedType))
                {
                    qCDebug(qtorm).noquote().nospace()
                        << "updating table " << relation.mapping()->tableName()
                        << ": data type of field " << field.name() << " is incompatible with its "
                        << relation.mapping()->className() << "::" << mapping->classPropertyName()
                        << " mapping.";
                    updateNeeded = true;
                }
            }
            else if (!canConvertFromSqliteToQProperty(field.type(), mapping->dataType()))
            {
                qCDebug(qtorm).noquote().nospace()
                    << "updating table " << relation.mapping()->tableName()
                    << ": data types of field " << field.name() << " and its mapping "
                    << relation.mapping()->className() << "::" << mapping->classPropertyName()
                    << " are incompatible.";
                updateNeeded = true;
            }
        }

        // Check if there are non-transient class properties that are not mapped in the database.
        for (size_t i = 0; i < relation.mapping()->propertyMappings().size() && !updateNeeded; ++i)
        {
            const QOrmPropertyMapping& mapping = relation.mapping()->propertyMappings()[i];

            if (!mapping.isTransient() && !record.contains(mapping.tableFieldName()))
            {
                qCDebug(qtorm).noquote().nospace()
                    << "updating table " << relation.mapping()->tableName()
                    << ": a non-transient class property " << relation.mapping()->className()
                    << "::" << mapping.classPropertyName() << " has no corresponding table field.";
                updateNeeded = true;
            }
        }

        if (updateNeeded)
        {
            qCInfo(qtorm).noquote() << "Updating schema for" << relation.mapping()->className()
                                    << "<->" << relation.mapping()->tableName();

            // Alter existing tables using the generalized 12-step process described in
            // https://sqlite.org/lang_altertable.html
            //
            // 1. If foreign key constraints are enabled, disable them using PRAGMA
            // foreign_keys=OFF.
            bool withForeignKeys = foreignKeysEnabled();

            QOrmError error = setForeignKeysEnabled(false);

            if (error.type() != QOrm::ErrorType::None)
            {
                return {QOrm::ErrorType::UnsynchronizedSchema, error.text()};
            }

            // 2. Start a transaction.
            q->beginTransaction();

            // 3. Remember the format of all indexes, triggers, and views associated with table X.
            //
            // QtOrm does not support indexes, triggers, and views yet.

            // 4. Use CREATE TABLE to construct a new table "new_X" that is in the desired revised
            // format of table X
            QString newTableName = QString{"%1_%2"}.arg(relation.mapping()->tableName(),
                                                        QUuid::createUuid().toString(QUuid::Id128));
            QString statement =
                m_statementGenerator.generateCreateTableStatement(*relation.mapping(),
                                                                  newTableName);
            QSqlQuery query = prepareAndExecute(statement);

            if (query.lastError().type() != QSqlError::NoError)
            {
                q->rollbackTransaction();
                return {QOrm::ErrorType::UnsynchronizedSchema, query.lastError().text()};
            }

            // 5. Transfer content from X into new_X
            QStringList tableColumns;
            for (const QOrmPropertyMapping& mapping : relation.mapping()->propertyMappings())
            {
                if (!mapping.isTransient() && record.contains(mapping.tableFieldName()))
                {
                    tableColumns.push_back(mapping.tableFieldName());
                }
            }

            statement = m_statementGenerator.generateInsertIntoStatement(
                newTableName, tableColumns, relation.mapping()->tableName(), tableColumns);
            query = prepareAndExecute(statement);

            if (query.lastError().type() != QSqlError::NoError)
            {
                q->rollbackTransaction();
                return {QOrm::ErrorType::UnsynchronizedSchema, query.lastError().text()};
            }

            // 6. Drop the old table X
            statement = m_statementGenerator.generateDropTableStatement(*relation.mapping());
            query = prepareAndExecute(statement);

            if (query.lastError().type() != QSqlError::NoError)
            {
                q->rollbackTransaction();
                return {QOrm::ErrorType::UnsynchronizedSchema, query.lastError().text()};
            }

            // 7. Change the name of new_X to X
            statement =
                m_statementGenerator.generateRenameTableStatement(newTableName,
                                                                  relation.mapping()->tableName());
            query = prepareAndExecute(statement);

            if (query.lastError().type() != QSqlError::NoError)
            {
                q->rollbackTransaction();
                return {QOrm::ErrorType::UnsynchronizedSchema, query.lastError().text()};
            }

            // 8. Use CREATE INDEX, CREATE TRIGGER, and CREATE VIEW to reconstruct indexes,
            // triggers, and views associated with table X.
            //
            // QtOrm does not support indexes, triggers, and views yet.

            // 9. If any views refer to table X in a way that is affected by the schema change, then
            // drop those views using DROP VIEW and recreate them with whatever changes are
            // necessary to accommodate the schema change using CREATE VIEW.
            //
            // QtOrm does not suport views yet.

            // 10. If foreign key constraints were originally enabled then run PRAGMA
            // foreign_key_check to verify that the schema change did not break any foreign key
            // constraints.
            if (withForeignKeys)
            {
                error = checkForeignKeys();

                if (error.type() != QOrm::ErrorType::None)
                {
                    q->rollbackTransaction();
                    return {QOrm::ErrorType::UnsynchronizedSchema, error.text()};
                }
            }

            // 11. Commit the transaction started in step 2.
            m_database.commit();

            // 12. If foreign keys constraints were originally enabled, reenable them now.
            if (withForeignKeys)
            {
                error = setForeignKeysEnabled(true);

                if (error.type() != QOrm::ErrorType::None)
                {
                    q->rollbackTransaction();
                    return {QOrm::ErrorType::UnsynchronizedSchema, error.text()};
                }
            }
        }
    }

    return QOrmError{QOrm::ErrorType::None, {}};
}

QOrmError QOrmSqliteProviderPrivate::validateSchema(const QOrmRelation& relation)
{
    Q_UNUSED(relation)
    Q_ORM_NOT_IMPLEMENTED;
    return {QOrm::ErrorType::Other, "Not implemented"};
}

QOrmError QOrmSqliteProviderPrivate::appendSchema(const QOrmRelation& relation)
{
    Q_ASSERT(m_database.isOpen());
    Q_ASSERT(relation.type() == QOrm::RelationType::Mapping);
    Q_ASSERT(relation.mapping() != nullptr);

    Q_Q(QOrmSqliteProvider);

    q->beginTransaction();

    // Create table if it does not exist.
    if (!m_database.tables().contains(relation.mapping()->tableName()))
    {
        QString statement = m_statementGenerator.generateCreateTableStatement(*relation.mapping());
        QSqlQuery query = prepareAndExecute(statement);

        if (query.lastError().type() != QSqlError::NoError)
        {
            q->rollbackTransaction();
            return QOrmError{QOrm::ErrorType::UnsynchronizedSchema, query.lastError().text()};
        }
    }
    // If the table exists, add missing columns, if any.
    else
    {
        QSqlRecord record = m_database.record(relation.mapping()->tableName());

        for (const QOrmPropertyMapping& mapping : relation.mapping()->propertyMappings())
        {
            if (!record.contains(mapping.tableFieldName()) && !mapping.isTransient())
            {
                QString statement =
                    m_statementGenerator.generateAlterTableAddColumnStatement(*relation.mapping(),
                                                                              mapping);

                QSqlQuery query = prepareAndExecute(statement);

                if (query.lastError().type() != QSqlError::NoError)
                {
                    q->rollbackTransaction();
                    return QOrmError{QOrm::ErrorType::UnsynchronizedSchema,
                                     query.lastError().text()};
                }
            }
        }
    }

    q->commitTransaction();

    return QOrmError{QOrm::ErrorType::None, {}};
}

QOrmQueryResult<QObject> QOrmSqliteProviderPrivate::read(
    const QOrmQuery& query,
    QOrmEntityInstanceCache& entityInstanceCache)
{
    Q_ASSERT(query.projection().has_value());

    auto [statement, boundParameters] = m_statementGenerator.generate(query);

    QSqlQuery sqlQuery = prepareAndExecute(statement, boundParameters);

    if (sqlQuery.lastError().type() != QSqlError::NoError)
    {
        return QOrmQueryResult<QObject>{QOrmError{QOrm::ErrorType::Provider,
                                                  sqlQuery.lastError().text()},
                                        sqlQuery.numRowsAffected()};
    }

    QVector<QObject*> resultSet;

    const QOrmPropertyMapping* objectIdMapping = query.projection()->objectIdMapping();

    // If there is an object ID, compare the cached entities with the ones read from the
    // backend. If there is an inconsistency, it will be reported.
    // All read entities are replaced with their cached versions if found.
    if (objectIdMapping != nullptr)
    {
        while (sqlQuery.next())
        {
            QVariant objectId = sqlQuery.value(objectIdMapping->tableFieldName());

            QObject* cachedInstance = entityInstanceCache.get(*query.projection(), objectId);

            // cached instance: check if consistent
            if (cachedInstance != nullptr)
            {
                // If inconsistent, return an error. Already cached instances remain in the cache
                if (entityInstanceCache.isModified(cachedInstance) &&
                    !query.flags().testFlag(QOrm::QueryFlags::OverwriteCachedInstances))
                {
                    QString errorString;
                    QDebug dbg{&errorString};
                    dbg << "Entity instance" << cachedInstance
                        << "was read from the database but has unsaved changes in the "
                           "OR-mapper. "
                           "Merge this instance or discard changes before reading.";

                    return QOrmQueryResult<QObject>{
                        QOrmError{QOrm::ErrorType::UnsynchronizedEntity, errorString}};
                }
                else if (query.flags().testFlag(QOrm::QueryFlags::OverwriteCachedInstances))
                {
                    QOrmError error = fillEntityInstance(*query.projection(),
                                                         cachedInstance,
                                                         sqlQuery.record(),
                                                         entityInstanceCache,
                                                         query.flags());

                    if (error != QOrm::ErrorType::None)
                    {
                        return QOrmQueryResult<QObject>{error};
                    }

                    entityInstanceCache.markUnmodified(cachedInstance);
                }

                resultSet.push_back(cachedInstance);
            }
            // new instance: it will be cached in makeEntityInstance
            else
            {
                QOrmPrivate::Expected<QObject*, QOrmError> entityInstance =
                    makeEntityInstance(*query.projection(), sqlQuery.record(), entityInstanceCache);

                if (entityInstance)
                {
                    resultSet.push_back(entityInstance.value());
                }
                else
                {
                    return QOrmQueryResult<QObject>{entityInstance.error()};
                }
            }
        }
    }
    // No object ID in this projection: cannot cache, just return the results
    else
    {
        while (sqlQuery.next())
        {
            QOrmPrivate::Expected<QObject*, QOrmError> entityInstance =
                makeEntityInstance(*query.projection(), sqlQuery.record(), entityInstanceCache);

            if (entityInstance)
            {
                resultSet.push_back(entityInstance.value());
            }
            else
            {
                // if error occurred, delete everything that was read from the database since no
                // caching was involved
                qDeleteAll(resultSet);
                return QOrmQueryResult<QObject>{entityInstance.error()};
            }
        }
    }

    if (query.invokableFilter().has_value())
    {
        auto it = std::remove_if(std::begin(resultSet),
                                 std::end(resultSet),
                                 [&query](const QObject* value)
                                 { return !(*query.invokableFilter()->invokable())(value); });
        resultSet.erase(it, std::end(resultSet));
    }

    return QOrmQueryResult<QObject>{resultSet, resultSet.size()};
}

QOrmQueryResult<QObject> QOrmSqliteProviderPrivate::merge(const QOrmQuery& query)
{
    Q_ASSERT(query.relation().type() == QOrm::RelationType::Mapping);
    Q_ASSERT(query.entityInstance() != nullptr);

    if (query.invokableFilter().has_value())
    {
        qFatal("qtorm: Invokable filter is unsupported for merge operation.");
    }

    auto [statement, boundParameters] = m_statementGenerator.generate(query);

    QSqlQuery sqlQuery = prepareAndExecute(statement, boundParameters);

    if (sqlQuery.lastError().type() != QSqlError::NoError)
    {
        return QOrmQueryResult<QObject>{{QOrm::ErrorType::Provider, sqlQuery.lastError().text()},
                                        sqlQuery.numRowsAffected()};
    }

    if (sqlQuery.numRowsAffected() != 1)
    {
        return QOrmQueryResult<QObject>{{QOrm::ErrorType::UnsynchronizedEntity,
                                         "Unexpected number of rows affected"},
                                        sqlQuery.numRowsAffected()};
    }

    return QOrmQueryResult<QObject>{sqlQuery.lastInsertId(), sqlQuery.numRowsAffected()};
}

QOrmQueryResult<QObject> QOrmSqliteProviderPrivate::remove(
    const QOrmQuery& query,
    QOrmEntityInstanceCache& entityInstanceCache)
{
    if (query.invokableFilter().has_value())
    {
        qFatal("qtorm: Invokable filter is unsupported for remove operation.");
    }

    auto [statement, boundParameters] = m_statementGenerator.generate(query);

    QSqlQuery sqlQuery = prepareAndExecute(statement, boundParameters);

    if (sqlQuery.lastError().type() != QSqlError::NoError)
    {
        return QOrmQueryResult<QObject>{{QOrm::ErrorType::Provider, sqlQuery.lastError().text()},
                                        sqlQuery.numRowsAffected()};
    }

    QVector<QObject*> resultSet;

    // If there is a RETURNING clause, it returns all IDs affected by the DELETE operation.
    // Remove them from the instance cache and return the ownership to the caller.
    if (m_statementGenerator.options().testFlag(QOrmSqliteStatementGenerator::WithReturningClause))
    {
        Q_ASSERT(query.relation().type() == QOrm::RelationType::Mapping);

        const QOrmMetadata& meta = *query.relation().mapping();
        QString idPropertyName = meta.objectIdMapping()->classPropertyName();

        while (sqlQuery.next())
        {
            QObject* instance = entityInstanceCache.get(meta, sqlQuery.value(idPropertyName));

            if (instance != nullptr)
            {
                resultSet.push_back(entityInstanceCache.take(instance));
            }
        }
    }

    return QOrmQueryResult<QObject>{resultSet, sqlQuery.numRowsAffected()};
}

bool QOrmSqliteProviderPrivate::foreignKeysEnabled()
{
    QSqlQuery query = m_database.exec("PRAGMA foreign_keys");

    if (query.next())
    {
        return query.value("foreign_keys").toBool();
    }

    return false;
}

QOrmError QOrmSqliteProviderPrivate::setForeignKeysEnabled(bool enabled)
{
    QSqlQuery query = enabled ? m_database.exec("PRAGMA foreign_keys=ON")
                              : m_database.exec("PRAGMA foreign_keys=OFF");

    if (query.lastError().type() != QSqlError::NoError)
    {
        return {QOrm::ErrorType::Provider, query.lastError().text()};
    }

    return {QOrm::ErrorType::None, {}};
}

QOrmError QOrmSqliteProviderPrivate::checkForeignKeys()
{
    QSqlQuery query = prepareAndExecute("PRAGMA foreign_keys_check");

    if (query.lastError().type() != QSqlError::NoError)
    {
        return {QOrm::ErrorType::Provider, query.lastError().text()};
    }

    return {QOrm::ErrorType::None, {}};
}

// The capabilities of the provider depend on the SQLite verison. Connect to an in-memory
// database to read the SQLite version.
void QOrmSqliteProviderPrivate::detectSqliteCapabilities()
{
    auto connectionName = QUuid::createUuid().toString();

    // The database connection should be removed AFTER QSqlDatabase destructor has been invoked to
    // avoid "connection is still in use" warning.
    //
    // See https://doc.qt.io/qt-5/qsqldatabase.html#removeDatabase
    auto scopeGuard =
        qScopeGuard([connectionName]() { QSqlDatabase::removeDatabase(connectionName); });

    auto inMemoryDatabase = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    inMemoryDatabase.setDatabaseName(":memory:");

    if (!inMemoryDatabase.open())
    {
        qFatal("qtorm: Cannot detect SQLite capabilities: %s",
               qUtf8Printable(inMemoryDatabase.lastError().text()));
    }

    QSqlQuery query = inMemoryDatabase.exec("SELECT sqlite_version() AS version");

    if (query.lastError().text() != QSqlError::NoError)
    {
        if (query.next())
        {
            QString versionStr = query.value("version").toString();
            qCDebug(qtorm) << "SQLite version:" << versionStr;

            QStringList parts = versionStr.split('.');

            auto version = std::make_tuple(parts.size() > 0 ? parts[0].toInt() : 0,
                                           parts.size() > 1 ? parts[1].toInt() : 0,
                                           parts.size() > 2 ? parts[2].toInt() : 0);

            if (version >= std::make_tuple(3, 35, 0))
            {
                m_capabilities.setFlag(QOrmSqliteProvider::SupportsReturningClause);

                m_statementGenerator.setOptions(m_statementGenerator.options() |
                                                QOrmSqliteStatementGenerator::WithReturningClause);
            }
        }
        else
        {
            qFatal("qtorm: Cannot read SQLite version: %s",
                   qUtf8Printable(query.lastError().text()));
        }
    }

    inMemoryDatabase.close();
}

QOrmSqliteProvider::QOrmSqliteProvider(const QOrmSqliteConfiguration& sqlConfiguration)
    : QOrmAbstractProvider{}
    , d_ptr{new QOrmSqliteProviderPrivate{sqlConfiguration, this}}
{    
}

QOrmSqliteProvider::~QOrmSqliteProvider()
{
    delete d_ptr;

    QSqlDatabase::removeDatabase("QtOrm");
}

QOrmError QOrmSqliteProvider::connectToBackend()
{
    Q_D(QOrmSqliteProvider);

    if (!d->m_database.isOpen())
    {        
        d->m_database.setConnectOptions(d->m_sqlConfiguration.connectOptions());
        d->m_database.setDatabaseName(d->m_sqlConfiguration.databaseName());

        if (!d->m_database.open())
        {
            return d->lastDatabaseError();
        }
    }

    return QOrmError{QOrm::ErrorType::None, {}};
}

QOrmError QOrmSqliteProvider::disconnectFromBackend()
{
    Q_D(QOrmSqliteProvider);

    d->m_database.close();

    return QOrmError{QOrm::ErrorType::None, {}};
}

bool QOrmSqliteProvider::isConnectedToBackend()
{
    Q_D(QOrmSqliteProvider);

    return d->m_database.isOpen();
}

QOrmError QOrmSqliteProvider::beginTransaction()
{
    Q_D(QOrmSqliteProvider);

    if (++d->m_transactionCounter == 1)
    {
        if (!d->m_database.transaction())
        {
            QSqlError error = d->m_database.lastError();

            if (error.type() != QSqlError::NoError)
                return d->lastDatabaseError();
            else
                return QOrmError{QOrm::ErrorType::Other,
                                 QStringLiteral("Unable to start transaction")};
        }
    }

    return QOrmError{QOrm::ErrorType::None, {}};
}

QOrmError QOrmSqliteProvider::commitTransaction()
{
    Q_D(QOrmSqliteProvider);

    if (--d->m_transactionCounter == 0)
    {
        if (!d->m_database.commit())
        {
            QSqlError error = d->m_database.lastError();

            if (error.type() != QSqlError::NoError)
                return d->lastDatabaseError();
            else
                return QOrmError{QOrm::ErrorType::Other,
                                 QStringLiteral("Unable to commit transaction")};
        }
    }

    return QOrmError{QOrm::ErrorType::None, {}};
}

QOrmError QOrmSqliteProvider::rollbackTransaction()
{
    Q_D(QOrmSqliteProvider);

    if (--d->m_transactionCounter == 0)
    {
        if (!d->m_database.rollback())
        {
            QSqlError error = d->m_database.lastError();

            if (error.type() != QSqlError::NoError)
                return d->lastDatabaseError();
            else
                return QOrmError{QOrm::ErrorType::Other,
                                 QStringLiteral("Unable to rollback transaction")};
        }
    }

    return QOrmError{QOrm::ErrorType::None, {}};
}

QOrmQueryResult<QObject> QOrmSqliteProvider::execute(const QOrmQuery& query,
                                                     QOrmEntityInstanceCache& entityInstanceCache)
{
    Q_D(QOrmSqliteProvider);

    QOrmError error = d->ensureSchemaSynchronized(query.relation());

    if (error.type() != QOrm::ErrorType::None)
    {
        return QOrmQueryResult<QObject>{error};
    }

    switch (query.operation())
    {
        case QOrm::Operation::Read:
            return d->read(query, entityInstanceCache);

        case QOrm::Operation::Create:
        case QOrm::Operation::Update:
            return d->merge(query);

        case QOrm::Operation::Delete:
            return d->remove(query, entityInstanceCache);

        case QOrm::Operation::Merge:
            Q_ORM_UNEXPECTED_STATE;
    }

    Q_ORM_UNEXPECTED_STATE;
}

int QOrmSqliteProvider::capabilities() const
{
    Q_D(const QOrmSqliteProvider);
    return d->m_capabilities;
}

QOrmSqliteConfiguration QOrmSqliteProvider::configuration() const
{
    Q_D(const QOrmSqliteProvider);

    return d->m_sqlConfiguration;
}

QSqlDatabase QOrmSqliteProvider::database() const
{
    Q_D(const QOrmSqliteProvider);

    return d->m_database;
}

QT_END_NAMESPACE
