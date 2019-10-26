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

#ifndef QORMQUERYBUILDER_H
#define QORMQUERYBUILDER_H

#include <QtOrm/qormfilter.h>
#include <QtOrm/qormfilterexpression.h>
#include <QtOrm/qormglobal.h>
#include <QtOrm/qormorderbuilder.h>
#include <QtOrm/qormquery.h>
#include <QtOrm/qormqueryresult.h>

#include <QtCore/qobject.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qvector.h>

QT_BEGIN_NAMESPACE

class QOrmAbstractProvider;
class QOrmFilterExpression;
class QOrmMetadataCache;
class QOrmOrderBuilder;
class QOrmQueryBuilderPrivate;
class QOrmRelation;
class QOrmSession;

class Q_ORM_EXPORT QOrmQueryBuilder
{
public:
    QOrmQueryBuilder(QOrmSession* ormSession, const QOrmRelation& relation);
    QOrmQueryBuilder(const QOrmQueryBuilder&);
    QOrmQueryBuilder(QOrmQueryBuilder&&);
    ~QOrmQueryBuilder();

    QOrmQueryBuilder& operator=(const QOrmQueryBuilder&);
    QOrmQueryBuilder& operator=(QOrmQueryBuilder&&);

    QOrmQueryBuilder& filter(QOrmFilterExpression expression);
    QOrmQueryBuilder& order(QOrmOrderBuilder orderBuilder);
    QOrmQueryBuilder& instance(const QMetaObject& qMetaObject, QObject* instance);

    template<typename T>
    QOrmQueryBuilder& instance(T* entityInstance)
    {
        return instance(T::staticMetaObject, entityInstance);
    }

    QOrmQuery build(QOrm::Operation operation) const;

    template<typename T>
    QOrmQueryBuilder& projection()
    {
        return projection(T::staticMetaObject);
    }

    template<typename T>
    QOrmQueryResult<T> select() const
    {
        return select(T::staticMetaObject);
    }

    QOrmQueryResult<QObject> remove(
        QOrm::RemoveMode removeMode = QOrm::RemoveMode::PreventRemoveAll) const;

    template<typename T>
    QOrmQueryResult<T> merge(T* entityInstance)
    {
        return merge(T::staticMetaObject, entityInstance);
    }

private:
    QOrmQueryBuilder& projection(const QMetaObject& projectionMetaObject);
    QOrmQueryResult<QObject> select(const QMetaObject& projectionMetaObject) const;
    QOrmQueryResult<QObject> merge(const QMetaObject& qMetaObject, QObject* entityInstance);

    QSharedDataPointer<QOrmQueryBuilderPrivate> d;
};

QT_END_NAMESPACE

#endif // QORMQUERYBUILDER_H
