#include "qormquery.h"
#include "qormwhereclause.h"
#include "qormorderclause.h"

QT_BEGIN_NAMESPACE

class QOrmQueryPrivate : public QSharedData
{
    friend class QOrmQuery;

    QOrmQueryPrivate(QOrm::Operation operation,
                     const QMetaObject& projection,
                     const QMetaObject& relation,
                     std::optional<QOrmWhereClause> where,
                     std::optional<QOrmOrderClause> order)
        : m_projection{projection},
          m_relation{relation},
          m_where{where},
          m_order{order}
    {
    }

    QOrm::Operation m_operation;
    QMetaObject m_projection;
    QMetaObject m_relation;
    std::optional<QOrmWhereClause> m_where;
    std::optional<QOrmOrderClause> m_order;
};

QOrmQuery::QOrmQuery(QOrm::Operation operation,
                     const QMetaObject& projection,
                     const QMetaObject& relation,
                     std::optional<QOrmWhereClause> where,
                     std::optional<QOrmOrderClause> order)
    : d{new QOrmQueryPrivate{operation, projection, relation, where, order}}
{
}

QOrmQuery::QOrmQuery(const QOrmQuery&) = default;

QOrmQuery::QOrmQuery(QOrmQuery&&) = default;

QOrmQuery::~QOrmQuery() = default;

QOrmQuery& QOrmQuery::operator=(const QOrmQuery&) = default;

QOrmQuery& QOrmQuery::operator=(QOrmQuery&&) = default;

QOrm::Operation QOrmQuery::operation() const
{
    return d->m_operation;
}

const QMetaObject& QOrmQuery::projection() const
{
    return d->m_projection;
}

const QMetaObject& QOrmQuery::relation() const
{
    return d->m_relation;
}

std::optional<QOrmWhereClause> QOrmQuery::where() const
{
    return d->m_where;
}

std::optional<QOrmOrderClause> QOrmQuery::order() const
{
    return d->m_order;
}

QT_END_NAMESPACE
