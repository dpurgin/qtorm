/*
 * Copyright (C) 2020 Dmitriy Purgin <dpurgin@gmail.com>
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

class Person : public QObject
{
    Q_OBJECT

    Q_PROPERTY(long id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    long m_id;
    QString m_name;

public:
    Q_INVOKABLE Person(QObject* parent = nullptr): QObject{parent}
    {}

    long id() const { return m_id; }
    void setId(long id);

    QString name() const { return m_name; }
    void setName(const QString& name);

signals:
    void idChanged();
    void nameChanged();
};

