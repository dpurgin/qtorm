#ifndef QORMGLOBAL_P_H
#define QORMGLOBAL_P_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormmetadata.h>

#include <QtCore/qhashfunctions.h>
#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

namespace std
{
    template<>
    struct hash<QString>
    {
        size_t operator()(const QString& s) const { return qHash(s); }
    };
} // namespace std

namespace QOrmPrivate
{
    Q_REQUIRED_RESULT
    inline QVariant propertyValue(const QObject* object, QString property)
    {
        return object->property(property.toUtf8().data());
    }

    inline void setPropertyValue(QObject* object, const QString& property, const QVariant& value)
    {
        object->setProperty(property.toUtf8().data(), value);
    }

    Q_REQUIRED_RESULT
    QVariant objectIdPropertyValue(const QObject* entityInstance, const QOrmMetadata& meta)
    {
        Q_ASSERT(meta.objectIdMapping() != nullptr);
        return propertyValue(entityInstance, meta.objectIdMapping()->classPropertyName());
    }
} // namespace QOrmPrivate

QT_END_NAMESPACE

#endif
