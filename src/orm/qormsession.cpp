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
#include <QScopeGuard>

QT_BEGIN_NAMESPACE

class QOrmSessionPrivate
{
    Q_DECLARE_PUBLIC(QOrmSession)
    QOrmSession* q_ptr{nullptr};
    QOrmSessionConfiguration m_sessionConfiguration;
    QOrmEntityInstanceCache m_entityInstanceCache;
    QOrmError m_lastError{QOrm::ErrorType::None, {}};
    QOrmMetadataCache m_metadataCache;
    QSet<const QObject*> m_mergingInstances;

    explicit QOrmSessionPrivate(QOrmSessionConfiguration sessionConfiguration, QOrmSession* parent);
    ~QOrmSessionPrivate();

    void ensureProviderConnected();

    bool needsMerge(const QObject* instance)
    {
        return instance != nullptr &&
               (!m_entityInstanceCache.contains(instance) ||
                m_entityInstanceCache.isModified(instance)) &&
               !m_mergingInstances.contains(instance);
    }

    void clearLastError();
    void setLastError(QOrmError lastError);
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

    QOrmQueryResult providerResult =
        d->m_sessionConfiguration.provider()->execute(query, d->m_entityInstanceCache);

    d->setLastError(providerResult.error());
    return providerResult;
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

    Q_ASSERT(entityInstance != nullptr);

    if (d->m_mergingInstances.contains(entityInstance))
        return true;

    d->m_mergingInstances.insert(entityInstance);

    auto mergeRemover =
        qScopeGuard([d, entityInstance]() { d->m_mergingInstances.remove(entityInstance); });

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

    if (auto result = QOrmPrivate::crossReferenceError(entity, entityInstance))
    {
        qFatal("QtOrm: %s", result->toUtf8().data());
    }

    // Merge modified referenced entity instances
    for (const QOrmPropertyMapping& mapping : entity.propertyMappings())
    {
        if (!mapping.isReference() || mapping.isTransient())
            continue;

        // T*: merge if modified
        QObject* referencedInstance =
            QOrmPrivate::propertyValue(entityInstance, mapping).value<QObject*>();

        if (!d->needsMerge(referencedInstance))
            continue;

        if (!doMerge(referencedInstance, *referencedInstance->metaObject()))
            return false;
    }

    QOrmQueryResult result = d->m_sessionConfiguration.provider()->execute(
        queryBuilderFor(qMetaObject).instance(qMetaObject, entityInstance).build(operation),
        d->m_entityInstanceCache);

    d->setLastError(result.error());

    if (d->m_lastError.type() == QOrm::ErrorType::None)
    {
        const QOrmPropertyMapping* objectIdMapping =
            d->m_metadataCache[qMetaObject].objectIdMapping();

        if (objectIdMapping != nullptr && objectIdMapping->isAutogenerated())
        {
            if (!QOrmPrivate::setPropertyValue(entityInstance,
                                               objectIdMapping->classPropertyName(),
                                               result.lastInsertedId()))
            {
                Q_ORM_UNEXPECTED_STATE;
            }
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
        d->m_sessionConfiguration.provider()->execute(queryBuilder.build(QOrm::Operation::Delete),
                                                      d->m_entityInstanceCache);

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
