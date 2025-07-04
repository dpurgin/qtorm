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
#include <QOrmOrder>
#include <QOrmQuery>
#include <QOrmRelation>
#include <QtTest>

#include "domain/community.h"
#include "domain/person.h"
#include "domain/province.h"
#include "domain/town.h"
#include "domain/withqvariant.h"
#include "domain/withenum.h"

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
    void testInsertForCustomizedEntity();
    void testInsertWithNamespace();

    void testFilterWithReference();
    void testFilterWithReferenceIsNull();
    void testFilterWithReferenceExplicitId();
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
    void testCreateTableWithEnum();
    void testCreateTableWithNamespace();

    void testAlterTableAddColumn();
    void testAlterTableAddColumnWithReference();

    void testSelectWithLimitOffset();
    void testSelectWithNamespace();
    void testLimitOffset();
    void testLimitOffset_data();
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

    QCOMPARE(statement, R"(INSERT INTO "Province"("name") VALUES(:name))");
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

    QCOMPARE(statement, R"(INSERT INTO "Town"("name","province_id") VALUES(:name,:province_id))");
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

    QCOMPARE(statement, R"(INSERT INTO "Town"("name","province_id") VALUES(:name,:province_id))");
    QCOMPARE(boundParameters[":name"], "Hagenberg");
    QCOMPARE(boundParameters[":province_id"], QVariant::fromValue(nullptr));
}

void SqliteStatementGenerator::testInsertForCustomizedEntity()
{
    QOrmSqliteStatementGenerator generator;
    QOrmMetadataCache cache;

    QScopedPointer<Community> hagenberg{new Community};
    hagenberg->setCommunityId(4232);
    hagenberg->setName("Hagenberg");
    hagenberg->setPopulation(4000);

    QVariantMap boundParameters;
    QString statement =
        generator.generateInsertStatement(cache.get<Community>(), hagenberg.get(), boundParameters);

    QCOMPARE(
        statement,
        R"(INSERT INTO "communities"("community_id","name","population","province_id") VALUES(:community_id,:name,:population,:province_id))");
    QCOMPARE(boundParameters[":community_id"], 4232);
    QCOMPARE(boundParameters[":name"], "Hagenberg");
    QCOMPARE(boundParameters[":population"], 4000);
    QCOMPARE(boundParameters[":province_id"], QVariant::fromValue(nullptr));
}

