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

#ifndef QORMENTITYLISTMODEL_H
#define QORMENTITYLISTMODEL_H

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qhash.h>
#include <QtCore/qvariant.h>
#include <QtCore/qvector.h>

#include <QtOrm/private/qormglobal_p.h>
#include <QtOrm/qormglobal.h>
#include <QtOrm/qormmetadata.h>
#include <QtOrm/qormmetadatacache.h>
#include <QtOrm/qormorder.h>
#include <QtOrm/qormqueryresult.h>
#include <QtOrm/qormsession.h>

#include <QDebug>

QT_BEGIN_NAMESPACE

class Q_ORM_EXPORT QOrmEntityListModelBase : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QVariantMap filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QVariantList order READ order WRITE setOrder NOTIFY orderChanged)

public:
    QOrmEntityListModelBase(QObject* parent = nullptr);

    QVariantMap filter() const;
    void setFilter(QVariantMap filter);

    QVariantList order() const;
    void setOrder(QVariantList order);

public Q_SLOTS:
    virtual QObject* at(int index) const = 0;
    virtual int indexOf(QObject* entityInstance) const = 0;
    virtual QObject* create(QVariantMap properties) = 0;
    virtual bool remove(QObject* entityInstance) = 0;
    virtual bool removeAt(int index) = 0;
    virtual bool update(QObject* entityInstance) = 0;
    virtual void read() = 0;

Q_SIGNALS:
    void entityInstanceCreated();
    void entityInstanceRemoved();
    void filterChanged();
    void orderChanged();    

protected Q_SLOTS:
    virtual void onFilterChanged() = 0;
    virtual void onOrderChanged() = 0;
    virtual void readData() = 0;

protected:
    QVariantMap m_filter;
    QVariantList m_order;
};

template<typename T>
class QOrmEntityListModel : public QOrmEntityListModelBase
{
public:
    QOrmEntityListModel(QOrmSession& session, QObject* parent = nullptr)
        : QOrmEntityListModelBase{parent}
        , m_session{session}
    {
        readData();

        int roleIndex = Qt::UserRole;

        for (const QOrmPropertyMapping& propertyMapping :
             m_session.metadataCache()->get<T>().propertyMappings())
        {
            m_roles.insert(std::make_pair(roleIndex, propertyMapping));
            m_roleNames.insert(roleIndex, propertyMapping.classPropertyName().toUtf8());
            roleIndex++;
        }
    }

    QObject* at(int index) const override
    {
        return index >= 0 && index < m_data.size() ? m_data[index] : nullptr;
    }

    int indexOf(QObject* instance) const override
    {
        for (int i = 0; i < m_data.size(); ++i)
        {
            if (static_cast<void*>(m_data[i]) == static_cast<void*>(instance))
            {
                return i;
            }
        }

        return -1;
    }

    QObject* create(QVariantMap properties) override
    {
        T* instance = new T{};

        for (auto it = std::cbegin(properties); it != std::cend(properties); ++it)
        {
            QString propertyName = it.key();
            QVariant propertyValue = it.value();
            const QOrmPropertyMapping* propertyMapping =
                m_session.metadataCache()->get<T>().classPropertyMapping(propertyName);
            Q_ASSERT(propertyMapping != nullptr);

            instance->setProperty(propertyName.toUtf8().data(), propertyValue);

            // Update back reference if any
            if (propertyMapping->isReference() && !propertyMapping->isTransient())
            {
                const QOrmPropertyMapping* backReference =
                    QOrmPrivate::backReference(*propertyMapping);

                Q_ASSERT(backReference != nullptr);

                if (backReference->dataTypeName().startsWith("QVector<") &&
                    backReference->dataTypeName().endsWith("*>"))
                {
                    QObject* referencedInstance = propertyValue.value<QObject*>();
                    auto backReferenceContainer =
                        QOrmPrivate::propertyValue(referencedInstance,
                                                   backReference->classPropertyName())
                            .value<QVector<QObject*>>();
                    backReferenceContainer.push_back(instance);
                    if (!QOrmPrivate::setPropertyValue(referencedInstance,
                                                       backReference->classPropertyName(),
                                                       QVariant::fromValue(backReferenceContainer)))
                    {
                        qFatal("Unable to update back-reference");
                    }
                }
            }
        }

        if (m_session.merge(instance))
        {
            Q_EMIT entityInstanceCreated();
            read();
            return instance;
        }

        delete instance;
        return nullptr;
    }

