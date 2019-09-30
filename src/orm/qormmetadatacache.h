#ifndef QORMMETADATACACHE_H
#define QORMMETADATACACHE_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormmetadata.h>

#include <memory>

QT_BEGIN_NAMESPACE

class QOrmMetadata;
class QOrmMetadataCachePrivate;
class QMetaObject;

class Q_ORM_EXPORT QOrmMetadataCache
{
    Q_DISABLE_COPY(QOrmMetadataCache)

public:
    QOrmMetadataCache();    
    QOrmMetadataCache(QOrmMetadataCache&&);
    ~QOrmMetadataCache();

    QOrmMetadataCache& operator=(QOrmMetadataCache&&);

    Q_REQUIRED_RESULT
    const QOrmMetadata& operator[](const QMetaObject& qMetaObject);

    template<typename T>
    Q_REQUIRED_RESULT
    const QOrmMetadata& get()
    {
        return operator[](T::staticMetaObject);
    }

    Q_REQUIRED_RESULT
    const QOrmMetadata& get(const QMetaObject& qMetaObject) { return operator[](qMetaObject); }

private:
    std::unique_ptr<QOrmMetadataCachePrivate> d;
};

QT_END_NAMESPACE

#endif // QORMMETADATACACHE_H
