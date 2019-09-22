#ifndef QORMFILTERBUILDER_H
#define QORMFILTERBUILDER_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormclassproperty.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

#include <variant>

QT_BEGIN_NAMESPACE

class QOrmClassProperty;
class QOrmFilterExpressionPrivate;
class QVariant;

struct QOrmFilterTerminalPredicate;
struct QOrmFilterBinaryPredicate;
struct QOrmFilterUnaryPredicate;

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

struct QOrmFilterTerminalPredicate
{
    QOrmFilterTerminalPredicate(const QOrmClassProperty& classProperty,
                                QOrm::Comparison comparison,
                                const QVariant& value)
        : classProperty{classProperty},
          comparison{comparison},
          value{value}
    {
    }

    QOrmClassProperty classProperty;
    QOrm::Comparison comparison;
    QVariant value;
};

struct QOrmFilterBinaryPredicate
{
    QOrmFilterBinaryPredicate(const QOrmFilterExpression& lhs,
                              QOrm::BinaryLogicalOperator logicalOperator,
                              const QOrmFilterExpression& rhs)
        : lhs{lhs},
          logicalOperator{logicalOperator},
          rhs{rhs}
    {
    }

    QOrmFilterExpression lhs;
    QOrm::BinaryLogicalOperator logicalOperator;
    QOrmFilterExpression rhs;
};

struct QOrmFilterUnaryPredicate
{
    QOrmFilterUnaryPredicate(QOrm::UnaryLogicalOperator logicalOperator,
                             const QOrmFilterExpression& rhs)
        : logicalOperator{logicalOperator},
          rhs{rhs}
    {
    }

    QOrm::UnaryLogicalOperator logicalOperator;
    QOrmFilterExpression rhs;
};

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
QOrmFilterBinaryPredicate operator||(const QOrmFilterExpression& lhs, const QOrmFilterExpression& rhs);

Q_REQUIRED_RESULT
Q_ORM_EXPORT
QOrmFilterBinaryPredicate operator&&(const QOrmFilterExpression& lhs, const QOrmFilterExpression& rhs);

QT_END_NAMESPACE

#endif // QORMFILTERBUILDER_H
