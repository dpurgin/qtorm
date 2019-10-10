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
    friend class QOrmSessionConfiguration;

    QOrmSessionConfigurationData(QOrmAbstractProvider* provider, bool isVerbose);

    std::unique_ptr<QOrmAbstractProvider> m_provider;
    bool m_isVerbose{false};
};

QOrmSessionConfigurationData::QOrmSessionConfigurationData(QOrmAbstractProvider* provider,
                                                           bool isVerbose)
    : m_provider{provider}
    , m_isVerbose{isVerbose}
{
    Q_ASSERT(provider != nullptr);
}

static QOrmSqliteConfiguration _build_json_sqlite_configuration(const QJsonObject& object)
{
    QOrmSqliteConfiguration sqlConfiguration;

    sqlConfiguration.setDatabaseName(object["databaseName"].toString());
    sqlConfiguration.setVerbose(object["verbose"].toBool(false));

    QString schemaModeStr = object["schemaMode"].toString("validate");

    if (schemaModeStr.compare("recreate", Qt::CaseInsensitive) == 0 ||
            schemaModeStr.compare("create", Qt::CaseInsensitive) == 0)
    {
        sqlConfiguration.setSchemaMode(QOrmSqliteConfiguration::SchemaMode::Recreate);
    }
    else if (schemaModeStr.compare("update", Qt::CaseInsensitive) == 0)
    {
        sqlConfiguration.setSchemaMode(QOrmSqliteConfiguration::SchemaMode::Update);
    }
    else if (schemaModeStr.compare("validate", Qt::CaseInsensitive) == 0)
    {
        sqlConfiguration.setSchemaMode(QOrmSqliteConfiguration::SchemaMode::Validate);
    }
    else if (schemaModeStr.compare("bypass", Qt::CaseInsensitive) == 0)
    {
        sqlConfiguration.setSchemaMode(QOrmSqliteConfiguration::SchemaMode::Bypass);
    }
    else
    {
        qWarning() << "QtORM: Invalid schemaMode in SQL provider configuration. Falling back to validate mode";
        sqlConfiguration.setSchemaMode(QOrmSqliteConfiguration::SchemaMode::Validate);
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

                std::unique_ptr<QOrmAbstractProvider> provider;
                bool isVerbose = rootObject["verbose"].toBool(false);

                if (rootObject["provider"].toString().compare("sqlite") == 0)
                {
                    QOrmSqliteConfiguration sqlConfiguration =
                        _build_json_sqlite_configuration(rootObject["sqlite"].toObject());
                    provider = std::make_unique<QOrmSqliteProvider>(sqlConfiguration);
                }

                return QOrmSessionConfiguration{provider.release(), isVerbose};
            }
        }
    }

    qFatal(
        "QtOrm: Unable to find a valid qtorm.json in resource, working directory, or application "
        "directory");
}

QOrmSessionConfiguration::QOrmSessionConfiguration(QOrmAbstractProvider* provider, bool isVerbose)
    : d{new QOrmSessionConfigurationData{provider, isVerbose}}
{
}

QOrmSessionConfiguration::QOrmSessionConfiguration(const QOrmSessionConfiguration&) = default;

QOrmSessionConfiguration::QOrmSessionConfiguration(QOrmSessionConfiguration&&) = default;

QOrmSessionConfiguration::~QOrmSessionConfiguration() = default;

QOrmSessionConfiguration& QOrmSessionConfiguration::
operator=(const QOrmSessionConfiguration&) = default;

QOrmSessionConfiguration& QOrmSessionConfiguration::operator=(QOrmSessionConfiguration&&) = default;

QOrmAbstractProvider* QOrmSessionConfiguration::provider() const
{
    return d->m_provider.get();
}

bool QOrmSessionConfiguration::isVerbose() const
{
    return d->m_isVerbose;
}

QT_END_NAMESPACE
