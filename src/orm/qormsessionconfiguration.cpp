#include "qormsessionconfiguration.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>
#include <QOrmMetadataCache>
#include <QOrmSqlConfiguration>
#include <QOrmSqliteProvider>

QT_BEGIN_NAMESPACE

class QOrmSessionConfigurationData : public QSharedData
{
public:
    ~QOrmSessionConfigurationData();

    QOrmAbstractProvider* m_provider{nullptr};
    bool m_isVerbose{false};
};

QOrmSessionConfigurationData::~QOrmSessionConfigurationData()
{
    delete m_provider;
}

static QOrmSqlConfiguration _build_json_sql_configuration(const QJsonObject& object)
{
    QOrmSqlConfiguration sqlConfiguration;

    sqlConfiguration.setDriverName(object["driverName"].toString("QSQLITE"));
    sqlConfiguration.setHostName(object["hostName"].toString());
    sqlConfiguration.setDatabaseName(object["databaseName"].toString());
    sqlConfiguration.setPassword(object["password"].toString());
    sqlConfiguration.setPort(object["port"].toInt(0));
    sqlConfiguration.setUserName(object["userName"].toString());
    sqlConfiguration.setVerbose(object["verbose"].toBool(false));

    QString schemaModeStr = object["schemaMode"].toString("validate");

    if (schemaModeStr.compare("recreate", Qt::CaseInsensitive) == 0 ||
            schemaModeStr.compare("create", Qt::CaseInsensitive) == 0)
    {
        sqlConfiguration.setSchemaMode(QOrmSqlConfiguration::SchemaMode::Recreate);
    }
    else if (schemaModeStr.compare("update", Qt::CaseInsensitive) == 0)
    {
        sqlConfiguration.setSchemaMode(QOrmSqlConfiguration::SchemaMode::Update);
    }
    else if (schemaModeStr.compare("validate", Qt::CaseInsensitive) == 0)
    {
        sqlConfiguration.setSchemaMode(QOrmSqlConfiguration::SchemaMode::Validate);
    }
    else if (schemaModeStr.compare("bypass", Qt::CaseInsensitive) == 0)
    {
        sqlConfiguration.setSchemaMode(QOrmSqlConfiguration::SchemaMode::Bypass);
    }
    else
    {
        qWarning() << "QtORM: Invalid schemaMode in SQL provider configuration. Falling back to validate mode";
        sqlConfiguration.setSchemaMode(QOrmSqlConfiguration::SchemaMode::Validate);
    }

    return sqlConfiguration;
}

QOrmSessionConfiguration QOrmSessionConfiguration::defaultConfiguration()
{
    static QStringList searchPaths = {
        ":",
        ".",
        QCoreApplication::applicationDirPath()
    };

    for (const QString& searchPath: searchPaths)
    {
        QString filePath{searchPath % "/qtorm.json"};

        QFile file{filePath};

        if (file.open(QFile::ReadOnly | QFile::Text))
        {
            QJsonParseError jsonParseError;
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &jsonParseError);

            if (jsonParseError.error == QJsonParseError::NoError && doc.isObject())
            {
                QJsonObject rootObject = doc.object();

                QOrmSessionConfiguration sessionConfiguration;

                if (rootObject["provider"].toString().compare("sql") == 0)
                {
                    QOrmSqlConfiguration sqlConfiguration =
                            _build_json_sql_configuration(rootObject["sql"].toObject());
                    sessionConfiguration.setProvider(new QOrmSqliteProvider{sqlConfiguration});
                }

                sessionConfiguration.setVerbose(rootObject["verbose"].toBool(false));

                return sessionConfiguration;
            }
        }
    }

    qWarning() << "Unable to find a valid qtorm.json in" << searchPaths;

    return {};
}

QOrmSessionConfiguration::QOrmSessionConfiguration()
    : d{new QOrmSessionConfigurationData}
{
}

QOrmSessionConfiguration::QOrmSessionConfiguration(const QOrmSessionConfiguration&) = default;

#ifdef Q_COMPILER_RVALUE_REFS
QOrmSessionConfiguration::QOrmSessionConfiguration(QOrmSessionConfiguration&&) = default;
#endif

QOrmSessionConfiguration::~QOrmSessionConfiguration() = default;

QOrmSessionConfiguration& QOrmSessionConfiguration::operator=(const QOrmSessionConfiguration&) = default;

#ifdef Q_COMPILER_RVALUE_REFS
QOrmSessionConfiguration& QOrmSessionConfiguration::operator=(QOrmSessionConfiguration&&) = default;
#endif

QOrmAbstractProvider* QOrmSessionConfiguration::provider() const
{
    Q_ASSERT(d != nullptr);

    return d->m_provider;
}

void QOrmSessionConfiguration::setProvider(QOrmAbstractProvider* provider)
{    
    Q_ASSERT(d != nullptr);

    d->m_provider = provider;
}

bool QOrmSessionConfiguration::isValid() const
{
    Q_ASSERT(d != nullptr);

    return d->m_provider != nullptr;
}

bool QOrmSessionConfiguration::isVerbose() const
{
    Q_ASSERT(d != nullptr);

    return d->m_isVerbose;
}

void QOrmSessionConfiguration::setVerbose(bool isVerbose)
{
    Q_ASSERT(d != nullptr);

    d->m_isVerbose = isVerbose;
}

QT_END_NAMESPACE
