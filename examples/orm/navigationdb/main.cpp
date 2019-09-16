#include <QVector>

#include <QCoreApplication>
#include <QOrmSession>
#include <QOrmQuery>
#include <QOrmWhereClause>
#include <QOrmOrderClause>
#include <QOrmField>
#include <QOrmTransactionToken>
#include <QVariant>

#include "domain/province.h"

#include <QDebug>





int main(int argc, char* argv[])
{
    qDebug() << __PRETTY_FUNCTION__;
    QCoreApplication app{argc, argv};

    QOrmSession session;


    session.merge(new Province{QString::fromUtf8("Burgenland")});
    session.merge(new Province{QString::fromUtf8("Kärnten")});
    session.merge(new Province{QString::fromUtf8("Niederösterreich")});
    session.merge(new Province{QString::fromUtf8("Oberösterreich")});
    session.merge(new Province{QString::fromUtf8("Salzburg")});
    session.merge(new Province{QString::fromUtf8("Steiermark")});
    session.merge(new Province{QString::fromUtf8("Tirol")});
    session.merge(new Province{QString::fromUtf8("Vorarlberg")});
    session.merge(new Province{QString::fromUtf8("Wien")});

    {
        auto result = session.from<Province>()
                             .select<Province>()
                             .toVector();

        for (QObject* entityInstance: result)
        {
            qDebug() << *qobject_cast<Province*>(entityInstance);
        }
    }

    {
        auto result = session.from<Province>()
                             .where(Q_ORM_FIELD(id) < 5)
                             .select<Province>()
                             .toVector();

        for (QObject* entityInstance: result)
        {
            qDebug() << *qobject_cast<Province*>(entityInstance);
        }
    }

//    session.declareTransaction(QOrm::TransactionMode::Supports);

//    auto d1 = session.select<Province>().toVector();

//    auto d2 = session.select<Province>()
//                     .where(Q_ORM_FIELD(Province::name) == "Oberösterreich")
//                     .toVector();

//    auto d3 = session.select<Province>()
//                     .first(3)
//                     .where(Q_ORM_FIELD(id) < 5)
//                     .order(QOrmOrderClause{})
//                     .toVector();

//    auto d4 = session.select<Province>()
//                     .first(3)
//                     .where(Q_ORM_FIELD(id) > 5)
//                     .toVector();

    return 0;
}
