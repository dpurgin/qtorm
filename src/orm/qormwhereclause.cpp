#include "qormwhereclause.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

QOrmWhereClause::QOrmWhereClause()
{

}

QOrmWhereClause operator==(const QOrmField& field, const QVariant& value)
{
    qDebug() << "Custom operator==...";
    return QOrmWhereClause{};
}

QOrmWhereClause operator!=(const QOrmField& field, const QVariant& value)
{
    return QOrmWhereClause{};
}

QOrmWhereClause operator<(const QOrmField& field, const QVariant& value)
{
    qDebug() << "Custom operator<...";
    return QOrmWhereClause{};
}

QOrmWhereClause operator<=(const QOrmField& field, const QVariant& value)
{

    return QOrmWhereClause{};
}

QOrmWhereClause operator>(const QOrmField& field, const QVariant& value)
{
    qDebug() << "Custom operator>...";
    return QOrmWhereClause{};
}


QOrmWhereClause operator>=(const QOrmField& field, const QVariant& value)
{

    return QOrmWhereClause{};
}

QOrmWhereClause operator!(const QOrmWhereClause& operand)
{
    return QOrmWhereClause{};
}

QOrmWhereClause operator||(const QOrmWhereClause& lhs, const QOrmWhereClause& rhs)
{
    return QOrmWhereClause{};
}

QOrmWhereClause operator&&(const QOrmWhereClause& lhs, const QOrmWhereClause& rhs)
{
    return QOrmWhereClause{};
}

QT_END_NAMESPACE
