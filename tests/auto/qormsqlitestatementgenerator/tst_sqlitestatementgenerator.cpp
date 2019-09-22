#include <QtTest>
#include <QOrmFilterExpression>

#include "private/qormsqlitestatementgenerator_p.h"

class SqliteStatementGenerator : public QObject
{
    Q_OBJECT

public:
    SqliteStatementGenerator();
    ~SqliteStatementGenerator();

private slots:
    void testGenerateConditionTerminalPredicate();

};

SqliteStatementGenerator::SqliteStatementGenerator()
{

}

SqliteStatementGenerator::~SqliteStatementGenerator()
{

}

void SqliteStatementGenerator::testGenerateConditionTerminalPredicate()
{

}

QTEST_APPLESS_MAIN(SqliteStatementGenerator)

#include "tst_sqlitestatementgenerator.moc"
