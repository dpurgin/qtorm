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

#include "qormtransactiontoken.h"

#include <QOrmSession>

class QOrmTransactionTokenPrivate
{
    friend class QOrmTransactionToken;

    QOrmTransactionTokenPrivate(QOrmSession* session, QOrm::TransactionAction action)
        : m_session{session}
        , m_action{action}
    {
    }

    QOrmSession* m_session{nullptr};
    QOrm::TransactionAction m_action;
    bool m_engaged{true};
};

QOrmTransactionToken::QOrmTransactionToken(QOrmSession* session, QOrm::TransactionAction action)
    : d{new QOrmTransactionTokenPrivate{session, action}}
{
}

QOrmTransactionToken::QOrmTransactionToken(QOrmTransactionToken&&) = default;

QOrmTransactionToken& QOrmTransactionToken::operator=(QOrmTransactionToken&&) = default;

QOrmTransactionToken::~QOrmTransactionToken()
{
    if (d->m_engaged)
    {
        if (d->m_action == QOrm::TransactionAction::Commit)
            d->m_session->commitTransaction();
        else
            d->m_session->rollbackTransaction();
    }
}

bool QOrmTransactionToken::isEngaged() const
{
    return d->m_engaged;
}

void QOrmTransactionToken::disengage()
{
    d->m_engaged = false;
}
