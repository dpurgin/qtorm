#ifndef QORMCLASSPROPERTY_H
#define QORMCLASSPROPERTY_H

#include <QtCore/qstring.h>

#include <QtOrm/qormglobal.h>

QT_BEGIN_NAMESPACE

class Q_ORM_EXPORT QOrmClassProperty
{
public:
    explicit QOrmClassProperty(const char* descriptor);

    QString descriptor() const;

private:
    QString m_descriptor;
};

extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, const QOrmClassProperty& classProperty);

#define Q_ORM_CLASS_PROPERTY(descriptor) (QOrmClassProperty{#descriptor})

QT_END_NAMESPACE

#endif // QORMCLASSPROPERTY_H
