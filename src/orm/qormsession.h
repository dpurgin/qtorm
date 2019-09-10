#ifndef QORMSESSION_H
#define QORMSESSION_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormentitymetadata.h>
#include <QtOrm/qormsessionconfiguration.h>
#include <QtOrm/qormquery.h>

#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

class QOrmAbstractProvider;
class QOrmQuery;
class QOrmSessionPrivate;
class QOrmTransactionToken;

class Q_ORM_EXPORT QOrmSession
{
    Q_DECLARE_PRIVATE(QOrmSession)

public:
    QOrmSession(QOrmSessionConfiguration sessionConfiguration =
                    QOrmSessionConfiguration::defaultConfiguration());
    ~QOrmSession();

    QOrmQuery select(const QMetaObject& entityMeta);

    template<typename T>
    bool merge(T* entityInstance, QOrm::MergeMode mode = QOrm::MergeMode::Auto)
    {
        return merge(entityInstance, T::staticMetaObject, mode);
    }

    void remove(QObject* entityInstance);

    Q_REQUIRED_RESULT QOrmTransactionToken declareTransaction(QOrm::TransactionMode transactionMode);

    Q_REQUIRED_RESULT QOrmEntityMetadata entityMetadata(QObject* entity);
    Q_REQUIRED_RESULT QOrmEntityMetadata entityMetadata(const QMetaObject& instanceMeta);

    template<typename T>
    Q_REQUIRED_RESULT QOrmEntityMetadata entityMetadata()
    {
        return entityMetadata(T::staticMetaObject);
    }

    template<typename T>
    QOrmQuery select()
    {
        return select(T::staticMetaObject);
    }  

protected:
    bool merge(QObject* entityInstance, const QMetaObject& qMetaObject, QOrm::MergeMode mode);

private:
    QOrmSessionPrivate* d_ptr{nullptr};
};

QT_END_NAMESPACE


#endif // QORMSESSION_H
