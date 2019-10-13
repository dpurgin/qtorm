#ifndef QORMGLOBAL_H
#define QORMGLOBAL_H

#include <QtCore/qglobal.h>
#include <QtCore/qhashfunctions.h>
#include <QtCore/qmetatype.h>

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
        Other
    };
    extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, QOrm::ErrorType error);

    enum class TransactionMode
    {
        Requires,
        RequiresNew,
        Supports,
        DoesntSupport
    };

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
}

namespace QOrmPrivate
{
    template<typename T>
    inline constexpr void qRegisterOrmEntity()
    {
        qRegisterMetaType<T*>();
        qRegisterMetaType<QVector<T*>>();
        qRegisterMetaType<QList<T*>>();
        qRegisterMetaType<QSet<T*>>();
    }
} // namespace QtOrmPrivate

template<typename... Ts>
inline constexpr void qRegisterOrmEntity()
{
    (..., QOrmPrivate::qRegisterOrmEntity<Ts>());
}

QT_END_NAMESPACE

#endif
