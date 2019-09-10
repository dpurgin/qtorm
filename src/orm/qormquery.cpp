#include "qormquery.h"

#include "qormwhereclause.h"
#include "qormorderclause.h"

QOrmQuery::QOrmQuery()
{

}

QOrmQuery& QOrmQuery::first(int n)
{
    return *this;
}

QOrmQuery& QOrmQuery::last(int n)
{
    return *this;
}

QOrmQuery& QOrmQuery::where(QOrmWhereClause whereClause)
{
    return *this;
}

QOrmQuery& QOrmQuery::order(QOrmOrderClause orderClause)
{
    return *this;
}
