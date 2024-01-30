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
        [[nodiscard]] bool hasError() const { return m_error.type() != QOrm::ErrorType::None; }

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

    using iterator = typename QVector<Projection*>::iterator;
    using const_iterator = typename QVector<Projection*>::const_iterator;

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

    [[nodiscard]] const QVector<Projection*>& toVector() const
    {
        if (Base::error().type() != QOrm::ErrorType::None)
        {
            qFatal("qtorm: QOrmQueryResult::toVector() has been called but the result contains an "
                   "error: %s",
                   qPrintable(Base::error().text()));
        }

        return m_result;
    }

    [[nodiscard]] QList<Projection*> toList() const
    {
        if (Base::error().type() != QOrm::ErrorType::None)
        {
            qFatal("qtorm: QOrmQueryResult::toList() has been called but the result contains an "
                   "error: %s",
                   qPrintable(Base::error().text()));
        }

        return m_result.toList();
    }

    [[nodiscard]] std::vector<Projection*> toStdVector() const
    {
        checkError(__FUNCTION__);
        return m_result.toStdVector();
    }

    [[nodiscard]] QSet<Projection*> toSet() const
    {
        checkError(__FUNCTION__);

        QSet<Projection*> result;

        for (Projection* instance : m_result)
            result.insert(instance);

        return result;
    }

    [[nodiscard]] Projection* first() const
    {
        checkError(__FUNCTION__);
        return !m_result.isEmpty() ? m_result.first() : nullptr;
    }

    [[nodiscard]] Projection* last() const
    {
        checkError(__FUNCTION__);
        return !m_result.isEmpty() ? m_result.last() : nullptr;
    }

    [[nodiscard]] iterator begin() const
    {
        checkError(__FUNCTION__);
        return std::begin(m_result);
    }

    [[nodiscard]] iterator end() const
    {
        checkError(__FUNCTION__);
        return std::end(m_result);
    }

    [[nodiscard]] const_iterator cbegin() const
    {
        checkError(__FUNCTION__);
        return std::cbegin(m_result);
    }

    [[nodiscard]] const_iterator cend() const
    {
        checkError(__FUNCTION__);
        return std::cend(m_result);
    }

private:
    void checkError(const char* function) const
    {
        if (Base::error().type() != QOrm::ErrorType::None)
        {
            qFatal("qtorm: %s has been called but the result contains an "
                   "error: %s",
                   function,
                   qPrintable(Base::error().text()));
        }
    }

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
