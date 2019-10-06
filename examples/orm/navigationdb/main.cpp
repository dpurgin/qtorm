#include <QVector>

#include <QCoreApplication>
#include <QOrmSession>
#include <QOrmError>

#include "domain/community.h"
#include "domain/province.h"

#include <QDebug>


#include <QOrmFilterExpression>


int main(int argc, char* argv[])
{
    qRegisterOrmEntity<Province>();

    qDebug() << __PRETTY_FUNCTION__;
    QCoreApplication app{argc, argv};

    QOrmSession session;

    auto result = session.from<Province>()
                      .filter(Q_ORM_CLASS_PROPERTY(name) == "Niederösterreich" ||
                              Q_ORM_CLASS_PROPERTY(name) == "Tirol")
                      .select();

    auto lowerAustria = session.from<Province>()
                            .filter(Q_ORM_CLASS_PROPERTY(name) == "Niederösterreich")
                            .select()
                            .toVector()
                            .first();
    session.remove(lowerAustria);

    session.merge(new Province{QString::fromUtf8("Burgenland")});
    session.merge(new Province{QString::fromUtf8("Kärnten")});
    session.merge(new Province{QString::fromUtf8("Salzburg")});
    session.merge(new Province{QString::fromUtf8("Steiermark")});
    session.merge(new Province{QString::fromUtf8("Tirol")});
    session.merge(new Province{QString::fromUtf8("Vorarlberg")});
    session.merge(new Province{QString::fromUtf8("Wien")});

    session.merge(new Community{QString::fromUtf8("Hagenberg")});


    return 0;
}
