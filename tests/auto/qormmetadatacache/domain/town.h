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

#pragma once

#include <QObject>
#include <QVector>

class Person;

class Town : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(
        QVector<Person*> population READ population WRITE setPopulation NOTIFY populationChanged)

    int m_id;
    QString m_name;
    QVector<Person*> m_population;

public:
    Q_INVOKABLE explicit Town(QObject* parent = nullptr);

    virtual ~Town() {}

    int id() const { return m_id; }
    void setId(int id);

    QString name() const { return m_name; }
    void setName(QString name);

    QVector<Person*> population() const { return m_population; }
    void setPopulation(QVector<Person*> population);

signals:
    void idChanged(int id);
    void nameChanged(QString name);
    void populationChanged(QVector<Person*> population);
};
