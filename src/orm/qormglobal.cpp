#include "qormglobal.h"

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

namespace QOrm
{
    uint qHash(QOrm::Comparison comparison) Q_DECL_NOTHROW
    {
        return ::qHash(static_cast<int>(comparison));
    }
}

QDebug operator<<(QDebug dbg, QOrm::Error error)
{
    QDebugStateSaver saver{dbg};

    dbg.nospace() << "QOrm::Error::";

    switch (error)
    {
        case QOrm::Error::None: dbg << "None"; break;
        case QOrm::Error::Other: dbg << "Other"; break;
        case QOrm::Error::Provider: dbg << "Provider"; break;
        case QOrm::Error::UnsynchronizedEntity: dbg << "UnsynchronizedEntity"; break;
        case QOrm::Error::UnsynchronizedSchema: dbg << "UnsynchronizedSchema"; break;
        case QOrm::Error::InvalidMapping: dbg << "InvalidMapping"; break;
    }

    return dbg;
}

QT_END_NAMESPACE
