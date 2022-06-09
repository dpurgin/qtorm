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

namespace QtOrmPrivate
{
    template<typename T>
    class QOrmQueryResultBase
    {
    public:
        [[nodiscard]] const QOrmError& error() const { return m_error; }
        [[nodiscard]] const QVariant& lastInsertedId() const { return m_lastInsertedId; }
        [[nodiscard]] int numRowsAffected() const { return m_numRowsAffected; }

    protected:
        QOrmQueryResultBase(const QOrmError& error,
                            const QVariant& lastInsertedId,
                            int numRowsAffected)
            : m_error{error}
            , m_lastInsertedId{lastInsertedId}
            , m_numRowsAffected{numRowsAffected}
        {
        }

        QOrmError m_error;
        QVariant m_lastInsertedId;
        int m_numRowsAffected{0};
    };
} // namespace QtOrmPrivate

template<typename T>
class QOrmQueryResult : public QtOrmPrivate::QOrmQueryResultBase<T>
{
    using Base = QtOrmPrivate::QOrmQueryResultBase<T>;

public:
    using Projection = T;
    static_assert(std::is_convertible_v<Projection*, QObject*>,
                  "Projection entity must be inherited from QObject");

    QOrmQueryResult(const QOrmQueryResult&) = delete;
    QOrmQueryResult(QOrmQueryResult&& other) = default;

    template<typename U>
    QOrmQueryResult(const QOrmQueryResult<U>& other)
        : QtOrmPrivate::QOrmQueryResultBase<T>{other.error(),
                                               other.lastInsertedId(),
                                               other.numRowsAffected()}
        , m_result{convertVector<U, T>(other.toVector())}
    {
    }

    explicit QOrmQueryResult(const QOrmError& error,
                             const QVector<T*>& result,
                             const QVariant& lastInsertedId,
                             int numRowsAffected)
        : QtOrmPrivate::QOrmQueryResultBase<T>{error, lastInsertedId, numRowsAffected}
        , m_result{result}
    {
    }

    explicit QOrmQueryResult(const QOrmError& error)
        : QOrmQueryResult<T>{error, 0}
    {
    }

    explicit QOrmQueryResult(const QOrmError& error, int numRowsAffected)
        : QOrmQueryResult<T>{error, {}, {}, numRowsAffected}
    {
    }

    explicit QOrmQueryResult(const QVector<T*>& result, int numRowsAffected)
        : QOrmQueryResult<T>{{}, result, {}, numRowsAffected}
    {
    }
    explicit QOrmQueryResult(QVariant lastInsertedId, int numRowsAffected)
        : QOrmQueryResult<T>{{}, {}, lastInsertedId, numRowsAffected}
    {
    }

    QOrmQueryResult& operator=(const QOrmQueryResult&) = delete;
    QOrmQueryResult& operator=(QOrmQueryResult&&) = default;

    Q_REQUIRED_RESULT
    const QVector<Projection*>& toVector() const
    {
        if (Base::error().type() != QOrm::ErrorType::None)
        {
            qFatal("qtorm: QOrmQueryResult::toVector() has been called but the result contains an "
                   "error: %s",
                   qPrintable(Base::error().text()));
        }

        return m_result;
    }

    Q_REQUIRED_RESULT
    QList<Projection*> toList() const
    {
        if (Base::error().type() != QOrm::ErrorType::None)
        {
            qFatal("qtorm: QOrmQueryResult::toList() has been called but the result contains an "
                   "error: %s",
                   qPrintable(Base::error().text()));
        }

        return m_result.toList();
    }

    Q_REQUIRED_RESULT
    std::vector<Projection*> toStdVector() const
    {
        if (Base::error().type() != QOrm::ErrorType::None)
        {
            qFatal(
                "qtorm: QOrmQueryResult::toStdVector() has been called but the result contains an "
                "error: %s",
                qPrintable(Base::error().text()));
        }

        return m_result.toStdVector();
    }
    Q_REQUIRED_RESULT
    QSet<Projection*> toSet() const
    {
        if (Base::error().type() != QOrm::ErrorType::None)
        {
            qFatal("qtorm: QOrmQueryResult::toSet() has been called but the result contains an "
                   "error: %s",
                   qPrintable(Base::error().text()));
        }

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
        {
            to.push_back(qobject_cast<To*>(instance));
        }

        return to;
    }

    QVector<Projection*> m_result;
};

template<>
class QOrmQueryResult<void> : public QtOrmPrivate::QOrmQueryResultBase<void>
{
    using Base = QtOrmPrivate::QOrmQueryResultBase<void>;

public:
    template<typename U>
    QOrmQueryResult(const QOrmQueryResult<U>& other)
        : Base{other.error(), other.lastInsertedId(), other.numRowsAffected()}
    {
    }
};

QT_END_NAMESPACE

#endif
