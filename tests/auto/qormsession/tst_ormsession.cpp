#include <QtTest>

#include <QOrmError>
#include <QOrmSession>
#include <QOrmSqliteProvider>
#include <QSqlDatabase>
#include <QSqlQuery>

class Province : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    int m_id;
    QString m_name;

public:
    explicit Province(QObject *parent = nullptr);

    int id() const;
    void setId(int id);

    QString name() const;
    void setName(QString name);

signals:
    void idChanged(int id);
    void nameChanged(QString name);
};

Province::Province(QObject *parent)
    : QObject(parent)
{
}

int Province::id() const
{
    return m_id;
}

QString Province::name() const
{
    return m_name;
}

void Province::setId(int id)
{
    if (m_id == id)
        return;

    m_id = id;
    emit idChanged(m_id);
}

void Province::setName(QString name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(m_name);
}

class QOrmSessionTest : public QObject
{
    Q_OBJECT

public:
    QOrmSessionTest();
    ~QOrmSessionTest();

private slots:
    void init();
    void cleanup();

    void testSuccessfulMergeWithExplicitCreate();
    void testSuccessfulMergeWithExplicitUpdate();

};

QOrmSessionTest::QOrmSessionTest()
{
}

QOrmSessionTest::~QOrmSessionTest()
{
}

void QOrmSessionTest::init()
{
    QFile db{"testdb.db"};

    if (db.exists())
        QVERIFY(db.remove());
}

void QOrmSessionTest::cleanup()
{

}

void QOrmSessionTest::testSuccessfulMergeWithExplicitCreate()
{
    QOrmSession session;

    Province* province = new Province();
    province->setName(QString::fromUtf8("Oberösterreich"));

    QVERIFY(session.merge(province, QOrm::MergeMode::Create));
    QCOMPARE(session.lastError().error(), QOrm::Error::None);
    QCOMPARE(province->id(), 1);
    QCOMPARE(province->name(), QString::fromUtf8("Oberösterreich"));

    // Check that the database contains the corresponding record
    QOrmSqliteProvider* provider = static_cast<QOrmSqliteProvider*>(session.configuration().provider());
    QSqlQuery query{provider->database()};
    QVERIFY(query.exec("SELECT * FROM Province"));
    QVERIFY(query.next());
    QCOMPARE(query.value("id"), 1);
    QCOMPARE(query.value("name"), QString::fromUtf8("Oberösterreich"));
    QVERIFY(!query.next());
}


void QOrmSessionTest::testSuccessfulMergeWithExplicitUpdate()
{
    QOrmSession session;

    Province* province = new Province();
    province->setName(QString::fromUtf8("Oberösterreich"));

    QVERIFY(session.merge(province, QOrm::MergeMode::Create));

    QCOMPARE(session.lastError().error(), QOrm::Error::None);
    QCOMPARE(province->id(), 1);
    QCOMPARE(province->name(), QString::fromUtf8("Oberösterreich"));

    province->setName(QString::fromUtf8("Niederoesterreich"));

    QVERIFY(session.merge(province, QOrm::MergeMode::Update));

    QCOMPARE(session.lastError().error(), QOrm::Error::None);
    QCOMPARE(province->id(), 1);
    QCOMPARE(province->name(), QString::fromUtf8("Niederoesterreich"));

    // Check that the database contains the corresponding record
    QOrmSqliteProvider* provider = static_cast<QOrmSqliteProvider*>(session.configuration().provider());
    QSqlQuery query{provider->database()};
    QVERIFY(query.exec("SELECT * FROM Province"));
    QVERIFY(query.next());
    QCOMPARE(query.value("id"), 1);
    QCOMPARE(query.value("name"), QString::fromUtf8("Niederoesterreich"));
    QVERIFY(!query.next());
}


QTEST_GUILESS_MAIN(QOrmSessionTest)

#include "tst_ormsession.moc"
