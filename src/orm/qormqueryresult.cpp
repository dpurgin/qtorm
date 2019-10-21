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

#include "qormqueryresult.h"
#include "qormerror.h"

#include <QVariant>

QT_BEGIN_NAMESPACE

class QOrmQueryResultPrivate : public QSharedData
{
    friend class QOrmQueryResult;

    QOrmQueryResultPrivate(QOrmError error, QVector<QObject*> resultSet, QVariant lastInsertedId)
        : m_error{std::move(error)}
        , m_resultSet{std::move(resultSet)}
        , m_lastInsertedId{std::move(lastInsertedId)}
    {
    }

    QOrmError m_error;
    QVector<QObject*> m_resultSet;
    QVariant m_lastInsertedId;
};

// Primary ctor
QOrmQueryResult::QOrmQueryResult(QOrmError error,
                                 QVector<QObject*> resultSet,
                                 QVariant lastInsertedId)
    : d{new QOrmQueryResultPrivate{std::move(error),
                                   std::move(resultSet),
                                   std::move(lastInsertedId)}}
{

}

QOrmQueryResult::QOrmQueryResult()
    : QOrmQueryResult{QOrmError{QOrm::ErrorType::None, ""}}
{
}

QOrmQueryResult::~QOrmQueryResult() = default;

QOrmQueryResult::QOrmQueryResult(const QOrmQueryResult&) = default;

QOrmQueryResult::QOrmQueryResult(QOrmQueryResult&&) = default;

QOrmQueryResult& QOrmQueryResult::operator=(const QOrmQueryResult&) = default;

QOrmQueryResult& QOrmQueryResult::operator=(QOrmQueryResult&&) = default;

QOrmQueryResult::QOrmQueryResult(QOrmError error)
    : QOrmQueryResult{std::move(error), {}, {}}
{
}

QOrmQueryResult::QOrmQueryResult(QVector<QObject*> resultSet)
    : QOrmQueryResult{{QOrm::ErrorType::None, ""}, std::move(resultSet), {}}
{    
}

QOrmQueryResult::QOrmQueryResult(QVariant lastInsertedId)
    : QOrmQueryResult{{QOrm::ErrorType::None, ""}, {}, std::move(lastInsertedId)}
{
}

const QOrmError& QOrmQueryResult::error() const
{
    return d->m_error;
}

const QVariant& QOrmQueryResult::lastInsertedId() const
{
    return d->m_lastInsertedId;
}

const QVector<QObject*>& QOrmQueryResult::toVector() const
{
    return d->m_resultSet;
}

QT_END_NAMESPACE
