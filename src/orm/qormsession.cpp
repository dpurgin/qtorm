#include "qormsession.h"

#include "qormabstractprovider.h"
#include "qormentityinstancecache.h"
#include "qormerror.h"
#include "qormglobal_p.h"
#include "qormmetadatacache.h"
#include "qormquery.h"
#include "qormrelation.h"
#include "qormsessionconfiguration.h"
#include "qormtransactiontoken.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

class QOrmSessionPrivate
{
    Q_DECLARE_PUBLIC(QOrmSession)
    QOrmSession* q_ptr{nullptr};
    QOrmSessionConfiguration m_sessionConfiguration;
    QOrmEntityInstanceCache m_entityInstanceCache;
    QOrmError m_lastError{QOrm::ErrorType::None, {}};
    QOrmMetadataCache m_metadataCache;

    explicit QOrmSessionPrivate(QOrmSessionConfiguration sessionConfiguration, QOrmSession* parent);
    ~QOrmSessionPrivate();

    void ensureProviderConnected();

    void clearLastError();
    void setLastError(QOrmError lastError);
    std::optional<QVector<QObject*>> buildResultSet(const QOrmQueryResult& providerResult,
                                                    const QOrmMetadata& projection);
};

QOrmSessionPrivate::QOrmSessionPrivate(QOrmSessionConfiguration sessionConfiguration,
                                       QOrmSession* parent)
    : q_ptr{parent}
    , m_sessionConfiguration{std::move(sessionConfiguration)}
{
}

QOrmSessionPrivate::~QOrmSessionPrivate() = default;

void QOrmSessionPrivate::ensureProviderConnected()
{
    if (!m_sessionConfiguration.provider()->isConnectedToBackend())
        m_sessionConfiguration.provider()->connectToBackend();
}

void QOrmSessionPrivate::clearLastError()
{
    m_lastError = QOrmError{QOrm::ErrorType::None, {}};
}

void QOrmSessionPrivate::setLastError(QOrmError lastError)
{
    m_lastError = lastError;
}

std::optional<QVector<QObject*>>
QOrmSessionPrivate::buildResultSet(const QOrmQueryResult& providerResult,
                                   const QOrmMetadata& projection)
{
    QVector<QObject*> resultSet;

    const QOrmPropertyMapping* objectIdMapping = projection.objectIdMapping();

    // No object ID in this projection: cannot cache, just return the results
    if (objectIdMapping == nullptr)
    {
        resultSet = providerResult.toVector();
    }
    // If there is an object ID, compare the cached entities with the ones read from the
    // backend. If there is an inconsistency, it will be reported.
    // All read entities are replaced with their cached versions if found.
    else
    {
        QVector<QObject*> providerResultSet = providerResult.toVector();

        QString errorString;

        std::transform(
            std::cbegin(providerResultSet),
            std::cend(providerResultSet),
            std::back_inserter(resultSet),
            [this, &projection, &errorString](QObject* record) -> QObject* {
                // if an error has occurred, just delete the rest of the provider result set
                if (errorString.isEmpty())
                {
                    delete record;
                    return nullptr;
                }

                // no error: check if this instance has been cached

                QVariant objectId = QOrmPrivate::objectIdPropertyValue(record, projection);
                Q_ASSERT(objectId.isValid() && !objectId.isNull());

                QObject* cachedInstance = m_entityInstanceCache.get(projection, objectId);

                // cached instance: check if it's consistent and put it into the result set
                if (cachedInstance != nullptr)
                {
                    // if inconsistent, return an error.
                    if (m_entityInstanceCache.isModified(cachedInstance))
                    {
                        QDebug dbg{&errorString};
                        dbg << "Entity instance" << cachedInstance
                            << "was read from the database but has unsaved changes in the "
                               "OR-mapper. "
                               "Merge this instance or discard changes before reading.";
                    }

                    // delete the provider's instance because the cached one is used instead
                    delete record;
                    return cachedInstance;
                }
                // new instance: put it into cache and into the result set
                else
                {
                    m_entityInstanceCache.insert(projection, record);
                    return record;
                }
            });

        if (!errorString.isEmpty())
        {
            setLastError({QOrm::ErrorType::UnsynchronizedEntity, errorString});
            // clear the result set container but don't delete the objects because they were
            // put into the instance cache
            return std::nullopt;
        }
    }

    return std::make_optional(resultSet);
}

QOrmSession::QOrmSession(QOrmSessionConfiguration sessionConfiguration)
    : d_ptr{new QOrmSessionPrivate{sessionConfiguration, this}}
{    
}

QOrmSession::~QOrmSession()
{
    Q_D(QOrmSession);

    if (d->m_sessionConfiguration.provider()->isConnectedToBackend())
        d->m_sessionConfiguration.provider()->disconnectFromBackend();

    delete d_ptr;
}

