#ifndef QORMSESSION_H
#define QORMSESSION_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormentitymetadata.h>
#include <QtOrm/qormsessionconfiguration.h>
#include <QtOrm/qormquerybuilder.h>

#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

class QOrmAbstractProvider;
class QOrmError;
class QOrmQuery;
class QOrmQueryBuilder;
class QOrmQueryResult;
class QOrmSessionPrivate;
class QOrmTransactionToken;

class Q_ORM_EXPORT QOrmSession
{
    Q_DECLARE_PRIVATE(QOrmSession)

public:
    explicit QOrmSession(QOrmSessionConfiguration configuration =
            QOrmSessionConfiguration::defaultConfiguration());
    ~QOrmSession();

    Q_REQUIRED_RESULT
    QOrmQueryResult execute(const QOrmQuery& query);

    template<typename T>
    bool merge(T* entityInstance, QOrm::MergeMode mode = QOrm::MergeMode::Auto)
    {
        return merge(entityInstance, T::staticMetaObject, mode);
    }

    template<typename T>
    bool remove(T* entityInstance)
    {
        return remove(entityInstance, T::staticMetaObject);
    }

    template<typename T>
    Q_REQUIRED_RESULT
    QOrmQueryBuilder select()
    {
        return select(T::staticMetaObject);
    }  

    Q_REQUIRED_RESULT
    QOrmTransactionToken declareTransaction(QOrm::TransactionMode transactionMode);

    Q_REQUIRED_RESULT
    QOrmError lastError() const;

    Q_REQUIRED_RESULT
    QOrmSessionConfiguration configuration() const;

private:
    bool merge(QObject* entityInstance, const QMetaObject& qMetaObject, QOrm::MergeMode mode);
    bool remove(QObject* entityInstance, const QMetaObject& qMetaObject);
    QOrmQueryBuilder select(const QMetaObject& projectionMetaObject);

private:
    QOrmSessionPrivate* d_ptr{nullptr};
};

QT_END_NAMESPACE


#endif // QORMSESSION_H
