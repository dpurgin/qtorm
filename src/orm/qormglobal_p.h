#ifndef QORMGLOBAL_P_H
#define QORMGLOBAL_P_H

#include <QtOrm/qormglobal.h>

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

namespace QtOrmPrivate
{
    inline QVariant propertyValue(QObject* object, QString property)
    {
        return object->property(property.toUtf8().data());
    }
} // namespace QtOrmPrivate

QT_END_NAMESPACE

#endif
