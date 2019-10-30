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

#include "qormfilter.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

QDebug operator<<(QDebug dbg, const QOrmFilter& filter)
{
    QDebugStateSaver saver{dbg};

    dbg.nospace().noquote() << "QOrmFilter(" << filter.type();

    switch (filter.type())
    {
        case QOrm::FilterType::Invokable:
            dbg << "Invokable";
            break;

        case QOrm::FilterType::Expression:
            dbg << "Expression, " << *filter.expression();
            break;
    }

    dbg << ")";

    return dbg;
}

QT_END_NAMESPACE
