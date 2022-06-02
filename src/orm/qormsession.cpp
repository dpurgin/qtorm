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

#include "qormsession.h"

#include "qormabstractprovider.h"
#include "qormentityinstancecache.h"
#include "qormerror.h"
#include "qormglobal_p.h"
#include "qormmetadatacache.h"
#include "qormorder.h"
#include "qormquery.h"
#include "qormrelation.h"
#include "qormsessionconfiguration.h"
#include "qormtransactiontoken.h"

#include <QDebug>
#include <QScopeGuard>

QT_BEGIN_NAMESPACE

class QOrmSessionPrivate
{
    using TrackedEntityInstance = std::pair<QObject*, QOrm::Operation>;

    Q_DECLARE_PUBLIC(QOrmSession)
    QOrmSession* q_ptr{nullptr};
    QOrmSessionConfiguration m_sessionConfiguration;
    QOrmEntityInstanceCache m_entityInstanceCache;
    QOrmError m_lastError{QOrm::ErrorType::None, {}};
    QOrmMetadataCache m_metadataCache;
    QSet<const QObject*> m_mergingInstances;
    int m_transactionCounter{0};
    std::vector<TrackedEntityInstance> m_trackedInstances;

    explicit QOrmSessionPrivate(QOrmSessionConfiguration sessionConfiguration, QOrmSession* parent);
    ~QOrmSessionPrivate();

    void ensureProviderConnected();

    bool needsMerge(const QObject* instance)
    {
        return instance != nullptr &&
               (!m_entityInstanceCache.contains(instance) ||
                m_entityInstanceCache.isModified(instance)) &&
               !m_mergingInstances.contains(instance);
    }

    void commitTrackedInstances();
    void rollbackTrackedInstances();

    void clearLastError();
    void setLastError(QOrmError lastError);
};

QOrmSessionPrivate::QOrmSessionPrivate(QOrmSessionConfiguration sessionConfiguration,
                                       QOrmSession* parent)
    : q_ptr{parent}
    , m_sessionConfiguration{std::move(sessionConfiguration)}
{
}

QOrmSessionPrivate::~QOrmSessionPrivate() = default;

void QOrmSessionPrivate::ensureProviderConnected()
{
    if (!m_sessionConfiguration.provider()->isConnectedToBackend())
        m_sessionConfiguration.provider()->connectToBackend();
}

void QOrmSessionPrivate::commitTrackedInstances()
{
    for (auto& [instance, operation] : m_trackedInstances)
    {
        if (operation == QOrm::Operation::Delete)
            instance->deleteLater();
    }

    m_trackedInstances.clear();
}

void QOrmSessionPrivate::rollbackTrackedInstances()
{
    for (auto& [instance, operation] : m_trackedInstances)
    {
        Q_UNUSED(operation)

        QOrmMetadata relation = m_metadataCache.get(*instance->metaObject());
        QOrmMetadata projection = relation;
        QOrmFilter filter{*relation.objectIdMapping() ==
                          QOrmPrivate::objectIdPropertyValue(instance, relation)};

        QOrmQuery query{QOrm::Operation::Read,
                        QOrmRelation{relation},
                        projection,
                        filter,
                        {},
                        QOrm::QueryFlags::OverwriteCachedInstances};
        QOrmQueryResult result =
            m_sessionConfiguration.provider()->execute(query, m_entityInstanceCache);

        if (result.error().type() != QOrm::ErrorType::None)
            qFatal("QtOrm: Inconsistent state: unable to rollback tracked instances.");
    }

    m_trackedInstances.clear();
}

void QOrmSessionPrivate::clearLastError()
{
    m_lastError = QOrmError{QOrm::ErrorType::None, {}};
}

void QOrmSessionPrivate::setLastError(QOrmError lastError)
{
    m_lastError = lastError;
}

QOrmSession::QOrmSession(QOrmSessionConfiguration sessionConfiguration)
    : d_ptr{new QOrmSessionPrivate{sessionConfiguration, this}}
{    
}

QOrmSession::~QOrmSession()
{
    Q_D(QOrmSession);

    if (d->m_sessionConfiguration.provider()->isConnectedToBackend())
        d->m_sessionConfiguration.provider()->disconnectFromBackend();

    delete d_ptr;
}

QOrmQueryResult<QObject> QOrmSession::execute(const QOrmQuery& query)
{
    Q_D(QOrmSession);

    d->clearLastError();
    d->ensureProviderConnected();

    QOrmQueryResult<QObject> providerResult =
        d->m_sessionConfiguration.provider()->execute(query, d->m_entityInstanceCache);

    d->setLastError(providerResult.error());
    return providerResult;
}

