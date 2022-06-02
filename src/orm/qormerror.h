/*
 * Copyright (C) 2019-2021 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2019-2021 sequality software engineering e.U. <office@sequality.at>
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

#ifndef QORMERROR_H
#define QORMERROR_H

#include <QtOrm/qormglobal.h>

#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

class QDebug;

class Q_ORM_EXPORT QOrmError
{
public:
    QOrmError();
    QOrmError(QOrm::ErrorType type, const QString& text);

    [[nodiscard]] QOrm::ErrorType type() const;
    [[nodiscard]] QString text() const;

    [[nodiscard]] bool operator==(QOrm::ErrorType errorType) const { return m_type == errorType; }

    [[nodiscard]] bool operator!=(QOrm::ErrorType errorType) const
    {
        return !operator==(errorType);
    }

private:
    QOrm::ErrorType m_type{QOrm::ErrorType::None};
    QString m_text;
};

extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, const QOrmError& error);

QT_END_NAMESPACE

#endif // QORMERROR_H
