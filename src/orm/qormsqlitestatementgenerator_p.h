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
class QOrmFilterUnaryPredicate;
class QOrmFilterTerminalPredicate;
class QOrmQuery;
class QOrmSqliteStatementGeneratorPrivate;

class Q_ORM_EXPORT QOrmSqliteStatementGenerator
{    
public:
    QOrmSqliteStatementGenerator();
    QOrmSqliteStatementGenerator(const QOrmSqliteStatementGenerator&);
    QOrmSqliteStatementGenerator(QOrmSqliteStatementGenerator&&);
    ~QOrmSqliteStatementGenerator();

    QOrmSqliteStatementGenerator& operator=(const QOrmSqliteStatementGenerator&);
    QOrmSqliteStatementGenerator& operator=(QOrmSqliteStatementGenerator&&);

    Q_REQUIRED_RESULT
    QString statement() const;

    Q_REQUIRED_RESULT
    QVariantMap parameters() const;

    void process(const QOrmQuery& query);

private:
    QSharedDataPointer<QOrmSqliteStatementGeneratorPrivate> d;
};

QT_END_NAMESPACE

#endif
