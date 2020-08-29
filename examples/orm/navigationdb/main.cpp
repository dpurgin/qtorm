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

#include <QVector>

#include <QElapsedTimer>
#include <QGuiApplication>
#include <QOrmEntityListModel>
#include <QOrmError>
#include <QOrmSession>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "domain/community.h"
#include "domain/province.h"

#include <QDebug>
#include <QOrmFilterExpression>


int main(int argc, char* argv[])
{
    qRegisterOrmEntity<Province, Community>();

    QGuiApplication app{argc, argv};

    QOrmSession session;

    Province* lowerAustria = nullptr;
    Province* upperAustria = nullptr;

    QElapsedTimer timer;
    timer.start();
    {
        auto token = session.declareTransaction(QOrm::TransactionPropagation::Require,
                                                QOrm::TransactionAction::Commit);

        session.merge(new Province{QString::fromUtf8("Burgenland")});
        session.merge(new Province{QString::fromUtf8("Kärnten")});
        session.merge(lowerAustria = new Province{QString::fromUtf8("Niederösterreich")});
        session.merge(upperAustria = new Province{QString::fromUtf8("Oberösterreich")});
        session.merge(new Province{QString::fromUtf8("Salzburg")});
        session.merge(new Province{QString::fromUtf8("Steiermark")});
        session.merge(new Province{QString::fromUtf8("Tirol")});
        session.merge(new Province{QString::fromUtf8("Vorarlberg")});
        session.merge(new Province{QString::fromUtf8("Wien")});

        auto upperAustrianCommunities = {
            new Community{
                QString::fromUtf8("Freistadt"), upperAustria, "4240", 7981, 48.501961, 14.502536},
            new Community{QString::fromUtf8("Hagenberg im Mühlkreis"),
                          upperAustria,
                          "4232",
                          2764,
                          48.366733,
                          14.516947},
            new Community{
                QString::fromUtf8("Kefermarkt"), upperAustria, "4292", 2136, 48.441826, 14.539221},
            new Community{QString::fromUtf8("Neumarkt im Mühlkreis"),
                          upperAustria,
                          "4212",
                          3173,
                          48.427728,
                          14.483987},
            new Community{
                QString::fromUtf8("Pregarten"), upperAustria, "4230", 5544, 48.349205, 14.527262}};

        upperAustria->setCommunityList(upperAustrianCommunities);

        auto lowerAustrianCommunities = {
            new Community{"Gmünd", lowerAustria, "3950", 2855, 48.771633, 14.985019},
            new Community{"Weitra", lowerAustria, "3970", 4092, 48.701348, 14.897371}};
        lowerAustria->setCommunityList(lowerAustrianCommunities);

        session.merge(upperAustria,
                      lowerAustria,
                      upperAustrianCommunities,
                      lowerAustrianCommunities);
    }
    qDebug() << "Elapsed:" << timer.elapsed();

    auto upperAustrianCommunities = session.from<Community>()
                                        .filter(Q_ORM_CLASS_PROPERTY(province) == upperAustria)
                                        .select()
                                        .toVector();

    qDebug() << "size:" << upperAustrianCommunities.size();

    QOrmEntityListModel<Community> communityListModel{session};
    QOrmEntityListModel<Province> provinceListModel{session};

    qmlRegisterType<Province>("QtOrm.NavigationDb.Domain", 1, 0, "Province");
    qmlRegisterType<Community>("QtOrm.NavigationDb.Domain", 1, 0, "Community");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("gCommunityListModel", &communityListModel);
    engine.rootContext()->setContextProperty("gProvinceListModel", &provinceListModel);

    engine.load(QUrl{"qrc:///main.qml"});

    return app.exec();
}
