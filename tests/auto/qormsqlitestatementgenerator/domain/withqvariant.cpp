/*
 * Copyright (C) 2019-2021 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2019-2021 sequality software engineering e.U. <office@sequality.at>
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

#include "withqvariant.h"

int WithQVariant::id() const
{
    return m_id;
}

void WithQVariant::setId(int id)
{
    if (m_id != id)
    {
        m_id = id;
        emit idChanged();
    }
}

QVariant WithQVariant::data() const
{
    return m_data;
}

void WithQVariant::setData(const QVariant& data)
{
    if (m_data != data)
    {
        m_data = data;
        emit dataChanged();
    }
}
