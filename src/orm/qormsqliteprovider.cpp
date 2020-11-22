/*
 * Copyright (C) 2019 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2019 sequality software engineering e.U. <office@sequality.at>
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

#include <QDebug>
#include <QMetaObject>
#include <QMetaProperty>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

QT_BEGIN_NAMESPACE

class QOrmSqliteProviderPrivate
{
    friend class QOrmSqliteProvider;

    explicit QOrmSqliteProviderPrivate(const QOrmSqliteConfiguration& configuration)
        : m_sqlConfiguration{configuration}
    {
    }

    QSqlDatabase m_database;
    QOrmSqliteConfiguration m_sqlConfiguration;
    QSet<QString> m_schemaSyncCache;

    Q_REQUIRED_RESULT
    QString toSqlType(QVariant::Type type);

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
    QOrmError validateSchema(const QOrmRelation& validateSchema);

    QOrmQueryResult<QObject> read(const QOrmQuery& query,
                                  QOrmEntityInstanceCache& entityInstanceCache);
    QOrmQueryResult<QObject> merge(const QOrmQuery& query);
    QOrmQueryResult<QObject> remove(const QOrmQuery& query);
};

QOrmError QOrmSqliteProviderPrivate::lastDatabaseError() const
{
    return QOrmError{QOrm::ErrorType::Provider, m_database.lastError().text()};
}

QSqlQuery QOrmSqliteProviderPrivate::prepareAndExecute(const QString& statement,
                                                    const QVariantMap& parameters = {})
{
    QSqlQuery query{m_database};

    if (m_sqlConfiguration.verbose())
        qDebug() << "Executing:" << statement;

    if (!query.prepare(statement))
        return query;

    if (!parameters.isEmpty())
    {
        if (m_sqlConfiguration.verbose())
            qDebug() << "Bound parameters:" << parameters;

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
                    propertyValue = QVariant::fromValue(result.toVector());
                else if (mapping.dataTypeName().startsWith("QSet<", Qt::CaseInsensitive))
                    propertyValue = QVariant::fromValue(result.toVector().toList().toSet());
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
                                    queryFlags};

                    QOrmQueryResult<QObject> result = read(query, entityInstanceCache);

                    // error during read: return this error and do not continue
                    if (result.error().type() != QOrm::ErrorType::None)
                    {
                        return result.error();
                    }

                    // sanity check: when selecting by object id, only one instance should be
                    // returned
                    Q_ASSERT(result.toVector().size() == 1);

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
        else
        {
            if (!QOrmPrivate::setPropertyValue(entityInstance,
                                               mapping.classPropertyName(),
                                               record.value(mapping.tableFieldName())))
            {
                qDebug("Unable to setPropertyValue() for %s <-> %s",
                       qPrintable(mapping.classPropertyName()),
                       qPrintable(mapping.tableFieldName()));
                Q_ORM_UNEXPECTED_STATE;
            }
        }
    }

    return QOrmError{QOrm::ErrorType::None, {}};
}

QOrmError QOrmSqliteProviderPrivate::ensureSchemaSynchronized(const QOrmRelation& relation)
{
    if (m_sqlConfiguration.schemaMode() == QOrmSqliteConfiguration::SchemaMode::Bypass)
        return {QOrm::ErrorType::None, ""};

    switch (relation.type())
    {
        case QOrm::RelationType::Mapping:
        {
            Q_ASSERT(relation.mapping() != nullptr);

            if (m_schemaSyncCache.contains(relation.mapping()->className()))
                return {QOrm::ErrorType::None, ""};

            std::optional<QOrmError> error;

            switch (m_sqlConfiguration.schemaMode())
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
                    Q_ORM_UNEXPECTED_STATE;
            }

            Q_ASSERT(error.has_value());

            if (error->type() == QOrm::ErrorType::None)
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

            return *error;
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
        QString statement =
            QOrmSqliteStatementGenerator::generateDropTableStatement(*relation.mapping());

        QSqlQuery query = prepareAndExecute(statement);

        if (query.lastError().type() != QSqlError::NoError)
            return QOrmError{QOrm::ErrorType::UnsynchronizedSchema, query.lastError().text()};
    }

    QString statement =
        QOrmSqliteStatementGenerator::generateCreateTableStatement(*relation.mapping());

    QSqlQuery query = prepareAndExecute(statement);

    if (query.lastError().type() != QSqlError::NoError)
        return QOrmError{QOrm::ErrorType::UnsynchronizedSchema, query.lastError().text()};

    return QOrmError{QOrm::ErrorType::None, ""};
}

QOrmError QOrmSqliteProviderPrivate::updateSchema(const QOrmRelation& relation)
{
    Q_UNUSED(relation)
    Q_ORM_NOT_IMPLEMENTED;
}

QOrmError QOrmSqliteProviderPrivate::validateSchema(const QOrmRelation& relation)
{
    Q_UNUSED(relation)
    Q_ORM_NOT_IMPLEMENTED;
}

QOrmQueryResult<QObject> QOrmSqliteProviderPrivate::read(
    const QOrmQuery& query,
    QOrmEntityInstanceCache& entityInstanceCache)
{
    Q_ASSERT(query.projection().has_value());

    auto [statement, boundParameters] = QOrmSqliteStatementGenerator::generate(query);

    QSqlQuery sqlQuery = prepareAndExecute(statement, boundParameters);

    if (sqlQuery.lastError().type() != QSqlError::NoError)
        return QOrmQueryResult<QObject>{
            QOrmError{QOrm::ErrorType::Provider, sqlQuery.lastError().text()}};

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
                        entityInstanceCache.markUnmodified(cachedInstance);
                        return QOrmQueryResult<QObject>{error};
                    }
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

    return QOrmQueryResult<QObject>{resultSet};
}

QOrmQueryResult<QObject> QOrmSqliteProviderPrivate::merge(const QOrmQuery& query)
{
    Q_ASSERT(query.relation().type() == QOrm::RelationType::Mapping);
    Q_ASSERT(query.entityInstance() != nullptr);

    auto [statement, boundParameters] = QOrmSqliteStatementGenerator::generate(query);

    QSqlQuery sqlQuery = prepareAndExecute(statement, boundParameters);

    if (sqlQuery.lastError().type() != QSqlError::NoError)
        return QOrmQueryResult<QObject>{{QOrm::ErrorType::Provider, sqlQuery.lastError().text()}};

    if (sqlQuery.numRowsAffected() != 1)
    {
        return QOrmQueryResult<QObject>{
            {QOrm::ErrorType::UnsynchronizedEntity, "Unexpected number of rows affected"}};
    }

    return QOrmQueryResult<QObject>{sqlQuery.lastInsertId()};
}

QOrmQueryResult<QObject> QOrmSqliteProviderPrivate::remove(const QOrmQuery& query)
{
    auto [statement, boundParameters] = QOrmSqliteStatementGenerator::generate(query);

    QSqlQuery sqlQuery = prepareAndExecute(statement, boundParameters);

    if (sqlQuery.lastError().type() != QSqlError::NoError)
        return QOrmQueryResult<QObject>{{QOrm::ErrorType::Provider, sqlQuery.lastError().text()}};

    return QOrmQueryResult<QObject>{sqlQuery.numRowsAffected()};
}

QOrmSqliteProvider::QOrmSqliteProvider(const QOrmSqliteConfiguration& sqlConfiguration)
    : QOrmAbstractProvider{}
    , d_ptr{new QOrmSqliteProviderPrivate{sqlConfiguration}}
{
}

QOrmSqliteProvider::~QOrmSqliteProvider()
{
    delete d_ptr;
}

QOrmError QOrmSqliteProvider::connectToBackend()
{
    Q_D(QOrmSqliteProvider);

    if (!d->m_database.isOpen())
    {
        d->m_database = QSqlDatabase::addDatabase("QSQLITE");
        d->m_database.setConnectOptions(d->m_sqlConfiguration.connectOptions());
        d->m_database.setDatabaseName(d->m_sqlConfiguration.databaseName());

        if (!d->m_database.open())
            return d->lastDatabaseError();
    }

    return QOrmError{QOrm::ErrorType::None, {}};
}

QOrmError QOrmSqliteProvider::disconnectFromBackend()
{
    Q_D(QOrmSqliteProvider);

    d->m_database.close();
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);

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

    if (!d->m_database.transaction())
    {
        QSqlError error = d->m_database.lastError();

        if (error.type() != QSqlError::NoError)
            return d->lastDatabaseError();
        else
            return QOrmError{QOrm::ErrorType::Other, QStringLiteral("Unable to start transaction")};
    }

    return QOrmError{QOrm::ErrorType::None, {}};
}

QOrmError QOrmSqliteProvider::commitTransaction()
{
    Q_D(QOrmSqliteProvider);

    if (!d->m_database.commit())
    {
        QSqlError error = d->m_database.lastError();

        if (error.type() != QSqlError::NoError)
            return d->lastDatabaseError();
        else
            return QOrmError{QOrm::ErrorType::Other,
                             QStringLiteral("Unable to commit transaction")};
    }

    return QOrmError{QOrm::ErrorType::None, {}};
}

QOrmError QOrmSqliteProvider::rollbackTransaction()
{
    Q_D(QOrmSqliteProvider);

    if (!d->m_database.rollback())
    {
        QSqlError error = d->m_database.lastError();

        if (error.type() != QSqlError::NoError)
            return d->lastDatabaseError();
        else
            return QOrmError{QOrm::ErrorType::Other,
                             QStringLiteral("Unable to rollback transaction")};
    }

    return QOrmError{QOrm::ErrorType::None, {}};
}

QOrmQueryResult<QObject> QOrmSqliteProvider::execute(const QOrmQuery& query,
                                                     QOrmEntityInstanceCache& entityInstanceCache)
{
    Q_D(QOrmSqliteProvider);

    d->ensureSchemaSynchronized(query.relation());

    switch (query.operation())
    {
        case QOrm::Operation::Read:
            return d->read(query, entityInstanceCache);

        case QOrm::Operation::Create:
        case QOrm::Operation::Update:
            return d->merge(query);

        case QOrm::Operation::Delete:
            return d->remove(query);

        case QOrm::Operation::Merge:
            Q_ORM_UNEXPECTED_STATE;
    }

    Q_ORM_UNEXPECTED_STATE;
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