QOrmQueryBuilder<QObject> QOrmSession::from(const QOrmQuery& query)
{
    Q_ASSERT(query.operation() == QOrm::Operation::Read);

    return QOrmQueryBuilder<QObject>{this, QOrmRelation{query}};
}

QOrmQueryBuilder<QObject> QOrmSession::queryBuilderFor(const QMetaObject& relationMetaObject)
{
    Q_D(QOrmSession);

    return QOrmQueryBuilder<QObject>{this, QOrmRelation{d->m_metadataCache[relationMetaObject]}};
}

bool QOrmSession::doMerge(QObject* entityInstance, const QMetaObject& qMetaObject)
{
    Q_D(QOrmSession);

    Q_ASSERT(entityInstance != nullptr);

    if (d->m_mergingInstances.contains(entityInstance))
        return true;

    auto token = declareTransaction(QOrm::TransactionPropagation::Require,
                                    QOrm::TransactionAction::Rollback);

    d->m_mergingInstances.insert(entityInstance);

    auto mergeFinalizer = qScopeGuard([d, entityInstance]() {
        d->m_mergingInstances.remove(entityInstance);
        d->m_trackedInstances.push_back(std::make_pair(entityInstance, QOrm::Operation::Merge));
    });

    d->clearLastError();
    d->ensureProviderConnected();

    QOrm::Operation operation = d->m_entityInstanceCache.contains(entityInstance)
                                    ? QOrm::Operation::Update
                                    : QOrm::Operation::Create;

    if (operation == QOrm::Operation::Update &&
        !d->m_entityInstanceCache.isModified(entityInstance))
    {
        return true;
    }

    QOrmMetadata entity = d->m_metadataCache[qMetaObject];

    if (auto result = QOrmPrivate::crossReferenceError(entity, entityInstance))
    {
        qFatal("QtOrm: %s", result->toUtf8().data());
    }

    // Merge modified referenced entity instances
    for (const QOrmPropertyMapping& mapping : entity.propertyMappings())
    {
        if (!mapping.isReference() || mapping.isTransient())
            continue;

        // T*: merge if modified
        QObject* referencedInstance =
            QOrmPrivate::propertyValue(entityInstance, mapping).value<QObject*>();

        if (!d->needsMerge(referencedInstance))
            continue;

        if (!doMerge(referencedInstance, *referencedInstance->metaObject()))
            return false;
    }

    QOrmQueryResult result = d->m_sessionConfiguration.provider()->execute(
        queryBuilderFor(qMetaObject).instance(qMetaObject, entityInstance).build(operation),
        d->m_entityInstanceCache);

    d->setLastError(result.error());

    if (d->m_lastError.type() == QOrm::ErrorType::None)
    {
        if (operation == QOrm::Operation::Create)
        {
            const QOrmPropertyMapping* objectIdMapping =
                d->m_metadataCache[qMetaObject].objectIdMapping();

            if (objectIdMapping != nullptr && objectIdMapping->isAutogenerated())
            {
                if (!QOrmPrivate::setPropertyValue(entityInstance,
                                                   objectIdMapping->classPropertyName(),
                                                   result.lastInsertedId()))
                {
                    Q_ORM_UNEXPECTED_STATE;
                }
            }

            d->m_entityInstanceCache.insert(d->m_metadataCache[qMetaObject], entityInstance);
            d->m_entityInstanceCache.finalize(d->m_metadataCache[qMetaObject], entityInstance);
        }
        else
            d->m_entityInstanceCache.markUnmodified(entityInstance);

        token.commit();
    }

    return d->m_lastError.type() == QOrm::ErrorType::None;
}

bool QOrmSession::doRemove(QObject* entityInstance, const QMetaObject& qMetaObject)
{
    Q_D(QOrmSession);

    d->clearLastError();
    d->ensureProviderConnected();

    QOrmQueryResult result =
        d->m_sessionConfiguration.provider()->execute(queryBuilderFor(qMetaObject)
                                                          .instance(qMetaObject, entityInstance)
                                                          .build(QOrm::Operation::Delete),
                                                      d->m_entityInstanceCache);

    d->setLastError(result.error());

    if (d->m_lastError.type() == QOrm::ErrorType::None)
    {
        d->m_entityInstanceCache.take(entityInstance);
    }

    return d->m_lastError.type() == QOrm::ErrorType::None;
}

