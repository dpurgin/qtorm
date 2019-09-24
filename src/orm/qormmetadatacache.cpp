#include "qormmetadatacache.h"
#include "qormglobal_p.h"
#include "qormpropertymapping.h"

#include <QMetaObject>
#include <QMetaProperty>
#include <QHash>

class QOrmMetadataCachePrivate
{
    friend class QOrmMetadataCache;

    std::unordered_map<QString, QOrmMetadata> m_cache;
};

QOrmMetadataCache::QOrmMetadataCache()
    : d{new QOrmMetadataCachePrivate}
{    
}

QOrmMetadataCache::QOrmMetadataCache(QOrmMetadataCache&&) = default;

QOrmMetadataCache& QOrmMetadataCache::operator=(QOrmMetadataCache&&) = default;

QOrmMetadataCache::~QOrmMetadataCache() = default;

const QOrmMetadata& QOrmMetadataCache::operator[](const QMetaObject& qMetaObject)
{
    QString className = QString::fromUtf8(qMetaObject.className());

    if (d->m_cache.find(className) == std::end(d->m_cache))
        d->m_cache.emplace(className, QOrmMetadata{qMetaObject});

    return d->m_cache.at(className);
}
