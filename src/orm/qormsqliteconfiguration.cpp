#include "qormsqliteconfiguration.h"

QT_BEGIN_NAMESPACE

QString QOrmSqliteConfiguration::connectOptions() const
{
    return m_connectOptions;
}

void QOrmSqliteConfiguration::setConnectOptions(const QString& connectOptions)
{
    m_connectOptions = connectOptions;
}

QString QOrmSqliteConfiguration::databaseName() const
{
    return m_databaseName;
}

void QOrmSqliteConfiguration::setDatabaseName(const QString& databaseName)
{
    m_databaseName = databaseName;
}

bool QOrmSqliteConfiguration::verbose() const
{
    return m_verbose;
}

void QOrmSqliteConfiguration::setVerbose(bool verbose)
{
    m_verbose = verbose;
}

QOrmSqliteConfiguration::SchemaMode QOrmSqliteConfiguration::schemaMode() const
{
    return m_schemaMode;
}

void QOrmSqliteConfiguration::setSchemaMode(SchemaMode schemaMode)
{
    m_schemaMode = schemaMode;
}

QT_END_NAMESPACE