void SqliteStatementGenerator::testInsertWithNamespace()
{
    QOrmSqliteStatementGenerator generator;
    QOrmMetadataCache cache;

    QScopedPointer<MyNamespace::WithNamespace> entity{new MyNamespace::WithNamespace{}};

    QVariantMap boundParameters;
    QString statement = generator.generateInsertStatement(cache.get<MyNamespace::WithNamespace>(),
                                                          entity.get(),
                                                          boundParameters);
    QCOMPARE(statement, R"(INSERT INTO "MyNamespace_WithNamespace"("value") VALUES(:value))");
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

void SqliteStatementGenerator::testFilterWithReferenceIsNull()
{
    QOrmSqliteStatementGenerator generator;
    QOrmMetadataCache cache;

    QScopedPointer<Town> hagenberg{new Town{"Hagenberg", nullptr}};

    QOrmFilter filter{
        QOrmPrivate::resolvedFilterExpression(QOrmRelation{cache.get<Town>()},
                                              Q_ORM_CLASS_PROPERTY(province) == nullptr)};

    QVariantMap boundParameters;
    QString statement = generator.generateWhereClause(filter, boundParameters);

    QCOMPARE(statement, R"(WHERE "province_id" IS NULL)");
    QVERIFY(boundParameters.isEmpty());
}

void SqliteStatementGenerator::testFilterWithReferenceExplicitId()
{
    QOrmSqliteStatementGenerator generator;
    QOrmMetadataCache cache;

    QScopedPointer<Province> upperAustria{new Province(1, "Oberösterreich")};
    QScopedPointer<Town> hagenberg{new Town{"Hagenberg", upperAustria.get()}};

    {
        QOrmFilter filter{
            QOrmPrivate::resolvedFilterExpression(QOrmRelation{cache.get<Town>()},
                                                  Q_ORM_CLASS_PROPERTY(province) == 1)};

        QVariantMap boundParameters;
        QString statement = generator.generateWhereClause(filter, boundParameters);

        QCOMPARE(statement, R"(WHERE "province_id" = :province_id)");
        QCOMPARE(boundParameters[":province_id"], 1);
    }   
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
        QOrmFilter filter{QOrmPrivate::resolvedFilterExpression(QOrmRelation{cache.get<Town>()},
                                                                Q_ORM_CLASS_PROPERTY(id) ==
                                                                    QVector<int>{1, 3, 5})};

        QVariantMap boundParameters;
        QString statement = generator.generateWhereClause(filter, boundParameters);

        QCOMPARE(statement, R"(WHERE "id" IN (:id_0, :id_1, :id_2))");
    }

    {
        QOrmFilter filter{QOrmPrivate::resolvedFilterExpression(QOrmRelation{cache.get<Town>()},
                                                                Q_ORM_CLASS_PROPERTY(id) !=
                                                                    QVector<int>{1, 3, 5})};

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

    QCOMPARE(statement, R"(DELETE FROM "Town" WHERE "id" = :id)");
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

    QCOMPARE(statement, R"(DELETE FROM "Town" WHERE "id" = :id RETURNING "Town"."id" AS "id")");
    QCOMPARE(boundParameters.size(), 1);
    QCOMPARE(boundParameters[":id"], 1);
}

void SqliteStatementGenerator::testCreateTableWithReference()
{
    QOrmMetadataCache cache;

    QCOMPARE(
        QOrmSqliteStatementGenerator{}.generateCreateTableStatement(cache.get<Town>()),
        R"(CREATE TABLE "Town"("id" INTEGER PRIMARY KEY AUTOINCREMENT,"name" TEXT,"province_id" INTEGER))");
}

void SqliteStatementGenerator::testCreateTableWithManyToOne()
{
    QOrmMetadataCache cache;

    QCOMPARE(QOrmSqliteStatementGenerator{}.generateCreateTableStatement(cache.get<Province>()),
             R"(CREATE TABLE "Province"("id" INTEGER PRIMARY KEY AUTOINCREMENT,"name" TEXT))");
}

void SqliteStatementGenerator::testCreateTableWithLong()
{
    QOrmMetadataCache cache;
    QCOMPARE(QOrmSqliteStatementGenerator{}.generateCreateTableStatement(cache.get<Person>()),
             R"(CREATE TABLE "Person"("id" INTEGER PRIMARY KEY AUTOINCREMENT,"name" TEXT))");
}

void SqliteStatementGenerator::testCreateTableForCustomizedEntity()
{
    QOrmMetadataCache cache;
    QCOMPARE(
        QOrmSqliteStatementGenerator{}.generateCreateTableStatement(cache.get<Community>()),
        R"(CREATE TABLE "communities"("community_id" INTEGER PRIMARY KEY,"name" TEXT,"population" INTEGER,"province_id" INTEGER))");
}

void SqliteStatementGenerator::testCreateTableWithQVariant()
{
    QOrmMetadataCache cache;
    QCOMPARE(QOrmSqliteStatementGenerator{}.generateCreateTableStatement(cache.get<WithQVariant>()),
             R"(CREATE TABLE "WithQVariant"("id" INTEGER PRIMARY KEY AUTOINCREMENT,"data" TEXT))");
}

void SqliteStatementGenerator::testCreateTableWithEnum()
{
    QOrmMetadataCache cache;
    QCOMPARE(
        QOrmSqliteStatementGenerator{}.generateCreateTableStatement(cache.get<WithEnum>()),
        R"(CREATE TABLE "WithEnum"("id" INTEGER PRIMARY KEY AUTOINCREMENT,"myenum" INTEGER,"myenumclass" INTEGER))");
}

void SqliteStatementGenerator::testCreateTableWithNamespace()
{
    QOrmMetadataCache cache;
    QCOMPARE(
        QOrmSqliteStatementGenerator{}.generateCreateTableStatement(
            cache.get<MyNamespace::WithNamespace>()),
        R"(CREATE TABLE "MyNamespace_WithNamespace"("id" INTEGER PRIMARY KEY AUTOINCREMENT,"value" TEXT))");
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

void SqliteStatementGenerator::testSelectWithLimitOffset()
{
    QOrmMetadataCache cache;

    QOrmRelation relation{cache.get<Town>()};
    QOrmMetadata projection{cache.get<Town>()};
    QOrmFilter filter{QOrmPrivate::resolvedFilterExpression(QOrmRelation{cache.get<Town>()},
                                                            Q_ORM_CLASS_PROPERTY(id) > 1)};

    QOrmQuery query{QOrm::Operation::Read,
                    relation,
                    projection,
                    filter,
                    std::nullopt,
                    {},
                    QOrm::QueryFlags::None};
    query.setLimit(10);
    query.setOffset(20);

    QVariantMap boundParameters;
    QString actual =
        QOrmSqliteStatementGenerator{}.generateSelectStatement(query, boundParameters).simplified();
    QString expected{R"(SELECT * FROM "Town" WHERE "id" > :id LIMIT :limit OFFSET :offset)"};

    QCOMPARE(actual, expected);
    QCOMPARE(boundParameters.value(":limit", 0), 10);
    QCOMPARE(boundParameters.value(":offset", 0), 20);
}

void SqliteStatementGenerator::testSelectWithNamespace()
{
    QOrmMetadataCache cache;
    QOrmRelation relation{cache.get<MyNamespace::WithNamespace>()};
    QOrmMetadata projection{cache.get<MyNamespace::WithNamespace>()};

    QOrmQuery query{QOrm::Operation::Read,
                    relation,
                    projection,
                    std::nullopt,
                    std::nullopt,
                    {},
                    QOrm::QueryFlags::None};

    QVariantMap boundParameters;
    QString actual{QOrmSqliteStatementGenerator{}
                       .generateSelectStatement(query, boundParameters)
                       .simplified()};
    QCOMPARE(actual, R"(SELECT * FROM "MyNamespace_WithNamespace")");
}

void SqliteStatementGenerator::testLimitOffset()
{
    QFETCH(QVariant, limit);
    QFETCH(QVariant, offset);
    QFETCH(QString, expected);

    std::optional<int> limitArg{!limit.isValid() ? std::nullopt
                                                 : std::make_optional(limit.toInt())};
    std::optional<int> offsetArg{!offset.isValid() ? std::nullopt
                                                   : std::make_optional(offset.toInt())};

    QVariantMap boundParameters;
    QString actual = QOrmSqliteStatementGenerator{}.generateLimitOffsetClause(limitArg,
                                                                              offsetArg,
                                                                              boundParameters);
    QCOMPARE(actual, expected);

    if (offset.isValid())
    {
        QVERIFY(boundParameters.contains(":limit"));
        QCOMPARE(boundParameters[":limit"], limitArg.value_or(-1));

        QVERIFY(boundParameters.contains(":offset"));
        QCOMPARE(boundParameters[":offset"], offsetArg.value());
    }
    else if (limit.isValid())
    {
        QVERIFY(boundParameters.contains(":limit"));
        QCOMPARE(boundParameters[":limit"], limitArg.value());

        QVERIFY(!boundParameters.contains(":offset"));
    }
    else
    {
        QVERIFY(!boundParameters.contains(":limit"));
        QVERIFY(!boundParameters.contains(":offset"));
    }
}

void SqliteStatementGenerator::testLimitOffset_data()
{
    QTest::addColumn<QVariant>("limit");
    QTest::addColumn<QVariant>("offset");
    QTest::addColumn<QString>("expected");

    QTest::addRow("no limit, no offset") << QVariant{} << QVariant{} << "";
    QTest::addRow("Limit 10, no offset") << QVariant{10} << QVariant{} << "LIMIT :limit";
    QTest::addRow("Limit 10, offset 20")
        << QVariant{10} << QVariant{20} << "LIMIT :limit OFFSET :offset";
    QTest::addRow("no limit, offset 42")
        << QVariant{} << QVariant{42} << "LIMIT :limit OFFSET :offset";
}

QTEST_APPLESS_MAIN(SqliteStatementGenerator)

#include "tst_sqlitestatementgenerator.moc"
