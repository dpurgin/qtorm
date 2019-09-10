#ifndef QORMABSTRACTPROVIDER_H
#define QORMABSTRACTPROVIDER_H

#include <QtOrm/qormglobal.h>

QT_BEGIN_NAMESPACE

class QObject;
class QOrmError;
class QOrmQuery;
class QMetaObject;

class Q_ORM_EXPORT QOrmAbstractProvider
{
public:
    QOrmAbstractProvider();
    virtual ~QOrmAbstractProvider();

    virtual QOrmError connectToBackend() = 0;
    virtual QOrmError disconnectFromBackend() = 0;
    virtual bool isConnectedToBackend() = 0;

    virtual QOrmError beginTransaction() = 0;
    virtual QOrmError commitTransaction() = 0;
    virtual QOrmError rollbackTransaction() = 0;

    virtual QOrmError create(QObject* entity, const QMetaObject& qMetaObject) = 0;
    virtual QOrmError read(const QOrmQuery& query) = 0;
    virtual QOrmError update(QObject* entity, const QMetaObject& qMetaObject) = 0;
    virtual QOrmError remove(QObject* entity, const QMetaObject& qMetaObject) = 0;
};

QT_END_NAMESPACE

#endif // QORMABSTRACTPROVIDER_H
