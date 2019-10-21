#include "community.h"

Community::Community(QObject *parent) : QObject(parent)
{
}

Community::Community(QString name, Province* province, QObject* parent)
    : QObject{parent}
    , m_name{std::move(name)}
    , m_province{province}
{
}

void Community::setId(int id)
{
    if (m_id == id)
        return;

    m_id = id;
    emit idChanged();
}

void Community::setName(QString name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged();
}

void Community::setProvince(Province* province)
{
    if (m_province == province)
        return;

    m_province = province;
    emit provinceChanged();
}
