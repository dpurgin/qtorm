#ifndef QORMSQLCONFIGURATION_H
#define QORMSQLCONFIGURATION_H

#include <QtOrm/qormglobal.h>
#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

class Q_ORM_EXPORT QOrmSqliteConfiguration
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
    QString databaseName() const;
    void setDatabaseName(const QString& databaseName);

    Q_REQUIRED_RESULT
    bool verbose() const;
    void setVerbose(bool verbose);

    Q_REQUIRED_RESULT
    SchemaMode schemaMode() const;
    void setSchemaMode(SchemaMode schemaMode);

private:
    QString m_connectOptions;
    QString m_databaseName;
    bool m_verbose{false};
    SchemaMode m_schemaMode;
};

QT_END_NAMESPACE

#endif // QORMSQLCONFIGURATION_H
