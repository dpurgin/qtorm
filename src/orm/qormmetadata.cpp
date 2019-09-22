#include "qormmetadata.h"

QT_BEGIN_NAMESPACE

class QOrmMetadataPrivate : public QSharedData
{
    friend class QOrmMetadata;

    QMetaObject m_qMetaObject;

    QString m_className;
    QString m_tableName;
    QVector<QOrmPropertyMapping> m_propertyMappings;
    std::optional<QOrmPropertyMapping> m_objectIdMapping;

    QHash<QString, int> m_tableFieldMappingIndex;
    QHash<QString, int> m_classPropertyMappingIndex;
};

QOrmMetadata::QOrmMetadata()
    : d{new QOrmMetadataPrivate}
{
}

QOrmMetadata::QOrmMetadata(const QMetaObject& qMetaObject)
    : d{new QOrmMetadataPrivate}
{
    d->m_qMetaObject = qMetaObject;
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

void QOrmMetadata::setClassName(const QString& className)
{
    d->m_className = className;
}

QString QOrmMetadata::tableName() const
{
    return d->m_tableName;
}

void QOrmMetadata::setTableName(const QString& tableName)
{
    d->m_tableName = tableName;
}

const QVector<QOrmPropertyMapping>& QOrmMetadata::propertyMappings() const
{
    return d->m_propertyMappings;
}

std::optional<QOrmPropertyMapping> QOrmMetadata::tableFieldMapping(const QString& fieldName) const
{
    auto it = d->m_tableFieldMappingIndex.find(fieldName);

    if (it == std::end(d->m_tableFieldMappingIndex))
        return {};

    return d->m_propertyMappings[it.value()];
}

std::optional<QOrmPropertyMapping> QOrmMetadata::classPropertyMapping(const QString& classProperty) const
{
    auto it = d->m_classPropertyMappingIndex.find(classProperty);

    if (it == std::end(d->m_classPropertyMappingIndex))
        return {};

    return d->m_propertyMappings[it.value()];
}

void QOrmMetadata::addPropertyMapping(const QOrmPropertyMapping& propertyMapping)
{
    d->m_propertyMappings.push_back(propertyMapping);
    d->m_tableFieldMappingIndex.insert(propertyMapping.tableFieldName(),
                                       d->m_propertyMappings.size());
    d->m_classPropertyMappingIndex.insert(propertyMapping.classPropertyName(),
                                          d->m_propertyMappings.size());
}

std::optional<QOrmPropertyMapping> QOrmMetadata::objectIdMapping() const
{
    return d->m_objectIdMapping;
}

void QOrmMetadata::setObjectIdMapping(const QOrmPropertyMapping& objectIdMapping)
{
    d->m_objectIdMapping = objectIdMapping;
}

QT_END_NAMESPACE
