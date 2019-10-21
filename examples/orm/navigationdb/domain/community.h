#ifndef COMMUNITY_H
#define COMMUNITY_H

#include <QObject>

class Province;

class Community : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(Province* province READ province WRITE setProvince NOTIFY provinceChanged)

    int m_id;
    QString m_name;
    Province* m_province{nullptr};

public:
    Q_INVOKABLE explicit Community(QObject* parent = nullptr);
    explicit Community(QString name, Province* province, QObject* parent = nullptr);

    int id() const { return m_id; }
    void setId(int id);

    QString name() const { return m_name; }
    void setName(QString name);

    Province* province() const { return m_province; }
    void setProvince(Province* province);

signals:
    void idChanged();
    void nameChanged();
    void provinceChanged();
};

#endif // COMMUNITY_H
