#pragma once

#include <QObject>

class Town;

class Person : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(Town* town READ town WRITE setTown NOTIFY townChanged)

    int m_id;
    QString m_name;
    Town* m_town{nullptr};

public:
    Q_INVOKABLE Person() = default;
    explicit Person(QString name);

    int id() const { return m_id; }
    void setId(int id);

    QString name() const { return m_name; }
    void setName(QString name);

    Town* town() const { return m_town; }
    void setTown(Town* town);

signals:
    void idChanged(int id);
    void nameChanged(QString name);
    void townChanged(Town* town);
};
