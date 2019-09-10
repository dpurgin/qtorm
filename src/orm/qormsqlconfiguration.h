#ifndef QORMSQLCONFIGURATION_H
#define QORMSQLCONFIGURATION_H

#include <QtOrm/qormglobal.h>
#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

class Q_ORM_EXPORT QOrmSqlConfiguration
{
public:
    enum class SchemaMode
    {
        Recreate,
        Update,
        Validate,
        Bypass
    };

public:
    Q_REQUIRED_RESULT
    QString connectOptions() const;
    void setConnectOptions(const QString& connectOptions);

    Q_REQUIRED_RESULT
    QString driverName() const;
    void setDriverName(const QString& driverName);

    Q_REQUIRED_RESULT
    QString hostName() const;
    void setHostName(const QString& hostName);

    Q_REQUIRED_RESULT
    QString databaseName() const;
    void setDatabaseName(const QString& databaseName);

    Q_REQUIRED_RESULT
    QString password() const;
    void setPassword(const QString& password);

    Q_REQUIRED_RESULT
    int port() const;
    void setPort(int port);

    Q_REQUIRED_RESULT
    QString userName() const;
    void setUserName(const QString& userName);

    Q_REQUIRED_RESULT
    bool verbose() const;
    void setVerbose(bool verbose);

    Q_REQUIRED_RESULT
    SchemaMode schemaMode() const;
    void setSchemaMode(SchemaMode schemaMode);

private:
    QString m_connectOptions;
    QString m_driverName;
    QString m_hostName;
    QString m_databaseName;
    QString m_password;
    int m_port{0};
    QString m_userName;
    bool m_verbose{false};
    SchemaMode m_schemaMode;
};

QT_END_NAMESPACE

#endif // QORMSQLCONFIGURATION_H
