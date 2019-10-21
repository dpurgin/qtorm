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

#ifndef QORMTRANSACTIONTOKEN_H
#define QORMTRANSACTIONTOKEN_H

#include <QtOrm/qormglobal.h>

#include <memory>

QT_BEGIN_NAMESPACE

class QOrmSession;
class QOrmTransactionTokenPrivate;

class Q_ORM_EXPORT QOrmTransactionToken
{
public:
    QOrmTransactionToken(QOrmSession* session, QOrm::TransactionAction finalAction);
    QOrmTransactionToken(const QOrmTransactionToken&) = delete;
    QOrmTransactionToken(QOrmTransactionToken&&);
    ~QOrmTransactionToken();

    QOrmTransactionToken& operator=(const QOrmTransactionToken&) = delete;
    QOrmTransactionToken& operator=(QOrmTransactionToken&&);

    Q_REQUIRED_RESULT bool isEngaged() const;
    void disengage();

private:
    std::unique_ptr<QOrmTransactionTokenPrivate> d;
};

QT_END_NAMESPACE

#endif // QORMTRANSACTIONTOKEN_H
