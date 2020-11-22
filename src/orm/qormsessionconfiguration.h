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

#ifndef QORMSESSIONCONFIGURATION_H
#define QORMSESSIONCONFIGURATION_H

#include <QtOrm/qormglobal.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qstring.h>
#include <QtCore/qshareddata.h>

#include <memory>

QT_BEGIN_NAMESPACE

class QOrmAbstractProvider;
class QOrmSessionConfigurationData;

class Q_ORM_EXPORT QOrmSessionConfiguration
{
public:
    static QOrmSessionConfiguration defaultConfiguration();
    static QOrmSessionConfiguration fromFile(const QString& filePath);

public:
    QOrmSessionConfiguration(QOrmAbstractProvider* provider, bool isVerbose);
    QOrmSessionConfiguration(const QOrmSessionConfiguration&);
    QOrmSessionConfiguration(QOrmSessionConfiguration&&);
    ~QOrmSessionConfiguration();

    QOrmSessionConfiguration& operator=(const QOrmSessionConfiguration&);
    QOrmSessionConfiguration& operator=(QOrmSessionConfiguration&&);

    Q_REQUIRED_RESULT
    QOrmAbstractProvider* provider() const;

    Q_REQUIRED_RESULT
    bool isVerbose() const;

private:
    QSharedDataPointer<QOrmSessionConfigurationData> d;
};

QT_END_NAMESPACE

#endif // QORMSESSIONCONFIGURATION_H
