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

#pragma once

#include <QObject>
#include <QVariant>

class WithQVariant : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QVariant data READ data WRITE setData NOTIFY dataChanged)

public:
    Q_INVOKABLE WithQVariant(QObject* parent = nullptr)
        : QObject{parent}
    {
    }

    int id() const;
    void setId(int id);

    QVariant data() const;
    void setData(const QVariant& data);

signals:
    void idChanged();
    void dataChanged();

private:
    int m_id{0};
    QVariant m_data;
};
