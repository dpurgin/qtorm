#include "qormentitymetadata.h"

QT_BEGIN_NAMESPACE

class QOrmEntityMetadataPrivate : public QSharedData
{
    friend class QOrmEntityMetadata;

    QMetaObject m_qMetaObject;

    QString m_className;
    QString m_tableName;
    QVector<QOrmPropertyMapping> m_propertyMappings;
    std::optional<QOrmPropertyMapping> m_objectIdMapping;

    QHash<QString, int> m_tableFieldMappingIndex;
    QHash<QString, int> m_classPropertyMappingIndex;
};

QOrmEntityMetadata::QOrmEntityMetadata()
    : d{new QOrmEntityMetadataPrivate}
{
}

QOrmEntityMetadata::QOrmEntityMetadata(const QMetaObject& qMetaObject)
    : d{new QOrmEntityMetadataPrivate}
{
    d->m_qMetaObject = qMetaObject;
}

QOrmEntityMetadata::QOrmEntityMetadata(const QOrmEntityMetadata&) = default;

#ifdef Q_COMPILER_RVALUE_REFS
QOrmEntityMetadata::QOrmEntityMetadata(QOrmEntityMetadata&&) = default;
#endif

QOrmEntityMetadata::~QOrmEntityMetadata() = default;

QOrmEntityMetadata& QOrmEntityMetadata::operator=(const QOrmEntityMetadata&) = default;

#ifdef Q_COMPILER_RVALUE_REFS
QOrmEntityMetadata& QOrmEntityMetadata::operator=(QOrmEntityMetadata&&) = default;
#endif

const QMetaObject& QOrmEntityMetadata::qMetaObject() const
{
    return d->m_qMetaObject;
}

QString QOrmEntityMetadata::className() const
{
    return d->m_className;
}

void QOrmEntityMetadata::setClassName(const QString& className)
{
    d->m_className = className;
}

QString QOrmEntityMetadata::tableName() const
{
    return d->m_tableName;
}

void QOrmEntityMetadata::setTableName(const QString& tableName)
{
    d->m_tableName = tableName;
}

const QVector<QOrmPropertyMapping>& QOrmEntityMetadata::propertyMappings() const
{
    return d->m_propertyMappings;
}

std::optional<QOrmPropertyMapping> QOrmEntityMetadata::tableFieldMapping(const QString& fieldName) const
{
    auto it = d->m_tableFieldMappingIndex.find(fieldName);

    if (it == std::end(d->m_tableFieldMappingIndex))
        return {};

    return d->m_propertyMappings[it.value()];
}

std::optional<QOrmPropertyMapping> QOrmEntityMetadata::classPropertyMapping(const QString& classProperty) const
{
    auto it = d->m_classPropertyMappingIndex.find(classProperty);

    if (it == std::end(d->m_classPropertyMappingIndex))
        return {};

    return d->m_propertyMappings[it.value()];
}

void QOrmEntityMetadata::addPropertyMapping(const QOrmPropertyMapping& propertyMapping)
{
    d->m_propertyMappings.push_back(propertyMapping);
    d->m_tableFieldMappingIndex.insert(propertyMapping.tableFieldName(),
                                       d->m_propertyMappings.size());
    d->m_classPropertyMappingIndex.insert(propertyMapping.classPropertyName(),
                                          d->m_propertyMappings.size());
}

std::optional<QOrmPropertyMapping> QOrmEntityMetadata::objectIdMapping() const
{
    return d->m_objectIdMapping;
}

void QOrmEntityMetadata::setObjectIdMapping(const QOrmPropertyMapping& objectIdMapping)
{
    d->m_objectIdMapping = objectIdMapping;
}

QT_END_NAMESPACE
