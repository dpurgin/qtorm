#include <QtTest>

#include <QOrmError>
#include <QOrmSession>
#include <QOrmSqlConfiguration>
#include <QOrmSqliteProvider>
#include <QSqlDatabase>
#include <QSqlQuery>

#include <domain/province.h>
#include <domain/town.h>

class SqliteSessionTest : public QObject
{
    Q_OBJECT

public:
    SqliteSessionTest();
    ~SqliteSessionTest();

private slots:
    void init();
    void cleanup();

    void testSuccessfulMergeWithExplicitCreate();
    void testSuccessfulMergeWithExplicitUpdate();

    void testCascadedCreate();

    void testSelectWithOneToMany();
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

    qRegisterOrmEntity<Town, Province>();
}

void SqliteSessionTest::cleanup()
{

}

void SqliteSessionTest::testSuccessfulMergeWithExplicitCreate()
{
    //    QOrmSession session;

    //    Province* province = new Province();
    //    province->setName(QString::fromUtf8("Oberösterreich"));

    //    QVERIFY(session.merge(province));
    //    QCOMPARE(session.lastError().type(), QOrm::ErrorType::None);
    //    QCOMPARE(province->id(), 1);
    //    QCOMPARE(province->name(), QString::fromUtf8("Oberösterreich"));

    //    // Check that the database contains the corresponding record
    //    QOrmSqliteProvider* provider =
    //    static_cast<QOrmSqliteProvider*>(session.configuration().provider()); QSqlQuery
    //    query{provider->database()}; QVERIFY(query.exec("SELECT * FROM Province"));
    //    QVERIFY(query.next());
    //    QCOMPARE(query.value("id"), 1);
    //    QCOMPARE(query.value("name"), QString::fromUtf8("Oberösterreich"));
    //    QVERIFY(!query.next());
}

void SqliteSessionTest::testSuccessfulMergeWithExplicitUpdate()
{
    //    QOrmSession session;

    //    Province* province = new Province();
    //    province->setName(QString::fromUtf8("Oberösterreich"));

    //    QVERIFY(session.merge(province));

    //    QCOMPARE(session.lastError().type(), QOrm::ErrorType::None);
    //    QCOMPARE(province->id(), 1);
    //    QCOMPARE(province->name(), QString::fromUtf8("Oberösterreich"));

    //    province->setName(QString::fromUtf8("Niederoesterreich"));

    //    QVERIFY(session.merge(province));

    //    QCOMPARE(session.lastError().type(), QOrm::ErrorType::None);
    //    QCOMPARE(province->id(), 1);
    //    QCOMPARE(province->name(), QString::fromUtf8("Niederoesterreich"));

    //    // Check that the database contains the corresponding record
    //    QOrmSqliteProvider* provider =
    //    static_cast<QOrmSqliteProvider*>(session.configuration().provider()); QSqlQuery
    //    query{provider->database()}; QVERIFY(query.exec("SELECT * FROM Province"));
    //    QVERIFY(query.next());
    //    QCOMPARE(query.value("id"), 1);
    //    QCOMPARE(query.value("name"), QString::fromUtf8("Niederoesterreich"));
    //    QVERIFY(!query.next());
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

    QVERIFY(session.merge(hagenberg, pregarten, melk, upperAustria, lowerAustria));

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
    //     prepare database
    {
        QOrmSession session;
        Province* upperAustria = new Province(QString::fromUtf8("Oberösterreich"));
        Province* lowerAustria = new Province(QString::fromUtf8("Niederösterreich"));

        Town* hagenberg = new Town(QString::fromUtf8("Hagenberg"), upperAustria);
        Town* pregarten = new Town(QString::fromUtf8("Pregarten"), upperAustria);
        Town* melk = new Town(QString::fromUtf8("Melk"), lowerAustria);

        QVERIFY(session.merge(hagenberg, pregarten, melk, upperAustria, lowerAustria));
    }

    // Load data from the database using new ORM session
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

QTEST_GUILESS_MAIN(SqliteSessionTest)

#include "tst_ormsession.moc"
