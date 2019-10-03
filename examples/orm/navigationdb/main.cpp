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

    auto nested = session.from<Province>().build(QOrm::Operation::Read);

    QOrmQueryResult result =
        session.from(nested).filter(Q_ORM_CLASS_PROPERTY(id) > 3).select<Province>();
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

        if (result.error().type() != QOrm::ErrorType::None)
        {
            qCritical() << result.error().text();
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

    {
        QOrmQueryResult result = session.from<Province>()
                                        .filter(Q_ORM_CLASS_PROPERTY(id) > 3)
                                        .remove();
    }

    return 0;
}
