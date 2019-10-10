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

class QOrmSessionConfigurationData /* : public QSharedData*/
{
    friend class QOrmSessionConfiguration;

    std::unique_ptr<QOrmAbstractProvider> m_provider;
    bool m_isVerbose{false};
};

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

                QOrmSessionConfiguration sessionConfiguration;

                if (rootObject["provider"].toString().compare("sqlite") == 0)
                {
                    QOrmSqliteConfiguration sqlConfiguration =
                        _build_json_sqlite_configuration(rootObject["sqlite"].toObject());
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

// QOrmSessionConfiguration::QOrmSessionConfiguration(const QOrmSessionConfiguration&) = default;

QOrmSessionConfiguration::QOrmSessionConfiguration(QOrmSessionConfiguration&&) = default;

QOrmSessionConfiguration::~QOrmSessionConfiguration() = default;

// QOrmSessionConfiguration& QOrmSessionConfiguration::operator=(const QOrmSessionConfiguration&) =
// default;

QOrmSessionConfiguration& QOrmSessionConfiguration::operator=(QOrmSessionConfiguration&&) = default;

QOrmAbstractProvider* QOrmSessionConfiguration::provider() const
{
    Q_ASSERT(d != nullptr);

    return d->m_provider.get();
}

void QOrmSessionConfiguration::setProvider(QOrmAbstractProvider* provider)
{    
    Q_ASSERT(d != nullptr);

    d->m_provider.reset(provider);
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
