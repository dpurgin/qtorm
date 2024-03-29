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

#include "qormquerybuilder.h"

#include "qormabstractprovider.h"
#include "qormerror.h"
#include "qormfilter.h"
#include "qormfilterexpression.h"
#include "qormglobal_p.h"
#include "qormmetadatacache.h"
#include "qormorder.h"
#include "qormquery.h"
#include "qormqueryresult.h"
#include "qormrelation.h"
#include "qormsession.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

namespace QOrmPrivate
{
    struct FoldedFilters
    {
        std::optional<QOrmFilter> expression;
        std::optional<QOrmFilter> invokable;
    };

    template<typename ForwardIterable>
    static FoldedFilters foldFilters(const QOrmRelation& relation, const ForwardIterable& filters)
    {
        return std::accumulate(
            std::cbegin(filters),
            std::cend(filters),
            FoldedFilters{},
            [relation](FoldedFilters lhs, const QOrmFilter& rhs) -> FoldedFilters
            {
                switch (rhs.type())
                {
                    case QOrm::FilterType::Expression:
                        if (!lhs.expression.has_value())
                        {
                            lhs.expression = QOrmFilter{
                                QOrmPrivate::resolvedFilterExpression(relation, *rhs.expression())};
                        }
                        else
                        {
                            Q_ASSERT(lhs.expression->type() == QOrm::FilterType::Expression);
                            Q_ASSERT(lhs.expression->expression() != nullptr);

                            lhs.expression = QOrmFilter{
                                *lhs.expression->expression() &&
                                QOrmPrivate::resolvedFilterExpression(relation, *rhs.expression())};
                        }
                        break;

                    case QOrm::FilterType::Invokable:
                        if (!lhs.invokable.has_value())
                        {
                            lhs.invokable = QOrmFilter{*rhs.invokable()};
                        }
                        else
                        {
                            lhs.invokable = QOrmFilter{[lhs, rhs](const QObject* value) {
                                return (*lhs.invokable->invokable())(value) &&
                                       (*rhs.invokable())(value);
                            }};
                        }
                        break;
                }

                return lhs;
            });
    }

    class QueryBuilderHelperPrivate
    {
    public:
        QueryBuilderHelperPrivate(QOrmSession* ormSession, const QOrmRelation& relation)
            : m_session{ormSession}
            , m_relation{relation}
            , m_projection{relation.type() == QOrm::RelationType::Mapping
                               ? std::make_optional(*relation.mapping())
                               : *relation.query()->projection()}
        {
            Q_ASSERT(ormSession != nullptr);
        }

        QOrmSession* m_session;
        QOrmRelation m_relation;
        std::optional<QOrmMetadata> m_projection;
        QObject* m_entityInstance{nullptr};
        std::vector<QOrmFilter> m_filters;
        std::vector<QOrmOrder> m_order;
        std::optional<int> m_limit{std::nullopt};
        std::optional<int> m_offset{std::nullopt};
    };

    QueryBuilderHelper::QueryBuilderHelper(QOrmSession* ormSession, const QOrmRelation& relation)
        : d{new QueryBuilderHelperPrivate{ormSession, relation}}
    {
    }

    QueryBuilderHelper::QueryBuilderHelper(QueryBuilderHelper&&) = default;

    QueryBuilderHelper::~QueryBuilderHelper() = default;

    QueryBuilderHelper& QueryBuilderHelper::operator=(QueryBuilderHelper&&) = default;

    void QueryBuilderHelper::setInstance(const QMetaObject& qMetaObject, QObject* instance)
    {
        Q_ASSERT(d->m_relation.type() == QOrm::RelationType::Mapping);
        Q_ASSERT(d->m_relation.mapping()->className() ==
                 QString::fromUtf8(qMetaObject.className()));

        d->m_entityInstance = instance;
    }

    void QueryBuilderHelper::addFilter(const QOrmFilter& filter) { d->m_filters.push_back(filter); }

    void QueryBuilderHelper::addOrder(const QOrmClassProperty& classProperty,
                                      Qt::SortOrder direction)
    {
        Q_ASSERT(d->m_projection.has_value());

        const QOrmPropertyMapping* mapping =
            d->m_projection->classPropertyMapping(classProperty.descriptor());
        Q_ASSERT(mapping != nullptr);

        d->m_order.emplace_back(*mapping, direction);
    }

    void QueryBuilderHelper::setLimit(int limit)
    {
        d->m_limit = limit;
    }

    void QueryBuilderHelper::setOffset(int offset)
    {
        d->m_offset = offset;
    }

    QOrmQuery QueryBuilderHelper::build(QOrm::Operation operation, QOrm::QueryFlags flags) const
    {
        if (operation == QOrm::Operation::Merge ||  //
            operation == QOrm::Operation::Create || //
            operation == QOrm::Operation::Update || //
            (operation == QOrm::Operation::Delete && d->m_entityInstance != nullptr))
        {
            Q_ASSERT(d->m_entityInstance != nullptr);

            return QOrmQuery{operation, *d->m_relation.mapping(), d->m_entityInstance};
        }
        else if (operation == QOrm::Operation::Read || operation == QOrm::Operation::Delete)
        {
            FoldedFilters filters = foldFilters(d->m_relation, d->m_filters);
            QOrmQuery query = QOrmQuery{operation,
                                        d->m_relation,
                                        d->m_projection,
                                        filters.expression,
                                        filters.invokable,
                                        d->m_order,
                                        flags};
            query.setLimit(d->m_limit);
            query.setOffset(d->m_offset);
            return query;
        }

        qFatal("Unexpected state");
    }

    QOrmQueryResult<QObject> QueryBuilderHelper::select(QOrm::QueryFlags flags) const
    {
        return d->m_session->execute(build(QOrm::Operation::Read, flags));
    }

    QOrmQueryResult<QObject> QueryBuilderHelper::remove() const
    {
        return d->m_session->execute(build(QOrm::Operation::Delete, QOrm::QueryFlags::None));
    }
} // namespace QOrmPrivate

QT_END_NAMESPACE
