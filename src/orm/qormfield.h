#ifndef QORMFIELD_H
#define QORMFIELD_H

#include <QtOrm/qormglobal.h>

QT_BEGIN_NAMESPACE

class Q_ORM_EXPORT QOrmField
{
public:
    explicit QOrmField(const char* descriptor);

};

#define Q_ORM_FIELD(name) (QOrmField{#name})

QT_END_NAMESPACE

#endif // QORMFIELD_H
