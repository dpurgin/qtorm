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

    QElapsedTimer timer;
    timer.start();
    {
        auto token = session.declareTransaction(QOrm::TransactionPropagation::Require,
                                                QOrm::TransactionAction::Commit);

        for (int i = 0; i < 100; ++i)
        {
            session.merge(new Province{QString::fromUtf8("Burgenland")});
            session.merge(new Province{QString::fromUtf8("Kärnten")});
            session.merge(new Province{QString::fromUtf8("Salzburg")});
            session.merge(new Province{QString::fromUtf8("Steiermark")});
            session.merge(new Province{QString::fromUtf8("Tirol")});
            session.merge(new Province{QString::fromUtf8("Vorarlberg")});
            session.merge(new Province{QString::fromUtf8("Wien")});

            session.merge(new Community{QString::fromUtf8("Hagenberg"), nullptr});
        }
    }
    qDebug() << "Elapsed:" << timer.elapsed();

    auto provinces = session.from<Province>().select();
    qDebug() << "size:" << provinces.toVector<Province>().size();

    return 0;
}
