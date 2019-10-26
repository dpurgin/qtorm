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
