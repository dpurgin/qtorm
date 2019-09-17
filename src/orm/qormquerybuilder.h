#ifndef QORMQUERYBUILDER_H
#define QORMQUERYBUILDER_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormquery.h>

#include <QtCore/qobject.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qvector.h>

QT_BEGIN_NAMESPACE

class QOrmAbstractProvider;
class QOrmWhereClauseBuilder;
class QOrmOrderClauseBuilder;
class QOrmQueryBuilderPrivate;
class QOrmSession;

class Q_ORM_EXPORT QOrmQueryBuilder
{
public:
    QOrmQueryBuilder(QOrmSession* ormSession, const QMetaObject& relationMetaObject);
    QOrmQueryBuilder(const QOrmQueryBuilder&);
    QOrmQueryBuilder(QOrmQueryBuilder&&);
    ~QOrmQueryBuilder();

    QOrmQueryBuilder& operator=(const QOrmQueryBuilder&);
    QOrmQueryBuilder& operator=(QOrmQueryBuilder&&);

    QOrmQueryBuilder& where(QOrmWhereClauseBuilder whereClause);
    QOrmQueryBuilder& order(QOrmOrderClauseBuilder orderClause);

    QOrmQuery build() const;

    template<typename T>
    QOrmQueryBuilder& projection()
    {
        return projection(T::staticMetaObject);
    }

    QOrmQueryResult select();

    template<typename T>
    QOrmQueryResult select() const
    {
        return select(T::staticMetaObject);
    }


private:
    QOrmQueryBuilder& projection(const QMetaObject& projectionMetaObject);
    QOrmQueryResult select(const QMetaObject& projectionMetaObject) const;

    QSharedDataPointer<QOrmQueryBuilderPrivate> d;
};

QT_END_NAMESPACE

#endif // QORMQUERYBUILDER_H
