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
                     const QOrmMetadata& projection,
                     const QOrmRelation& relation,
                     QOrmFilter filter,
                     QOrmOrder order)
        : m_operation{operation}
        , m_projection{projection}
        , m_relation{relation}
        , m_filter{filter}
        , m_order{order}
    {
    }

    QOrm::Operation m_operation;
    QOrmMetadata m_projection;
    QOrmRelation m_relation;
    QOrmFilter m_filter;
    QOrmOrder m_order;    
};

QOrmQuery::QOrmQuery(QOrm::Operation operation,
                     const QOrmMetadata& projection,
                     const QOrmRelation& relation,
                     QOrmFilter filter,
                     QOrmOrder order)
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

const QOrmMetadata& QOrmQuery::projection() const
{
    return d->m_projection;
}

const QOrmRelation& QOrmQuery::relation() const
{
    return d->m_relation;
}

QOrmFilter QOrmQuery::filter() const
{
    return d->m_filter;
}

QOrmOrder QOrmQuery::order() const
{
    return d->m_order;
}

QDebug operator<<(QDebug dbg, const QOrmQuery& query)
{
    QDebugStateSaver saver{dbg};

    dbg.nospace().noquote() << "QOrmQuery(" << query.operation() << ", " << query.projection()
                            << query.relation() << query.filter() << query.order() << ")";

    return dbg;
}

QT_END_NAMESPACE
