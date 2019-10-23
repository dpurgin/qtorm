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

#ifndef QORMQUERY_H
#define QORMQUERY_H

#include <QtCore/qglobal.h>
#include <QtCore/qshareddata.h>

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormqueryresult.h>

QT_BEGIN_NAMESPACE

class QOrmFilter;
class QOrmOrder;
class QOrmQueryPrivate;
class QOrmRelation;
class QOrmMetadata;

class Q_ORM_EXPORT QOrmQuery
{
public:
    QOrmQuery(QOrm::Operation operation,
              const QOrmRelation& relation,
              const std::optional<QOrmMetadata>& projection,
              const std::optional<QOrmFilter>& filter,
              const std::optional<QOrmOrder>& order,
              const QFlags<QOrm::QueryFlags>& flags);
    QOrmQuery(QOrm::Operation operation, const QOrmMetadata& relation, QObject* entityInstance);
    QOrmQuery(const QOrmQuery&);
    QOrmQuery(QOrmQuery&&);
    ~QOrmQuery();

    QOrmQuery& operator=(const QOrmQuery&);
    QOrmQuery& operator=(QOrmQuery&&);

    Q_REQUIRED_RESULT
    QOrm::Operation operation() const;

    Q_REQUIRED_RESULT
    const QOrmRelation& relation() const;

    Q_REQUIRED_RESULT
    const std::optional<QOrmMetadata>& projection() const;

    Q_REQUIRED_RESULT
    const std::optional<QOrmFilter>& filter() const;

    Q_REQUIRED_RESULT
    const std::optional<QOrmOrder>& order() const;

    Q_REQUIRED_RESULT
    const QObject* entityInstance() const;

    Q_REQUIRED_RESULT
    const QFlags<QOrm::QueryFlags>& flags() const;

private:
    QSharedDataPointer<QOrmQueryPrivate> d;
};

QDebug operator<<(QDebug dbg, const QOrmQuery& query);

QT_END_NAMESPACE

#endif // QORMQUERY_H
