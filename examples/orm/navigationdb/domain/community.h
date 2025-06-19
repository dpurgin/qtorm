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

#ifndef COMMUNITY_H
#define COMMUNITY_H

#include <QObject>

#include "province.h"

class Community : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(Province* province READ province WRITE setProvince NOTIFY provinceChanged)
    Q_PROPERTY(QString postCode READ postCode WRITE setPostCode NOTIFY postCodeChanged)
    Q_PROPERTY(int population READ population WRITE setPopulation NOTIFY populationChanged)
    Q_PROPERTY(qreal latitude READ latitude WRITE setLatitude NOTIFY latitudeChanged)
    Q_PROPERTY(qreal longitude READ longitude WRITE setLongitude NOTIFY longitudeChanged)

    int m_id;
    QString m_name;
    Province* m_province{nullptr};
    QString m_postCode;
    int m_population{0};
    qreal m_latitude{0};
    qreal m_longitude{0};

public:
    Q_INVOKABLE explicit Community(QObject* parent = nullptr);
    explicit Community(QString name,
                       Province* province,
                       QString postCode,
                       int population,
                       qreal latitude,
                       qreal longitude,
                       QObject* parent = nullptr);

    int id() const;
    void setId(int id);

    QString name() const;
    void setName(QString name);

    Province* province() const;
    void setProvince(Province* province);

    QString postCode() const;
    void setPostCode(QString postCode);

    int population() const;
    void setPopulation(int population);

    qreal latitude() const;
    void setLatitude(qreal latitude);

    qreal longitude() const;
    void setLongitude(qreal longitude);

public slots:

signals:
    void idChanged();
    void nameChanged();
    void provinceChanged();
    void postCodeChanged();
    void populationChanged();
    void latitudeChanged();
    void longitudeChanged();    
};

#endif // COMMUNITY_H
