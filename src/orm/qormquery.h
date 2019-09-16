#ifndef QORMQUERY_H
#define QORMQUERY_H

#include <QtCore/qglobal.h>
#include <QtCore/qshareddata.h>

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormqueryresult.h>

QT_BEGIN_NAMESPACE

class QOrmWhereClause;
class QOrmOrderClause;
class QOrmQueryPrivate;

class Q_ORM_EXPORT QOrmQuery
{
public:
    QOrmQuery(QOrm::Operation operation,
              const QMetaObject& projection,
              const QMetaObject& relation,
              QOrmWhereClause where,
              QOrmOrderClause order);
    QOrmQuery(const QOrmQuery&);
    QOrmQuery(QOrmQuery&&);
    ~QOrmQuery();

    QOrmQuery& operator=(const QOrmQuery&);
    QOrmQuery& operator=(QOrmQuery&&);

    Q_REQUIRED_RESULT
    QOrm::Operation operation() const;

    Q_REQUIRED_RESULT
    const QMetaObject& projection() const;
    Q_REQUIRED_RESULT
    const QMetaObject& relation() const;
    Q_REQUIRED_RESULT
    QOrmWhereClause where() const;
    Q_REQUIRED_RESULT
    QOrmOrderClause order() const;   

private:
    QSharedDataPointer<QOrmQueryPrivate> d;
};

QT_END_NAMESPACE

#endif // QORMQUERY_H
