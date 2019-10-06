#include "province.h"

Province::Province(QObject* parent)
    : QObject(parent)
{
}

int Province::id() const
{
    return m_id;
}

QString Province::name() const
{
    return m_name;
}

void Province::setId(int id)
{
    if (m_id == id)
        return;

    m_id = id;
    emit idChanged(m_id);
}

void Province::setName(QString name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(m_name);
}
