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

    Province* province = new Province();
    province->setName(QString::fromUtf8("Oberösterreich"));

    session.merge(province, QOrm::MergeMode::Create);

    province->setName(QString::fromUtf8("Oberoesterreich"));
    session.merge(province, QOrm::MergeMode::Update);

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
