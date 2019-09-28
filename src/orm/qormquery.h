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
class QOrmRelation;
class QOrmMetadata;

class Q_ORM_EXPORT QOrmQuery
{
public:
    QOrmQuery(QOrm::Operation operation,
              const QOrmRelation& relation,
              std::optional<QOrmMetadata> projection,
              std::optional<QOrmFilter> filter,
              std::optional<QOrmOrder> order);
    QOrmQuery(const QOrmQuery&);
    QOrmQuery(QOrmQuery&&);
    ~QOrmQuery();

    QOrmQuery& operator=(const QOrmQuery&);
    QOrmQuery& operator=(QOrmQuery&&);

    Q_REQUIRED_RESULT
    QOrm::Operation operation() const;

    Q_REQUIRED_RESULT
    const QOrmRelation& relation() const;

    Q_REQUIRED_RESULT
    std::optional<QOrmMetadata> projection() const;

    Q_REQUIRED_RESULT
    std::optional<QOrmFilter> filter() const;

    Q_REQUIRED_RESULT
    std::optional<QOrmOrder> order() const;

private:
    QSharedDataPointer<QOrmQueryPrivate> d;
};

QDebug operator<<(QDebug dbg, const QOrmQuery& query);

QT_END_NAMESPACE

#endif // QORMQUERY_H
