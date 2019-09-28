#include "qormquery.h"
#include "qormfilter.h"
#include "qormmetadata.h"
#include "qormorder.h"
#include "qormrelation.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

class QOrmQueryPrivate : public QSharedData
{
public:
    QOrmQueryPrivate(QOrm::Operation operation,
                     const QOrmRelation& relation,
                     std::optional<QOrmMetadata> projection,
                     std::optional<QOrmFilter> filter,
                     std::optional<QOrmOrder> order)
        : m_operation{operation}
        , m_relation{relation}
        , m_projection{std::move(projection)}
        , m_filter{std::move(filter)}
        , m_order{std::move(order)}
    {
    }

    QOrm::Operation m_operation;
    const QOrmRelation& m_relation;
    std::optional<QOrmMetadata> m_projection;
    std::optional<QOrmFilter> m_filter;
    std::optional<QOrmOrder> m_order;
};

QOrmQuery::QOrmQuery(QOrm::Operation operation,
                     const QOrmRelation& relation,
                     std::optional<QOrmMetadata> projection,
                     std::optional<QOrmFilter> filter,
                     std::optional<QOrmOrder> order)
    : d{new QOrmQueryPrivate{operation,
                             relation,
                             std::move(projection),
                             std::move(filter),
                             std::move(order)}}
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

const QOrmRelation& QOrmQuery::relation() const
{
    return d->m_relation;
}

std::optional<QOrmMetadata> QOrmQuery::projection() const
{
    return d->m_projection;
}

std::optional<QOrmFilter> QOrmQuery::filter() const
{
    return d->m_filter;
}

std::optional<QOrmOrder> QOrmQuery::order() const
{
    return d->m_order;
}

QDebug operator<<(QDebug dbg, const QOrmQuery& query)
{
    QDebugStateSaver saver{dbg};

    dbg.nospace().noquote() << "QOrmQuery(" << query.operation() << ", " << query.relation();

    if (query.projection().has_value())
        dbg << ", " << *query.projection();

    if (query.filter().has_value())
        dbg << ", " << *query.filter();

    if (query.order().has_value())
        dbg << ", " << *query.order();

    dbg << ")";

    return dbg;
}

QT_END_NAMESPACE
