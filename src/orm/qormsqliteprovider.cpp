/*
 * Copyright (C) 2020-2024 Dmitriy Purgin <dpurgin@gmail.com>
 * Copyright (C) 2019-2025 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2019-2025 sequality software engineering e.U. <office@sequality.at>
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

#include <optional>

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
    QString toSqlType(QMetaType::Type type);
    [[nodiscard]] bool canConvertFromSqliteToQProperty(QMetaType::Type fromSqlType,
                                                       QMetaType::Type toQPropertyType);
    [[nodiscard]] bool fieldHasForeignKey(const QSqlField& field);
    [[nodiscard]] QString fieldGetUniqueConstraint(const QSqlField& field);

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
    QOrmError createTable(const QOrmMetadata& entity);
    QOrmError migrateTable(const QOrmMetadata& entity);
    [[nodiscard]] std::optional<QString> schemaMismatch(const QOrmMetadata& entity);
    [[nodiscard]] std::optional<QString> fieldMismatch(const QSqlField& field,
                                                       const QOrmMetadata& entity);
    [[nodiscard]] std::optional<QString> uniqueConstraintMismatch(
        const QSqlField& field,
        const QOrmPropertyMapping& mapping,
        const QOrmMetadata& entity);
    [[nodiscard]] QOrmError executeSchemaStatement(const QString& statement);
    QOrmError validateSchema(const QOrmRelation& entityMetadata);
    QOrmError appendSchema(const QOrmRelation& entityMetadata);

    QOrmQueryResult<QObject> read(const QOrmQuery& query,
                                  QOrmEntityInstanceCache& entityInstanceCache);
    QOrmQueryResult<QObject> merge(const QOrmQuery& query);
    QOrmQueryResult<QObject> remove(const QOrmQuery& query,
                                    QOrmEntityInstanceCache& entityInstanceCache);
    QOrmQueryResult<QObject> count(const QOrmQuery& query);

    [[nodiscard]] bool foreignKeysEnabled();
    [[nodiscard]] QOrmError setForeignKeysEnabled(bool enabled);
    [[nodiscard]] QOrmError checkForeignKeys();    void detectSqliteCapabilities();
};

// Returns whether the data type stored in the database column is compatible with its QProperty
// counterpart. Most of the time, QVariant::canConvert() is enough but there are special cases like
// Date and Time datatype in SQLite. This datatype is stored in SQLite as Text, Real or Integer, and
// QVariant::canConvert() reports a false negative.
//
// See https://www.sqlite.org/datatype3.html
bool QOrmSqliteProviderPrivate::canConvertFromSqliteToQProperty(QMetaType::Type fromSqlType,
                                                                QMetaType::Type toQPropertyType)
{
    if ((fromSqlType == QMetaType::Double || //
         fromSqlType == QMetaType::Int ||    //
         fromSqlType == QMetaType::QString) &&
        toQPropertyType == QMetaType::QDateTime)
    {
        return true;
    }
    // If target type is QVariant, assume it is convertible.
    else if (toQPropertyType == QMetaType::QVariant)
    {
        return true;
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    auto fromVariantSqlType = static_cast<QVariant::Type>(fromSqlType);
    return QVariant{fromVariantSqlType}.canConvert(toQPropertyType);
#else
    return QMetaType::canConvert(QMetaType{fromSqlType}, QMetaType{toQPropertyType});
#endif
}

static QMetaType::Type fieldMetaType(const QSqlField& field)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return static_cast<QMetaType::Type>(field.type());
#else
    return static_cast<QMetaType::Type>(field.metaType().id());
#endif
}

bool QOrmSqliteProviderPrivate::fieldHasForeignKey(const QSqlField& field)
{
    QSqlQuery query = prepareAndExecute(
        R"(SELECT 1 FROM pragma_foreign_key_list(:table_name) WHERE "from" = :column)",
        {{":table_name", field.tableName()},{":column", field.name()}});

    // query.size() is not supported for sqlite, but first() will confirm whether we got at least
    // one result which is what we care about
    return query.first();
}

QString QOrmSqliteProviderPrivate::fieldGetUniqueConstraint(const QSqlField& field)
{
    // This query returns a string representing the columns in a unique index for the field
    // The column names are also wrapped between ! to avoid partial matches with LIKE
    // This can then be used to check if the schema needs updating
    QSqlQuery query = prepareAndExecute(
        R"(
            SELECT group_concat(printf('!%s!', index_info.name)) AS columns
            FROM pragma_index_list(:table_name) AS index_list,
                 pragma_index_info(index_list.name) AS index_info
            WHERE index_list.[unique] = 1
            GROUP BY index_list.name
            HAVING columns LIKE printf('%%!%s!%%', :column))",
        {{":table_name", field.tableName()},{":column", field.name()}});

    if (!query.first())
    {
        return "";
    }

    return query.record().field("columns").value().toString();
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
    {
        // Do not leave a partially filled instance in the cache: later reads would return it as
        // if it was valid. It is not deleted because instances read while filling it may already
        // reference it.
        entityInstanceCache.take(entityInstance);
        return QOrmPrivate::makeUnexpected(fillError);
    }

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

                if (mapping.dataTypeName().startsWith("QList<"))
                {
                    propertyValue = QVariant::fromValue(result.toList());
                }
                else if (mapping.dataTypeName().startsWith("QVector<"))
                {
                    propertyValue = QVariant::fromValue(result.toVector());
                }
                else if (mapping.dataTypeName().startsWith("QSet<"))
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

                // referenced instance is in cache: assign it to the corresponding property as is.
                // Only its object ID is needed to resolve the reference, so its unsaved changes
                // (if any) do not conflict with the data read from the database.
                if (referencedEntityInstance != nullptr)
                {
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
                        QString errorText;
                        QDebug{&errorText}
                            << "Database inconsistency detected: a row in table"
                            << mapping.enclosingEntity().tableName() << "references"
                            << mapping.referencedEntity()->tableName() << "in column"
                            << mapping.tableFieldName() << "using a non-existing object ID"
                            << referencedObjectId
                            << ", or there are multiple rows in the referenced table with this "
                               "value";

                        return QOrmError{QOrm::ErrorType::Provider, errorText};
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

    const QOrmMetadata& entity = *relation.mapping();

    if (!m_database.tables().contains(entity.tableName()))
        return createTable(entity);

    if (std::optional<QString> reason = schemaMismatch(entity))
    {
        qCInfo(qtorm).noquote() << "Updating schema for" << entity.className() << "<->"
                                << entity.tableName() << ":" << *reason;

        return migrateTable(entity);
    }

    return QOrmError{QOrm::ErrorType::None, {}};
}

QOrmError QOrmSqliteProviderPrivate::createTable(const QOrmMetadata& entity)
{
    Q_Q(QOrmSqliteProvider);

    q->beginTransaction();

    QOrmError error =
        executeSchemaStatement(m_statementGenerator.generateCreateTableStatement(entity));

    if (error.type() != QOrm::ErrorType::None)
    {
        q->rollbackTransaction();
        return error;
    }

    q->commitTransaction();

    return QOrmError{QOrm::ErrorType::None, {}};
}

// Returns why the table of the entity needs to be updated, or nullopt if it is up to date. An
// update is needed if not all columns appear in both the database and the entity metadata, or if
// their definitions differ.
std::optional<QString> QOrmSqliteProviderPrivate::schemaMismatch(const QOrmMetadata& entity)
{
    QSqlRecord record = m_database.record(entity.tableName());

    for (int i = 0; i < record.count(); ++i)
    {
        if (std::optional<QString> reason = fieldMismatch(record.field(i), entity))
            return reason;
    }

    for (const QOrmPropertyMapping& mapping : entity.propertyMappings())
    {
        if (!mapping.isTransient() && !record.contains(mapping.tableFieldName()))
        {
            return QStringLiteral("a non-transient class property %1::%2 has no corresponding "
                                  "table field")
                .arg(entity.className(), mapping.classPropertyName());
        }
    }

    return std::nullopt;
}

// Returns why the table field does not match its mapping, or nullopt if it does.
std::optional<QString> QOrmSqliteProviderPrivate::fieldMismatch(const QSqlField& field,
                                                                const QOrmMetadata& entity)
{
    const QOrmPropertyMapping* mapping = entity.tableFieldMapping(field.name());

    if (mapping == nullptr)
    {
        return QStringLiteral("field %1 has no mapping in entity %2")
            .arg(field.name(), entity.className());
    }

    if (mapping->isTransient())
    {
        return QStringLiteral("field %1 is mapped to a transient property %2::%3")
            .arg(field.name(), entity.className(), mapping->classPropertyName());
    }

    if (mapping->referencedEntity() != nullptr)
    {
        Q_ASSERT(mapping->referencedEntity()->objectIdMapping() != nullptr);

        if (!canConvertFromSqliteToQProperty(
                fieldMetaType(field), mapping->referencedEntity()->objectIdMapping()->dataType()))
        {
            return QStringLiteral("data type of field %1 is incompatible with its %2::%3 mapping")
                .arg(field.name(), entity.className(), mapping->classPropertyName());
        }

        if (fieldHasForeignKey(field) != mapping->hasForeignKey())
        {
            return QStringLiteral("field %1 foreign key (%2) differs from the mapping (%3)")
                .arg(field.name())
                .arg(!mapping->hasForeignKey())
                .arg(mapping->hasForeignKey());
        }

        // Nullability and uniqueness are not checked for references.
        return std::nullopt;
    }

    if (!canConvertFromSqliteToQProperty(fieldMetaType(field), mapping->dataType()))
    {
        return QStringLiteral("data types of field %1 and its mapping %2::%3 are incompatible")
            .arg(field.name(), entity.className(), mapping->classPropertyName());
    }

    bool fieldIsNotNull = field.requiredStatus() == QSqlField::Required;

    if (fieldIsNotNull != mapping->isNotNull())
    {
        return QStringLiteral("field %1 nullability (NOT NULL: %2) differs from the mapping (%3)")
            .arg(field.name())
            .arg(fieldIsNotNull)
            .arg(mapping->isNotNull());
    }

    return uniqueConstraintMismatch(field, *mapping, entity);
}

// Returns why the unique constraint of the table field does not match its mapping, or nullopt if
// it does.
std::optional<QString> QOrmSqliteProviderPrivate::uniqueConstraintMismatch(
    const QSqlField& field,
    const QOrmPropertyMapping& mapping,
    const QOrmMetadata& entity)
{
    // Columns of the unique index containing the field, formatted as "!col1!,!col2!"
    QString fieldUniqueColumns = fieldGetUniqueConstraint(field);

    bool fieldIsUnique = !fieldUniqueColumns.isEmpty();
    bool isSingleColumnUnique = mapping.isUnique() && mapping.uniqueGroup().isEmpty();

    if (mapping.isUnique() != fieldIsUnique ||
        (isSingleColumnUnique && fieldUniqueColumns != QStringLiteral("!%1!").arg(field.name())))
    {
        return QStringLiteral("field %1 unique constraint differs from the mapping (%2, %3)")
            .arg(field.name())
            .arg(mapping.isUnique())
            .arg(mapping.uniqueGroup());
    }

    if (!mapping.isUnique() || mapping.uniqueGroup().isEmpty())
        return std::nullopt;

    QStringList columnsInIndex = fieldUniqueColumns.split(',');
    for (QString& column : columnsInIndex)
        column.remove('!');

    // Every column in the index must belong to the same unique group...
    for (const QString& column : columnsInIndex)
    {
        const QOrmPropertyMapping* columnMapping = entity.tableFieldMapping(column);

        if (columnMapping == nullptr || !columnMapping->isUnique() ||
            columnMapping->uniqueGroup() != mapping.uniqueGroup())
        {
            return QStringLiteral("field %1 should be in the same unique constraint as %2")
                .arg(column, field.name());
        }
    }

    // ...and every property of the unique group must be in the index.
    for (const QOrmPropertyMapping& propertyMapping : entity.propertyMappings())
    {
        if (propertyMapping.isUnique() && propertyMapping.uniqueGroup() == mapping.uniqueGroup() &&
            !columnsInIndex.contains(propertyMapping.tableFieldName()))
        {
            return QStringLiteral("field %1 should be in the same unique constraint as %2")
                .arg(propertyMapping.tableFieldName(), field.name());
        }
    }

    return std::nullopt;
}

// Alters an existing table using the generalized 12-step process described in
// https://sqlite.org/lang_altertable.html
QOrmError QOrmSqliteProviderPrivate::migrateTable(const QOrmMetadata& entity)
{
    Q_Q(QOrmSqliteProvider);

    QSqlRecord record = m_database.record(entity.tableName());

    // 1. If foreign key constraints are enabled, disable them using PRAGMA foreign_keys=OFF.
    bool withForeignKeys = foreignKeysEnabled();

    QOrmError error = setForeignKeysEnabled(false);

    if (error.type() != QOrm::ErrorType::None)
        return {QOrm::ErrorType::UnsynchronizedSchema, error.text()};

    // Re-enable foreign keys on every early return below. Declared before the transaction guard,
    // so it runs after the rollback and the pragma takes effect outside of the transaction.
    auto foreignKeysGuard = qScopeGuard(
        [this, withForeignKeys]()
        {
            if (!withForeignKeys)
                return;

            QOrmError restoreError = setForeignKeysEnabled(true);

            if (restoreError.type() != QOrm::ErrorType::None)
            {
                qCWarning(qtorm).noquote()
                    << "Unable to re-enable foreign keys:" << restoreError.text();
            }
        });

    // 2. Start a transaction. It is rolled back on every early return below.
    auto transactionGuard = qScopeGuard([q]() { q->rollbackTransaction(); });

    error = q->beginTransaction();

    if (error.type() != QOrm::ErrorType::None)
        return {QOrm::ErrorType::UnsynchronizedSchema, error.text()};

    // 3. Remember the format of all indexes, triggers, and views associated with table X.
    //
    // QtOrm does not support indexes, triggers, and views yet.

    // 4. Use CREATE TABLE to construct a new table "new_X" that is in the desired revised format
    // of table X
    QString newTableName =
        QString{"%1_%2"}.arg(entity.tableName(), QUuid::createUuid().toString(QUuid::Id128));

    error = executeSchemaStatement(
        m_statementGenerator.generateCreateTableStatement(entity, newTableName));

    if (error.type() != QOrm::ErrorType::None)
        return error;

    // 5. Transfer content from X into new_X
    QStringList tableColumns;
    for (const QOrmPropertyMapping& mapping : entity.propertyMappings())
    {
        if (!mapping.isTransient() && record.contains(mapping.tableFieldName()))
            tableColumns.push_back(mapping.tableFieldName());
    }

    error = executeSchemaStatement(m_statementGenerator.generateInsertIntoStatement(
        newTableName, tableColumns, entity.tableName(), tableColumns));

    if (error.type() != QOrm::ErrorType::None)
        return error;

    // 6. Drop the old table X
    error = executeSchemaStatement(m_statementGenerator.generateDropTableStatement(entity));

    if (error.type() != QOrm::ErrorType::None)
        return error;

    // 7. Change the name of new_X to X
    error = executeSchemaStatement(
        m_statementGenerator.generateRenameTableStatement(newTableName, entity.tableName()));

    if (error.type() != QOrm::ErrorType::None)
        return error;

    // 8. Use CREATE INDEX, CREATE TRIGGER, and CREATE VIEW to reconstruct indexes, triggers, and
    // views associated with table X.
    //
    // QtOrm does not support indexes, triggers, and views yet.

    // 9. If any views refer to table X in a way that is affected by the schema change, then drop
    // those views using DROP VIEW and recreate them with whatever changes are necessary to
    // accommodate the schema change using CREATE VIEW.
    //
    // QtOrm does not suport views yet.

    // 10. If foreign key constraints were originally enabled then run PRAGMA foreign_key_check to
    // verify that the schema change did not break any foreign key constraints.
    if (withForeignKeys)
    {
        error = checkForeignKeys();

        if (error.type() != QOrm::ErrorType::None)
            return {QOrm::ErrorType::UnsynchronizedSchema, error.text()};
    }

    // 11. Commit the transaction started in step 2. Go through the provider so that the
    // transaction counter stays balanced and an enclosing transaction is not committed.
    transactionGuard.dismiss();
    error = q->commitTransaction();

    if (error.type() != QOrm::ErrorType::None)
    {
        // commitTransaction() has already released the counter; a failed commit only happens for
        // the outermost transaction, so roll it back directly.
        m_database.rollback();
        return {QOrm::ErrorType::UnsynchronizedSchema, error.text()};
    }

    // 12. If foreign keys constraints were originally enabled, reenable them now.
    foreignKeysGuard.dismiss();

    if (withForeignKeys)
    {
        error = setForeignKeysEnabled(true);

        if (error.type() != QOrm::ErrorType::None)
            return {QOrm::ErrorType::UnsynchronizedSchema, error.text()};
    }

    return QOrmError{QOrm::ErrorType::None, {}};
}

QOrmError QOrmSqliteProviderPrivate::executeSchemaStatement(const QString& statement)
{
    QSqlQuery query = prepareAndExecute(statement);

    if (query.lastError().type() != QSqlError::NoError)
        return {QOrm::ErrorType::UnsynchronizedSchema, query.lastError().text()};

    return {QOrm::ErrorType::None, {}};
}

// Checks that the table matches the entity without modifying the database.
QOrmError QOrmSqliteProviderPrivate::validateSchema(const QOrmRelation& relation)
{
    Q_ASSERT(m_database.isOpen());
    Q_ASSERT(relation.type() == QOrm::RelationType::Mapping);
    Q_ASSERT(relation.mapping() != nullptr);

    const QOrmMetadata& entity = *relation.mapping();

    if (!m_database.tables().contains(entity.tableName()))
    {
        return {QOrm::ErrorType::UnsynchronizedSchema,
                QStringLiteral("Schema validation failed for %1: table %2 does not exist")
                    .arg(entity.className(), entity.tableName())};
    }

    if (std::optional<QString> reason = schemaMismatch(entity))
    {
        return {QOrm::ErrorType::UnsynchronizedSchema,
                QStringLiteral("Schema validation failed for %1 <-> %2: %3")
                    .arg(entity.className(), entity.tableName(), *reason)};
    }

    return {QOrm::ErrorType::None, {}};
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

    return QOrmQueryResult<QObject>{resultSet, static_cast<int>(resultSet.size())};
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

QOrmQueryResult<QObject> QOrmSqliteProviderPrivate::count(const QOrmQuery& query)
{
    Q_ASSERT(query.operation() == QOrm::Operation::Count);

    auto [statement, boundParameters] = m_statementGenerator.generate(query);

    QSqlQuery sqlQuery = prepareAndExecute(statement, boundParameters);

    if (sqlQuery.lastError().type() != QSqlError::NoError || !sqlQuery.next())
    {
        return QOrmQueryResult<QObject>{QOrmError{QOrm::ErrorType::Provider,
                                                  sqlQuery.lastError().text()},
                                        sqlQuery.numRowsAffected()};
    }

    int count = sqlQuery.value(0).toInt();

    return QOrmQueryResult{QVector<QObject*>{}, count};
}

bool QOrmSqliteProviderPrivate::foreignKeysEnabled()
{
    QSqlQuery query{m_database};

    if (query.exec("PRAGMA foreign_keys") && query.next())
    {
        return query.value("foreign_keys").toBool();
    }

    return false;
}

QOrmError QOrmSqliteProviderPrivate::setForeignKeysEnabled(bool enabled)
{
    QString queryText = enabled ? "PRAGMA foreign_keys=ON" : "PRAGMA foreign_keys=OFF";
    QSqlQuery query{m_database};

    bool result = query.exec(queryText);
    if (!result)
    {
        return {QOrm::ErrorType::Provider, query.lastError().text()};
    }

    // PRAGMA foreign_keys is a no-op inside an active transaction.
    if (foreignKeysEnabled() != enabled)
    {
        return {QOrm::ErrorType::Provider,
                QStringLiteral("Unable to %1 foreign keys inside an active transaction")
                    .arg(enabled ? QStringLiteral("enable") : QStringLiteral("disable"))};
    }

    return {QOrm::ErrorType::None, {}};
}

QOrmError QOrmSqliteProviderPrivate::checkForeignKeys()
{
    QSqlQuery query = prepareAndExecute("PRAGMA foreign_key_check");

    if (query.lastError().type() != QSqlError::NoError)
    {
        return {QOrm::ErrorType::Provider, query.lastError().text()};
    }

    // Each returned row is a foreign key violation: (table, rowid, parent, fkid)
    if (query.next())
    {
        return {QOrm::ErrorType::Provider,
                QStringLiteral("Foreign key violation: a row in table %1 (rowid %2) references a "
                               "missing row in table %3")
                    .arg(query.value(0).toString(),
                         query.value(1).toString(),
                         query.value(2).toString())};
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

    QSqlQuery query{inMemoryDatabase};

    if (query.exec("SELECT sqlite_version() AS version"))
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

        if (d->m_sqlConfiguration.foreignKeysEnabled())
        {
            return d->setForeignKeysEnabled(true);
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

        case QOrm::Operation::Count:
            return d->count(query);

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
