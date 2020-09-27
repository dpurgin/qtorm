/*
 * Copyright (C) 2020 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2020 sequality software engineering e.U. <office@sequality.at>
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

#include "qormentitylistmodel.h"

QT_BEGIN_NAMESPACE

QOrmEntityListModelBase::QOrmEntityListModelBase(QObject* parent)
    : QAbstractListModel{parent}
{
}

QVariantMap QOrmEntityListModelBase::filter() const
{
    return m_filter;
}

void QOrmEntityListModelBase::setFilter(QVariantMap filter)
{
    if (m_filter != filter)
    {
        m_filter = filter;
        Q_EMIT filterChanged();
        onFilterChanged();
    }
}

QVariantList QOrmEntityListModelBase::order() const
{
    return m_order;
}

void QOrmEntityListModelBase::setOrder(QVariantList order)
{
    if (m_order != order)
    {
        m_order = order;
        Q_EMIT orderChanged();
        onOrderChanged();
    }
}

QT_END_NAMESPACE
