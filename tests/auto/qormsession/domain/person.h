#pragma once

#include <QObject>

class Town;

class Person : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString firstName READ firstName WRITE setFirstName NOTIFY firstNameChanged)
    Q_PROPERTY(QString lastName READ lastName WRITE setLastName NOTIFY lastNameChanged)
    Q_PROPERTY(Town* town READ town WRITE setTown NOTIFY townChanged)

    int m_id;
    QString m_firstName;
    QString m_lastName;
    Town* m_town{nullptr};

public:
    Q_INVOKABLE Person() = default;
    Person(QString firstName, QString lastName, Town* town)
        : m_firstName{firstName}
        , m_lastName{lastName}
        , m_town{town}
    {
    }
    explicit Person(QString name);

    int id() const { return m_id; }
    void setId(int id);

    QString firstName() const { return m_firstName; }
    void setFirstName(QString firstName);

    Town* town() const { return m_town; }
    void setTown(Town* town);

    QString lastName() const;
    void setLastName(QString lastName);

signals:
    void idChanged(int id);
    void firstNameChanged(QString firstName);
    void lastNameChanged(QString lastName);
    void townChanged(Town* town);
};
