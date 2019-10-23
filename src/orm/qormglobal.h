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

#ifndef QORMGLOBAL_H
#define QORMGLOBAL_H

#include <algorithm>

#include <QtCore/qglobal.h>
#include <QtCore/qhashfunctions.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qobject.h>
#include <QtCore/qset.h>
#include <QtCore/qvector.h>

QT_BEGIN_NAMESPACE

#ifndef QT_STATIC
    #ifdef QT_BUILD_ORM_LIB
        #define Q_ORM_EXPORT Q_DECL_EXPORT
    #else
        #define Q_ORM_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define Q_ORM_EXPORT
#endif

class QDebug;

namespace QOrm
{
    enum class ErrorType
    {
        None,
        Provider,
        UnsynchronizedEntity,
        UnsynchronizedSchema,
        InvalidMapping,
        TransactionNotActive,
        Other
    };
    extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, QOrm::ErrorType error);

    enum class TransactionPropagation
    {
        Require,
        Support,
        DontSupport
    };
    extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, QOrm::TransactionPropagation propagation);

    enum class TransactionAction
    {
        Commit,
        Rollback
    };
    extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, QOrm::TransactionAction action);

    enum class Comparison
    {
        Equal,
        NotEqual,
        Less,
        LessOrEqual,
        Greater,
        GreaterOrEqual
    };
    extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, QOrm::Comparison comparison);
    extern Q_ORM_EXPORT uint qHash(Comparison comparison) Q_DECL_NOTHROW;

    enum class BinaryLogicalOperator
    {
        And,
        Or
    };
    extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, QOrm::BinaryLogicalOperator logicalOperator);

    enum class UnaryLogicalOperator
    {
        Not
    };
    extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, QOrm::UnaryLogicalOperator logicalOperator);

    enum class Operation
    {
        Create,
        Read,
        Update,
        Delete,
        Merge
    };
    extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, Operation operation);

    enum class FilterType
    {
        Expression,
        Invokable
    };
    extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, FilterType filterType);

    enum class FilterExpressionType
    {
        TerminalPredicate,
        BinaryPredicate,
        UnaryPredicate
    };
    extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, FilterExpressionType expressionType);

    enum class RemoveMode
    {
        PreventRemoveAll,
        ForceRemoveAll
    };

    enum class RelationType
    {
        Query,
        Mapping
    };
    extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, QOrm::RelationType relationType);

    enum class QueryFlags
    {
        None = 0x00,
        OverwriteCachedInstances = 0x01
    };
}

namespace QOrmPrivate
{
    template<typename ContainerFrom,
             typename ContainerTo,
             typename ToValueType = typename ContainerTo::value_type>
    inline ContainerTo convertContainer(const ContainerFrom& v)
    {
        ContainerTo result;

        std::transform(std::begin(v),
                       std::end(v),
                       std::inserter(result, std::end(result)),
                       [](auto o) { return qobject_cast<ToValueType>(o); });

        return result;
    }

    template<typename ContainerFrom,
             typename ContainerTo,
             typename ToValueType = typename ContainerTo::value_type,
             typename = std::enable_if_t<std::is_same_v<ContainerTo, QSet<ToValueType>>>>
    inline QSet<ToValueType> convertContainer(const ContainerFrom& v)
    {
        QSet<ToValueType> result;

        for (auto o : v)
            result.insert(qobject_cast<ToValueType>(o));

        return result;
    }
    template<typename EntityContainer, typename Entity = typename EntityContainer::value_type>
    void registerContainerConverter()
    {
        if (!QMetaType::hasRegisteredConverterFunction<EntityContainer, QVector<QObject*>>())
        {
            QMetaType::registerConverter<EntityContainer, QVector<QObject*>>(
                &convertContainer<EntityContainer, QVector<QObject*>>);
        }

        if (!QMetaType::hasRegisteredConverterFunction<QVector<QObject*>, EntityContainer>())
        {
            QMetaType::registerConverter<QVector<QObject*>, EntityContainer>(
                &convertContainer<QVector<QObject*>, EntityContainer>);
        }
    }

    template<typename T>
    inline void qRegisterOrmEntity()
    {
        qRegisterMetaType<T*>();
        qRegisterMetaType<QVector<T*>>();
        qRegisterMetaType<QSet<T*>>();

        registerContainerConverter<QVector<T*>>();
        registerContainerConverter<QSet<T*>>();
    }
} // namespace QtOrmPrivate

template<typename... Ts>
inline constexpr void qRegisterOrmEntity()
{
    (..., QOrmPrivate::qRegisterOrmEntity<Ts>());
}

QT_END_NAMESPACE

#endif
