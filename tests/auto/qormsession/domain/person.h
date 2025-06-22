/*
 * Copyright (C) 2021-2025 Dmitriy Purgin <dpurgin@gmail.com>
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

#pragma once

#include <QObject>
#include <QVector>

#include "person.h"
#include "town.h"

class Person : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString firstName READ firstName WRITE setFirstName NOTIFY firstNameChanged)
    Q_PROPERTY(QString lastName READ lastName WRITE setLastName NOTIFY lastNameChanged)
    Q_PROPERTY(Town* town READ town WRITE setTown NOTIFY townChanged)
    Q_PROPERTY(
        Person* personParent READ personParent WRITE setPersonParent NOTIFY personParentChanged)
    Q_PROPERTY(QVector<Person*> personChildren READ personChildren WRITE setPersonChildren NOTIFY
                   personChildrenChanged)

    int m_id;
    QString m_firstName;
    QString m_lastName;
    Town* m_town{nullptr};
    Person* m_personParent{nullptr};
    QVector<Person*> m_personChildren;

public:
    Q_INVOKABLE Person() = default;
    Person(QString firstName, QString lastName, Town* town)
        : m_firstName{firstName}
        , m_lastName{lastName}
        , m_town{town}
    {
    }
    explicit Person(QString name);

    int id() const { return m_id; }
    void setId(int id);

    QString firstName() const { return m_firstName; }
    void setFirstName(QString firstName);

    Town* town() const { return m_town; }
    void setTown(Town* town);

    QString lastName() const;
    void setLastName(QString lastName);

    Person* personParent() const;
    void setPersonParent(Person* personParent);

    const QVector<Person*> personChildren() const;
    void setPersonChildren(const QVector<Person*> personChildren);

signals:
    void idChanged(int id);
    void firstNameChanged(QString firstName);
    void lastNameChanged(QString lastName);
    void townChanged(Town* town);
    void personParentChanged(Person* personParent);
    void personChildrenChanged(const QVector<Person*>& personChildren);
};
