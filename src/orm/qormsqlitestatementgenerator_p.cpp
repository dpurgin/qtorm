#include "qormfilter.h"
#include "qormmetadata.h"
#include "qormquery.h"
#include "qormsqlitestatementgenerator_p.h"

QT_BEGIN_NAMESPACE

class QOrmSqliteStatementGeneratorPrivate : public QSharedData
{
    friend class QOrmSqliteStatementGenerator;

    QString m_statement;
    QVariantMap m_parameters;

    Q_REQUIRED_RESULT
    QString insertParameter(QString name, QVariant value);

    Q_REQUIRED_RESULT
    QString generateWhereClause(const QOrmFilter& filter);

    Q_REQUIRED_RESULT
    QString generateCondition(const QOrmFilterExpression& expression);
    Q_REQUIRED_RESULT
    QString generateCondition(const QOrmFilterTerminalPredicate& predicate);
    Q_REQUIRED_RESULT
    QString generateCondition(const QOrmFilterBinaryPredicate& predicate);
    Q_REQUIRED_RESULT
    QString generateCondition(const QOrmFilterUnaryPredicate& predicate);

};

QString QOrmSqliteStatementGeneratorPrivate::insertParameter(QString name, QVariant value)
{
    QString key = ':' % name;

    for (int i = 0; m_parameters.contains(key); ++i)
        key = ':' % name % QString::number(i);

    m_parameters.insert(key, value);

    return key;
}

QString QOrmSqliteStatementGeneratorPrivate::generateWhereClause(const QOrmFilter& filter)
{
    QString whereClause;

    if (filter.type() == QOrm::FilterType::Expression)
    {
        Q_ASSERT(filter.expression() != nullptr);

        whereClause = generateCondition(*filter.expression());

        if (!whereClause.isEmpty())
            whereClause = "WHERE " + whereClause;
    }

    return whereClause;
}

QString QOrmSqliteStatementGeneratorPrivate::generateCondition(const QOrmFilterExpression& expression)
{
    switch (expression.type())
    {
        case QOrm::FilterExpressionType::TerminalPredicate:
            Q_ASSERT(expression.terminalPredicate() != nullptr);
            return generateCondition(*expression.terminalPredicate());

        case QOrm::FilterExpressionType::BinaryPredicate:
            Q_ASSERT(expression.binaryPredicate() != nullptr);
            return generateCondition(*expression.binaryPredicate());

        case QOrm::FilterExpressionType::UnaryPredicate:
            Q_ASSERT(expression.unaryPredicate() != nullptr);
            return generateCondition(*expression.unaryPredicate());
    }

    qFatal("Unexpected state in QOrmSqliteStatementGenerator::generateCondition()");
}

QString QOrmSqliteStatementGeneratorPrivate::generateCondition(const QOrmFilterTerminalPredicate& predicate)
{
    static const QHash<QOrm::Comparison, QString> comparisonOps = {
        { QOrm::Comparison::Less, "<" },
        { QOrm::Comparison::Equal, "=" },
        { QOrm::Comparison::Greater, ">" },
        { QOrm::Comparison::NotEqual, "<>" },
        { QOrm::Comparison::LessOrEqual, "<=" },
        { QOrm::Comparison::GreaterOrEqual, ">=" }
    };

    Q_ASSERT(comparisonOps.contains(predicate.comparison));

    QString parameterKey = insertParameter(predicate.classProperty.descriptor(), predicate.value);

    QString statement = QString{"%1 %2 %3"}.arg(predicate.classProperty.descriptor(),
                                                comparisonOps[predicate.comparison],
                                                parameterKey);

    return statement;
}

QString QOrmSqliteStatementGeneratorPrivate::generateCondition(const QOrmFilterBinaryPredicate& predicate)
{
    QString lhsExpr = generateCondition(predicate.lhs);
    QString rhsExpr = generateCondition(predicate.rhs);

    QString op;

    switch (predicate.logicalOperator)
    {
        case QOrm::BinaryLogicalOperator::Or: op = "OR"; break;
        case QOrm::BinaryLogicalOperator::And: op = "AND"; break;
    }

    Q_ASSERT(!op.isEmpty());

    return QString{"(%1) %2 (%3)"}.arg(lhsExpr, op, rhsExpr);
}

QString QOrmSqliteStatementGeneratorPrivate::generateCondition(const QOrmFilterUnaryPredicate& predicate)
{
    QString rhsExpr = generateCondition(predicate.rhs);
    Q_ASSERT(predicate.logicalOperator == QOrm::UnaryLogicalOperator::Not);

    return QString{"NOT (%1)"}.arg(rhsExpr);
}

QOrmSqliteStatementGenerator::QOrmSqliteStatementGenerator()
    : d{new QOrmSqliteStatementGeneratorPrivate}
{
}

QOrmSqliteStatementGenerator::QOrmSqliteStatementGenerator(const QOrmSqliteStatementGenerator&) = default;

QOrmSqliteStatementGenerator::QOrmSqliteStatementGenerator(QOrmSqliteStatementGenerator&&) = default;

QOrmSqliteStatementGenerator::~QOrmSqliteStatementGenerator() = default;

QOrmSqliteStatementGenerator& QOrmSqliteStatementGenerator::operator=(const QOrmSqliteStatementGenerator&) = default;

QOrmSqliteStatementGenerator& QOrmSqliteStatementGenerator::operator=(QOrmSqliteStatementGenerator&&) = default;

void QOrmSqliteStatementGenerator::process(const QOrmQuery& query)
{
    d->m_statement.clear();
    d->m_parameters.clear();

    switch (query.operation())
    {
        case QOrm::Operation::Read:
        {
            QString selectClause = "SELECT *";
            QString fromClause = "FROM " + query.relation().tableName();
            QString whereClause = d->generateWhereClause(query.filter());
            QString orderByClause = "";

            QStringList statementParts{selectClause, fromClause, whereClause, orderByClause};

            d->m_statement = statementParts.join('\n');
            break;
        }

        default:
            qFatal("Not implemented");
    }
}

QString QOrmSqliteStatementGenerator::statement() const
{
    return d->m_statement;
}

QVariantMap QOrmSqliteStatementGenerator::parameters() const
{
    return d->m_parameters;
}



QT_END_NAMESPACE
