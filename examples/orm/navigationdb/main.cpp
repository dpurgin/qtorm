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

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QOrmError>
#include <QOrmSession>

#include "domain/community.h"
#include "domain/province.h"

#include <QDebug>


#include <QOrmFilterExpression>


int main(int argc, char* argv[])
{
    qRegisterOrmEntity<Province, Community>();

    qDebug() << __PRETTY_FUNCTION__;
    QCoreApplication app{argc, argv};

    QOrmSession session;

    Province* upperAustria = nullptr;

    QElapsedTimer timer;
    timer.start();
    {
        auto token = session.declareTransaction(QOrm::TransactionPropagation::Require,
                                                QOrm::TransactionAction::Commit);

        session.merge(new Province{QString::fromUtf8("Burgenland")});
        session.merge(new Province{QString::fromUtf8("Kärnten")});
        session.merge(new Province{QString::fromUtf8("Niederösterreich")});
        session.merge(new Province{QString::fromUtf8("Oberösterreich")});
        session.merge(new Province{QString::fromUtf8("Salzburg")});
        session.merge(new Province{QString::fromUtf8("Steiermark")});
        session.merge(new Province{QString::fromUtf8("Tirol")});
        session.merge(new Province{QString::fromUtf8("Vorarlberg")});
        session.merge(new Province{QString::fromUtf8("Wien")});

        upperAustria =
            session.from<Province>()
                .filter(Q_ORM_CLASS_PROPERTY(name) == QString::fromUtf8("Oberösterreich"))
                .select()
                .toVector()
                .first();

        auto communities = {new Community{QString::fromUtf8("Freistadt"), upperAustria},
                            new Community{QString::fromUtf8("Hagenberg im Mühlkreis"),
                                          upperAustria},
                            new Community{QString::fromUtf8("Kefermarkt"), upperAustria},
                            new Community{QString::fromUtf8("Neumarkt im Mühlkreis"), upperAustria},
                            new Community{QString::fromUtf8("Pregarten"), upperAustria}};

        upperAustria->setCommunityList(communities);

        session.merge(upperAustria, communities);
    }
    qDebug() << "Elapsed:" << timer.elapsed();

    auto upperAustrianCommunities = session.from<Community>()
                                        .filter(Q_ORM_CLASS_PROPERTY(province) == upperAustria)
                                        .select()
                                        .toVector();

    qDebug() << "size:" << upperAustrianCommunities.size();

    return 0;
}
