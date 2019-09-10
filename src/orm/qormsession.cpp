#include "qormsession.h"

#include "qormabstractprovider.h"
#include "qormerror.h"
#include "qormquery.h"
#include "qormsessionconfiguration.h"
#include "qormtransactiontoken.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

class QOrmSessionPrivate
{
    Q_DECLARE_PUBLIC(QOrmSession)
    QOrmSession* q_ptr{nullptr};
    QOrmSessionConfiguration m_sessionConfiguration;

    explicit QOrmSessionPrivate(QOrmSessionConfiguration sessionConfiguration, QOrmSession* parent);
    ~QOrmSessionPrivate();

    void ensureProviderConnected();
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
}

void QOrmSessionPrivate::ensureProviderConnected()
{
    if (!m_sessionConfiguration.provider()->isConnectedToBackend())
        m_sessionConfiguration.provider()->connectToBackend();
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

QOrmQuery QOrmSession::select(const QMetaObject& entityMeta)
{
    return QOrmQuery{};
}

bool QOrmSession::merge(QObject* entityInstance, const QMetaObject& qMetaObject, QOrm::MergeMode mode)
{
    Q_D(QOrmSession);

    d->ensureProviderConnected();

    std::optional<QOrmError> error;

    if (mode == QOrm::MergeMode::Create)
        error = d->m_sessionConfiguration.provider()->create(entityInstance, qMetaObject);
    else if (mode == QOrm::MergeMode::Update)
        error = d->m_sessionConfiguration.provider()->update(entityInstance, qMetaObject);
    else if (mode == QOrm::MergeMode::Auto)
    {

    }

    if (d->m_sessionConfiguration.isVerbose() &&
            error.has_value() && error->error() != QOrm::Error::None)
    {
        qWarning() << "QtORM: unable to merge:" << error.value();
    }

    return error.has_value() && error->error() != QOrm::Error::None;
}

void QOrmSession::remove(QObject* entityInstance)
{
    Q_UNUSED(entityInstance);
}

QOrmTransactionToken QOrmSession::declareTransaction(QOrm::TransactionMode transactionMode)
{
    return {};
}



QT_END_NAMESPACE
