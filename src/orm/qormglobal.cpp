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

            case Operation::Merge:
                dbg << "Merge";
                break;
        }

        return dbg;
    }

    QDebug operator<<(QDebug dbg, ErrorType error)
    {
        QDebugStateSaver saver{dbg};
        dbg.nospace() << "QOrm::ErrorType::";

        switch (error)
        {
            case ErrorType::None:
                dbg << "None";
                break;
            case ErrorType::Other:
                dbg << "Other";
                break;
            case ErrorType::Provider:
                dbg << "Provider";
                break;
            case ErrorType::UnsynchronizedEntity:
                dbg << "UnsynchronizedEntity";
                break;
            case ErrorType::UnsynchronizedSchema:
                dbg << "UnsynchronizedSchema";
                break;
            case ErrorType::InvalidMapping:
                dbg << "InvalidMapping";
                break;
            case ErrorType::TransactionNotActive:
                dbg << "TransactionNotActive";
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
            case FilterType::Invokable:
                dbg << "Invokable";
                break;

            case FilterType::Expression:
                dbg << "Expression";
                break;
        }

        return dbg;
    }

    QDebug operator<<(QDebug dbg, BinaryLogicalOperator logicalOperator)
    {
        QDebugStateSaver saver{dbg};

        dbg.noquote().nospace() << "QOrm::BinaryLogicalOperator::";

        switch (logicalOperator)
        {
            case BinaryLogicalOperator::Or:
                dbg << "Or";
                break;

            case BinaryLogicalOperator::And:
                dbg << "And";
                break;
        }

        return dbg;
    }

    QDebug operator<<(QDebug dbg, UnaryLogicalOperator logicalOperator)
    {
        QDebugStateSaver saver{dbg};

        dbg.noquote().nospace() << "QOrm::UnaryLogicalOperator::";

        switch (logicalOperator)
        {
            case UnaryLogicalOperator::Not:
                dbg << "Not";
                break;
        }

        return dbg;
    }

    QDebug operator<<(QDebug dbg, TransactionPropagation propagation)
    {
        QDebugStateSaver saver{dbg};

        dbg.noquote().nospace() << "QOrm::TransactionPropagation::";

        switch (propagation)
        {
            case TransactionPropagation::Require:
                dbg << "Require";
                break;

            case TransactionPropagation::Support:
                dbg << "Support";
                break;

            case TransactionPropagation::DontSupport:
                dbg << "DontSupport";
                break;
        }

        return dbg;
    }

    QDebug operator<<(QDebug dbg, TransactionAction action)
    {
        QDebugStateSaver saver{dbg};

        dbg.noquote().nospace() << "QOrm::TransactionAction::";

        switch (action)
        {
            case TransactionAction::Commit:
                dbg << "Commit";
                break;

            case TransactionAction::Rollback:
                dbg << "Action";
                break;
        }

        return dbg;
    }

} // namespace QOrm

QT_END_NAMESPACE
