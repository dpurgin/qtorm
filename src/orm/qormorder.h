#ifndef QORMORDERCLAUSE_H
#define QORMORDERCLAUSE_H

#include <QtOrm/qormglobal.h>

QT_BEGIN_NAMESPACE

class Q_ORM_EXPORT QOrmOrder
{
};

extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, const QOrmOrder& order);

QT_END_NAMESPACE

#endif
