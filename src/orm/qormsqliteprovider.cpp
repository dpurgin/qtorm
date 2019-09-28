#include "qormsqliteprovider.h"

#include "qormclassproperty.h"
#include "qormerror.h"
#include "qormfilter.h"
#include "qormmetadatacache.h"
#include "qormpropertymapping.h"
#include "qormquery.h"
#include "qormqueryresult.h"
#include "qormrelation.h"
#include "qormsqlconfiguration.h"

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

namespace std
{
    template<>
    struct hash<QString>
    {
        std::size_t operator()(const QString& str) const noexcept
        {
            return qHash(str);
        }
    };
}

class QOrmSqliteProviderPrivate
{
    friend class QOrmSqliteProvider;

    explicit QOrmSqliteProviderPrivate(const QOrmSqlConfiguration& configuration)
        : m_sqlConfiguration{configuration}
    {
    }

    QSqlDatabase m_database;
    QOrmSqlConfiguration m_sqlConfiguration;
    QSet<QString> m_schemaSyncCache;

    Q_REQUIRED_RESULT
    QString toSqlType(QVariant::Type type);

    Q_REQUIRED_RESULT
    QOrmError lastDatabaseError() const;

    Q_REQUIRED_RESULT
    QSqlQuery prepareAndExecute(const QString& statement, const QVariantMap& parameters);

    Q_REQUIRED_RESULT
    QVariant propertyValue(QObject* entityInstance, const QString& value)
    {
        return entityInstance->property(value.toUtf8().data());
    }

    void setPropertyValue(QObject* entityInstance, const QString& property, const QVariant& value)
    {
        entityInstance->setProperty(property.toUtf8().data(), value);
    }

    Q_REQUIRED_RESULT
    QObject* makeEntityInstance(const QOrmMetadata& entityMetadata, const QSqlRecord& record);

    QOrmError ensureSchemaSynchronized(const QOrmRelation& entityMetadata);
    QOrmError recreateSchema(const QOrmRelation& entityMetadata);
    QOrmError updateSchema(const QOrmRelation& entityMetadata);
    QOrmError validateSchema(const QOrmRelation& validateSchema);

    QOrmQueryResult read(const QOrmQuery& query);
};

