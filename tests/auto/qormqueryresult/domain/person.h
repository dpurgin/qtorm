#pragma once

#include <QObject>

class Person : public QObject
{
    Q_OBJECT

    Q_PROPERTY(long id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    Q_INVOKABLE Person() = default;

    [[nodiscard]] long id() const { return m_id; }
    void setId(long id)
    {
        if (m_id != id)
        {
            m_id = id;
            emit idChanged();
        }
    }

    [[nodiscard]] QString name() const { return m_name; }
    void setName(QString name)
    {
        if (m_name != name)
        {
            m_name = name;
            emit nameChanged();
        }
    }

signals:
    void idChanged();
    void nameChanged();

private:
    long m_id{-1};
    QString m_name;
};
