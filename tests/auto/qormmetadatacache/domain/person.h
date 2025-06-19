/*
 * Copyright (C) 2019-2025 Dmitriy Purgin <dmitriy.purgin@sequality.at>
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

#pragma once

#include <QObject>

#include "town.h"

class Person : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(Town* town READ town WRITE setTown NOTIFY townChanged)

    int m_id;
    QString m_name;
    Town* m_town{nullptr};

public:
    Q_INVOKABLE Person() = default;
    explicit Person(QString name);

    int id() const { return m_id; }
    void setId(int id);

    QString name() const { return m_name; }
    void setName(QString name);

    Town* town() const { return m_town; }
    void setTown(Town* town);

signals:
    void idChanged(int id);
    void nameChanged(QString name);
    void townChanged(Town* town);
};
