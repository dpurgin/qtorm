#include "province.h"

#include <QDebug>
#include <QDebugStateSaver>

QDebug operator<<(QDebug dbg, const Province &province)
{
    QDebugStateSaver saver{dbg};
    dbg.noquote().nospace()
        << "Province(" << province.id() << ", \"" << province.name() << "\")";
    return dbg;
}

Province::Province(QObject *parent) : QObject(parent)
{
}

Province::Province(const QString &name, QObject *parent)
    : QObject{parent}, m_name{name}
{
}

int Province::id() const
{
    return m_id;
}

QString Province::name() const
{
    return m_name;
}

void Province::setId(int id)
{
    if (m_id == id)
    {
        return;
    }

    m_id = id;
    emit idChanged(m_id);
}

void Province::setName(QString name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(m_name);
}
