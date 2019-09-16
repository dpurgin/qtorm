#ifndef QORMGLOBAL_H
#define QORMGLOBAL_H

#include <QtCore/qglobal.h>

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
        Equals,
        NotEquals,
        Less,
        LessOrEquals,
        Greater,
        GreaterOrEquals,
        Not
    };

    enum class LogicalOperator
    {
        And,
        Or,
        Not
    };

    enum class Operation
    {
        Create,
        Read,
        Update,
        Delete
    };
}

extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, QOrm::Error error);

QT_END_NAMESPACE

#endif
