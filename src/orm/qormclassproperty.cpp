/*
 * Copyright (C) 2019-2022 Dmitriy Purgin <dmitriy.purgin@sequality.at>
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

#include "qormclassproperty.h"
#include "qormfilterexpression.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

QDebug operator<<(QDebug dbg, const QOrmClassProperty& classProperty)
{
    QDebugStateSaver saver{dbg};

    dbg.noquote().nospace() << "QOrmClassProperty(\"" << classProperty.descriptor() << "\")";

    return dbg;
}

QOrmClassProperty::QOrmClassProperty(const char* descriptor)
    : m_descriptor{descriptor}
{
}

QString QOrmClassProperty::descriptor() const
{
    return m_descriptor;
}

QOrmFilterExpression QOrmClassProperty::contains(const QVariant& value) const
{
    return QOrmFilterTerminalPredicate{*this, QOrm::Comparison::Contains, value};
}

QOrmFilterExpression QOrmClassProperty::notContains(const QVariant& value) const
{
    return QOrmFilterTerminalPredicate{*this, QOrm::Comparison::NotContains, value};
}

QT_END_NAMESPACE
