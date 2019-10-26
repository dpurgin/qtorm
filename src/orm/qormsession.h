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

#ifndef QORMSESSION_H
#define QORMSESSION_H

#include <QtOrm/qormclassproperty.h>
#include <QtOrm/qormglobal.h>
#include <QtOrm/qormmetadata.h>
#include <QtOrm/qormquerybuilder.h>
#include <QtOrm/qormqueryresult.h>
#include <QtOrm/qormsessionconfiguration.h>
#include <QtOrm/qormtransactiontoken.h>

#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

class QOrmAbstractProvider;
class QOrmEntityInstanceCache;
class QOrmError;
class QOrmQuery;
class QOrmQueryBuilder;
class QOrmSessionPrivate;

class Q_ORM_EXPORT QOrmSession
{
    Q_DECLARE_PRIVATE(QOrmSession)

public:
    explicit QOrmSession(
        QOrmSessionConfiguration configuration = QOrmSessionConfiguration::defaultConfiguration());
    ~QOrmSession();

    Q_REQUIRED_RESULT
    QOrmQueryResult<QObject> execute(const QOrmQuery& query);

    Q_REQUIRED_RESULT
    QOrmQueryBuilder from(const QOrmQuery& query);

    template<typename T>
    bool merge(T* entityInstance)
    {
        return doMerge(entityInstance, T::staticMetaObject);
    }

    template<typename T>
    bool merge(std::initializer_list<T*> instances)
    {
        QOrmTransactionToken token = declareTransaction(QOrm::TransactionPropagation::Require,
                                                        QOrm::TransactionAction::Commit);

        for (T* instance : instances)
        {
            if (!merge(instance))
            {
                token.rollback();
                return false;
            }
        }

        return true;
    }

    template<typename... Ts>
    bool merge(Ts... instances)
    {
        QOrmTransactionToken token = declareTransaction(QOrm::TransactionPropagation::Require,
                                                        QOrm::TransactionAction::Commit);

        if (!(... && merge(instances)))
        {
            token.rollback();
            return false;
        }

        return true;
    }

    template<typename T>
    bool remove(T*& entityInstance)
    {
        return doRemove(entityInstance, T::staticMetaObject);
    }

    template<typename T>
    QOrmQueryBuilder from()
    {
        return queryBuilderFor(T::staticMetaObject);
    }

    template<typename T>
    QOrmQueryBuilder into()
    {
        return queryBuilderFor(T::staticMetaObject);
    }

    Q_REQUIRED_RESULT
    QOrmTransactionToken declareTransaction(QOrm::TransactionPropagation propagation,
                                            QOrm::TransactionAction finalAction);

    Q_REQUIRED_RESULT
    QOrmError lastError() const;

    Q_REQUIRED_RESULT
    const QOrmSessionConfiguration& configuration() const;

    Q_REQUIRED_RESULT
    QOrmMetadataCache* metadataCache();

    Q_REQUIRED_RESULT
    QOrmEntityInstanceCache* entityInstanceCache();

    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
    bool isTransactionActive() const;

private:
    bool doMerge(QObject* entityInstance, const QMetaObject& qMetaObject);
    bool doRemove(QObject*& entityInstance, const QMetaObject& qMetaObject);

    QOrmQueryBuilder queryBuilderFor(const QMetaObject& relationMetaObject);

private:
    QOrmSessionPrivate* d_ptr{nullptr};
};

QT_END_NAMESPACE


#endif // QORMSESSION_H
