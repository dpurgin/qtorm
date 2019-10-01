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
    QOrmQueryResult(QOrmQueryResult&&);
    ~QOrmQueryResult();

    explicit QOrmQueryResult(QOrmError error);
    explicit QOrmQueryResult(QVector<QObject*> resultSet);
    explicit QOrmQueryResult(QVariant lastInsertedId);

    QOrmQueryResult& operator=(const QOrmQueryResult&);
    QOrmQueryResult& operator=(QOrmQueryResult&&);

    Q_REQUIRED_RESULT
    const QOrmError& error() const;
    Q_REQUIRED_RESULT
    const QVariant& lastInsertedId() const;

    Q_REQUIRED_RESULT
    const QVector<QObject*>& toVector() const;

private:
    explicit QOrmQueryResult(QOrmError error, QVector<QObject*> resultSet, QVariant lastInsertedId);

    QSharedDataPointer<QOrmQueryResultPrivate> d;
};

QT_END_NAMESPACE

#endif
