#include "qormsqlconfiguration.h"

QT_BEGIN_NAMESPACE

QString QOrmSqlConfiguration::connectOptions() const
{
    return m_connectOptions;
}

void QOrmSqlConfiguration::setConnectOptions(const QString& connectOptions)
{
    m_connectOptions = connectOptions;
}

QString QOrmSqlConfiguration::driverName() const
{
    return m_driverName;
}

void QOrmSqlConfiguration::setDriverName(const QString& driverName)
{
    m_driverName = driverName;
}

QString QOrmSqlConfiguration::hostName() const
{
    return m_hostName;
}

void QOrmSqlConfiguration::setHostName(const QString& hostName)
{
    m_hostName = hostName;
}

QString QOrmSqlConfiguration::databaseName() const
{
    return m_databaseName;
}

void QOrmSqlConfiguration::setDatabaseName(const QString& databaseName)
{
    m_databaseName = databaseName;
}

QString QOrmSqlConfiguration::password() const
{
    return m_password;
}

void QOrmSqlConfiguration::setPassword(const QString& password)
{
    m_password = password;
}

int QOrmSqlConfiguration::port() const
{
    return m_port;
}

void QOrmSqlConfiguration::setPort(int port)
{
    m_port = port;
}

QString QOrmSqlConfiguration::userName() const
{
    return m_userName;
}

void QOrmSqlConfiguration::setUserName(const QString& userName)
{
    m_userName = userName;
}

bool QOrmSqlConfiguration::verbose() const
{
    return m_verbose;
}

void QOrmSqlConfiguration::setVerbose(bool verbose)
{
    m_verbose = verbose;
}

QOrmSqlConfiguration::SchemaMode QOrmSqlConfiguration::schemaMode() const
{
    return m_schemaMode;
}

void QOrmSqlConfiguration::setSchemaMode(SchemaMode schemaMode)
{
    m_schemaMode = schemaMode;
}

QT_END_NAMESPACE
