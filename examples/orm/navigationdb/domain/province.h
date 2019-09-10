#ifndef PROVINCE_H
#define PROVINCE_H

#include <QObject>

class Province : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    int m_id;
    QString m_name;

public:
    explicit Province(QObject *parent = nullptr);

    int id() const;
    void setId(int id);

    QString name() const;
    void setName(QString name);

signals:
    void idChanged(int id);
    void nameChanged(QString name);
};

#endif // PROVINCE_H
