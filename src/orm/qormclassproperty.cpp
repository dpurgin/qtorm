#include "qormclassproperty.h"

QT_BEGIN_NAMESPACE

QOrmClassProperty::QOrmClassProperty(const char* descriptor)
    : m_descriptor{descriptor}
{
}

QString QOrmClassProperty::descriptor() const
{
    return m_descriptor;
}

QT_END_NAMESPACE
