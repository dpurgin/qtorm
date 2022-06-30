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

#ifndef QORMFILTER_H
#define QORMFILTER_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormfilterexpression.h>

#include <variant>

QT_BEGIN_NAMESPACE

class Q_ORM_EXPORT QOrmFilter
{
public:
    using Predicate = std::function<bool(const QObject*)>;

    explicit QOrmFilter(QOrmFilterExpression expression)
        : m_type{QOrm::FilterType::Expression}
        , m_filter{std::move(expression)}
    {
    }

    explicit QOrmFilter(Predicate invokable)
        : m_type{QOrm::FilterType::Invokable}
        , m_filter{std::move(invokable)}
    {
    }

    Q_REQUIRED_RESULT
    QOrm::FilterType type() const
    {
        return m_type;
    }

    Q_REQUIRED_RESULT
    const QOrmFilterExpression* expression() const
    {
        return std::get_if<QOrmFilterExpression>(&m_filter);
    }

    Q_REQUIRED_RESULT
    const Predicate* invokable() const { return std::get_if<Predicate>(&m_filter); }

private:
    QOrm::FilterType m_type;
    std::variant<QOrmFilterExpression, Predicate> m_filter{nullptr};
};

extern Q_ORM_EXPORT QDebug operator<<(QDebug debug, const QOrmFilter& filter);

QT_END_NAMESPACE

#endif
