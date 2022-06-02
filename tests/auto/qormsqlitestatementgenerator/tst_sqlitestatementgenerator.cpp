/*
 * Copyright (C) 2020-2021 Dmitriy Purgin <dpurgin@gmail.com>
 * Copyright (C) 2019-2022 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2019-2022 sequality software engineering e.U. <office@sequality.at>
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

#include "domain/community.h"
#include "domain/person.h"
#include "domain/province.h"
#include "domain/town.h"
#include "domain/withqvariant.h"

#include "private/qormglobal_p.h"
#include "private/qormsqlitestatementgenerator_p.h"

class SqliteStatementGenerator : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void testInsertWithManyToOne();
    void testInsertWithOneToMany();
    void testInsertWithOneToManyNullReference();

    void testFilterWithReference();
    void testFilterWithNull();
    void testFilterWithList();

    void testUpdateWithManyToOne();
    void testUpdateWithOneToMany();
    void testUpdateWithOneToManyNullReference();

    void testDeleteWhere();
    void testDeleteWhereWithReturning();

    void testCreateTableWithReference();
    void testCreateTableWithManyToOne();
    void testCreateTableWithLong();
    void testCreateTableForCustomizedEntity();
    void testCreateTableWithQVariant();

    void testAlterTableAddColumn();
    void testAlterTableAddColumnWithReference();
};

void SqliteStatementGenerator::init()
{
    qRegisterOrmEntity<Town, Province, Person, WithQVariant>();
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

    QCOMPARE(statement, R"(WHERE "province_id" = :province_id)");
    QCOMPARE(boundParameters[":province_id"], 1);
}

void SqliteStatementGenerator::testFilterWithNull()
{
    QOrmSqliteStatementGenerator generator;
    QOrmMetadataCache cache;

    {
        QOrmFilter filter{
            QOrmPrivate::resolvedFilterExpression(QOrmRelation{cache.get<WithQVariant>()},
                                                  Q_ORM_CLASS_PROPERTY(data) == QVariant{})};

        QVariantMap boundParameters;
        QString statement = generator.generateWhereClause(filter, boundParameters);

        QCOMPARE(statement, R"(WHERE "data" IS NULL)");
    }

    {
        QOrmFilter filter{
            QOrmPrivate::resolvedFilterExpression(QOrmRelation{cache.get<WithQVariant>()},
                                                  Q_ORM_CLASS_PROPERTY(data) != QVariant{})};

        QVariantMap boundParameters;
        QString statement = generator.generateWhereClause(filter, boundParameters);

        QCOMPARE(statement, R"(WHERE "data" IS NOT NULL)");
    }
}

void SqliteStatementGenerator::testFilterWithList()
{
    QOrmSqliteStatementGenerator generator;
    QOrmMetadataCache cache;

    {
        QOrmFilter filter{
            QOrmPrivate::resolvedFilterExpression(QOrmRelation{cache.get<Town>()},
                                                  Q_ORM_CLASS_PROPERTY(id) == QVector{1, 3, 5})};

        QVariantMap boundParameters;
        QString statement = generator.generateWhereClause(filter, boundParameters);

        QCOMPARE(statement, R"(WHERE "id" IN (:id_0, :id_1, :id_2))");
    }

    {
        QOrmFilter filter{
            QOrmPrivate::resolvedFilterExpression(QOrmRelation{cache.get<Town>()},
                                                  Q_ORM_CLASS_PROPERTY(id) != QVector{1, 3, 5})};

        QVariantMap boundParameters;
        QString statement = generator.generateWhereClause(filter, boundParameters);

        QCOMPARE(statement, R"(WHERE "id" NOT IN (:id_0, :id_1, :id_2))");
    }
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

    QCOMPARE(statement, R"(UPDATE Province SET name = :name WHERE "id" = :id)");
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

    QCOMPARE(statement,
             R"(UPDATE Town SET name = :name,province_id = :province_id WHERE "id" = :id)");
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

    QCOMPARE(statement,
             R"(UPDATE Town SET name = :name,province_id = :province_id WHERE "id" = :id)");
    QCOMPARE(boundParameters[":name"], QString::fromUtf8("Hagenberg"));
    QCOMPARE(boundParameters[":province_id"], QVariant::fromValue(nullptr));
    QCOMPARE(boundParameters[":id"], 2);
}

void SqliteStatementGenerator::testDeleteWhere()
{
    QOrmSqliteStatementGenerator generator;
    QOrmMetadataCache cache;

    QOrmFilter filter{QOrmPrivate::resolvedFilterExpression(QOrmRelation{cache.get<Town>()},
                                                            Q_ORM_CLASS_PROPERTY(id) == 1)};

    QVariantMap boundParameters;
    QString statement =
        generator.generateDeleteStatement(cache.get<Town>(), filter, boundParameters);

    QCOMPARE(statement, R"(DELETE FROM Town WHERE "id" = :id)");
    QCOMPARE(boundParameters.size(), 1);
    QCOMPARE(boundParameters[":id"], 1);
}

void SqliteStatementGenerator::testDeleteWhereWithReturning()
{
    QOrmSqliteStatementGenerator generator{QOrmSqliteStatementGenerator::WithReturningClause};
    QOrmMetadataCache cache;

    QOrmFilter filter{QOrmPrivate::resolvedFilterExpression(QOrmRelation{cache.get<Town>()},
                                                            Q_ORM_CLASS_PROPERTY(id) == 1)};

    QVariantMap boundParameters;
    QString statement =
        generator.generateDeleteStatement(cache.get<Town>(), filter, boundParameters);

    QCOMPARE(statement, R"(DELETE FROM Town WHERE "id" = :id RETURNING "Town"."id" AS "id")");
    QCOMPARE(boundParameters.size(), 1);
    QCOMPARE(boundParameters[":id"], 1);
}

void SqliteStatementGenerator::testCreateTableWithReference()
{
    QOrmMetadataCache cache;

    QCOMPARE(
        QOrmSqliteStatementGenerator{}.generateCreateTableStatement(cache.get<Town>()),
        "CREATE TABLE Town(id INTEGER PRIMARY KEY AUTOINCREMENT,name TEXT,province_id INTEGER)");
}

void SqliteStatementGenerator::testCreateTableWithManyToOne()
{
    QOrmMetadataCache cache;

    QCOMPARE(QOrmSqliteStatementGenerator{}.generateCreateTableStatement(cache.get<Province>()),
             "CREATE TABLE Province(id INTEGER PRIMARY KEY AUTOINCREMENT,name TEXT)");
}

void SqliteStatementGenerator::testCreateTableWithLong()
{
    QOrmMetadataCache cache;
    QCOMPARE(QOrmSqliteStatementGenerator{}.generateCreateTableStatement(cache.get<Person>()),
             "CREATE TABLE Person(id INTEGER PRIMARY KEY AUTOINCREMENT,name TEXT)");
}

void SqliteStatementGenerator::testCreateTableForCustomizedEntity()
{
    QOrmMetadataCache cache;
    QCOMPARE(QOrmSqliteStatementGenerator{}.generateCreateTableStatement(cache.get<Community>()),
             "CREATE TABLE communities(community_id INTEGER PRIMARY KEY,name TEXT,population "
             "INTEGER,province_id INTEGER)");
}

void SqliteStatementGenerator::testCreateTableWithQVariant()
{
    QOrmMetadataCache cache;
    QCOMPARE(QOrmSqliteStatementGenerator{}.generateCreateTableStatement(cache.get<WithQVariant>()),
             "CREATE TABLE WithQVariant(id INTEGER PRIMARY KEY AUTOINCREMENT,data TEXT)");
}

void SqliteStatementGenerator::testAlterTableAddColumn()
{
    QOrmMetadataCache cache;

    QString actual = QOrmSqliteStatementGenerator{}.generateAlterTableAddColumnStatement(
        cache.get<Person>(), *cache.get<Person>().classPropertyMapping("name"));

    QCOMPARE(actual, R"(ALTER TABLE "Person" ADD COLUMN "name" TEXT)");
}

void SqliteStatementGenerator::testAlterTableAddColumnWithReference()
{
    QOrmMetadataCache cache;
    QString actual = QOrmSqliteStatementGenerator{}.generateAlterTableAddColumnStatement(
        cache.get<Town>(), *cache.get<Town>().classPropertyMapping("province"));

    QCOMPARE(actual, R"(ALTER TABLE "Town" ADD COLUMN "province_id" INTEGER)");
}

QTEST_APPLESS_MAIN(SqliteStatementGenerator)

#include "tst_sqlitestatementgenerator.moc"
