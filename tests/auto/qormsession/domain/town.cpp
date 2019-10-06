#include "town.h"

Town::Town(QObject* parent)
    : QObject(parent)
{
}

int Town::id() const
{
    return m_id;
}

QString Town::name() const
{
    return m_name;
}

void Town::setId(int id)
{
    if (m_id == id)
        return;

    m_id = id;
    emit idChanged(m_id);
}

void Town::setName(QString name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(m_name);
}

Province* Town::province() const
{
    return m_province;
}

void Town::setProvince(Province* province)
{
    if (m_province == province)
        return;

    m_province = province;
    emit provinceChanged(m_province);
}