    bool remove(QObject* entityInstance) override
    {
        // update back reference

        for (const QOrmPropertyMapping& propertyMapping :
             m_session.metadataCache()->get<T>().propertyMappings())
        {
            if (propertyMapping.isReference() && !propertyMapping.isTransient())
            {
                QObject* referencedInstance =
                    QOrmPrivate::propertyValue(entityInstance, propertyMapping).value<QObject*>();
                const QOrmPropertyMapping* backReference =
                    QOrmPrivate::backReference(propertyMapping);

                Q_ASSERT(backReference != nullptr);

                if (backReference->dataTypeName().startsWith("QVector<") &&
                    backReference->dataTypeName().endsWith("*>"))
                {
                    auto backReferenceContainer =
                        QOrmPrivate::propertyValue(referencedInstance,
                                                   backReference->classPropertyName())
                            .value<QVector<QObject*>>();
                    backReferenceContainer.removeAll(entityInstance);
                    if (!QOrmPrivate::setPropertyValue(referencedInstance,
                                                       backReference->classPropertyName(),
                                                       QVariant::fromValue(backReferenceContainer)))
                    {
                        qFatal("Unable to update back-reference");
                    }
                }
            }
        }

        if (m_session.remove(qobject_cast<T*>(entityInstance)))
        {
            Q_EMIT entityInstanceRemoved();
            read();
            return true;
        }

        return false;
    }

    bool removeAt(int index) override
    {
        if (index >= 0 && index <= m_data.size() && m_session.remove(m_data[index]))
        {
            read();
            return true;
        }
        return false;
    }

    bool update(QObject* instance) override {
        T* t = qobject_cast<T*>(instance);
        if (!t)
            return false;
        if (m_session.merge(t)) {
            int row = indexOf(instance);
            Q_ASSERT(row >= 0);
            emit dataChanged(index(row), index(row));
            return true;
        }
        return false;
    }

    void read() override
    {
        Q_EMIT beginResetModel();
        readData();
        Q_EMIT endResetModel();
    }

    int rowCount(const QModelIndex& = QModelIndex{}) const { return m_data.size(); }

    QHash<int, QByteArray> roleNames() const { return m_roleNames; }

    QVariant data(const QModelIndex& index, int role) const
    {
        if (index.row() >= 0 && index.row() < m_data.size())
        {
            const QOrmPropertyMapping& propertyMapping = m_roles.at(role);

            QVariant propertyValue =
                m_data[index.row()]->property(propertyMapping.classPropertyName().toUtf8().data());

            if (propertyValue.type() == QVariant::UserType &&
                QString{propertyValue.typeName()}.startsWith("QVector<") &&
                QString{propertyValue.typeName()}.endsWith("*>"))
            {
                QVariantList list;

                for (const QObject* o : propertyValue.value<QVector<QObject*>>())
                {
                    list.push_back(o);
                }

                propertyValue = list;
            }

            return propertyValue;
        }
        return {};
    }

private:
    void onFilterChanged() override { readData(); }

    void onOrderChanged() override { readData(); }

    void readData() override
    {
        std::optional<QOrmFilterExpression> filterExpression;
        std::vector<QOrmOrder> order;

        for (auto it = std::begin(m_filter); it != std::end(m_filter); ++it)
        {
            auto predicate = QOrmClassProperty{it.key().toUtf8().data()} == it.value();

            if (filterExpression.has_value())
            {
                filterExpression = *filterExpression && predicate;
            }
            else
            {
                filterExpression = predicate;
            }
        }

        auto query = m_session.from<T>();

        if (filterExpression.has_value())
        {
            query.filter(*filterExpression);
        }

        for (const QVariant& orderItem : m_order)
        {
            if (orderItem.type() == QVariant::Map)
            {
                QVariantMap orderItemMap = orderItem.toMap();

                for (auto it = std::cbegin(orderItemMap); it != std::cend(orderItemMap); ++it)
                {
                    query.order(QOrmClassProperty{it.key().toUtf8().data()},
                                it.value().value<Qt::SortOrder>());
                }
            }
            else if (orderItem.type() == QVariant::String)
            {
                query.order(QOrmClassProperty{orderItem.toString().toUtf8().data()},
                            Qt::AscendingOrder);
            }
            else
            {
                qCritical("Unexpected order type");
            }
        }

        m_data = query.select().toVector();
    }

private:
    QOrmSession& m_session;
    QVector<T*> m_data;
    QHash<int, QByteArray> m_roleNames;
    std::unordered_map<int, QOrmPropertyMapping> m_roles;
};

QT_END_NAMESPACE

#endif // QORMENTITYLISTMODEL_H
