/*
 * Copyright (C) 2026 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2026 sequality software engineering e.U. <office@sequality.at>
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

#pragma once

#include <QObject>

#include <QtOrm/qormglobal.h>

// An entity with a property whose column name is an SQL keyword
class WithKeyword : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int order READ order WRITE setOrder NOTIFY orderChanged)

public:
    Q_INVOKABLE WithKeyword(QObject* parent = nullptr);

    [[nodiscard]] int id() const { return m_id; }
    void setId(int id)
    {
        if (m_id != id)
        {
            m_id = id;
            emit idChanged();
        }
    }

    [[nodiscard]] int order() const { return m_order; }
    void setOrder(int order)
    {
        if (m_order != order)
        {
            m_order = order;
            emit orderChanged();
        }
    }

signals:
    void idChanged();
    void orderChanged();

private:
    int m_id{0};
    int m_order{0};
};
