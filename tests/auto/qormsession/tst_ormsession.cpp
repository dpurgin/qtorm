#include <QtTest>

#include <QOrmError>
#include <QOrmMetadataCache>
#include <QOrmSession>
#include <QOrmSqliteConfiguration>
#include <QOrmSqliteProvider>
#include <QSqlDatabase>
#include <QSqlQuery>

#include <domain/person.h>
#include <domain/province.h>
#include <domain/town.h>

#include "private/qormglobal_p.h"

class SqliteSessionTest : public QObject
{
    Q_OBJECT

public:
    SqliteSessionTest();
    ~SqliteSessionTest();

private slots:
    void init();
    void cleanup();

    void testCascadedCreate();

    void testSelectWithOneToMany();
    void testSelectWithManyToOne();
    void testSelectReturnsCachedInstances();
    void testSelectWithSingleStringFilter();
    void testSelectWithOrder();
    void testSelectFromNestedSelect();

    void testMergeFailsWithInconsistentReferences();

    void testRemoveInstance();

    void testTransactionRollback();
};

SqliteSessionTest::SqliteSessionTest()
{
}

SqliteSessionTest::~SqliteSessionTest()
{
}

void SqliteSessionTest::init()
{
    QFile db{"testdb.db"};

    if (db.exists())
        QVERIFY(db.remove());

    qRegisterOrmEntity<Town, Province, Person>();
}

void SqliteSessionTest::cleanup()
{

}

void SqliteSessionTest::testCascadedCreate()
{
    QOrmSqliteConfiguration sqliteConfiguration{};
    sqliteConfiguration.setVerbose(true);
    sqliteConfiguration.setSchemaMode(QOrmSqliteConfiguration::SchemaMode::Recreate);
    sqliteConfiguration.setDatabaseName("testdb.db");
    QOrmSqliteProvider* sqliteProvider = new QOrmSqliteProvider{sqliteConfiguration};
    QOrmSessionConfiguration sessionConfiguration{sqliteProvider, true};
    QOrmSession session{sessionConfiguration};

    Province* upperAustria = new Province(QString::fromUtf8("Oberösterreich"));
    Province* lowerAustria = new Province(QString::fromUtf8("Niederösterreich"));

    Town* hagenberg = new Town(QString::fromUtf8("Hagenberg"), upperAustria);
    Town* pregarten = new Town(QString::fromUtf8("Pregarten"), upperAustria);
    Town* melk = new Town(QString::fromUtf8("Melk"), lowerAustria);

    upperAustria->setTowns({hagenberg, pregarten});
    lowerAustria->setTowns({melk});

    QVERIFY(session.merge(hagenberg, pregarten, melk, upperAustria, lowerAustria));

    QCOMPARE(upperAustria->id(), 1);
    QCOMPARE(lowerAustria->id(), 2);

    QCOMPARE(hagenberg->id(), 1);
    QCOMPARE(pregarten->id(), 2);
    QCOMPARE(melk->id(), 3);

    QOrmSqliteProvider* provider =
        static_cast<QOrmSqliteProvider*>(session.configuration().provider());
    QSqlQuery query{provider->database()};

    QVERIFY(query.exec(QString::fromUtf8("SELECT * FROM Province WHERE name = 'Oberösterreich'")));
    QCOMPARE(query.numRowsAffected(), 1);

    QVERIFY(
        query.exec(QString::fromUtf8("SELECT * FROM Province WHERE name = 'Niederösterreich'")));
    QCOMPARE(query.numRowsAffected(), 1);
}

void SqliteSessionTest::testSelectWithOneToMany()
{
    // prepare database
    {
        QOrmSession session;
        Province* upperAustria = new Province(QString::fromUtf8("Oberösterreich"));
        Province* lowerAustria = new Province(QString::fromUtf8("Niederösterreich"));

        Town* hagenberg = new Town(QString::fromUtf8("Hagenberg"), upperAustria);
        Town* pregarten = new Town(QString::fromUtf8("Pregarten"), upperAustria);
        Town* melk = new Town(QString::fromUtf8("Melk"), lowerAustria);

        upperAustria->setTowns({hagenberg, pregarten});
        lowerAustria->setTowns({melk});

        QVERIFY(session.merge(hagenberg, pregarten, melk, upperAustria, lowerAustria));
    }

    // Load data from the database using a new ORM session
    QOrmSqliteConfiguration sqliteConfiguration;
    sqliteConfiguration.setVerbose(true);
    sqliteConfiguration.setSchemaMode(QOrmSqliteConfiguration::SchemaMode::Bypass);
    sqliteConfiguration.setDatabaseName("testdb.db");
    QOrmSqliteProvider* sqliteProvider = new QOrmSqliteProvider{sqliteConfiguration};
    QOrmSessionConfiguration sessionConfiguration{sqliteProvider, true};
    QOrmSession session{sessionConfiguration};

    QOrmQueryResult result = session.from<Province>().select();

    QCOMPARE(result.error().type(), QOrm::ErrorType::None);

    auto data = result.toVector();
    QCOMPARE(data.size(), 2);

    QCOMPARE(qobject_cast<Province*>(data[0])->id(), 1);
    QCOMPARE(qobject_cast<Province*>(data[0])->name(), QString::fromUtf8("Oberösterreich"));
    QCOMPARE(qobject_cast<Province*>(data[0])->towns().size(), 2);

    QCOMPARE(qobject_cast<Province*>(data[0])->towns()[0]->id(), 1);
    QCOMPARE(qobject_cast<Province*>(data[0])->towns()[0]->name(), QString::fromUtf8("Hagenberg"));

    QCOMPARE(qobject_cast<Province*>(data[0])->towns()[1]->id(), 2);
    QCOMPARE(qobject_cast<Province*>(data[0])->towns()[1]->name(), QString::fromUtf8("Pregarten"));

    QCOMPARE(qobject_cast<Province*>(data[1])->id(), 2);
    QCOMPARE(qobject_cast<Province*>(data[1])->name(), QString::fromUtf8("Niederösterreich"));

    QCOMPARE(qobject_cast<Province*>(data[1])->towns()[0]->id(), 3);
    QCOMPARE(qobject_cast<Province*>(data[1])->towns()[0]->name(), QString::fromUtf8("Melk"));
}

