#include "qormmetadata.h"
#include "qormglobal_p.h"
#include "qormmetadata_p.h"

#include <QDebug>
#include <QMetaProperty>

QT_BEGIN_NAMESPACE

QOrmMetadata::QOrmMetadata(const QOrmMetadataPrivate* data)
    : d{data}
{
}

QOrmMetadata::QOrmMetadata(const QOrmMetadata&) = default;

QOrmMetadata::QOrmMetadata(QOrmMetadata&&) = default;

QOrmMetadata::~QOrmMetadata() = default;

QOrmMetadata& QOrmMetadata::operator=(const QOrmMetadata&) = default;

QOrmMetadata& QOrmMetadata::operator=(QOrmMetadata&&) = default;

const QMetaObject& QOrmMetadata::qMetaObject() const
{
    return d->m_qMetaObject;
}

QString QOrmMetadata::className() const
{
    return d->m_className;
}

QString QOrmMetadata::tableName() const
{
    return d->m_tableName;
}

const std::vector<QOrmPropertyMapping>& QOrmMetadata::propertyMappings() const
{
    return d->m_propertyMappings;
}

const QOrmPropertyMapping* QOrmMetadata::tableFieldMapping(const QString& fieldName) const
{
    auto it = d->m_tableFieldMappingIndex.find(fieldName);

    if (it == std::end(d->m_tableFieldMappingIndex))
        return nullptr;

    return &d->m_propertyMappings[it.value()];
}

const QOrmPropertyMapping* QOrmMetadata::classPropertyMapping(const QString& classProperty) const
{
    auto it = d->m_classPropertyMappingIndex.find(classProperty);

    if (it == std::end(d->m_classPropertyMappingIndex))
        return nullptr;

    return &d->m_propertyMappings[it.value()];
}

const QOrmPropertyMapping* QOrmMetadata::objectIdMapping() const
{
    return d->m_objectIdPropertyMappingIdx == -1
               ? nullptr
               : &d->m_propertyMappings[d->m_objectIdPropertyMappingIdx];
}

QDebug operator<<(QDebug dbg, const QOrmMetadata& metadata)
{
    QDebugStateSaver saver{dbg};
    dbg.nospace().nospace() << "QOrmMetadata("
                            << metadata.className() << " => " << metadata.tableName()
                            << ")";
    return dbg;
}

QT_END_NAMESPACE
