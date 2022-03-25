/*
 * Copyright (C) 2019-2022 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2019-2022 sequality software engineering e.U. <office@sequality.at>
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

#include "qormfilter.h"
#include "qormfilterexpression.h"
#include "qormclassproperty.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

/*
 * TerminalPredicate = PROP COMP VAL ;
 * Expr = [ '(' ] ( TerminalPredicate | BinaryPredicate | UnaryPredicate ) [ ')' ] ;
 * BinaryPredicate = Expr OP Expr
 * UnaryPredicate = '!' Expr
 * COMP = '<' | '>' | '<=' | '>=' | '==' | '!=' ;
 * OP = '&&' | '||' ;
 *
 */

class QOrmFilterExpressionPrivate : public QSharedData
{
public:
    using Predicate = std::variant<QOrmFilterTerminalPredicate,
                                   QOrmFilterBinaryPredicate,
                                   QOrmFilterUnaryPredicate>;

    QOrmFilterExpressionPrivate(Predicate predicate)
        : m_predicate{predicate}
    {
    }

    Predicate m_predicate;
};

QOrmFilterExpression::QOrmFilterExpression(const QOrmFilterTerminalPredicate& terminalPredicate)
    : d{new QOrmFilterExpressionPrivate{terminalPredicate}}
{
}

QOrmFilterExpression::QOrmFilterExpression(const QOrmFilterBinaryPredicate& binaryPredicate)
    : d{new QOrmFilterExpressionPrivate{binaryPredicate}}
{
}

QOrmFilterExpression::QOrmFilterExpression(const QOrmFilterUnaryPredicate& unaryPredicate)
    : d{new QOrmFilterExpressionPrivate{unaryPredicate}}
{
}

QOrmFilterExpression::QOrmFilterExpression(const QOrmFilterExpression&) = default;

QOrmFilterExpression::QOrmFilterExpression(QOrmFilterExpression&&) = default;

QOrmFilterExpression::~QOrmFilterExpression() = default;

QOrmFilterExpression& QOrmFilterExpression::operator=(const QOrmFilterExpression&) = default;

QOrmFilterExpression& QOrmFilterExpression::operator=(QOrmFilterExpression&&) = default;

QOrm::FilterExpressionType QOrmFilterExpression::type() const
{
    if (std::holds_alternative<QOrmFilterTerminalPredicate>(d->m_predicate))
        return QOrm::FilterExpressionType::TerminalPredicate;
    else if (std::holds_alternative<QOrmFilterBinaryPredicate>(d->m_predicate))
        return QOrm::FilterExpressionType::BinaryPredicate;
    else if (std::holds_alternative<QOrmFilterUnaryPredicate>(d->m_predicate))
        return QOrm::FilterExpressionType::UnaryPredicate;

    qFatal("Unexpected state of QOrmFilterExpression");
}

QOrmFilterTerminalPredicate::QOrmFilterTerminalPredicate(
        QOrmFilterTerminalPredicate::FilterProperty filterProperty,
        QOrm::Comparison comparison,
        QVariant value)
    : m_filterProperty{std::move(filterProperty)},
      m_comparison{comparison},
      m_value{std::move(value)}
{
}

/*!
 * \class QOrmFilterTerminalPredicate
 */

bool QOrmFilterTerminalPredicate::isResolved() const
{
    return std::holds_alternative<QOrmPropertyMapping>(m_filterProperty);
}

const QOrmClassProperty* QOrmFilterTerminalPredicate::classProperty() const
{
    return std::get_if<QOrmClassProperty>(&m_filterProperty);
}

const QOrmPropertyMapping* QOrmFilterTerminalPredicate::propertyMapping() const
{
    return std::get_if<QOrmPropertyMapping>(&m_filterProperty);
}

QOrm::Comparison QOrmFilterTerminalPredicate::comparison() const
{
    return m_comparison;
}

QVariant QOrmFilterTerminalPredicate::value() const
{
    return m_value;
}

const QOrmFilterTerminalPredicate* QOrmFilterExpression::terminalPredicate() const
{
    return std::get_if<QOrmFilterTerminalPredicate>(&d->m_predicate);
}

