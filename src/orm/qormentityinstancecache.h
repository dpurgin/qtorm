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

#ifndef QORMENTITYINSTANCECACHE_H
#define QORMENTITYINSTANCECACHE_H

#include <QtCore/qglobal.h>
#include <QtCore/qscopedpointer.h>
#include <QtOrm/qormglobal.h>

QT_BEGIN_NAMESPACE

class QOrmEntityInstanceCachePrivate;
class QOrmMetadata;

class Q_ORM_EXPORT QOrmEntityInstanceCache
{
    Q_DISABLE_COPY(QOrmEntityInstanceCache)

public:
    QOrmEntityInstanceCache();
    ~QOrmEntityInstanceCache();

    QObject* get(const QOrmMetadata& meta, const QVariant& objectId);
    bool contains(const QObject* instance) const;
    void insert(const QOrmMetadata& meta, QObject* instance);
    QObject* take(QObject* instance);

    void finalize(const QOrmMetadata& metadata, QObject* instance);
    bool isModified(const QObject* instance) const;
    void markUnmodified(const QObject* instance) const;

private:
    QScopedPointer<QOrmEntityInstanceCachePrivate> d;
};

QT_END_NAMESPACE

#endif
