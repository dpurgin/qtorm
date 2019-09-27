#include "qormglobal.h"

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

namespace QOrm
{
    uint qHash(QOrm::Comparison comparison) Q_DECL_NOTHROW
    {
        return ::qHash(static_cast<int>(comparison));
    }

    QDebug operator<<(QDebug dbg, Operation operation)
    {
        QDebugStateSaver saver{dbg};
        dbg.nospace().noquote() << "QOrm::Operation::";

        switch (operation)
        {
            case Operation::Create:
                dbg << "Create";
                break;

            case Operation::Read:
                dbg << "Read";
                break;

            case Operation::Update:
                dbg << "Update";
                break;

            case Operation::Delete:
                dbg << "Delete";
                break;
        }

        return dbg;
    }

    QDebug operator<<(QDebug dbg, Error error)
    {
        QDebugStateSaver saver{dbg};
        dbg.nospace() << "QOrm::Error::";

        switch (error)
        {
            case Error::None:
                dbg << "None";
                break;
            case Error::Other:
                dbg << "Other";
                break;
            case Error::Provider:
                dbg << "Provider";
                break;
            case Error::UnsynchronizedEntity:
                dbg << "UnsynchronizedEntity";
                break;
            case Error::UnsynchronizedSchema:
                dbg << "UnsynchronizedSchema";
                break;
            case Error::InvalidMapping:
                dbg << "InvalidMapping";
                break;
        }

        return dbg;
    }

    QDebug operator<<(QDebug dbg, RelationType relationType)
    {
        QDebugStateSaver saver{dbg};
        dbg.nospace() << "OQrm::RelationType::";

        switch (relationType)
        {
            case RelationType::Query:
                dbg << "Query";
                break;

            case RelationType::Mapping:
                dbg << "Mapping";
                break;
        }

        return dbg;
    }

    QDebug operator<<(QDebug dbg, FilterExpressionType expressionType)
    {
        QDebugStateSaver saver{dbg};
        dbg.nospace() << "QOrm::FilterExpressionType::";

        switch (expressionType)
        {
            case FilterExpressionType::TerminalPredicate:
                dbg << "TerminalPredicate";
                break;

            case FilterExpressionType::BinaryPredicate:
                dbg << "BinaryPredicate";
                break;

            case FilterExpressionType::UnaryPredicate:
                dbg << "UnaryPredicate";
                break;
        }

        return dbg;
    }

    QDebug operator<<(QDebug dbg, Comparison comparison)
    {
        QDebugStateSaver saver{dbg};

        dbg.nospace().noquote() << "QOrm::Comparison::";

        switch (comparison)
        {
            case Comparison::Less:
                dbg << "Less";
                break;

            case Comparison::Equal:
                dbg << "Equal";
                break;

            case Comparison::Greater:
                dbg << "Greator";
                break;

            case Comparison::NotEqual:
                dbg << "NotEqual";
                break;

            case Comparison::LessOrEqual:
                dbg << "LessOrEqual";
                break;

            case Comparison::GreaterOrEqual:
                dbg << "GreaterOrEqual";
                break;
        }

        return dbg;
    }

    QDebug operator<<(QDebug dbg, FilterType filterType)
    {
        QDebugStateSaver saver{dbg};

        dbg.nospace().noquote() << "QOrm::FilterType::";

        switch (filterType)
        {
            case FilterType::Empty:
                dbg << "Empty";
                break;

            case FilterType::Invokable:
                dbg << "Invokable";
                break;

            case FilterType::Expression:
                dbg << "Expression";
                break;
        }

        return dbg;
    }

} // namespace QOrm

QT_END_NAMESPACE
