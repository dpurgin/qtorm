#include "qormentityinstancecache.h"
#include "qormglobal_p.h"
#include "qormmetadata.h"

#include <QMap>
#include <QMetaProperty>
#include <QSet>
#include <QVariant>

QT_BEGIN_NAMESPACE

class QOrmEntityInstanceCachePrivate : public QObject
{    
    Q_OBJECT        

    friend class QOrmEntityInstanceCache;
    using ObjectId = QPair<QString, QVariant>;

private slots:
    void onEntityInstanceChanged();

private:
    QHash<QObject*, ObjectId> m_cache;
    QMap<ObjectId, QObject*> m_byObjectId;
    QSet<QObject*> m_modifiedInstances;
};

void QOrmEntityInstanceCachePrivate::onEntityInstanceChanged()
{
    Q_ASSERT(m_cache.contains(sender()));
    m_modifiedInstances.insert(sender());
}

QOrmEntityInstanceCache::QOrmEntityInstanceCache()
    : d{new QOrmEntityInstanceCachePrivate}
{
}

QOrmEntityInstanceCache::~QOrmEntityInstanceCache()
{
    for (auto it = std::begin(d->m_cache); it != std::end(d->m_cache); ++it)
    {
        it.key()->disconnect(d.get());
        delete it.key();
    }

    d->m_cache.clear();
}

QObject* QOrmEntityInstanceCache::get(const QOrmMetadata& meta, const QVariant& objectId)
{
    return d->m_byObjectId.value(qMakePair(meta.className(), objectId), nullptr);
}

bool QOrmEntityInstanceCache::contains(QObject* instance) const
{
    return d->m_cache.contains(instance);
}

void QOrmEntityInstanceCache::insert(const QOrmMetadata& metadata, QObject* instance)
{
    Q_ASSERT(metadata.objectIdMapping() != nullptr);
    Q_ASSERT(instance != nullptr);

    if (d->m_cache.contains(instance))
        return;

    auto objectId =
        qMakePair(metadata.className(), QOrmPrivate::objectIdPropertyValue(instance, metadata));

    d->m_cache.insert(instance, objectId);
    d->m_byObjectId.insert(objectId, instance);

    for (const QOrmPropertyMapping& mapping : metadata.propertyMappings())
    {
        if (mapping.isTransient())
            continue;

        // connect to NOTIFY signals of the entity to mark the instance dirty on any change
        QMetaMethod notifySignal = mapping.qMetaProperty().notifySignal();
        int slotIndex = QOrmEntityInstanceCachePrivate::staticMetaObject.indexOfSlot(
            "onEntityInstanceChanged()");
        QMetaMethod slot = QOrmEntityInstanceCachePrivate::staticMetaObject.method(slotIndex);

        QObject::connect(instance, notifySignal, d.get(), slot);
    }
}

QObject* QOrmEntityInstanceCache::take(QObject* instance)
{
    d->m_byObjectId.remove(d->m_cache[instance]);
    d->m_modifiedInstances.remove(instance);
    d->m_cache.remove(instance);

    return instance;
}

bool QOrmEntityInstanceCache::isModified(QObject* instance) const
{
    return d->m_modifiedInstances.contains(instance);
}

void QOrmEntityInstanceCache::markUnmodified(QObject* instance) const
{
    d->m_modifiedInstances.remove(instance);
}

QT_END_NAMESPACE

#include "qormentityinstancecache.moc"
