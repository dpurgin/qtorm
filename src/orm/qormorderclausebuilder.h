#ifndef QORMORDERCLAUSEBUILDER_H
#define QORMORDERCLAUSEBUILDER_H

#include <QtOrm/qormglobal.h>

QT_BEGIN_NAMESPACE

class QOrmOrderClause;

class Q_ORM_EXPORT QOrmOrderClauseBuilder
{
public:
    QOrmOrderClauseBuilder();

    Q_REQUIRED_RESULT
    QOrmOrderClause build() const;
};

QT_END_NAMESPACE

#endif // QORMORDERCLAUSE_H
