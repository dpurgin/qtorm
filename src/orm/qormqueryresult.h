#ifndef QORMQUERYRESULT_H
#define QORMQUERYRESULT_H

#include <QtOrm/qormglobal.h>

#include <QtCore/qobject.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qvector.h>

QT_BEGIN_NAMESPACE

class QOrmError;
class QOrmQueryResultPrivate;

class Q_ORM_EXPORT QOrmQueryResult
{
public:
    QOrmQueryResult();    
    QOrmQueryResult(const QOrmQueryResult&);
    ~QOrmQueryResult();

    explicit QOrmQueryResult(const QOrmError& error);
    explicit QOrmQueryResult(const QVector<QObject*>& resultSet);

    QOrmQueryResult& operator=(const QOrmQueryResult&);

#ifdef Q_COMPILER_RVALUE_REFS
    QOrmQueryResult(QOrmQueryResult&&);
    QOrmQueryResult& operator=(QOrmQueryResult&&);
#endif

    QOrmError error() const;

    QVector<QObject*> toVector() const;

private:
    explicit QOrmQueryResult(const QOrmError& error,
                             const QVector<QObject*>& resultSet);

    QSharedDataPointer<QOrmQueryResultPrivate> d;
};

QT_END_NAMESPACE

#endif
