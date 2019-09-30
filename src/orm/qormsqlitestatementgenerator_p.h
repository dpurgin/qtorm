#ifndef QORMSQLITESTATEMENTGENERATOR_H
#define QORMSQLITESTATEMENTGENERATOR_H

#include <QtOrm/qormglobal.h>

#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>
#include <QtCore/qshareddata.h>

#include <utility>

QT_BEGIN_NAMESPACE

class QOrmFilter;
class QOrmFilterBinaryPredicate;
class QOrmFilterExpression;
class QOrmFilterTerminalPredicate;
class QOrmFilterUnaryPredicate;
class QOrmMetadata;
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
    static std::pair<QString, QVariantMap> generateInsertStatement(const QOrmMetadata& relation,
                                                                   const QObject* instance);

    Q_REQUIRED_RESULT
    static std::pair<QString, QVariantMap> generateUpdateStatement(const QOrmQuery& query);

    Q_REQUIRED_RESULT
    static QString generateFromClause(const QOrmRelation& relation, QVariantMap& boundParameters);

    Q_REQUIRED_RESULT
    static QString generateWhereClause(const QOrmFilter& filter, QVariantMap& boundParameters);

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
};

QT_END_NAMESPACE

#endif
