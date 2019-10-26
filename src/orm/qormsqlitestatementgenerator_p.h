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

#ifndef QORMSQLITESTATEMENTGENERATOR_H
#define QORMSQLITESTATEMENTGENERATOR_H

#include <QtOrm/qormglobal.h>

#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>
#include <QtCore/qshareddata.h>

#include <utility>
#include <vector>

QT_BEGIN_NAMESPACE

class QOrmFilter;
class QOrmFilterBinaryPredicate;
class QOrmFilterExpression;
class QOrmFilterTerminalPredicate;
class QOrmFilterUnaryPredicate;
class QOrmMetadata;
class QOrmOrder;
class QOrmQuery;
class QOrmRelation;

class Q_ORM_EXPORT QOrmSqliteStatementGenerator
{    
public:
    Q_REQUIRED_RESULT
    static std::pair<QString, QVariantMap> generate(const QOrmQuery& query);

    Q_REQUIRED_RESULT
    static QString generate(const QOrmQuery& query, QVariantMap& boundParameters);

    Q_REQUIRED_RESULT
    static QString generateInsertStatement(const QOrmMetadata& relation,
                                           const QObject* instance,
                                           QVariantMap& boundParameters);

    Q_REQUIRED_RESULT
    static QString generateUpdateStatement(const QOrmMetadata& relation,
                                           const QObject* instance,
                                           QVariantMap& boundParameters);

    Q_REQUIRED_RESULT
    static QString generateSelectStatement(const QOrmQuery& query, QVariantMap& boundParameters);

    Q_REQUIRED_RESULT
    static QString generateDeleteStatement(const QOrmMetadata& relation,
                                           const QOrmFilter& filter,
                                           QVariantMap& boundParameters);

    Q_REQUIRED_RESULT
    static QString generateDeleteStatement(const QOrmMetadata& relation,
                                           const QObject* instance,
                                           QVariantMap& boundParameters);

    Q_REQUIRED_RESULT
    static QString generateFromClause(const QOrmRelation& relation, QVariantMap& boundParameters);

    Q_REQUIRED_RESULT
    static QString generateWhereClause(const QOrmFilter& filter, QVariantMap& boundParameters);

    Q_REQUIRED_RESULT
    static QString generateOrderClause(const std::vector<QOrmOrder>& order);

    Q_REQUIRED_RESULT
    static QString generateCondition(const QOrmFilterExpression& expression,
                                     QVariantMap& boundParameters);
    Q_REQUIRED_RESULT
    static QString generateCondition(const QOrmFilterTerminalPredicate& predicate,
                                     QVariantMap& boundParameters);
    Q_REQUIRED_RESULT
    static QString generateCondition(const QOrmFilterBinaryPredicate& predicate,
                                     QVariantMap& boundParameters);
    Q_REQUIRED_RESULT
    static QString generateCondition(const QOrmFilterUnaryPredicate& predicate,
                                     QVariantMap& boundParameters);

    Q_REQUIRED_RESULT
    static QString generateCreateTableStatement(const QOrmMetadata& entity);

    Q_REQUIRED_RESULT
    static QString generateDropTableStatement(const QOrmMetadata& entity);

    Q_REQUIRED_RESULT
    static QString toSqliteType(QVariant::Type type);
};

QT_END_NAMESPACE

#endif
