/*
 * Copyright (C) 2019 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2019 sequality software engineering e.U. <office@sequality.at>
 *
 * This file is part of QtOrm library.
 *
 * QtOrm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QtOrm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with QtOrm.  If not, see <https://www.gnu.org/licenses/>.
 */

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
                     const std::optional<QOrmMetadata>& projection,
                     const std::optional<QOrmFilter>& expressionFilter,
                     const std::optional<QOrmFilter>& invokableFilter,
                     const std::vector<QOrmOrder>& order,
                     const QFlags<QOrm::QueryFlags>& flags)
        : m_operation{operation}
        , m_relation{relation}
        , m_projection{projection}
        , m_expressionFilter{expressionFilter}
        , m_invokableFilter{invokableFilter}
        , m_order{order}
        , m_flags{flags}
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
    std::optional<QOrmFilter> m_expressionFilter;
    std::optional<QOrmFilter> m_invokableFilter;
    std::vector<QOrmOrder> m_order;
    QObject* m_entityInstance{nullptr};
    QFlags<QOrm::QueryFlags> m_flags;
    std::optional<int> m_limit;
    std::optional<int> m_offset;
};

QOrmQuery::QOrmQuery(QOrm::Operation operation,
                     const QOrmRelation& relation,
                     const std::optional<QOrmMetadata>& projection,
                     const std::optional<QOrmFilter>& expressionFilter,
                     const std::optional<QOrmFilter>& invokableFilter,
                     const std::vector<QOrmOrder>& order,
                     const QFlags<QOrm::QueryFlags>& flags)
    : d{new QOrmQueryPrivate{operation,
                             relation,
                             projection,
                             expressionFilter,
                             invokableFilter,
                             order,
                             flags}}
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

const std::optional<QOrmFilter>& QOrmQuery::expressionFilter() const
{
    return d->m_expressionFilter;
}

const std::optional<QOrmFilter>& QOrmQuery::invokableFilter() const
{
    return d->m_invokableFilter;
}

const std::vector<QOrmOrder>& QOrmQuery::order() const
{
    return d->m_order;
}

const QObject* QOrmQuery::entityInstance() const
{
    return d->m_entityInstance;
}

const QFlags<QOrm::QueryFlags>& QOrmQuery::flags() const
{
    return d->m_flags;
}

std::optional<int> QOrmQuery::limit() const
{
    return d->m_limit;
}

void QOrmQuery::setLimit(std::optional<int> limit)
{
    d->m_limit = limit;
}

std::optional<int> QOrmQuery::offset() const
{
    return d->m_offset;
}

void QOrmQuery::setOffset(std::optional<int> offset)
{
    d->m_offset = offset;
}

QDebug operator<<(QDebug dbg, const QOrmQuery& query)
{
    QDebugStateSaver saver{dbg};

    dbg.nospace().noquote() << "QOrmQuery(" << query.operation() << ", " << query.relation();

    if (query.projection().has_value())
    {
        dbg << ", " << *query.projection();
    }

    if (query.expressionFilter().has_value())
    {
        dbg << ", " << *query.expressionFilter();
    }

    if (query.invokableFilter().has_value())
    {
        dbg << ", " << *query.invokableFilter();
    }

    if (!query.order().empty())
    {
        dbg << ", " << query.order();
    }

    if (query.entityInstance() != nullptr)
    {
        dbg << ", " << query.entityInstance();
    }

    if (query.limit().has_value())
    {
        dbg << ", limit " << *query.limit();
    }

    if (query.offset().has_value())
    {
        dbg << ", offset " << *query.offset();
    }

    dbg << ")";

    return dbg;
}

QT_END_NAMESPACE
