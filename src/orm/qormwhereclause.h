#ifndef QORMWHERECLAUSE_H
#define QORMWHERECLAUSE_H

#include <QtOrm/qormglobal.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOrmField;
class QOrmWhereClausePrivate;
class QVariant;

class Q_ORM_EXPORT QOrmWhereClause
{
public:
    explicit QOrmWhereClause(const QOrmField& field,
                             QOrm::Comparison comparison,
                             const QVariant& value);
    QOrmWhereClause(const QOrmWhereClause&);
    QOrmWhereClause(QOrmWhereClause&&);
    ~QOrmWhereClause();

    QOrmWhereClause& operator=(const QOrmWhereClause&);
    QOrmWhereClause& operator=(QOrmWhereClause&&);

    QOrmField field() const;
    QOrm::Comparison comparison() const;
    QVariant value() const;

private:
    QSharedDataPointer<QOrmWhereClausePrivate> d;
};

QT_END_NAMESPACE

#endif
