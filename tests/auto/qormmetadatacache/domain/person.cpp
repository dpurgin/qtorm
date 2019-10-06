#include "person.h"

void Person::setId(int id)
{
    if (m_id == id)
        return;

    m_id = id;
    emit idChanged(m_id);
}

void Person::setName(QString name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(m_name);
}

void Person::setTown(Town* town)
{
    if (m_town == town)
        return;

    m_town = town;
    emit townChanged(m_town);
}
