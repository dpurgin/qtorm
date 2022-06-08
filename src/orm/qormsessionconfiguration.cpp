/*
 * Copyright (C) 2019-2021 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2019-2021 sequality software engineering e.U. <office@sequality.at>
 *
 * This file is part of QtOrm library.
 *
 * QtOrm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QtOrm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with QtOrm.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "qormsessionconfiguration.h"
#include "qormglobal_p.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QOrmMetadataCache>
#include <QOrmSqliteConfiguration>
#include <QOrmSqliteProvider>

#include <QtCore/qstringbuilder.h>

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
    sqlConfiguration.setConnectOptions(object["connectOptions"].toString());

    QString schemaModeStr = object["schemaMode"].toString("validate").toLower();

    static QHash<QString, QOrmSqliteConfiguration::SchemaMode> schemaModes = {
        {"recreate", QOrmSqliteConfiguration::SchemaMode::Recreate},
        {"create", QOrmSqliteConfiguration::SchemaMode::Recreate},
        {"update", QOrmSqliteConfiguration::SchemaMode::Update},
        {"validate", QOrmSqliteConfiguration::SchemaMode::Validate},
        {"bypass", QOrmSqliteConfiguration::SchemaMode::Bypass},
        {"append", QOrmSqliteConfiguration::SchemaMode::Append}};

    if (schemaModes.contains(schemaModeStr))
    {
        sqlConfiguration.setSchemaMode(schemaModes[schemaModeStr]);
    }
    else
    {
        qCWarning(qtorm)
            << "Invalid schemaMode in SQL provider configuration. Falling back to validate mode";
        sqlConfiguration.setSchemaMode(QOrmSqliteConfiguration::SchemaMode::Validate);
    }

    return sqlConfiguration;
}

QOrmSessionConfiguration QOrmSessionConfiguration::defaultConfiguration()
{
    static QStringList searchPaths = {":", ".", QCoreApplication::applicationDirPath()};

    for (const QString& searchPath : searchPaths)
    {
        QString filePath{searchPath % "/qtorm.json"};

        if (QFile::exists(filePath))
        {
            return fromFile(filePath);
        }
    }

    qFatal(
        "QtOrm: Unable to find a valid qtorm.json in resource, working directory, or application "
        "directory");
}

QOrmSessionConfiguration QOrmSessionConfiguration::fromFile(const QString& filePath)
{
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

    qFatal("qtorm: Unable to open session configuration file %s", qPrintable(filePath));
}

QOrmSessionConfiguration::QOrmSessionConfiguration(QOrmAbstractProvider* provider, bool isVerbose)
    : d{new QOrmSessionConfigurationData{provider, isVerbose}}
{
}

QOrmSessionConfiguration::QOrmSessionConfiguration(const QOrmSessionConfiguration&) = default;

QOrmSessionConfiguration::QOrmSessionConfiguration(QOrmSessionConfiguration&&) = default;

QOrmSessionConfiguration::~QOrmSessionConfiguration() = default;

QOrmSessionConfiguration& QOrmSessionConfiguration::operator=(const QOrmSessionConfiguration&) =
    default;

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
