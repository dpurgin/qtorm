/*
 * Copyright (C) 2019 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2019 sequality software engineering e.U. <office@sequality.at>
 *
 * This file is part of QtOrm library.
 *
 * QtOrm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QtOrm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with QtOrm.  If not, see <https://www.gnu.org/licenses/>.
 */

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
