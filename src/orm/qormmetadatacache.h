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

#ifndef QORMMETADATACACHE_H
#define QORMMETADATACACHE_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormmetadata.h>

#include <memory>

QT_BEGIN_NAMESPACE

class QOrmMetadata;
class QOrmMetadataCachePrivate;
class QMetaObject;

class Q_ORM_EXPORT QOrmMetadataCache
{
    Q_DISABLE_COPY(QOrmMetadataCache)

public:
    QOrmMetadataCache();    
    QOrmMetadataCache(QOrmMetadataCache&&);
    ~QOrmMetadataCache();

    QOrmMetadataCache& operator=(QOrmMetadataCache&&);

    Q_REQUIRED_RESULT
    const QOrmMetadata& operator[](const QMetaObject& qMetaObject);

    template<typename T>
    Q_REQUIRED_RESULT
    const QOrmMetadata& get()
    {
        return operator[](T::staticMetaObject);
    }

    Q_REQUIRED_RESULT
    const QOrmMetadata& get(const QMetaObject& qMetaObject) { return operator[](qMetaObject); }

private:
    std::unique_ptr<QOrmMetadataCachePrivate> d;
};

QT_END_NAMESPACE

#endif // QORMMETADATACACHE_H
