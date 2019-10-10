#include <QtTest>

#include <QOrmError>
#include <QOrmSession>
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
    QOrmSession session;

    Province* upperAustria = new Province(QString::fromUtf8("Oberösterreich"));
    Province* lowerAustria = new Province(QString::fromUtf8("Niederösterreich"));

    Town* hagenberg = new Town(QString::fromUtf8("Hagenberg"), upperAustria);
    Town* pregarten = new Town(QString::fromUtf8("Pregarten"), upperAustria);
    Town* melk = new Town(QString::fromUtf8("Melk"), lowerAustria);

    QVERIFY(session.merge(hagenberg, pregarten, melk, upperAustria, lowerAustria));

    QOrmSqliteProvider* provider =
        static_cast<QOrmSqliteProvider*>(session.configuration().provider());
    QSqlQuery query{provider->database()};

    QVERIFY(query.exec(QString::fromUtf8("SELECT * Province WHERE name = \"Oberösterreich\"")));
    QVERIFY(query.size() == 1);

    QVERIFY(query.exec(QString::fromUtf8("SELECT * Province WHERE name = \"Niederösterreich\"")));
    QVERIFY(query.size() == 1);
}

QTEST_GUILESS_MAIN(SqliteSessionTest)

#include "tst_ormsession.moc"
