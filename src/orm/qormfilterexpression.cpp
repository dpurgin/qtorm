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
