#ifndef QORMGLOBAL_H
#define QORMGLOBAL_H

#include <QtCore/qglobal.h>
#include <QtCore/qhashfunctions.h>

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
    enum class Error
    {
        None,
        Provider,
        UnsynchronizedEntity,
        UnsynchronizedSchema,
        InvalidMapping,
        Other
    };

    enum class TransactionMode
    {
        Requires,
        RequiresNew,
        Supports,
        DoesntSupport
    };

    enum class MergeMode
    {
        Auto,
        Create,
        Update
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

    enum class BinaryLogicalOperator
    {
        And,
        Or
    };

    enum class UnaryLogicalOperator
    {
        Not
    };

    enum class Operation
    {
        Create,
        Read,
        Update,
        Delete
    };

    enum class FilterType
    {
        Empty,
        Expression,
        Invokable
    };

    enum class FilterExpressionType
    {
        TerminalPredicate,
        BinaryPredicate,
        UnaryPredicate
    };

    extern Q_ORM_EXPORT uint qHash(Comparison comparison) Q_DECL_NOTHROW;
}

extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, QOrm::Error error);

QT_END_NAMESPACE

#endif
