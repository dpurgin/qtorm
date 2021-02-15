/*
 * Copyright (C) 2019-2021 Dmitriy Purgin <dpurgin@gmail.com>
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

class Province;

class Community : public QObject
{
    Q_OBJECT

    Q_PROPERTY(long communityId READ communityId WRITE setCommunityId NOTIFY communityIdChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int population READ population WRITE setPopulation NOTIFY populationChanged)
    Q_PROPERTY(Province* province READ province WRITE setProvince NOTIFY provinceChanged)
    Q_PROPERTY(bool hasLargePopulation READ hasLargePopulation STORED false)
    Q_PROPERTY(bool hasSmallPopulation READ hasSmallPopulation)

    Q_ORM_CLASS(TABLE communities)
    Q_ORM_PROPERTY(communityId COLUMN community_id IDENTITY)
    Q_ORM_PROPERTY(hasSmallPopulation TRANSIENT)

public:
    Q_INVOKABLE Community(QObject* parent = nullptr);

    long communityId() const { return m_communityId; }
    void setCommunityId(long communityId)
    {
        if (m_communityId != communityId)
        {
            m_communityId = communityId;
            emit communityIdChanged();
        }
    }

    QString name() const { return m_name; }
    void setName(QString name)
    {
        if (m_name != name)
        {
            m_name = name;
            emit nameChanged();
        }
    }

    int population() const { return m_population; }
    void setPopulation(int population)
    {
        if (m_population != population)
        {
            m_population = population;
            emit populationChanged();
        }
    }

    Province* province() const { return m_province; }
    void setProvince(Province* province)
    {
        if (m_province != province)
        {
            m_province = province;
            emit provinceChanged();
        }
    }

    bool hasLargePopulation() const { return m_population > 5000; }
    bool hasSmallPopulation() const { return !hasLargePopulation(); }

signals:
    void communityIdChanged();
    void nameChanged();
    void populationChanged();
    void provinceChanged();

private:
    long m_communityId{0};
    QString m_name;
    int m_population{0};
    Province* m_province{nullptr};
};
