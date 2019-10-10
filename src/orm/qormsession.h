#ifndef QORMSESSION_H
#define QORMSESSION_H

#include <QtOrm/qormclassproperty.h>
#include <QtOrm/qormglobal.h>
#include <QtOrm/qormmetadata.h>
#include <QtOrm/qormquerybuilder.h>
#include <QtOrm/qormsessionconfiguration.h>
#include <QtOrm/qormtransactiontoken.h>

#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

class QOrmAbstractProvider;
class QOrmEntityInstanceCache;
class QOrmError;
class QOrmQuery;
class QOrmQueryBuilder;
class QOrmQueryResult;
class QOrmSessionPrivate;

class Q_ORM_EXPORT QOrmSession
{
    Q_DECLARE_PRIVATE(QOrmSession)

public:
    explicit QOrmSession(const QOrmSessionConfiguration& configuration =
                             QOrmSessionConfiguration::defaultConfiguration());
    ~QOrmSession();

    Q_REQUIRED_RESULT
    QOrmQueryResult execute(const QOrmQuery& query);

    Q_REQUIRED_RESULT
    QOrmQueryBuilder from(const QOrmQuery& query);

    template<typename T>
    bool merge(T* entityInstance)
    {
        return doMerge(entityInstance, T::staticMetaObject);
    }

    template<typename... Ts>
    bool merge(Ts... instances)
    {
        return (... && merge(instances));
    }

    template<typename T>
    bool remove(T*& entityInstance)
    {
        return doRemove(entityInstance, T::staticMetaObject);
    }

    template<typename T>
    QOrmQueryBuilder from()
    {
        return queryBuilderFor(T::staticMetaObject);
    }

    template<typename T>
    QOrmQueryBuilder into()
    {
        return queryBuilderFor(T::staticMetaObject);
    }

    Q_REQUIRED_RESULT
    QOrmTransactionToken declareTransaction(QOrm::TransactionMode transactionMode);

    Q_REQUIRED_RESULT
    QOrmError lastError() const;

    Q_REQUIRED_RESULT
    const QOrmSessionConfiguration& configuration() const;

    Q_REQUIRED_RESULT
    QOrmMetadataCache* metadataCache();

    Q_REQUIRED_RESULT
    QOrmEntityInstanceCache* entityInstanceCache();

private:
    bool doMerge(QObject* entityInstance, const QMetaObject& qMetaObject);
    bool doRemove(QObject*& entityInstance, const QMetaObject& qMetaObject);
    QOrmQueryBuilder queryBuilderFor(const QMetaObject& relationMetaObject);

private:
    QOrmSessionPrivate* d_ptr{nullptr};
};

QT_END_NAMESPACE


#endif // QORMSESSION_H
