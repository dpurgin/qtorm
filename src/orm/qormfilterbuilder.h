#ifndef QORMFILTERBUILDER_H
#define QORMFILTERBUILDER_H

#include <QtOrm/qormglobal.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOrmClassProperty;
class QOrmFilter;
class QOrmFilterBuilderPrivate;
class QVariant;

class Q_ORM_EXPORT QOrmFilterBuilder
{
public:    
    explicit QOrmFilterBuilder(const QOrmClassProperty& field,
                                    QOrm::Comparison comparison,
                                    const QVariant& value);
    QOrmFilterBuilder(const QOrmFilterBuilder&);
    QOrmFilterBuilder(QOrmFilterBuilder&&);
    ~QOrmFilterBuilder();

    QOrmFilterBuilder& operator=(const QOrmFilterBuilder&);
    QOrmFilterBuilder& operator=(QOrmFilterBuilder&&);

    Q_REQUIRED_RESULT
    QOrmFilter build() const;

private:
    QSharedDataPointer<QOrmFilterBuilderPrivate> d;
};

Q_ORM_EXPORT QOrmFilterBuilder operator==(const QOrmClassProperty& property, const QVariant& value);
Q_ORM_EXPORT QOrmFilterBuilder operator!=(const QOrmClassProperty& property, const QVariant& value);
Q_ORM_EXPORT QOrmFilterBuilder operator<(const QOrmClassProperty& property, const QVariant& value);
Q_ORM_EXPORT QOrmFilterBuilder operator<=(const QOrmClassProperty& property, const QVariant& value);
Q_ORM_EXPORT QOrmFilterBuilder operator>(const QOrmClassProperty& property, const QVariant& value);
Q_ORM_EXPORT QOrmFilterBuilder operator>=(const QOrmClassProperty& property, const QVariant& value);

//Q_ORM_EXPORT QOrmWhereClause operator!(const QOrmWhereClause& operand);
//Q_ORM_EXPORT QOrmWhereClause operator||(const QOrmWhereClause& lhs, const QOrmWhereClause& rhs);
//Q_ORM_EXPORT QOrmWhereClause operator&&(const QOrmWhereClause& lhs, const QOrmWhereClause& rhs);

QT_END_NAMESPACE

#endif // QORMFILTERBUILDER_H
