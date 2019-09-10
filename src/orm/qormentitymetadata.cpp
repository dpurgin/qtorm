#include "qormentitymetadata.h"

QT_BEGIN_NAMESPACE

QString QOrmEntityMetadata::className() const
{
    return m_className;
}

void QOrmEntityMetadata::setClassName(const QString& className)
{
    m_className = className;
}

QString QOrmEntityMetadata::tableName() const
{
    return m_tableName;
}

void QOrmEntityMetadata::setTableName(const QString& tableName)
{
    m_tableName = tableName;
}

const QVector<QOrmPropertyMapping>& QOrmEntityMetadata::propertyMappings() const
{
    return m_propertyMappings;
}

void QOrmEntityMetadata::addPropertyMapping(const QOrmPropertyMapping& propertyMapping)
{
    m_propertyMappings.push_back(propertyMapping);
}

void QOrmEntityMetadata::addPropertyMapping(QOrmPropertyMapping&& propertyMapping)
{
    m_propertyMappings.push_back(propertyMapping);
}

const QOrmPropertyMapping& QOrmEntityMetadata::objectIdPropertyMapping() const
{
    return m_objectIdPropertyMapping;
}

void QOrmEntityMetadata::setObjectIdPropertyMapping(const QOrmPropertyMapping& objectIdPropertyMapping)
{
    m_objectIdPropertyMapping = objectIdPropertyMapping;
}

QT_END_NAMESPACE
