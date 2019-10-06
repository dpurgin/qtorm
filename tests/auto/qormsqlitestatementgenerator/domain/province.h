#pragma once

#include <QObject>
#include <QVector>

class Town;

class Province : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Province)

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QVector<Town*> towns READ towns WRITE setTowns NOTIFY townsChanged)

    int m_id;

    QString m_name;

    QVector<Town*> m_towns;

public:
    Q_INVOKABLE Province(QObject* parent = nullptr)
        : QObject(parent)
    {
    }
    explicit Province(const QString& name, QObject* parent = nullptr)
        : QObject{parent}
        , m_name{name}
    {
    }
    Province(int id, QString name, QObject* parent = nullptr)
        : QObject{parent}
        , m_id{id}
        , m_name{name}
    {
    }

    virtual ~Province() {}
    int id() const { return m_id; }
    QString name() const { return m_name; }

    QVector<Town*> towns() const { return m_towns; }

public slots:
    void setId(int id);
    void setName(QString name);
    void setTowns(QVector<Town*> towns);

signals:
    void idChanged(int id);
    void nameChanged(QString name);
    void townsChanged(QVector<Town*> towns);
};
