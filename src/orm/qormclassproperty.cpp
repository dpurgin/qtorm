#include "qormclassproperty.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

QDebug operator<<(QDebug dbg, const QOrmClassProperty& classProperty)
{
    QDebugStateSaver saver{dbg};

    dbg.noquote().nospace() << "QOrmClassProperty(\"" << classProperty.descriptor() << "\")";

    return dbg;
}

QOrmClassProperty::QOrmClassProperty(const char* descriptor)
    : m_descriptor{descriptor}
{
}

QString QOrmClassProperty::descriptor() const
{
    return m_descriptor;
}

QT_END_NAMESPACE
