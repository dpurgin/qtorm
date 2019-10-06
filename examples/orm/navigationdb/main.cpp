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
    qRegisterMetaType<Town*>();

    qDebug() << __PRETTY_FUNCTION__;
    QCoreApplication app{argc, argv};

    QOrmSession session;

    //    session.merge(new Province{QString::fromUtf8("Burgenland")});
    //    session.merge(new Province{QString::fromUtf8("Kärnten")});
    //    session.merge(new Province{QString::fromUtf8("Niederösterreich")});
    //    session.merge(new Province{QString::fromUtf8("Oberösterreich")});
    //    session.merge(new Province{QString::fromUtf8("Salzburg")});
    //    session.merge(new Province{QString::fromUtf8("Steiermark")});
    //    session.merge(new Province{QString::fromUtf8("Tirol")});
    //    session.merge(new Province{QString::fromUtf8("Vorarlberg")});
    //    session.merge(new Province{QString::fromUtf8("Wien")});

    session.merge(new Community{QString::fromUtf8("Hagenberg")});

    auto result = session.from<Community>().select();

    return 0;
}
