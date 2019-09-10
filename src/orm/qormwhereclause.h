#ifndef QORMWHERECLAUSE_H
#define QORMWHERECLAUSE_H

#include <QtOrm/qormglobal.h>

QT_BEGIN_NAMESPACE

class QOrmField;
class QVariant;

class Q_ORM_EXPORT QOrmWhereClause
{
public:
    QOrmWhereClause();
};

Q_ORM_EXPORT QOrmWhereClause operator==(const QOrmField& field, const QVariant& value);
Q_ORM_EXPORT QOrmWhereClause operator!=(const QOrmField& field, const QVariant& value);
Q_ORM_EXPORT QOrmWhereClause operator<(const QOrmField& field, const QVariant& value);
Q_ORM_EXPORT QOrmWhereClause operator<=(const QOrmField& field, const QVariant& value);
Q_ORM_EXPORT QOrmWhereClause operator>(const QOrmField& field, const QVariant& value);
Q_ORM_EXPORT QOrmWhereClause operator>=(const QOrmField& field, const QVariant& value);

Q_ORM_EXPORT QOrmWhereClause operator!(const QOrmWhereClause& operand);
Q_ORM_EXPORT QOrmWhereClause operator||(const QOrmWhereClause& lhs, const QOrmWhereClause& rhs);
Q_ORM_EXPORT QOrmWhereClause operator&&(const QOrmWhereClause& lhs, const QOrmWhereClause& rhs);

QT_END_NAMESPACE

#endif // QORMWHERECLAUSE_H