void SqliteSessionTest::testSelectWithManyToOne()
{
    // prepare database
    {
        QOrmSession session;

        Town* hagenberg = new Town{QString::fromUtf8("Hagenberg"), nullptr};
        Town* pregarten = new Town{QString::fromUtf8("Pregarten"), nullptr};

        Person* franzHuber =
            new Person{QString::fromUtf8("Franz"), QString::fromUtf8("Huber"), hagenberg};
        Person* lisaMaier =
            new Person{QString::fromUtf8("Lisa"), QString::fromUtf8("Maier"), hagenberg};

        Province* upperAustria = new Province(QString::fromUtf8("Oberösterreich"));

        QVERIFY(session.merge(hagenberg, pregarten, franzHuber, lisaMaier, upperAustria));
    }

    // Load data from the database using a new ORM session
    QOrmSqliteConfiguration sqliteConfiguration;
    sqliteConfiguration.setVerbose(true);
    sqliteConfiguration.setSchemaMode(QOrmSqliteConfiguration::SchemaMode::Bypass);
    sqliteConfiguration.setDatabaseName("testdb.db");
    QOrmSqliteProvider* sqliteProvider = new QOrmSqliteProvider{sqliteConfiguration};
    QOrmSessionConfiguration sessionConfiguration{sqliteProvider, true};
    QOrmSession session{sessionConfiguration};

    auto result = session.from<Person>().select();
    QVERIFY((std::is_same_v<decltype(result)::Projection, Person>));

    QCOMPARE(result.error().type(), QOrm::ErrorType::None);

    auto data = result.toVector();
    QCOMPARE(data.size(), 2);

    Person* franzHuber{data[0]};

    QCOMPARE(franzHuber->id(), 1);
    QCOMPARE(franzHuber->firstName(), QString::fromUtf8("Franz"));
    QCOMPARE(franzHuber->lastName(), QString::fromUtf8("Huber"));
    QVERIFY(franzHuber->town() != nullptr);
    QCOMPARE(franzHuber->town()->name(), QString::fromUtf8("Hagenberg"));

    Person* lisaMaier{data[1]};
    QCOMPARE(lisaMaier->id(), 2);
    QCOMPARE(lisaMaier->firstName(), QString::fromUtf8("Lisa"));
    QCOMPARE(lisaMaier->lastName(), QString::fromUtf8("Maier"));
    QVERIFY(lisaMaier->town() != nullptr);
    QCOMPARE(lisaMaier->town()->name(), QString::fromUtf8("Hagenberg"));
}

void SqliteSessionTest::testSelectReturnsCachedInstances()
{
    QOrmSession session;

    Province* upperAustria = new Province(QString::fromUtf8("Oberösterreich"));
    Province* lowerAustria = new Province(QString::fromUtf8("Niederösterreich"));

    QVERIFY(session.merge(upperAustria, lowerAustria));

    auto result = session.from<Province>().select().toVector();
    QVERIFY((std::is_same_v<QVector<Province*>, decltype(result)>));
    QCOMPARE(result.size(), 2);
    QVERIFY(result.contains(upperAustria));
    QVERIFY(result.contains(lowerAustria));
}

void SqliteSessionTest::testSelectWithSingleStringFilter()
{
    QOrmSession session;
    session.merge(new Province(QString::fromUtf8("Oberösterreich")),
                  new Province(QString::fromUtf8("Niederösterreich")),
                  new Province(QString::fromUtf8("Salzburg")));

    auto result = session.from<Province>()
                      .filter(Q_ORM_CLASS_PROPERTY(name) == QString::fromUtf8("Oberösterreich"))
                      .select();

    QCOMPARE(result.error().type(), QOrm::ErrorType::None);
    QCOMPARE(result.toVector().size(), 1);

    auto upperAustria = result.toVector().front();
    QCOMPARE(upperAustria->name(), QString::fromUtf8("Oberösterreich"));
}

