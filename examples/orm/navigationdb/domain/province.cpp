#include "province.h"

#include <QDebug>
#include <QDebugStateSaver>

QDebug operator<<(QDebug dbg, const Town &province)
{
    QDebugStateSaver saver{dbg};
    dbg.noquote().nospace()
        << "Province(" << province.id() << ", \"" << province.name() << "\")";
    return dbg;
}

Town::Town(QObject *parent) : QObject(parent)
{
}

Town::Town(const QString &name, QObject *parent)
    : QObject{parent}, m_name{name}
{
}

int Town::id() const
{
    return m_id;
}

QString Town::name() const
{
    return m_name;
}

void Town::setId(int id)
{
    if (m_id == id)
    {
        return;
    }

    m_id = id;
    emit idChanged(m_id);
}

void Town::setName(QString name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(m_name);
}

QVector<Community*> Town::communityList() const
{
    return m_communityList;
}

void Town::setCommunityList(const QVector<Community*>& communityList)
{
    if (m_communityList == communityList)
        return;

    m_communityList = communityList;
    emit communityListChanged();
}
