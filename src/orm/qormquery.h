#ifndef QORMQUERY_H
#define QORMQUERY_H

#include <QtOrm/qormglobal.h>

#include <QtCore/qobject.h>
#include <QtCore/qvector.h>

QT_BEGIN_NAMESPACE

class QOrmWhereClause;
class QOrmOrderClause;

class Q_ORM_EXPORT QOrmQuery
{
public:
    QOrmQuery();

    QOrmQuery& first(int n);
    QOrmQuery& last(int n);

    QOrmQuery& where(QOrmWhereClause whereClause);
    QOrmQuery& order(QOrmOrderClause orderClause);

    template<typename T = QObject>
    Q_REQUIRED_RESULT QVector<T*> toVector()
    {
        return QVector<T*>{};
    }
};

QT_END_NAMESPACE

#endif // QORMQUERY_H
