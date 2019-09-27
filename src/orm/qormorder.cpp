#include "qormorder.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

QDebug operator<<(QDebug dbg, const QOrmOrder& order)
{
    QDebugStateSaver saver{dbg};
    dbg.noquote().nospace() << "QOrmOrder()";
    return dbg;
}
