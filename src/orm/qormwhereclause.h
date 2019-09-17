#ifndef QORMWHERECLAUSE_H
#define QORMWHERECLAUSE_H

#include <QtOrm/qormglobal.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOrmClassProperty;
class QOrmWhereClausePrivate;
class QVariant;

class Q_ORM_EXPORT QOrmWhereClause
{
public:
    explicit QOrmWhereClause(const QOrmClassProperty& property,
                             QOrm::Comparison comparison,
                             const QVariant& value);
    QOrmWhereClause(const QOrmWhereClause&);
    QOrmWhereClause(QOrmWhereClause&&);
    ~QOrmWhereClause();

    QOrmWhereClause& operator=(const QOrmWhereClause&);
    QOrmWhereClause& operator=(QOrmWhereClause&&);

    Q_REQUIRED_RESULT
    QOrmClassProperty property() const;
    Q_REQUIRED_RESULT
    QOrm::Comparison comparison() const;
    Q_REQUIRED_RESULT
    QVariant value() const;

private:
    QSharedDataPointer<QOrmWhereClausePrivate> d;
};

QT_END_NAMESPACE

#endif