QOrmQueryResult QOrmSession::execute(const QOrmQuery& query)
{
    Q_D(QOrmSession);

    d->clearLastError();
    d->ensureProviderConnected();

    QOrmQueryResult providerResult = d->m_sessionConfiguration.provider()->execute(query);

    // provider error: return immediately.
    if (providerResult.error().type() != QOrm::ErrorType::None)
    {
        d->setLastError({providerResult.error()});
        qDeleteAll(providerResult.toVector());
        return QOrmQueryResult{lastError()};
    }

    // If some data was read, build the result set and cache the instances
    if (query.projection().has_value())
    {
        std::optional<QVector<QObject*>> resultSet =
            d->buildResultSet(providerResult, *query.projection());

        if (!resultSet.has_value())
        {
            return QOrmQueryResult{d->m_lastError, {}, providerResult.lastInsertedId()};
        }

        // retrieve referenced entities if any

        // TODO

        return QOrmQueryResult{QOrmError{QOrm::ErrorType::None, {}},
                               *resultSet,
                               providerResult.lastInsertedId()};
    }

    return QOrmQueryResult{QOrmError{QOrm::ErrorType::None, ""},
                           {},
                           providerResult.lastInsertedId()};
}

QOrmQueryBuilder QOrmSession::from(const QOrmQuery& query)
{
    Q_ASSERT(query.operation() == QOrm::Operation::Read);

    return QOrmQueryBuilder{this, QOrmRelation{query}};
}

QOrmQueryBuilder QOrmSession::queryBuilderFor(const QMetaObject& relationMetaObject)
{
    Q_D(QOrmSession);

    return QOrmQueryBuilder{this, QOrmRelation{d->m_metadataCache[relationMetaObject]}};
}

bool QOrmSession::doMerge(QObject* entityInstance, const QMetaObject& qMetaObject)
{
    Q_D(QOrmSession);

    d->clearLastError();
    d->ensureProviderConnected();

    QOrm::Operation operation = d->m_entityInstanceCache.contains(entityInstance)
                                    ? QOrm::Operation::Update
                                    : QOrm::Operation::Create;

    if (operation == QOrm::Operation::Update &&
        !d->m_entityInstanceCache.isModified(entityInstance))
    {
        return true;
    }

    QOrmMetadata entity = d->m_metadataCache[qMetaObject];

    // Cascade merge of referenced entities if needed
    for (const QOrmPropertyMapping& mapping : entity.propertyMappings())
    {
        if (mapping.isReference() && !mapping.isTransient())
        {
            QObject* referencedInstance =
                QOrmPrivate::propertyValue(entityInstance, mapping.classPropertyName())
                    .value<QObject*>();

            if (!d->m_entityInstanceCache.contains(referencedInstance) ||
                d->m_entityInstanceCache.isModified(referencedInstance))
            {
                const QMetaObject& qMetaObject = mapping.referencedEntity()->qMetaObject();

                if (!doMerge(referencedInstance, qMetaObject))
                {
                    return false;
                }
            }
        }
    }

    QOrmQueryResult result = d->m_sessionConfiguration.provider()->execute(
        queryBuilderFor(qMetaObject).instance(qMetaObject, entityInstance).build(operation));

    d->setLastError(result.error());

    if (d->m_lastError.type() == QOrm::ErrorType::None)
    {
        const QOrmPropertyMapping* objectIdMapping =
            d->m_metadataCache[qMetaObject].objectIdMapping();

        if (objectIdMapping != nullptr && objectIdMapping->isAutogenerated())
        {
            QOrmPrivate::setPropertyValue(entityInstance,
                                          objectIdMapping->classPropertyName(),
                                          result.lastInsertedId());
        }

        if (operation == QOrm::Operation::Create)
            d->m_entityInstanceCache.insert(d->m_metadataCache[qMetaObject], entityInstance);
        else
            d->m_entityInstanceCache.markUnmodified(entityInstance);
    }

    return d->m_lastError.type() == QOrm::ErrorType::None;
}

bool QOrmSession::doRemove(QObject*& entityInstance, const QMetaObject& qMetaObject)
{
    Q_D(QOrmSession);

    d->clearLastError();
    d->ensureProviderConnected();

    const QOrmMetadata& relation = d->m_metadataCache[qMetaObject];

    QOrmQueryBuilder queryBuilder = queryBuilderFor(qMetaObject);

    if (relation.objectIdMapping() != nullptr)
    {
        QOrmFilterTerminalPredicate predicate{
            *relation.objectIdMapping(),
            QOrm::Comparison::Equal,
            QOrmPrivate::propertyValue(entityInstance,
                                       relation.objectIdMapping()->tableFieldName())};

        queryBuilder.filter(predicate);
    }
    else
    {
        qCritical() << "QtORM: Unable to remove from" << relation << "without object ID property";
        qFatal("QtORM: Consistency check failure");
    }

    QOrmQueryResult result =
        d->m_sessionConfiguration.provider()->execute(queryBuilder.build(QOrm::Operation::Delete));

    d->setLastError(result.error());

    if (d->m_lastError.type() == QOrm::ErrorType::None)
    {
        delete d->m_entityInstanceCache.take(entityInstance);
        entityInstance = nullptr;
    }

    return d->m_lastError.type() == QOrm::ErrorType::None;
}

QOrmTransactionToken QOrmSession::declareTransaction(QOrm::TransactionMode /*transactionMode*/)
{
    return {};
}

QOrmError QOrmSession::lastError() const
{
    Q_D(const QOrmSession);

    return d->m_lastError;
}

const QOrmSessionConfiguration& QOrmSession::configuration() const
{
    Q_D(const QOrmSession);

    return d->m_sessionConfiguration;
}

QOrmMetadataCache* QOrmSession::metadataCache()
{
    Q_D(QOrmSession);
    return &d->m_metadataCache;
}


QT_END_NAMESPACE
