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

#ifndef QORMCLASSPROPERTY_H
#define QORMCLASSPROPERTY_H

#include <QtCore/qstring.h>

#include <QtOrm/qormglobal.h>

QT_BEGIN_NAMESPACE

class Q_ORM_EXPORT QOrmClassProperty
{
public:
    explicit QOrmClassProperty(const char* descriptor);

    QString descriptor() const;

private:
    QString m_descriptor;
};

extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, const QOrmClassProperty& classProperty);

#define Q_ORM_CLASS_PROPERTY(descriptor) (QOrmClassProperty{#descriptor})

QT_END_NAMESPACE

#endif // QORMCLASSPROPERTY_H