void SqliteSessionTest::testSelectWithOrder()
{
    QOrmSession session;
    session.merge(new Province(QString::fromUtf8("Tirol")),
                  new Province(QString::fromUtf8("Oberösterreich")),
                  new Province(QString::fromUtf8("Niederösterreich")),
                  new Province(QString::fromUtf8("Salzburg")));

    {
        auto result =
            session.from<Province>().order(Q_ORM_CLASS_PROPERTY(name)).select().toVector();

        QCOMPARE(result.size(), 4);
        QCOMPARE(result[0]->name(), QString::fromUtf8("Niederösterreich"));
        QCOMPARE(result[1]->name(), QString::fromUtf8("Oberösterreich"));
        QCOMPARE(result[2]->name(), QString::fromUtf8("Salzburg"));
        QCOMPARE(result[3]->name(), QString::fromUtf8("Tirol"));
    }

    {
        auto result = session.from<Province>()
                          .order(Q_ORM_CLASS_PROPERTY(name), Qt::DescendingOrder)
                          .select()
                          .toVector();

        QCOMPARE(result.size(), 4);
        QCOMPARE(result[0]->name(), QString::fromUtf8("Tirol"));
        QCOMPARE(result[1]->name(), QString::fromUtf8("Salzburg"));
        QCOMPARE(result[2]->name(), QString::fromUtf8("Oberösterreich"));
        QCOMPARE(result[3]->name(), QString::fromUtf8("Niederösterreich"));
    }
}

void SqliteSessionTest::testSelectFromNestedSelect()
{
    QOrmSession session;

    auto upperAustria = new Province{QString::fromUtf8("Oberösterreich")};
    auto lowerAustria = new Province{QString::fromUtf8("Niederösterreich")};

    auto freistadt = new Town{QString::fromUtf8("Freistadt"), upperAustria};
    auto hagenberg = new Town{QString::fromUtf8("Hagenberg im Mühlkreis"), upperAustria};
    auto melk = new Town{QString::fromUtf8("Melk"), lowerAustria};

    upperAustria->setTowns({freistadt, hagenberg});
    lowerAustria->setTowns({melk});

    session.merge(upperAustria, lowerAustria, freistadt, hagenberg, melk);

    auto nested = session.from<Town>()
                      .filter(Q_ORM_CLASS_PROPERTY(province) == upperAustria)
                      .build(QOrm::Operation::Read);

    auto result =
        session.from(nested).order(Q_ORM_CLASS_PROPERTY(name), Qt::DescendingOrder).select();

    QCOMPARE(result.toVector().size(), 2);
}

void SqliteSessionTest::testMergeFailsWithInconsistentReferences()
{
    QOrmSession session;
    QOrmMetadataCache* metadataCache = session.metadataCache();

    Province* upperAustria = new Province(QString::fromUtf8("Oberösterreich"));
    Province* lowerAustria = new Province(QString::fromUtf8("Niederösterreich"));

    Town* hagenberg = new Town(QString::fromUtf8("Hagenberg"), upperAustria);
    Town* pregarten = new Town(QString::fromUtf8("Pregarten"), upperAustria);
    Town* melk = new Town(QString::fromUtf8("Melk"), lowerAustria);

    upperAustria->setTowns({pregarten});

    QVERIFY(QOrmPrivate::crossReferenceError(metadataCache->get<Town>(), hagenberg).has_value());
    QVERIFY(!QOrmPrivate::crossReferenceError(metadataCache->get<Town>(), pregarten).has_value());
    QVERIFY(QOrmPrivate::crossReferenceError(metadataCache->get<Town>(), melk).has_value());

    // Provine instances to have a cross-reference error but it cannot be detected because the
    // referencing issues are not in the list. The cross-reference is detected on the other side
    // of the relation.
    QVERIFY(!QOrmPrivate::crossReferenceError(metadataCache->get<Province>(), upperAustria)
                 .has_value());
    QVERIFY(!QOrmPrivate::crossReferenceError(metadataCache->get<Province>(), lowerAustria)
                 .has_value());
}

void SqliteSessionTest::testTransactionRollback()
{
    QOrmSession session;

    Province* upperAustria = new Province(QString::fromUtf8("Oberösterreich"));
    Province* lowerAustria = new Province(QString::fromUtf8("Niederösterreich"));

    QVERIFY(session.merge(upperAustria, lowerAustria));

    {
        auto transactionToken = session.declareTransaction(QOrm::TransactionPropagation::Require,
                                                           QOrm::TransactionAction::Rollback);

        upperAustria->setName(QString::fromUtf8("Upper Austria"));
        QVERIFY(session.merge(upperAustria));
    }

    QCOMPARE(upperAustria->name(), QString::fromUtf8("Oberösterreich"));
}

void SqliteSessionTest::testRemoveInstance()
{
    QOrmSession session;

    Province* upperAustria = new Province{QString::fromUtf8("Oberösterreich")};
    QVERIFY(session.merge(upperAustria));
    QVERIFY(session.remove(upperAustria));
    QVERIFY(session.from<Province>().select().toVector().empty());
}

QTEST_GUILESS_MAIN(SqliteSessionTest)

#include "tst_ormsession.moc"
