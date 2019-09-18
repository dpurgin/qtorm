#ifndef QORMORDERCLAUSEBUILDER_H
#define QORMORDERCLAUSEBUILDER_H

#include <QtOrm/qormglobal.h>

QT_BEGIN_NAMESPACE

class QOrmOrder;

class Q_ORM_EXPORT QOrmOrderBuilder
{
public:
    QOrmOrderBuilder();

    Q_REQUIRED_RESULT
    QOrmOrder build() const;
};

QT_END_NAMESPACE

#endif // QORMORDERCLAUSE_H
