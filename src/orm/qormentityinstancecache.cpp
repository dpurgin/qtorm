/*
 * Copyright (C) 2019 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2019 sequality software engineering e.U. <office@sequality.at>
 *
 * This file is part of QtOrm library.
 *
 * QtOrm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QtOrm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with QtOrm.  If not, see <https://www.gnu.org/licenses/>.
 */

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
    QSet<const QObject*> m_modifiedInstances;    
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

bool QOrmEntityInstanceCache::contains(const QObject* instance) const
{
    return d->m_cache.contains(const_cast<QObject*>(instance));
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
}

QObject* QOrmEntityInstanceCache::take(QObject* instance)
{
    d->m_byObjectId.remove(d->m_cache[instance]);
    d->m_modifiedInstances.remove(instance);
    d->m_cache.remove(instance);

    return instance;
}

void QOrmEntityInstanceCache::finalize(const QOrmMetadata& metadata, QObject* instance)
{
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

bool QOrmEntityInstanceCache::isModified(const QObject* instance) const
{
    return d->m_modifiedInstances.contains(instance);
}

void QOrmEntityInstanceCache::markUnmodified(const QObject* instance) const
{
    d->m_modifiedInstances.remove(instance);
}

QT_END_NAMESPACE

#include "qormentityinstancecache.moc"
