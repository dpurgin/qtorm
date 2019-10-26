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

#ifndef QORMQUERYBUILDER_H
#define QORMQUERYBUILDER_H

#include <QtOrm/qormfilter.h>
#include <QtOrm/qormfilterexpression.h>
#include <QtOrm/qormglobal.h>
#include <QtOrm/qormquery.h>
#include <QtOrm/qormqueryresult.h>

#include <QtCore/qobject.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qvector.h>

#include <memory>

QT_BEGIN_NAMESPACE

class QOrmAbstractProvider;
class QOrmFilterExpression;
class QOrmMetadataCache;
class QOrmQueryBuilderPrivate;
class QOrmRelation;
class QOrmSession;

namespace QOrmPrivate
{
    class QueryBuilderHelperPrivate;

    class Q_ORM_EXPORT QueryBuilderHelper
    {
    public:
        QueryBuilderHelper(QOrmSession* session, const QOrmRelation& relation);
        QueryBuilderHelper(const QueryBuilderHelper&) = delete;
        QueryBuilderHelper(QueryBuilderHelper&&);
        ~QueryBuilderHelper();

        QueryBuilderHelper& operator=(const QueryBuilderHelper&) = delete;
        QueryBuilderHelper& operator=(QueryBuilderHelper&&);

        void setInstance(const QMetaObject& qMetaObject, QObject* instance);
        void addFilter(const QOrmFilter& filter);
        void addOrder(const QOrmClassProperty& classProperty, Qt::SortOrder direction);

        Q_REQUIRED_RESULT
        QOrmQuery build(QOrm::Operation operation) const;

        Q_REQUIRED_RESULT
        QOrmQueryResult<QObject> select() const;

    private:
        std::unique_ptr<QueryBuilderHelperPrivate> d;
    };
} // namespace QOrmPrivate

template<typename T>
class QOrmQueryBuilder
{
    template<typename>
    friend class QOrmQueryBuilder;

public:
    using Projection = T;

    explicit QOrmQueryBuilder(QOrmSession* session, const QOrmRelation& relation)
        : m_helper{session, relation}
    {
    }

    QOrmQueryBuilder(const QOrmQueryBuilder&) = delete;

    template<typename U,
             typename = std::enable_if_t<std::is_same_v<U, QObject> || std::is_same_v<U, T>>>
    QOrmQueryBuilder(QOrmQueryBuilder<U>&& other)
        : m_helper{std::move(other.m_helper)}
    {
    }

    QOrmQueryBuilder& operator=(const QOrmQueryBuilder&) = delete;
    QOrmQueryBuilder& operator=(QOrmQueryBuilder&&) = default;

    QOrmQueryBuilder& filter(QOrmFilterExpression expression)
    {
        m_helper.addFilter(QOrmFilter{expression});
        return *this;
    }

    QOrmQueryBuilder& order(const QOrmClassProperty& classProperty,
                            Qt::SortOrder direction = Qt::AscendingOrder)
    {
        m_helper.addOrder(classProperty, direction);
        return *this;
    }

    QOrmQueryBuilder& instance(const QMetaObject& qMetaObject, QObject* instance)
    {
        m_helper.setInstance(qMetaObject, instance);
        return *this;
    }

    Q_REQUIRED_RESULT
    QOrmQueryResult<Projection> select() const { return m_helper.select(); }

    Q_REQUIRED_RESULT
    QOrmQuery build(QOrm::Operation operation) const { return m_helper.build(operation); }

private:
    QOrmPrivate::QueryBuilderHelper m_helper;
};

QT_END_NAMESPACE

#endif // QORMQUERYBUILDER_H
