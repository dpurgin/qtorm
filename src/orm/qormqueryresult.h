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

#include <QtOrm/qormerror.h>
#include <QtOrm/qormglobal.h>

#include <QtCore/qobject.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>
#include <QtCore/qvector.h>

QT_BEGIN_NAMESPACE

class QOrmError;
class QOrmQueryResultPrivate;

template<typename T>
class Q_ORM_EXPORT QOrmQueryResult
{
public:
    using Projection = T;
    static_assert(std::is_convertible_v<Projection*, QObject*>,
                  "Projection entity must be inherited from QObject");

    QOrmQueryResult(const QOrmQueryResult&) = delete;
    QOrmQueryResult(QOrmQueryResult&& other) = default;

    template<typename U>
    QOrmQueryResult(const QOrmQueryResult<U>& other)
        : m_error{other.error()}
        , m_result{convertVector<U, T>(other.toVector())}
        , m_lastInsertedId{other.lastInsertedId()}
    {
    }

    explicit QOrmQueryResult(const QOrmError& error,
                             const QVector<T*>& result,
                             const QVariant& lastInsertedId)
        : m_error{error}
        , m_result{result}
        , m_lastInsertedId{lastInsertedId}
    {
    }

    explicit QOrmQueryResult(const QOrmError& error)
        : QOrmQueryResult<T>{error, {}, {}}
    {
    }

    explicit QOrmQueryResult(const QVector<T*>& result)
        : QOrmQueryResult<T>{{QOrm::ErrorType::None, {}}, result, {}}
    {
    }
    explicit QOrmQueryResult(QVariant lastInsertedId)
        : QOrmQueryResult<T>{{QOrm::ErrorType::None, {}}, {}, lastInsertedId}
    {
    }

    QOrmQueryResult& operator=(const QOrmQueryResult&) = delete;
    QOrmQueryResult& operator=(QOrmQueryResult&&) = default;

    Q_REQUIRED_RESULT
    const QOrmError& error() const { return m_error; }
    Q_REQUIRED_RESULT
    const QVariant& lastInsertedId() const { return m_lastInsertedId; }
    Q_REQUIRED_RESULT
    const QVector<Projection*>& toVector() const { return m_result; }

    Q_REQUIRED_RESULT
    std::vector<Projection*> toStdVector() const { return m_result.toStdVector(); }
    Q_REQUIRED_RESULT
    QSet<Projection*> toSet() const
    {
        QSet<Projection*> result;

        for (Projection* instance : m_result)
            result.insert(instance);

        return result;
    }

private:
    template<typename From, typename To>
    static QVector<To*> convertVector(const QVector<From*>& from)
    {
        QVector<To*> to;
        to.reserve(from.size());

        for (From* instance : from)
            to.push_back(qobject_cast<To*>(instance));

        return to;
    }

    QOrmError m_error;
    QVector<Projection*> m_result;
    QVariant m_lastInsertedId;
};

QT_END_NAMESPACE

#endif
