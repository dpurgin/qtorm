#ifndef QORMQUERY_H
#define QORMQUERY_H

#include <QtCore/qglobal.h>
#include <QtCore/qshareddata.h>

#include <QtOrm/qormglobal.h>

QT_BEGIN_NAMESPACE

class QOrmWhereClause;
class QOrmOrderClause;
class QOrmQueryPrivate;

class Q_ORM_EXPORT QOrmQuery
{
public:
    QOrmQuery(const QMetaObject& projection,
              int first,
              int last,
              QOrmWhereClause where,
              QOrmOrderClause order);
    QOrmQuery(const QOrmQuery&);
    ~QOrmQuery();

    QOrmQuery& operator=(const QOrmQuery&);

#ifdef Q_COMPILER_RVALUE_REFS
    QOrmQuery(QOrmQuery&&);
    QOrmQuery& operator=(QOrmQuery&&);
#endif

    const QMetaObject& projection() const;

    int first() const;
    int last() const;
    QOrmWhereClause where() const;
    QOrmOrderClause order() const;

private:
    QSharedDataPointer<QOrmQueryPrivate> d;
};

QT_END_NAMESPACE

#endif // QORMQUERY_H
