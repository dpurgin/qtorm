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

#ifndef QORMMETADATA_H
#define QORMMETADATA_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormpropertymapping.h>

#include <QtCore/qstring.h>

#include <memory>
#include <vector>

QT_BEGIN_NAMESPACE

class QDebug;
class QOrmMetadataPrivate;

class Q_ORM_EXPORT QOrmMetadata
{
public:
    //    explicit QOrmMetadata(const QMetaObject& qMetaObject);
    explicit QOrmMetadata(const QOrmMetadataPrivate* data);

    QOrmMetadata(const QOrmMetadata&);
    QOrmMetadata(QOrmMetadata&&);
    ~QOrmMetadata();

    QOrmMetadata& operator=(const QOrmMetadata&);
    QOrmMetadata& operator=(QOrmMetadata&&);

    Q_REQUIRED_RESULT const QMetaObject& qMetaObject() const;

    Q_REQUIRED_RESULT QString className() const;
    Q_REQUIRED_RESULT QString tableName() const;

    Q_REQUIRED_RESULT
    const std::vector<QOrmPropertyMapping>& propertyMappings() const;
    Q_REQUIRED_RESULT
    const QOrmPropertyMapping* tableFieldMapping(const QString& fieldName) const;
    Q_REQUIRED_RESULT
    const QOrmPropertyMapping* classPropertyMapping(const QString& classProperty) const;
    Q_REQUIRED_RESULT
    const QOrmPropertyMapping* objectIdMapping() const;

private:
    QSharedDataPointer<const QOrmMetadataPrivate> d;
};

extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, const QOrmMetadata& metadata);

QT_END_NAMESPACE

#endif // QORMMETADATA_H
