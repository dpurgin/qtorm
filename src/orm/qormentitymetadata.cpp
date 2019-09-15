#include "qormentitymetadata.h"

QT_BEGIN_NAMESPACE

QOrmEntityMetadata::QOrmEntityMetadata() = default;

QOrmEntityMetadata::QOrmEntityMetadata(const QMetaObject& qMetaObject)
    : m_qMetaObject{qMetaObject}
{
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
    return m_qMetaObject;
}

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
