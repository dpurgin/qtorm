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

    QOrmError ensureSchemaSynchronized(const QOrmRelation& entityMetadata);
    QOrmError recreateSchema(const QOrmRelation& entityMetadata);
    QOrmError updateSchema(const QOrmRelation& entityMetadata);
    QOrmError validateSchema(const QOrmRelation& validateSchema);

    QOrmQueryResult read(const QOrmQuery& query, QOrmEntityInstanceCache& entityInstanceCache);
    QOrmQueryResult merge(const QOrmQuery& query);
    QOrmQueryResult remove(const QOrmQuery& query);
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
    std::unique_ptr<QObject> entityInstance{entityMetadata.qMetaObject().newInstance()};
    Q_ASSERT(entityInstance.get() != nullptr);

    for (const QOrmPropertyMapping& mapping : entityMetadata.propertyMappings())
    {
        // if this is a reference, retrieve referenced entities and assign
        if (mapping.isReference())
        {
            // transient references are one-to-many references
            if (mapping.isTransient())
            {
                //                Q_ORM_NOT_IMPLEMENTED;
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
                    if (entityInstanceCache.isModified(referencedEntityInstance))
                    {
                        Q_ORM_UNEXPECTED_STATE;
                    }

                    QOrmPrivate::setPropertyValue(entityInstance.get(),
                                                  mapping.classPropertyName(),
                                                  QVariant::fromValue(referencedEntityInstance));
                }
                // referenced instance is not in cache: retrieve it from the database by ID
                else
                {
                    QOrmFilter filter{*mapping.referencedEntity()->objectIdMapping() ==
                                      referencedObjectId};

                    QOrmRelation referencedRelation{*mapping.referencedEntity()};

                    QOrmError syncError = ensureSchemaSynchronized(referencedRelation);
                    if (syncError.type() != QOrm::ErrorType::None)
                        return QOrmPrivate::makeUnexpected(syncError);

                    QOrmQuery query{QOrm::Operation::Read,
                                    referencedRelation,
                                    *mapping.referencedEntity(),
                                    filter,
                                    std::nullopt};

                    QOrmQueryResult result = read(query, entityInstanceCache);

                    // error during read: return this error and do not continue
                    if (result.error().type() != QOrm::ErrorType::None)
                    {
                        return QOrmPrivate::makeUnexpected(result.error());
                    }

                    // sanity check: when selecting by object id, only one instance should be
                    // returned
                    Q_ASSERT(result.toVector().size() == 1);

                    QOrmPrivate::setPropertyValue(entityInstance.get(),
                                                  mapping.classPropertyName(),
                                                  QVariant::fromValue(result.toVector().front()));
                }
            }
        }
        // just a value: set the property value
        else
        {
            QOrmPrivate::setPropertyValue(entityInstance.get(),
                                          mapping.classPropertyName(),
                                          record.value(mapping.tableFieldName()));
        }
    }

    return entityInstance.release();
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
                m_schemaSyncCache.insert(relation.mapping()->className());

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

QOrmQueryResult QOrmSqliteProviderPrivate::read(const QOrmQuery& query,
                                                QOrmEntityInstanceCache& entityInstanceCache)
{
    Q_ASSERT(query.projection().has_value());

    auto [statement, boundParameters] = QOrmSqliteStatementGenerator::generate(query);

    QSqlQuery sqlQuery = prepareAndExecute(statement, boundParameters);

    if (sqlQuery.lastError().type() != QSqlError::NoError)
        return QOrmQueryResult{QOrmError{QOrm::ErrorType::Provider, sqlQuery.lastError().text()}};

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
                if (entityInstanceCache.isModified(cachedInstance))
                {
                    QString errorString;
                    QDebug dbg{&errorString};
                    dbg << "Entity instance" << cachedInstance
                        << "was read from the database but has unsaved changes in the OR-mapper. "
                           "Merge this instance or discard changes before reading.";

                    return QOrmQueryResult{
                        QOrmError{QOrm::ErrorType::UnsynchronizedEntity, errorString}};
                }

                resultSet.push_back(cachedInstance);
            }
            // new instance: put it into the cache
            else
            {
                QOrmPrivate::Expected<QObject*, QOrmError> entityInstance =
                    makeEntityInstance(*query.projection(), sqlQuery.record(), entityInstanceCache);

                if (entityInstance)
                {
                    resultSet.push_back(entityInstance.value());
                    entityInstanceCache.insert(*query.projection(), entityInstance.value());
                }
                else
                {
                    return QOrmQueryResult{entityInstance.error()};
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
                return QOrmQueryResult{entityInstance.error()};
            }
        }
    }

    return QOrmQueryResult{resultSet};
}

QOrmQueryResult QOrmSqliteProviderPrivate::merge(const QOrmQuery& query)
{
    Q_ASSERT(query.relation().type() == QOrm::RelationType::Mapping);
    Q_ASSERT(query.entityInstance() != nullptr);

    auto [statement, boundParameters] = QOrmSqliteStatementGenerator::generate(query);

    QSqlQuery sqlQuery = prepareAndExecute(statement, boundParameters);

    if (sqlQuery.lastError().type() != QSqlError::NoError)
        return QOrmQueryResult{{QOrm::ErrorType::Provider, sqlQuery.lastError().text()}};

    if (sqlQuery.numRowsAffected() != 1)
    {
        return QOrmQueryResult{
            {QOrm::ErrorType::UnsynchronizedEntity, "Unexpected number of rows affected"}};
    }

    return QOrmQueryResult{sqlQuery.lastInsertId()};
}

QOrmQueryResult QOrmSqliteProviderPrivate::remove(const QOrmQuery& query)
{
    auto [statement, boundParameters] = QOrmSqliteStatementGenerator::generate(query);

    QSqlQuery sqlQuery = prepareAndExecute(statement, boundParameters);

    if (sqlQuery.lastError().type() != QSqlError::NoError)
        return QOrmQueryResult{{QOrm::ErrorType::Provider, sqlQuery.lastError().text()}};

    return QOrmQueryResult{sqlQuery.numRowsAffected()};
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

QOrmQueryResult QOrmSqliteProvider::execute(const QOrmQuery& query,
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
