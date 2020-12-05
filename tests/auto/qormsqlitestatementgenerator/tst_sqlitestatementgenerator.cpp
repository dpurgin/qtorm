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

#include <QOrmFilter>
#include <QOrmFilterExpression>
#include <QOrmMetadataCache>
#include <QOrmRelation>
#include <QtTest>

#include "domain/province.h"
#include "domain/town.h"
#include "domain/person.h"

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
    void testInsertWithOneToManyNullReference();
    void testFilterWithReference();
    void testUpdateWithManyToOne();
    void testUpdateWithOneToMany();
    void testUpdateWithOneToManyNullReference();
    void testCreateTableWithReference();
    void testCreateTableWithManyToOne();
    void testCreateTableWithLong();
};

void SqliteStatementGenerator::init()
{
    qRegisterOrmEntity<Town, Province, Person>();
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

void SqliteStatementGenerator::testInsertWithOneToManyNullReference()
{
    QOrmSqliteStatementGenerator generator;
    QOrmMetadataCache cache;

    QScopedPointer<Town> hagenberg{new Town{"Hagenberg", nullptr}};

    QVariantMap boundParameters;
    QString statement =
        generator.generateInsertStatement(cache.get<Town>(), hagenberg.get(), boundParameters);

    QCOMPARE(statement, "INSERT INTO Town(name,province_id) VALUES(:name,:province_id)");
    QCOMPARE(boundParameters[":name"], "Hagenberg");
    QCOMPARE(boundParameters[":province_id"], QVariant::fromValue(nullptr));
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

void SqliteStatementGenerator::testUpdateWithOneToManyNullReference()
{
    QOrmSqliteStatementGenerator generator;
    QOrmMetadataCache cache;

    QScopedPointer<Town> hagenberg{new Town{2, "Hagenberg", nullptr}};

    QVariantMap boundParameters;
    QString statement =
        generator.generateUpdateStatement(cache.get<Town>(), hagenberg.get(), boundParameters);

    QCOMPARE(statement, "UPDATE Town SET name = :name,province_id = :province_id WHERE id = :id");
    QCOMPARE(boundParameters[":name"], QString::fromUtf8("Hagenberg"));
    QCOMPARE(boundParameters[":province_id"], QVariant::fromValue(nullptr));
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

void SqliteStatementGenerator::testCreateTableWithLong()
{
    QOrmMetadataCache cache;
    QCOMPARE(QOrmSqliteStatementGenerator::generateCreateTableStatement(cache.get<Person>()),
             "CREATE TABLE Person(id INTEGER PRIMARY KEY AUTOINCREMENT,name TEXT)");
}

QTEST_APPLESS_MAIN(SqliteStatementGenerator)

#include "tst_sqlitestatementgenerator.moc"
