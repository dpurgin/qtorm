#include <QOrmFilter>
#include <QOrmFilterExpression>
#include <QOrmMetadataCache>
#include <QOrmRelation>
#include <QtTest>

#include "domain/province.h"
#include "domain/town.h"

#include "private/qormglobal_p.h"
#include "private/qormsqlitestatementgenerator_p.h"

class SqliteStatementGenerator : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void testGenerateConditionTerminalPredicate();
    void testInsertWithManyToOne();
    void testInsertWithOneToMany();
    void testFilterWithReference();
    void testUpdateWithManyToOne();
    void testUpdateWithOneToMany();
    void testCreateTableWithReference();
    void testCreateTableWithManyToOne();
};

void SqliteStatementGenerator::init()
{
    qRegisterOrmEntity<Town, Province>();
}

void SqliteStatementGenerator::testGenerateConditionTerminalPredicate()
{

}

void SqliteStatementGenerator::testInsertWithManyToOne()
{
    QOrmSqliteStatementGenerator generator;
    QOrmMetadataCache cache;

    QScopedPointer<Province> upperAustria{new Province("Oberösterreich")};

    QVariantMap boundParameters;
    QString statement = generator.generateInsertStatement(cache.get<Province>(),
                                                          upperAustria.get(),
                                                          boundParameters);

    QCOMPARE(statement, "INSERT INTO Province(name) VALUES(:name)");
    QCOMPARE(boundParameters[":name"], "Oberösterreich");
}

void SqliteStatementGenerator::testInsertWithOneToMany()
{
    QOrmSqliteStatementGenerator generator;
    QOrmMetadataCache cache;

    QScopedPointer<Province> upperAustria{new Province(1, "Oberösterreich")};
    QScopedPointer<Town> hagenberg{new Town{"Hagenberg", upperAustria.get()}};

    QVariantMap boundParameters;
    QString statement =
        generator.generateInsertStatement(cache.get<Town>(), hagenberg.get(), boundParameters);

    QCOMPARE(statement, "INSERT INTO Town(name,province_id) VALUES(:name,:province_id)");
    QCOMPARE(boundParameters[":name"], "Hagenberg");
    QCOMPARE(boundParameters[":province_id"], 1);
}

void SqliteStatementGenerator::testFilterWithReference()
{
    QOrmSqliteStatementGenerator generator;
    QOrmMetadataCache cache;

    QScopedPointer<Province> upperAustria{new Province(1, "Oberösterreich")};
    QScopedPointer<Town> hagenberg{new Town{"Hagenberg", upperAustria.get()}};

    QOrmFilter filter{QOrmPrivate::resolvedFilterExpression(QOrmRelation{cache.get<Town>()},
                                                            Q_ORM_CLASS_PROPERTY(province) ==
                                                                upperAustria.get())};

    QVariantMap boundParameters;
    QString statement = generator.generateWhereClause(filter, boundParameters);

    QCOMPARE(statement, "WHERE province_id = :province_id");
    QCOMPARE(boundParameters[":province_id"], 1);
}

void SqliteStatementGenerator::testUpdateWithManyToOne()
{
    QOrmSqliteStatementGenerator generator;
    QOrmMetadataCache cache;

    QScopedPointer<Province> upperAustria{new Province(1, QString::fromUtf8("Oberösterreich"))};
    QVariantMap boundParameters;
    QString statement = generator.generateUpdateStatement(cache.get<Province>(),
                                                          upperAustria.get(),
                                                          boundParameters);

    QCOMPARE(statement, "UPDATE Province SET name = :name WHERE id = :id");
    QCOMPARE(boundParameters[":name"], QString::fromUtf8("Oberösterreich"));
    QCOMPARE(boundParameters[":id"], 1);
}

void SqliteStatementGenerator::testUpdateWithOneToMany()
{
    QOrmSqliteStatementGenerator generator;
    QOrmMetadataCache cache;

    QScopedPointer<Province> upperAustria{new Province(1, "Oberösterreich")};
    QScopedPointer<Town> hagenberg{new Town{2, "Hagenberg", upperAustria.get()}};

    QVariantMap boundParameters;
    QString statement =
        generator.generateUpdateStatement(cache.get<Town>(), hagenberg.get(), boundParameters);

    QCOMPARE(statement, "UPDATE Town SET name = :name,province_id = :province_id WHERE id = :id");
    QCOMPARE(boundParameters[":name"], QString::fromUtf8("Hagenberg"));
    QCOMPARE(boundParameters[":province_id"], 1);
    QCOMPARE(boundParameters[":id"], 2);
}

void SqliteStatementGenerator::testCreateTableWithReference()
{
    QOrmMetadataCache cache;

    QCOMPARE(
        QOrmSqliteStatementGenerator::generateCreateTableStatement(cache.get<Town>()),
        "CREATE TABLE Town(id INTEGER PRIMARY KEY AUTOINCREMENT,name TEXT,province_id INTEGER)");
}

void SqliteStatementGenerator::testCreateTableWithManyToOne()
{
    QOrmMetadataCache cache;

    QCOMPARE(QOrmSqliteStatementGenerator::generateCreateTableStatement(cache.get<Province>()),
             "CREATE TABLE Province(id INTEGER PRIMARY KEY AUTOINCREMENT,name TEXT)");
}

QTEST_APPLESS_MAIN(SqliteStatementGenerator)

#include "tst_sqlitestatementgenerator.moc"
