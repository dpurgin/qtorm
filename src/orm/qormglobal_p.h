#ifndef QORMGLOBAL_P_H
#define QORMGLOBAL_P_H

#include <QtOrm/qormglobal.h>

#include <QtCore/qhashfunctions.h>

QT_BEGIN_NAMESPACE

namespace std
{
    template<>
    struct hash<QString>
    {
        size_t operator()(const QString& s) const { return qHash(s); }
    };
} // namespace std

QT_END_NAMESPACE

#endif
