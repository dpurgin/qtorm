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
};

QOrmSessionPrivate::QOrmSessionPrivate(QOrmSessionConfiguration sessionConfiguration,
                                       QOrmSession* parent)
    : q_ptr{parent},
      m_sessionConfiguration{sessionConfiguration}
{
    Q_ASSERT(sessionConfiguration.isValid());
}

QOrmSessionPrivate::~QOrmSessionPrivate()
{
    qDeleteAll(m_entityInstanceCache);
}

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

    return d->m_sessionConfiguration.provider()->execute(query);
}

QOrmQueryBuilder QOrmSession::queryBuilderFor(const QMetaObject& relationMetaObject)
{
    Q_D(QOrmSession);

    return QOrmQueryBuilder{this, QOrmRelation{d->m_metadataCache[relationMetaObject]}};
}

bool QOrmSession::merge(QObject* entityInstance, const QMetaObject& qMetaObject)
{
    Q_D(QOrmSession);

    d->clearLastError();
    d->ensureProviderConnected();

    QOrmQueryResult result =
        d->m_sessionConfiguration.provider()->execute(queryBuilderFor(qMetaObject)
                                                          .instance(qMetaObject, entityInstance)
                                                          .build(QOrm::Operation::Merge));

    d->setLastError(result.error());

    return d->m_lastError.type() == QOrm::ErrorType::None;
}

bool QOrmSession::remove(QObject*& entityInstance, const QMetaObject& qMetaObject)
{
    Q_D(QOrmSession);

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

QOrmTransactionToken QOrmSession::declareTransaction(QOrm::TransactionMode transactionMode)
{
    return {};
}

QOrmError QOrmSession::lastError() const
{
    Q_D(const QOrmSession);

    return d->m_lastError;
}

QOrmSessionConfiguration QOrmSession::configuration() const
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
