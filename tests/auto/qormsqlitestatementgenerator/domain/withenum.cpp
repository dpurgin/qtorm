/*
* Copyright (C) 2019-2024 Dmitriy Purgin <dmitriy.purgin@sequality.at>
* Copyright (C) 2019-2024 sequality software engineering e.U. <office@sequality.at>
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

#include "withenum.h"

namespace MyNamespace
{
    void WithNamespace::setId(int id)
    {
        if (m_id != id)
        {
            m_id = id;
            emit idChanged();
        }
    }

    void WithNamespace::setValue(QString value)
    {
        if (m_value != value)
        {
            m_value = value;
            emit valueChanged();
        }
    }

} // namespace MyNamespace

int WithEnum::id() const
{
    return m_id;
}

void WithEnum::setId(int id)
{
    if (m_id != id)
    {
        m_id = id;
        emit idChanged();
    }
}

MyNamespace::MyEnum WithEnum::myEnum() const
{
    return m_myEnum;
}

void WithEnum::setMyEnum(MyNamespace::MyEnum myEnum)
{
    if (m_myEnum != myEnum)
    {
        m_myEnum = myEnum;
        emit myEnumChanged();
    }
}

MyNamespace::MyEnumClass WithEnum::myEnumClass() const
{
    return m_myEnumClass;
}

void WithEnum::setMyEnumClass(MyNamespace::MyEnumClass myEnumClass)
{
    if (m_myEnumClass != myEnumClass)
    {
        m_myEnumClass = myEnumClass;
        emit myEnumClassChanged();
    }
}
