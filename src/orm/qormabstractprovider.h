/*
 * Copyright (C) 2019 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2019-2022 sequality software engineering e.U. <office@sequality.at>
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

#ifndef QORMABSTRACTPROVIDER_H
#define QORMABSTRACTPROVIDER_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormqueryresult.h>

QT_BEGIN_NAMESPACE

class QObject;
class QOrmEntityInstanceCache;
class QOrmError;
class QOrmMetadataCache;
class QOrmQuery;

class Q_ORM_EXPORT QOrmAbstractProvider
{
public:
    virtual ~QOrmAbstractProvider();        

    virtual QOrmError connectToBackend() = 0;
    virtual QOrmError disconnectFromBackend() = 0;
    virtual bool isConnectedToBackend() = 0;

    virtual QOrmError beginTransaction() = 0;
    virtual QOrmError commitTransaction() = 0;
    virtual QOrmError rollbackTransaction() = 0;

    virtual QOrmQueryResult<QObject> execute(const QOrmQuery& query,
                                             QOrmEntityInstanceCache& entityInstanceCache) = 0;

    [[nodiscard]] virtual int capabilities() const = 0;
};

QT_END_NAMESPACE

#endif // QORMABSTRACTPROVIDER_H
