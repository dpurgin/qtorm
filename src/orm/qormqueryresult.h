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

#ifndef QORMQUERYRESULT_H
#define QORMQUERYRESULT_H

#include <QtOrm/qormglobal.h>

#include <QtCore/qobject.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qvector.h>

QT_BEGIN_NAMESPACE

class QOrmError;
class QOrmQueryResultPrivate;

class Q_ORM_EXPORT QOrmQueryResult
{
public:
    QOrmQueryResult();    
    QOrmQueryResult(const QOrmQueryResult&);
    QOrmQueryResult(QOrmQueryResult&&);
    ~QOrmQueryResult();

    explicit QOrmQueryResult(QOrmError error, QVector<QObject*> resultSet, QVariant lastInsertedId);
    explicit QOrmQueryResult(QOrmError error);
    explicit QOrmQueryResult(QVector<QObject*> resultSet);
    explicit QOrmQueryResult(QVariant lastInsertedId);

    QOrmQueryResult& operator=(const QOrmQueryResult&);
    QOrmQueryResult& operator=(QOrmQueryResult&&);

    Q_REQUIRED_RESULT
    const QOrmError& error() const;
    Q_REQUIRED_RESULT
    const QVariant& lastInsertedId() const;

    Q_REQUIRED_RESULT
    const QVector<QObject*>& toVector() const;

    template<typename T>
    Q_REQUIRED_RESULT QVector<T*> toVector() const
    {
        QVector<T*> result;
        std::transform(std::begin(toVector()),
                       std::end(toVector()),
                       std::back_inserter(result),
                       [](QObject* v) { return qobject_cast<T*>(v); });
        return result;
    }

    QSharedDataPointer<QOrmQueryResultPrivate> d;
};

QT_END_NAMESPACE

#endif
