#include "qormfilter.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

QDebug operator<<(QDebug dbg, const QOrmFilter& filter)
{
    QDebugStateSaver saver{dbg};

    dbg.nospace().noquote() << "QOrmFilter(" << filter.type();

    switch (filter.type())
    {
        case QOrm::FilterType::Empty:
            dbg << "Empty";
            break;

        case QOrm::FilterType::Invokable:
            dbg << "Invokable";
            break;

        case QOrm::FilterType::Expression:
            dbg << "Expression, " << *filter.expression();
            break;
    }

    dbg << ")";

    return dbg;
}

QT_END_NAMESPACE
