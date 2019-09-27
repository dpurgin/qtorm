#ifndef QORMRELATION_H
#define QORMRELATION_H

#include <QtOrm/qormglobal.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOrmMetadata;
class QOrmQuery;
class QOrmRelationPrivate;

class Q_ORM_EXPORT QOrmRelation
{
public:
    explicit QOrmRelation(const QOrmMetadata& mapping);
    explicit QOrmRelation(const QOrmQuery& query);
    QOrmRelation(const QOrmRelation&);
    QOrmRelation(QOrmRelation&&);
    ~QOrmRelation();

    QOrmRelation& operator=(const QOrmRelation&);
    QOrmRelation& operator=(QOrmRelation&&);

    QOrm::RelationType type() const;
    const QOrmMetadata* mapping() const;
    const QOrmQuery* query() const;

private:
    QSharedDataPointer<QOrmRelationPrivate> d;
};

extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, const QOrmRelation& relation);

QT_END_NAMESPACE

#endif
