#ifndef QORMWHERECLAUSEBUILDER_H
#define QORMWHERECLAUSEBUILDER_H

#include <QtOrm/qormglobal.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOrmField;
class QOrmWhereClause;
class QOrmWhereClauseBuilderPrivate;
class QVariant;

class Q_ORM_EXPORT QOrmWhereClauseBuilder
{
public:    
    explicit QOrmWhereClauseBuilder();
    explicit QOrmWhereClauseBuilder(const QOrmField& field,
                                    QOrm::Comparison comparison,
                                    const QVariant& value);
    QOrmWhereClauseBuilder(const QOrmWhereClauseBuilder&);
    QOrmWhereClauseBuilder(QOrmWhereClauseBuilder&&);
    ~QOrmWhereClauseBuilder();

    QOrmWhereClauseBuilder& operator=(const QOrmWhereClauseBuilder&);
    QOrmWhereClauseBuilder& operator=(QOrmWhereClauseBuilder&&);

    Q_REQUIRED_RESULT
    QOrmWhereClause build() const;

private:
    QSharedDataPointer<QOrmWhereClauseBuilderPrivate> d;
};

Q_ORM_EXPORT QOrmWhereClauseBuilder operator==(const QOrmField& field, const QVariant& value);
Q_ORM_EXPORT QOrmWhereClauseBuilder operator!=(const QOrmField& field, const QVariant& value);
Q_ORM_EXPORT QOrmWhereClauseBuilder operator<(const QOrmField& field, const QVariant& value);
Q_ORM_EXPORT QOrmWhereClauseBuilder operator<=(const QOrmField& field, const QVariant& value);
Q_ORM_EXPORT QOrmWhereClauseBuilder operator>(const QOrmField& field, const QVariant& value);
Q_ORM_EXPORT QOrmWhereClauseBuilder operator>=(const QOrmField& field, const QVariant& value);

//Q_ORM_EXPORT QOrmWhereClause operator!(const QOrmWhereClause& operand);
//Q_ORM_EXPORT QOrmWhereClause operator||(const QOrmWhereClause& lhs, const QOrmWhereClause& rhs);
//Q_ORM_EXPORT QOrmWhereClause operator&&(const QOrmWhereClause& lhs, const QOrmWhereClause& rhs);

QT_END_NAMESPACE

#endif // QORMWHERECLAUSE_H
