#include "qormquery.h"
#include "qormfilter.h"
#include "qormorder.h"

QT_BEGIN_NAMESPACE

class QOrmQueryPrivate : public QSharedData
{
    friend class QOrmQuery;

    QOrmQueryPrivate(QOrm::Operation operation,
                     const QMetaObject& projection,
                     const QMetaObject& relation,
                     std::optional<QOrmFilter> filter,
                     std::optional<QOrmOrder> order)
        : m_projection{projection},
          m_relation{relation},
          m_filter{filter},
          m_order{order}
    {
    }

    QOrm::Operation m_operation;
    QMetaObject m_projection;
    QMetaObject m_relation;
    std::optional<QOrmFilter> m_filter;
    std::optional<QOrmOrder> m_order;
};

QOrmQuery::QOrmQuery(QOrm::Operation operation,
                     const QMetaObject& projection,
                     const QMetaObject& relation,
                     std::optional<QOrmFilter> filter,
                     std::optional<QOrmOrder> order)
    : d{new QOrmQueryPrivate{operation, projection, relation, filter, order}}
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

std::optional<QOrmFilter> QOrmQuery::filter() const
{
    return d->m_filter;
}

std::optional<QOrmOrder> QOrmQuery::order() const
{
    return d->m_order;
}

QT_END_NAMESPACE
