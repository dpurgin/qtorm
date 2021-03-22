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

#ifndef QORMMETADATA_P_H
#define QORMMETADATA_P_H

#include "QtOrm/qormpropertymapping.h"

#include <QtCore/qshareddata.h>

#include <vector>

class QOrmMetadataPrivate : public QSharedData
{
public:
    QOrmMetadataPrivate(const QMetaObject& metaObject)
        : m_qMetaObject{metaObject}
    {
    }

    const QMetaObject& m_qMetaObject;

    QString m_className;
    QString m_tableName;
    std::vector<QOrmPropertyMapping> m_propertyMappings;

    int m_objectIdPropertyMappingIdx{-1};
    QHash<QString, int> m_classPropertyMappingIndex;
    QHash<QString, int> m_tableFieldMappingIndex;
    QOrmUserMetadata m_userMetadata;
};

#endif
