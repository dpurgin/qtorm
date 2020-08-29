/*
 * Copyright (C) 2019 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2019 sequality software engineering e.U. <office@sequality.at>
 * All rights reserved.
 *
 * This file is part of the examples of the QtOrm library
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the sequality software engineering e.U. nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "community.h"

Community::Community(QObject* parent)
    : QObject{parent}
{
}

Community::Community(QString name,
                     Province* province,
                     QString postCode,
                     int population,
                     qreal latitude,
                     qreal longitude,
                     QObject* parent)
    : QObject{parent}
    , m_name{std::move(name)}
    , m_province{province}
    , m_postCode{std::move(postCode)}
    , m_population{population}
    , m_latitude{latitude}
    , m_longitude{longitude}
{
}

int Community::id() const
{
    return m_id;
}

void Community::setId(int id)
{
    if (m_id == id)
        return;

    m_id = id;
    emit idChanged();
}

QString Community::name() const
{
    return m_name;
}

void Community::setName(QString name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged();
}

Province* Community::province() const
{
    return m_province;
}

void Community::setProvince(Province* province)
{
    if (m_province == province)
        return;

    m_province = province;
    emit provinceChanged();
}

QString Community::postCode() const
{
    return m_postCode;
}

void Community::setPostCode(QString postCode)
{
    if (m_postCode == postCode)
        return;

    m_postCode = postCode;
    emit postCodeChanged();
}

int Community::population() const
{
    return m_population;
}

void Community::setPopulation(int population)
{
    if (m_population == population)
        return;

    m_population = population;
    emit populationChanged();
}

qreal Community::longitude() const
{
    return m_longitude;
}

void Community::setLongitude(qreal longitude)
{
    if (qFuzzyCompare(m_longitude, longitude))
        return;

    m_longitude = longitude;
    emit longitudeChanged();
}

qreal Community::latitude() const
{
    return m_latitude;
}

void Community::setLatitude(qreal latitude)
{
    if (qFuzzyCompare(m_latitude, latitude))
        return;

    m_latitude = latitude;
    emit latitudeChanged();
}
