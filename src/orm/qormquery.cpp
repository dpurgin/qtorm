#include "qormquery.h"
#include "qormwhereclause.h"
#include "qormorderclause.h"

QT_BEGIN_NAMESPACE

class QOrmQueryPrivate : public QSharedData
{
    friend class QOrmQuery;

    QOrmQueryPrivate(const QMetaObject& projection,
                     int first,
                     int last,
                     QOrmWhereClause where,
                     QOrmOrderClause order)
        : m_projection{projection},
          m_first{first},
          m_last{last},
          m_where{where},
          m_order{order}
    {
    }

    const QMetaObject& m_projection;
    int m_first{-1};
    int m_last{-1};
    QOrmWhereClause m_where;
    QOrmOrderClause m_order;
};

QOrmQuery::QOrmQuery(const QMetaObject& projection,
                     int first,
                     int last,
                     QOrmWhereClause where,
                     QOrmOrderClause order)
    : d{new QOrmQueryPrivate{projection, first, last, where, order}}
{
}

QOrmQuery::QOrmQuery(const QOrmQuery&) = default;

#ifdef Q_COMPILER_RVALUE_REFS
QOrmQuery::QOrmQuery(QOrmQuery&&) = default;
#endif

QOrmQuery::~QOrmQuery() = default;

QOrmQuery& QOrmQuery::operator=(const QOrmQuery&) = default;

#ifdef Q_COMPILER_RVALUE_REFS
QOrmQuery& QOrmQuery::operator=(QOrmQuery&&) = default;
#endif

const QMetaObject& QOrmQuery::projection() const
{
    return d->m_projection;
}

int QOrmQuery::first() const
{
    return d->m_first;
}

int QOrmQuery::last() const
{
    return d->m_last;
}

QOrmWhereClause QOrmQuery::where() const
{
    return d->m_where;
}

QOrmOrderClause QOrmQuery::order() const
{
    return d->m_order;
}

QT_END_NAMESPACE
