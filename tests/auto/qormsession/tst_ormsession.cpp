#include <QtTest>

#include <QOrmError>
#include <QOrmSession>
#include <QOrmSqlConfiguration>
#include <QOrmSqliteProvider>
#include <QSqlDatabase>
#include <QSqlQuery>

#include <domain/person.h>
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
    void testSelectWithManyToOne();
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
    // prepare database
    {
        QOrmSession session;
        Province* upperAustria = new Province(QString::fromUtf8("Oberösterreich"));
        Province* lowerAustria = new Province(QString::fromUtf8("Niederösterreich"));

        Town* hagenberg = new Town(QString::fromUtf8("Hagenberg"), upperAustria);
        Town* pregarten = new Town(QString::fromUtf8("Pregarten"), upperAustria);
        Town* melk = new Town(QString::fromUtf8("Melk"), lowerAustria);

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

    QOrmQueryResult result = session.from<Person>().select();

    QCOMPARE(result.error().type(), QOrm::ErrorType::None);

    auto data = result.toVector();
    QCOMPARE(data.size(), 2);

    auto franzHuber = qobject_cast<Person*>(data[0]);
    QCOMPARE(franzHuber->id(), 1);
    QCOMPARE(franzHuber->firstName(), QString::fromUtf8("Franz"));
    QCOMPARE(franzHuber->lastName(), QString::fromUtf8("Huber"));
    QVERIFY(franzHuber->town() != nullptr);
    QCOMPARE(franzHuber->town()->name(), QString::fromUtf8("Hagenberg"));

    auto lisaMaier = qobject_cast<Person*>(data[1]);
    QCOMPARE(lisaMaier->id(), 2);
    QCOMPARE(lisaMaier->firstName(), QString::fromUtf8("Lisa"));
    QCOMPARE(lisaMaier->lastName(), QString::fromUtf8("Maier"));
    QVERIFY(lisaMaier->town() != nullptr);
    QCOMPARE(lisaMaier->town()->name(), QString::fromUtf8("Hagenberg"));
}

QTEST_GUILESS_MAIN(SqliteSessionTest)

#include "tst_ormsession.moc"
