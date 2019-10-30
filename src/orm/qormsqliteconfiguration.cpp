/*
 * Copyright (C) 2019 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2019 sequality software engineering e.U. <office@sequality.at>
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
