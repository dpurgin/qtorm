#ifndef QORMQUERY_H
#define QORMQUERY_H

#include <QtCore/qglobal.h>
#include <QtCore/qshareddata.h>

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormqueryresult.h>

QT_BEGIN_NAMESPACE

class QOrmFilter;
class QOrmOrder;
class QOrmQueryPrivate;
class QOrmMetadata;

class Q_ORM_EXPORT QOrmQuery
{
public:
    QOrmQuery(QOrm::Operation operation,
              const QOrmMetadata& projection,
              const QOrmMetadata& relation,
              QOrmFilter filter,
              QOrmOrder order);    
    QOrmQuery(const QOrmQuery&);
    QOrmQuery(QOrmQuery&&);
    ~QOrmQuery();

    QOrmQuery& operator=(const QOrmQuery&);
    QOrmQuery& operator=(QOrmQuery&&);

    Q_REQUIRED_RESULT
    QOrm::Operation operation() const;

    Q_REQUIRED_RESULT
    const QOrmMetadata& projection() const;

    Q_REQUIRED_RESULT
    const QOrmMetadata& relation() const;

    Q_REQUIRED_RESULT
    QOrmFilter filter() const;

    Q_REQUIRED_RESULT
    QOrmOrder order() const;

private:
    QSharedDataPointer<QOrmQueryPrivate> d;
};

QT_END_NAMESPACE

#endif // QORMQUERY_H
