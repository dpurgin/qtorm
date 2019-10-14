#include "person.h"

void Person::setId(int id)
{
    if (m_id == id)
        return;

    m_id = id;
    emit idChanged(m_id);
}

void Person::setFirstName(QString firstName)
{
    if (m_firstName == firstName)
        return;

    m_firstName = firstName;
    emit firstNameChanged(m_firstName);
}

void Person::setTown(Town* town)
{
    if (m_town == town)
        return;

    m_town = town;
    emit townChanged(m_town);
}

QString Person::lastName() const
{
    return m_lastName;
}

void Person::setLastName(QString lastName)
{
    if (m_lastName == lastName)
        return;

    m_lastName = lastName;
    emit lastNameChanged(m_lastName);
}
