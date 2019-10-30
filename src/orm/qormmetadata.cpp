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

#include "qormmetadata.h"
#include "qormglobal_p.h"
#include "qormmetadata_p.h"

#include <QDebug>
#include <QMetaProperty>

QT_BEGIN_NAMESPACE

QOrmMetadata::QOrmMetadata(const QOrmMetadataPrivate* data)
    : d{data}
{
}

QOrmMetadata::QOrmMetadata(const QOrmMetadata&) = default;

QOrmMetadata::QOrmMetadata(QOrmMetadata&&) = default;

QOrmMetadata::~QOrmMetadata() = default;

QOrmMetadata& QOrmMetadata::operator=(const QOrmMetadata&) = default;

QOrmMetadata& QOrmMetadata::operator=(QOrmMetadata&&) = default;

const QMetaObject& QOrmMetadata::qMetaObject() const
{
    return d->m_qMetaObject;
}

QString QOrmMetadata::className() const
{
    return d->m_className;
}

QString QOrmMetadata::tableName() const
{
    return d->m_tableName;
}

const std::vector<QOrmPropertyMapping>& QOrmMetadata::propertyMappings() const
{
    return d->m_propertyMappings;
}

const QOrmPropertyMapping* QOrmMetadata::tableFieldMapping(const QString& fieldName) const
{
    auto it = d->m_tableFieldMappingIndex.find(fieldName);

    if (it == std::end(d->m_tableFieldMappingIndex))
        return nullptr;

    return &d->m_propertyMappings[it.value()];
}

const QOrmPropertyMapping* QOrmMetadata::classPropertyMapping(const QString& classProperty) const
{
    auto it = d->m_classPropertyMappingIndex.find(classProperty);

    if (it == std::end(d->m_classPropertyMappingIndex))
        return nullptr;

    return &d->m_propertyMappings[it.value()];
}

const QOrmPropertyMapping* QOrmMetadata::objectIdMapping() const
{
    return d->m_objectIdPropertyMappingIdx == -1
               ? nullptr
               : &d->m_propertyMappings[d->m_objectIdPropertyMappingIdx];
}

QDebug operator<<(QDebug dbg, const QOrmMetadata& metadata)
{
    QDebugStateSaver saver{dbg};
    dbg.nospace().nospace() << "QOrmMetadata("
                            << metadata.className() << " => " << metadata.tableName()
                            << ")";
    return dbg;
}

QT_END_NAMESPACE
