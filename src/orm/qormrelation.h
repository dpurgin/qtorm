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

#ifndef QORMRELATION_H
#define QORMRELATION_H

#include <QtOrm/qormglobal.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOrmMetadata;
class QOrmQuery;
class QOrmRelationPrivate;

class Q_ORM_EXPORT QOrmRelation
{
public:
    explicit QOrmRelation(const QOrmMetadata& mapping);
    explicit QOrmRelation(const QOrmQuery& query);
    QOrmRelation(const QOrmRelation&);
    QOrmRelation(QOrmRelation&&);
    ~QOrmRelation();

    QOrmRelation& operator=(const QOrmRelation&);
    QOrmRelation& operator=(QOrmRelation&&);

    QOrm::RelationType type() const;
    const QOrmMetadata* mapping() const;
    const QOrmQuery* query() const;

private:
    QSharedDataPointer<QOrmRelationPrivate> d;
};

extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, const QOrmRelation& relation);

QT_END_NAMESPACE

#endif
