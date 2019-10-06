#include "province.h"

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

void Province::setTowns(QVector<Town*> towns)
{
    if (m_towns == towns)
        return;

    m_towns = towns;
    emit townsChanged(m_towns);
}
