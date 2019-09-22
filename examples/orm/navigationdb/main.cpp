#include <QVector>

#include <QCoreApplication>
#include <QOrmSession>
#include <QOrmError>

#include "domain/province.h"

#include <QDebug>


#include <QOrmFilterExpression>


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

//    {
//        QOrmQueryResult result = session.from<Province>()
//                                        .select();

//        for (QObject* entityInstance: result.toVector())
//        {
//            qDebug() << *qobject_cast<Province*>(entityInstance);
//        }
//    }

    {
        QOrmQueryResult result = session.from<Province>()
                                        .filter(Q_ORM_CLASS_PROPERTY(id) > 3)
                                        .select();

        if (result.error().error() != QOrm::Error::None)
        {
            qCritical() << result.error().errorText();
        }
        else
        {
            for (QObject* entityInstance: result.toVector())
            {
                qDebug() << *qobject_cast<Province*>(entityInstance);
            }
        }

    }



    {
        QOrmQueryResult result = session.from<Province>()
                                        .filter(Q_ORM_CLASS_PROPERTY(id) == 0 ||
                                                (Q_ORM_CLASS_PROPERTY(id) > 3 && Q_ORM_CLASS_PROPERTY(id) < 5))
                                        .select();

        for (QObject* entityInstance: result.toVector())
        {
            qDebug() << *qobject_cast<Province*>(entityInstance);
        }
    }

    {
        QOrmQueryResult result = session.from<Province>()
                                        .filter(Q_ORM_CLASS_PROPERTY(id) > 3)
                                        .filter(Q_ORM_CLASS_PROPERTY(id) < 5)
                                        .select();

        for (QObject* entityInstance: result.toVector())
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