QOrmTransactionToken QOrmSession::declareTransaction(QOrm::TransactionPropagation propagation,
                                                     QOrm::TransactionAction finalAction)
{
    Q_D(QOrmSession);

    switch (propagation)
    {
        case QOrm::TransactionPropagation::DontSupport:
            if (isTransactionActive())
            {
                qCritical() << "QtOrm:" << propagation << " requested but a transaction is active!";
                qFatal("QtOrm: Invalid transactional state.");
            }
            break;

        case QOrm::TransactionPropagation::Require:
            if (!beginTransaction())
            {
                qCritical() << "QtOrm: Error starting transaction:" << d->m_lastError;
                qFatal("QtOrm: transaction was requested but it could not be started");
            }
            break;

        case QOrm::TransactionPropagation::Support:
            // don't care, support both
            break;
    }

    return QOrmTransactionToken{this, finalAction};
}

QOrmError QOrmSession::lastError() const
{
    Q_D(const QOrmSession);

    return d->m_lastError;
}

const QOrmSessionConfiguration& QOrmSession::configuration() const
{
    Q_D(const QOrmSession);

    return d->m_sessionConfiguration;
}

QOrmMetadataCache* QOrmSession::metadataCache()
{
    Q_D(QOrmSession);
    return &d->m_metadataCache;
}

QOrmEntityInstanceCache* QOrmSession::entityInstanceCache()
{
    Q_D(QOrmSession);
    return &d->m_entityInstanceCache;
}

bool QOrmSession::beginTransaction()
{
    Q_D(QOrmSession);

    d->setLastError({QOrm::ErrorType::None, {}});

    if (d->m_transactionCounter == 0)
    {
        if (d->m_sessionConfiguration.isVerbose())
            qCDebug(qtorm) << "Beginning transaction";

        d->ensureProviderConnected();
        d->setLastError(d->m_sessionConfiguration.provider()->beginTransaction());

        if (d->m_lastError.type() == QOrm::ErrorType::None)
        {
            d->m_transactionCounter++;
        }
        else if (d->m_sessionConfiguration.isVerbose())
        {
            qCWarning(qtorm) << "Unable to begin transaction:" << d->m_lastError.text();
        }
    }
    else
    {
        d->m_transactionCounter++;
    }

    return d->m_transactionCounter > 0;
}

bool QOrmSession::commitTransaction()
{
    Q_D(QOrmSession);

    d->setLastError(QOrmError{QOrm::ErrorType::None, {}});

    if (d->m_transactionCounter == 0)
    {
        d->setLastError({QOrm::ErrorType::TransactionNotActive, "Transaction is not active"});
    }
    else if (d->m_transactionCounter == 1)
    {
        if (d->m_sessionConfiguration.isVerbose())
            qCDebug(qtorm) << "Committing transaction";

        d->ensureProviderConnected();
        d->setLastError(d->m_sessionConfiguration.provider()->commitTransaction());

        if (d->m_lastError.type() == QOrm::ErrorType::None)
        {
            d->commitTrackedInstances();
            d->m_transactionCounter = 0;
        }
        else if (d->m_sessionConfiguration.isVerbose())
        {
            qCWarning(qtorm) << "Unable to commit transaction:" << d->m_lastError.text();
        }
    }
    else
    {
        d->m_transactionCounter--;
    }

    return d->m_lastError.type() == QOrm::ErrorType::None;
}

bool QOrmSession::rollbackTransaction()
{
    Q_D(QOrmSession);

    d->setLastError(QOrmError{QOrm::ErrorType::None, {}});

    if (d->m_transactionCounter == 0)
    {
        d->setLastError({QOrm::ErrorType::TransactionNotActive, "Transaction is not active"});
    }
    else if (d->m_transactionCounter == 1)
    {
        if (d->m_sessionConfiguration.isVerbose())
            qCDebug(qtorm) << "Rolling back transaction";

        d->ensureProviderConnected();
        d->setLastError(d->m_sessionConfiguration.provider()->rollbackTransaction());

        if (d->m_lastError.type() == QOrm::ErrorType::None)
        {
            d->rollbackTrackedInstances();
            d->m_transactionCounter = 0;
        }
        else if (d->m_sessionConfiguration.isVerbose())
        {
            qCWarning(qtorm) << "Unable to rollback transaction:" << d->m_lastError.text();
        }
    }
    else
    {
        d->m_transactionCounter--;
    }

    return d->m_lastError.type() == QOrm::ErrorType::None;
}

bool QOrmSession::isTransactionActive() const
{
    Q_D(const QOrmSession);
    return d->m_transactionCounter > 0;
}

QT_END_NAMESPACE
