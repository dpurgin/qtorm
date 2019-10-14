#pragma once

#include <QObject>
#include <QVector>

class Person;

class Town : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(
        QVector<Person*> population READ population WRITE setPopulation NOTIFY populationChanged)

    int m_id;
    QString m_name;
    QVector<Person*> m_population;

public:
    Q_INVOKABLE explicit Town(QObject* parent = nullptr);

    virtual ~Town() {}

    int id() const { return m_id; }
    void setId(int id);

    QString name() const { return m_name; }
    void setName(QString name);

    QVector<Person*> population() const { return m_population; }
    void setPopulation(QVector<Person*> population);

signals:
    void idChanged(int id);
    void nameChanged(QString name);
    void populationChanged(QVector<Person*> population);
};