const QOrmFilterBinaryPredicate* QOrmFilterExpression::binaryPredicate() const
{
    return std::get_if<QOrmFilterBinaryPredicate>(&d->m_predicate);
}

const QOrmFilterUnaryPredicate* QOrmFilterExpression::unaryPredicate() const
{
    return std::get_if<QOrmFilterUnaryPredicate>(&d->m_predicate);
}

/*!
 * \class QOrmFilterBinaryPredicate
 */

QOrmFilterBinaryPredicate::QOrmFilterBinaryPredicate(QOrmFilterExpression lhs,
                                                     QOrm::BinaryLogicalOperator logicalOperator,
                                                     QOrmFilterExpression rhs)
    : m_lhs{std::move(lhs)},
      m_logicalOperator{logicalOperator},
      m_rhs{std::move(rhs)}
{
}

const QOrmFilterExpression& QOrmFilterBinaryPredicate::lhs() const
{
    return m_lhs;
}

QOrm::BinaryLogicalOperator QOrmFilterBinaryPredicate::logicalOperator() const
{
    return m_logicalOperator;
}

const QOrmFilterExpression& QOrmFilterBinaryPredicate::rhs() const
{
    return m_rhs;
}

/*!
 * \class QOrmFilterUnaryPredicate
 */

QOrmFilterUnaryPredicate::QOrmFilterUnaryPredicate(QOrm::UnaryLogicalOperator logicalOperator,
                                                   QOrmFilterExpression rhs)
    : m_logicalOperator{logicalOperator},
      m_rhs{std::move(rhs)}
{
}

QOrm::UnaryLogicalOperator QOrmFilterUnaryPredicate::logicalOperator() const
{
    return m_logicalOperator;
}

const QOrmFilterExpression& QOrmFilterUnaryPredicate::rhs() const
{
    return m_rhs;
}

QDebug operator<<(QDebug dbg, const QOrmFilterExpression& expression)
{
    QDebugStateSaver saver{dbg};
    dbg << "QOrmFilterExpression(" << expression.type() << ", ";

    switch (expression.type())
    {
        case QOrm::FilterExpressionType::TerminalPredicate:
            dbg << *expression.terminalPredicate();
            break;

        case QOrm::FilterExpressionType::BinaryPredicate:
            dbg << *expression.binaryPredicate();
            break;

        case QOrm::FilterExpressionType::UnaryPredicate:
            dbg << *expression.unaryPredicate();
            break;
    }

    dbg << ")";
    return dbg;
}

QDebug operator<<(QDebug dbg, const QOrmFilterTerminalPredicate& predicate)
{
    QDebugStateSaver saver{dbg};

    dbg << "QOrmFilterTerminalPredicate(";

    if (predicate.isResolved())
        dbg << *predicate.propertyMapping();
    else
        dbg << *predicate.classProperty();

    dbg << ", " << predicate.comparison() << ", " << predicate.value() << ")";

    return dbg;
}

QDebug operator<<(QDebug dbg, const QOrmFilterUnaryPredicate& predicate)
{
    QDebugStateSaver saver{dbg};

    dbg.nospace().noquote() << "QOrmFilterUnaryPredicate(" << predicate.logicalOperator() << ", "
                            << predicate.rhs() << ")";

    return dbg;
}

QDebug operator<<(QDebug dbg, const QOrmFilterBinaryPredicate& predicate)
{
    QDebugStateSaver saver{dbg};

    dbg.nospace().noquote() << "QOrmFilterBinaryPredicate(" << predicate.lhs() << ", "
                            << predicate.logicalOperator() << ", " << predicate.rhs() << ")";

    return dbg;
}

QOrmFilterUnaryPredicate operator!(const QOrmFilterExpression& rhs)
{
    return {QOrm::UnaryLogicalOperator::Not, rhs};
}

QOrmFilterBinaryPredicate operator||(const QOrmFilterExpression& lhs,
                                     const QOrmFilterExpression& rhs)
{
    return {lhs, QOrm::BinaryLogicalOperator::Or, rhs};
}

QOrmFilterBinaryPredicate operator&&(const QOrmFilterExpression& lhs,
                                     const QOrmFilterExpression& rhs)
{
    return {lhs, QOrm::BinaryLogicalOperator::And, rhs};
}


QT_END_NAMESPACE

