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
    friend class QOrmFilterExpression;

    using Predicate = std::variant<QOrmFilterTerminalPredicate,
                                   QOrmFilterBinaryPredicate,
                                   QOrmFilterUnaryPredicate>;

    QOrmFilterExpressionPrivate(Predicate predicate)
        : m_predicate{predicate}
    {
    }

    Predicate m_predicate;
};

QOrmFilterExpression::QOrmFilterExpression(const QOrmFilterTerminalPredicate& predicate)
    : d{new QOrmFilterExpressionPrivate{predicate}}
{
}

QOrmFilterExpression::QOrmFilterExpression(const QOrmFilterBinaryPredicate& predicate)
    : d{new QOrmFilterExpressionPrivate{predicate}}
{
}

QOrmFilterExpression::QOrmFilterExpression(const QOrmFilterUnaryPredicate& predicate)
    : d{new QOrmFilterExpressionPrivate{predicate}}
{
}

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

QOrmFilterTerminalPredicate operator==(const QOrmClassProperty& property, const QVariant& value)
{
    return {property, QOrm::Comparison::Equal, value};
}

QOrmFilterTerminalPredicate operator!=(const QOrmClassProperty& property, const QVariant& value)
{
    return {property, QOrm::Comparison::NotEqual, value};
}

QOrmFilterTerminalPredicate operator<(const QOrmClassProperty& property, const QVariant& value)
{
    return {property, QOrm::Comparison::Less, value};
}

QOrmFilterTerminalPredicate operator<=(const QOrmClassProperty& property, const QVariant& value)
{
    return {property, QOrm::Comparison::LessOrEqual, value};
}

QOrmFilterTerminalPredicate operator>(const QOrmClassProperty& property, const QVariant& value)
{    
    return {property, QOrm::Comparison::Greater, value};
}

QOrmFilterTerminalPredicate operator>=(const QOrmClassProperty& property, const QVariant& value)
{
    return {property, QOrm::Comparison::GreaterOrEqual, value};
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
