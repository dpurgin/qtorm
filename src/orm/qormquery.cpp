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

    QOrmQueryPrivate(QOrm::Operation operation,
                     const QOrmMetadata& relation,
                     QObject* entityInstance)
        : m_operation{operation}
        , m_relation{relation}
        , m_entityInstance{entityInstance}
    {
    }

    QOrm::Operation m_operation;
    QOrmRelation m_relation;
    std::optional<QOrmMetadata> m_projection;
    std::optional<QOrmFilter> m_filter;
    std::optional<QOrmOrder> m_order;
    QObject* m_entityInstance{nullptr};
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

QOrmQuery::QOrmQuery(QOrm::Operation operation,
                     const QOrmMetadata& relation,
                     QObject* entityInstance)
    : d{new QOrmQueryPrivate{operation, relation, entityInstance}}
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

const std::optional<QOrmMetadata>& QOrmQuery::projection() const
{
    return d->m_projection;
}

const std::optional<QOrmFilter>& QOrmQuery::filter() const
{
    return d->m_filter;
}

const std::optional<QOrmOrder>& QOrmQuery::order() const
{
    return d->m_order;
}

const QObject* QOrmQuery::entityInstance() const
{
    return d->m_entityInstance;
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

    if (query.entityInstance() != nullptr)
        dbg << ", " << query.entityInstance();

    dbg << ")";

    return dbg;
}

QT_END_NAMESPACE
