#ifndef PROVINCE_H
#define PROVINCE_H

#include <QObject>
#include <QVector>

class Community;

class Town : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QVector<Community*> communityList READ communityList WRITE setCommunityList NOTIFY
                   communityListChanged)

    int m_id;
    QString m_name;
    QVector<Community*> m_communityList;

public:
    Q_INVOKABLE explicit Town(QObject *parent = nullptr);
    explicit Town(const QString& name, QObject* parent = nullptr);

    int id() const;
    void setId(int id);

    QString name() const;
    void setName(QString name);

    QVector<Community*> communityList() const;
    void setCommunityList(const QVector<Community*>& communityList);

signals:
    void idChanged(int id);
    void nameChanged(QString name);
    void communityListChanged();
};

extern QDebug operator<<(QDebug dbg, const Town& province);

#endif // PROVINCE_H
