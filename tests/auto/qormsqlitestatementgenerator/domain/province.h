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

class Town;

class Province : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Province)

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QVector<Town*> towns READ towns WRITE setTowns NOTIFY townsChanged)

    int m_id;

    QString m_name;

    QVector<Town*> m_towns;

public:
    Q_INVOKABLE Province(QObject* parent = nullptr)
        : QObject(parent)
    {
    }
    explicit Province(const QString& name, QObject* parent = nullptr)
        : QObject{parent}
        , m_name{name}
    {
    }
    Province(int id, QString name, QObject* parent = nullptr)
        : QObject{parent}
        , m_id{id}
        , m_name{name}
    {
    }

    virtual ~Province() {}
    int id() const { return m_id; }
    QString name() const { return m_name; }

    QVector<Town*> towns() const { return m_towns; }

public slots:
    void setId(int id);
    void setName(QString name);
    void setTowns(QVector<Town*> towns);

signals:
    void idChanged(int id);
    void nameChanged(QString name);
    void townsChanged(QVector<Town*> towns);
};
