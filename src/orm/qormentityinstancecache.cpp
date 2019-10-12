#include "qormentityinstancecache.h"
#include "qormmetadata.h"

#include <QMetaProperty>
#include <QSet>

QT_BEGIN_NAMESPACE

class QOrmEntityInstanceCachePrivate : public QObject
{
    Q_OBJECT

    friend class QOrmEntityInstanceCache;

private slots:
    void onEntityInstanceChanged();

private:
    QSet<QObject*> m_persistedInstances;
    QSet<QObject*> m_modifiedInstances;
};

void QOrmEntityInstanceCachePrivate::onEntityInstanceChanged()
{
    Q_ASSERT(m_persistedInstances.contains(sender()));
    m_modifiedInstances.insert(sender());
}

QOrmEntityInstanceCache::QOrmEntityInstanceCache()
    : d{new QOrmEntityInstanceCachePrivate}
{
}

QOrmEntityInstanceCache::~QOrmEntityInstanceCache()
{
    for (const QObject* instance : d->m_persistedInstances)
        instance->disconnect(d.get());

    qDeleteAll(d->m_persistedInstances);
}

bool QOrmEntityInstanceCache::contains(QObject* instance) const
{
    return d->m_persistedInstances.contains(instance);
}

void QOrmEntityInstanceCache::insert(const QOrmMetadata& metadata, QObject* instance)
{
    Q_UNUSED(metadata)
    d->m_persistedInstances.insert(instance);

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
    d->m_persistedInstances.remove(instance);
    d->m_modifiedInstances.remove(instance);
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
