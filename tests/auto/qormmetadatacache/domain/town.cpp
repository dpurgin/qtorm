#include "town.h"

Town::Town(QObject* parent)
    : QObject(parent)
{
}

void Town::setId(int id)
{
    if (m_id == id)
        return;

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

void Town::setPopulation(QVector<Person*> population)
{
    if (m_population == population)
        return;

    m_population = population;
    emit populationChanged(m_population);
}
