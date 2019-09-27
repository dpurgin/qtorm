#ifndef QORMFILTERBUILDER_H
#define QORMFILTERBUILDER_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormclassproperty.h>
#include <QtOrm/qormpropertymapping.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

#include <variant>

QT_BEGIN_NAMESPACE

class QOrmFilterExpressionPrivate;
class QVariant;

class QOrmFilterTerminalPredicate;
class QOrmFilterBinaryPredicate;
class QOrmFilterUnaryPredicate;

class Q_ORM_EXPORT QOrmFilterExpression
{
public:    
    QOrmFilterExpression(const QOrmFilterTerminalPredicate& terminalPredicate);
    QOrmFilterExpression(const QOrmFilterBinaryPredicate& binaryPredicate);
    QOrmFilterExpression(const QOrmFilterUnaryPredicate& unaryPredicate);
    QOrmFilterExpression(const QOrmFilterExpression&);
    QOrmFilterExpression(QOrmFilterExpression&&);
    ~QOrmFilterExpression();

    QOrmFilterExpression& operator=(const QOrmFilterExpression&);
    QOrmFilterExpression& operator=(QOrmFilterExpression&&);

    Q_REQUIRED_RESULT QOrm::FilterExpressionType type() const;

    Q_REQUIRED_RESULT const QOrmFilterTerminalPredicate* terminalPredicate() const;
    Q_REQUIRED_RESULT const QOrmFilterBinaryPredicate* binaryPredicate() const;
    Q_REQUIRED_RESULT const QOrmFilterUnaryPredicate* unaryPredicate() const;

private:
    QSharedDataPointer<QOrmFilterExpressionPrivate> d;
};

class Q_ORM_EXPORT QOrmFilterTerminalPredicate
{
public:
    using FilterProperty = std::variant<QOrmClassProperty, QOrmPropertyMapping>;

    QOrmFilterTerminalPredicate(FilterProperty filterProperty,
                                QOrm::Comparison comparison,
                                QVariant value);

    Q_REQUIRED_RESULT bool isResolved() const;

    Q_REQUIRED_RESULT const QOrmClassProperty* classProperty() const;
    Q_REQUIRED_RESULT const QOrmPropertyMapping* propertyMapping() const;

    Q_REQUIRED_RESULT QOrm::Comparison comparison() const;

    Q_REQUIRED_RESULT QVariant value() const;

private:
    std::variant<QOrmClassProperty, QOrmPropertyMapping> m_filterProperty;
    QOrm::Comparison m_comparison;
    QVariant m_value;
};

class Q_ORM_EXPORT QOrmFilterBinaryPredicate
{
public:
    QOrmFilterBinaryPredicate(QOrmFilterExpression lhs,
                              QOrm::BinaryLogicalOperator logicalOperator,
                              QOrmFilterExpression rhs);

    Q_REQUIRED_RESULT const QOrmFilterExpression& lhs() const;
    Q_REQUIRED_RESULT QOrm::BinaryLogicalOperator logicalOperator() const;
    Q_REQUIRED_RESULT const QOrmFilterExpression& rhs() const;

private:
    QOrmFilterExpression m_lhs;
    QOrm::BinaryLogicalOperator m_logicalOperator;
    QOrmFilterExpression m_rhs;
};

class Q_ORM_EXPORT QOrmFilterUnaryPredicate
{
public:
    QOrmFilterUnaryPredicate(QOrm::UnaryLogicalOperator logicalOperator,
                             QOrmFilterExpression rhs);

    Q_REQUIRED_RESULT QOrm::UnaryLogicalOperator logicalOperator() const;
    Q_REQUIRED_RESULT const QOrmFilterExpression& rhs() const;

private:
    QOrm::UnaryLogicalOperator m_logicalOperator;
    QOrmFilterExpression m_rhs;
};

extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, const QOrmFilterExpression& expression);
extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, const QOrmFilterTerminalPredicate& predicate);
extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, const QOrmFilterBinaryPredicate& predicate);
extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, const QOrmFilterUnaryPredicate& predicate);

Q_REQUIRED_RESULT
Q_ORM_EXPORT
QOrmFilterTerminalPredicate operator==(const QOrmClassProperty& property, const QVariant& value);

Q_REQUIRED_RESULT
Q_ORM_EXPORT
QOrmFilterTerminalPredicate operator!=(const QOrmClassProperty& property, const QVariant& value);

Q_REQUIRED_RESULT
Q_ORM_EXPORT
QOrmFilterTerminalPredicate operator<(const QOrmClassProperty& property, const QVariant& value);

Q_REQUIRED_RESULT
Q_ORM_EXPORT
QOrmFilterTerminalPredicate operator<=(const QOrmClassProperty& property, const QVariant& value);

Q_REQUIRED_RESULT
Q_ORM_EXPORT
QOrmFilterTerminalPredicate operator>(const QOrmClassProperty& property, const QVariant& value);

Q_REQUIRED_RESULT
Q_ORM_EXPORT
QOrmFilterTerminalPredicate operator>=(const QOrmClassProperty& property, const QVariant& value);

Q_REQUIRED_RESULT
Q_ORM_EXPORT
QOrmFilterUnaryPredicate operator!(const QOrmFilterExpression& operand);

Q_REQUIRED_RESULT
Q_ORM_EXPORT
QOrmFilterBinaryPredicate operator||(const QOrmFilterExpression& lhs,
                                     const QOrmFilterExpression& rhs);

Q_REQUIRED_RESULT
Q_ORM_EXPORT
QOrmFilterBinaryPredicate operator&&(const QOrmFilterExpression& lhs,
                                     const QOrmFilterExpression& rhs);

QT_END_NAMESPACE

#endif // QORMFILTERBUILDER_H