QString QOrmSqliteProviderPrivate::toSqlType(QVariant::Type type)
{
    // SQLite data types: https://sqlite.org/datatype3.html
    switch (type)
    {
        case QVariant::Int:
        case QVariant::UInt:
        case QVariant::LongLong:
        case QVariant::ULongLong:
            return QStringLiteral("INTEGER");

        case QVariant::Double:
            return QStringLiteral("REAL");

        case QVariant::Bool:
        case QVariant::Date:
        case QVariant::Time:
        case QVariant::DateTime:
            return QStringLiteral("NUMERIC");

        case QVariant::Char:
        case QVariant::String:
            return QStringLiteral("TEXT");

        default:
            return QStringLiteral("BLOB");
    }
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

QObject* QOrmSqliteProviderPrivate::makeEntityInstance(const QOrmMetadata& entityMetadata,
                                                    const QSqlRecord& record)
{
    QObject* entityInstance = entityMetadata.qMetaObject().newInstance();

    for (const QOrmPropertyMapping& mapping: entityMetadata.propertyMappings())
    {
        setPropertyValue(entityInstance,
                         mapping.classPropertyName(),
                         record.value(mapping.tableFieldName()));
    }

    return entityInstance;
}

QOrmError QOrmSqliteProviderPrivate::ensureSchemaSynchronized(const QOrmRelation& relation)
{
    if (m_sqlConfiguration.schemaMode() == QOrmSqlConfiguration::SchemaMode::Bypass)
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
                case QOrmSqlConfiguration::SchemaMode::Recreate:
                    error = recreateSchema(relation);
                    break;

                case QOrmSqlConfiguration::SchemaMode::Update:
                    error = updateSchema(relation);
                    break;

                case QOrmSqlConfiguration::SchemaMode::Validate:
                    error = validateSchema(relation);
                    break;

                case QOrmSqlConfiguration::SchemaMode::Bypass:
                    qFatal("QtORM: Unexpected state");
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
}

QOrmError QOrmSqliteProviderPrivate::recreateSchema(const QOrmRelation& relation)
{
    Q_ASSERT(m_database.isOpen());
    Q_ASSERT(relation.type() == QOrm::RelationType::Mapping);
    Q_ASSERT(relation.mapping() != nullptr);

    if (m_database.tables().contains(relation.mapping()->tableName()))
    {
        QString statement = QString{"DROP TABLE %1"}.arg(relation.mapping()->tableName());

        QSqlQuery query = prepareAndExecute(statement);

        if (query.lastError().type() != QSqlError::NoError)
            return QOrmError{QOrm::ErrorType::UnsynchronizedSchema, query.lastError().text()};
    }

    QStringList fields;

    for (const QOrmPropertyMapping& mapping : relation.mapping()->propertyMappings())
    {
        QStringList columnDefs = { mapping.tableFieldName(), toSqlType(mapping.dataType()) };

        if (mapping.isObjectId())
            columnDefs.push_back("PRIMARY KEY");

        if (mapping.isAutogenerated())
            columnDefs.push_back("AUTOINCREMENT");

        fields.push_back(columnDefs.join(' '));
    }

    QString fieldsStr = fields.join(',');

    QString statement =
        QString::fromUtf8("CREATE TABLE %1(%2)").arg(relation.mapping()->tableName(), fieldsStr);

    QSqlQuery query = prepareAndExecute(statement);

    if (query.lastError().type() != QSqlError::NoError)
        return QOrmError{QOrm::ErrorType::UnsynchronizedSchema, query.lastError().text()};

    return QOrmError{QOrm::ErrorType::None, ""};
}

QOrmError QOrmSqliteProviderPrivate::updateSchema(const QOrmRelation& relation)
{
    Q_UNUSED(relation)
    return QOrmError{QOrm::ErrorType::UnsynchronizedSchema, "Not implemented"};
}

QOrmError QOrmSqliteProviderPrivate::validateSchema(const QOrmRelation& relation)
{
    Q_UNUSED(relation)
    return QOrmError{QOrm::ErrorType::UnsynchronizedSchema, "Not implemented"};
}

QOrmQueryResult QOrmSqliteProviderPrivate::read(const QOrmQuery& query)
{
    std::optional<QOrmMetadata> projectionMeta = query.projection();
    Q_ASSERT(projectionMeta.has_value());

    auto [statement, boundParameters] = QOrmSqliteStatementGenerator::generate(query);

    QSqlQuery sqlQuery = prepareAndExecute(statement, boundParameters);

    if (sqlQuery.lastError().type() != QSqlError::NoError)
        return QOrmQueryResult{QOrmError{QOrm::ErrorType::Provider, sqlQuery.lastError().text()}};

    QVector<QObject*> resultSet;

    while (sqlQuery.next())
    {
        resultSet.push_back(makeEntityInstance(*projectionMeta, sqlQuery.record()));
    }

    return QOrmQueryResult{resultSet};
}

QOrmSqliteProvider::QOrmSqliteProvider(const QOrmSqlConfiguration& sqlConfiguration)
    : QOrmAbstractProvider{},
      d_ptr{new QOrmSqliteProviderPrivate{sqlConfiguration}}
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
        d->m_database = QSqlDatabase::addDatabase(d->m_sqlConfiguration.driverName());
        d->m_database.setConnectOptions(d->m_sqlConfiguration.connectOptions());
        d->m_database.setHostName(d->m_sqlConfiguration.hostName());
        d->m_database.setDatabaseName(d->m_sqlConfiguration.databaseName());
        d->m_database.setPassword(d->m_sqlConfiguration.password());
        d->m_database.setPort(d->m_sqlConfiguration.port());
        d->m_database.setUserName(d->m_sqlConfiguration.userName());

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

QOrmQueryResult QOrmSqliteProvider::execute(const QOrmQuery& query)
{
    Q_D(QOrmSqliteProvider);

    switch (query.operation())
    {
        case QOrm::Operation::Read:
            d->ensureSchemaSynchronized(query.relation());
            return d->read(query);

        default:
            qFatal("Not implemented");
    }
}

//QOrmError QOrmSqlProvider::create(QObject* entityInstance, const QMetaObject& qMetaObject)
//{
//    Q_D(QOrmSqlProvider);

//    const QOrmMetadata& entityMeta = (*d->m_metadataCache)[qMetaObject];

//    QOrmError error = d->ensureSchemaSynchronized(entityMeta);

//    if (error.error() != QOrm::Error::None)
//    {
//        return error;
//    }

//    QStringList fieldsList;
//    QStringList valuesList;
//    QVariantMap parameters;

//    for (const QOrmPropertyMapping& propertyMapping: entityMeta.propertyMappings())
//    {
//        if (propertyMapping.isAutogenerated())
//            continue;

//        fieldsList.push_back(propertyMapping.tableFieldName());
//        valuesList.push_back(':' % propertyMapping.tableFieldName());
//        parameters[':' % propertyMapping.tableFieldName()] =
//                d->propertyValue(entityInstance, propertyMapping.classPropertyName());
//    }

//    QString fieldsStr = fieldsList.join(',');
//    QString valuesStr = valuesList.join(',');

//    QString statement = QStringLiteral("INSERT INTO %1(%2) VALUES(%3)")
//                        .arg(entityMeta.tableName(), fieldsStr, valuesStr);


//    QSqlQuery query = d->prepareAndExecute(statement, parameters);

//    if (query.lastError().type() != QSqlError::NoError)
//        return QOrmError{QOrm::Error::Provider, query.lastError().text()};

//    // Update autogenerated ID if any

//    std::optional<QOrmPropertyMapping> objectIdMapping = entityMeta.objectIdMapping();

//    if (objectIdMapping.has_value() && objectIdMapping->isAutogenerated())
//    {
//        d->setPropertyValue(entityInstance,
//                            objectIdMapping->classPropertyName(),
//                            query.lastInsertId());
//    }

//    return QOrmError{QOrm::Error::None, {}};
//}

//QOrmQueryResult QOrmSqlProvider::read(const QOrmQuery& ormQuery)
//{
//    Q_D(QOrmSqlProvider);

//    const QOrmMetadata& projectionMeta = ormQuery.projection();

//    QOrmSqliteStatementGenerator generator;

//    std::pair<QString, QVariantMap> statement = generator.generate(ormQuery);

//    QSqlQuery sqlQuery = d->prepareAndExecute(statement.first, statement.second);

//    if (sqlQuery.lastError().type() != QSqlError::NoError)
//        return QOrmQueryResult{QOrmError{QOrm::Error::Provider, sqlQuery.lastError().text()}};

//    QVector<QObject*> resultSet;

//    while (sqlQuery.next())
//    {
//        resultSet.push_back(d->makeEntityInstance(projectionMeta, sqlQuery.record()));
//    }

//    return QOrmQueryResult{resultSet};
//}

//QOrmError QOrmSqlProvider::update(QObject* entityInstance, const QMetaObject& qMetaObject)
//{
//    Q_D(QOrmSqlProvider);

//    const QOrmMetadata& entityMeta = (*d->m_metadataCache)[qMetaObject];
//    std::optional<QOrmPropertyMapping> objectIdMapping = entityMeta.objectIdMapping();

//    if (!objectIdMapping.has_value())
//        return QOrmError{QOrm::Error::InvalidMapping, "Cannot update entity without object ID property"};

//    QStringList setList;
//    QVariantMap parameters;

//    for (const QOrmPropertyMapping& propertyMapping: entityMeta.propertyMappings())
//    {
//        parameters[':' % propertyMapping.tableFieldName()] =
//                d->propertyValue(entityInstance, propertyMapping.classPropertyName());

//        if (propertyMapping.isAutogenerated())
//            continue;

//        setList.push_back(QString{"%1 = :%1"}.arg(propertyMapping.tableFieldName()));
//    }

//    QString setStr = setList.join(',');
//    QString whereStr = QString{"%1 = :%1"}.arg(objectIdMapping->tableFieldName());

//    QString statement = QString{"UPDATE %1 SET %2 WHERE %3"}
//                        .arg(entityMeta.tableName(), setStr, whereStr);

//    QSqlQuery query = d->prepareAndExecute(statement, parameters);

//    if (query.lastError().type() != QSqlError::NoError)
//        return QOrmError{QOrm::Error::Provider, query.lastError().text()};

//    if (query.numRowsAffected() != -1 && query.numRowsAffected() != 1)
//        return QOrmError{QOrm::Error::UnsynchronizedEntity, QObject::tr("Unsynchronized entity")};

//    return QOrmError{QOrm::Error::None, {}};
//}

//QOrmError QOrmSqlProvider::remove(QObject* entityInstance, const QMetaObject& qMetaObject)
//{
//    Q_D(QOrmSqlProvider);

//    const QOrmMetadata& entityMeta = (*d->m_metadataCache)[qMetaObject];

//    QStringList whereList;
//    QVariantMap parameters;

//    for (const QOrmPropertyMapping& propertyMapping: entityMeta.propertyMappings())
//    {
//        parameters[':' % propertyMapping.tableFieldName()] =
//                d->propertyValue(entityInstance, propertyMapping.classPropertyName());
//        whereList.push_back(QString{"(%1 = :%1)"}.arg(propertyMapping.tableFieldName()));
//    }

//    auto statement = QString{"DELETE FROM %1 WHERE %2"}
//                     .arg(entityMeta.tableName(), whereList.join(" AND "));

//    QSqlQuery query = d->prepareAndExecute(statement, parameters);

//    if (query.lastError().type() != QSqlError::NoError)
//        return QOrmError{QOrm::Error::Provider, query.lastError().text()};

//    if (query.numRowsAffected() != -1 && query.numRowsAffected() != 1)
//        return QOrmError{QOrm::Error::UnsynchronizedEntity, QObject::tr("Unsynchronized entity")};

//    return QOrmError{QOrm::Error::None, {}};
//}

QOrmSqlConfiguration QOrmSqliteProvider::configuration() const
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
