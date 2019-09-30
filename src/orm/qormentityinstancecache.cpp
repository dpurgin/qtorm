#include "qormentityinstancecache.h"
#include "qormmetadata.h"

#include <QSet>

QT_BEGIN_NAMESPACE

class QOrmEntityInstanceCachePrivate
{
    friend class QOrmEntityInstanceCache;

    QSet<QObject*> m_cache;
};

QOrmEntityInstanceCache::QOrmEntityInstanceCache()
    : d{new QOrmEntityInstanceCachePrivate}
{
}

QOrmEntityInstanceCache::~QOrmEntityInstanceCache()
{
    qDeleteAll(d->m_cache);
}

bool QOrmEntityInstanceCache::contains(QObject* instance) const
{
    return d->m_cache.contains(instance);
}

void QOrmEntityInstanceCache::insert(const QOrmMetadata& metadata, QObject* instance)
{
    d->m_cache.insert(instance);
}

QObject* QOrmEntityInstanceCache::take(QObject* instance)
{
    d->m_cache.remove(instance);
    return instance;
}

QT_END_NAMESPACE
