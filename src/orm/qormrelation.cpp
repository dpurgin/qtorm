
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

#include "qormrelation.h"
#include "qormmetadata.h"
#include "qormquery.h"

#include <QDebug>
#include <variant>

QT_BEGIN_NAMESPACE

class QOrmRelationPrivate : public QSharedData
{
    friend class QOrmRelation;

    using RelationStorageType = std::variant<QOrmMetadata, QOrmQuery>;

    QOrmRelationPrivate(RelationStorageType relation)
        : m_relation{std::move(relation)}
    {
    }

    std::variant<QOrmMetadata, QOrmQuery> m_relation;
};

QOrmRelation::QOrmRelation(const QOrmMetadata& mapping)
    : d{new QOrmRelationPrivate{mapping}}
{
}

QOrmRelation::QOrmRelation(const QOrmQuery& query)
    : d{new QOrmRelationPrivate{query}}
{
}

QOrmRelation::QOrmRelation(const QOrmRelation&) = default;

QOrmRelation::QOrmRelation(QOrmRelation&&) = default;

QOrmRelation::~QOrmRelation() = default;

QOrmRelation& QOrmRelation::operator=(const QOrmRelation&) = default;

QOrmRelation& QOrmRelation::operator=(QOrmRelation&&) = default;

QOrm::RelationType QOrmRelation::type() const
{
    if (std::holds_alternative<QOrmMetadata>(d->m_relation))
        return QOrm::RelationType::Mapping;
    else if (std::holds_alternative<QOrmQuery>(d->m_relation))
        return QOrm::RelationType::Query;

    qFatal("QtORM: Unexpected state of QOrmRelation");
}

const QOrmMetadata* QOrmRelation::mapping() const
{
    return std::get_if<QOrmMetadata>(&d->m_relation);
}

const QOrmQuery* QOrmRelation::query() const
{
    return std::get_if<QOrmQuery>(&d->m_relation);
}

QDebug operator<<(QDebug dbg, const QOrmRelation& relation)
{
    QDebugStateSaver saver{dbg};

    dbg.noquote().nospace() << "QOrmRelation(" << relation.type() << ", ";

    switch (relation.type())
    {
        case QOrm::RelationType::Query:
            dbg << *relation.query();
            break;

        case QOrm::RelationType::Mapping:
            dbg << *relation.mapping();
            break;
    }

    dbg << ")";

    return dbg;
}

QT_END_NAMESPACE
