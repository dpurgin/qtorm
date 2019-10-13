#ifndef QORMABSTRACTPROVIDER_H
#define QORMABSTRACTPROVIDER_H

#include <QtOrm/qormglobal.h>

QT_BEGIN_NAMESPACE

class QObject;
class QOrmEntityInstanceCache;
class QOrmError;
class QOrmMetadataCache;
class QOrmQuery;
class QOrmQueryResult;

class Q_ORM_EXPORT QOrmAbstractProvider
{
public:
    virtual ~QOrmAbstractProvider();

    virtual QOrmError connectToBackend() = 0;
    virtual QOrmError disconnectFromBackend() = 0;
    virtual bool isConnectedToBackend() = 0;

    virtual QOrmError beginTransaction() = 0;
    virtual QOrmError commitTransaction() = 0;
    virtual QOrmError rollbackTransaction() = 0;

    virtual QOrmQueryResult execute(const QOrmQuery& query,
                                    QOrmEntityInstanceCache& entityInstanceCache) = 0;
};

QT_END_NAMESPACE

#endif // QORMABSTRACTPROVIDER_H
