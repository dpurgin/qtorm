#include <QtTest>

#include <QOrmQuery>
#include <QOrmQueryResult>
#include <QOrmSession>

#include "domain/person.h"

class QueryResultTest : public QObject
{
    Q_OBJECT

private slots:
    void testStlStyleConstIterator();
    void testStlStyleMutableIterator();
};

void QueryResultTest::testStlStyleConstIterator()
{
    QOrmSession session;

    for (int i = 0; i < 3; ++i)
    {
        Person* person = new Person;
        person->setName(QString{"Person %1"}.arg(i));

        session.merge(person);
    }

    QOrmQueryResult result = session.from<Person>().select();

    auto it = std::cbegin(result);
    QCOMPARE((*it)->id(), 1);
    QCOMPARE((*it)->name(), "Person 0");

    ++it;
    QCOMPARE((*it)->id(), 2);
    QCOMPARE((*it)->name(), "Person 1");

    ++it;
    QCOMPARE((*it)->id(), 3);
    QCOMPARE((*it)->name(), "Person 2");

    ++it;
    QCOMPARE(it, std::cend(result));
}

void QueryResultTest::testStlStyleMutableIterator()
{
    QOrmSession session;

    for (int i = 0; i < 3; ++i)
    {
        Person* person = new Person;
        person->setName(QString{"Person %1"}.arg(i));

        session.merge(person);
    }

    {
        QOrmQueryResult result = session.from<Person>().select();

        auto it = std::begin(result);
        Person* p1 = *it;
        QCOMPARE(p1->id(), 1);
        QCOMPARE(p1->name(), "Person 0");
        p1->setName("Person 00");

        ++it;
        Person* p2 = *it;
        QCOMPARE(p2->id(), 2);
        QCOMPARE(p2->name(), "Person 1");
        p2->setName("Person 11");

        ++it;
        Person* p3 = *it;
        QCOMPARE(p3->id(), 3);
        QCOMPARE(p3->name(), "Person 2");
        p3->setName("Person 22");

        ++it;
        QCOMPARE(it, std::end(result));

        QVERIFY(session.merge(p1, p2, p3));
    }

    {
        QOrmQueryResult result = session.from<Person>().select();

        for (Person* person : result)
        {
            QVERIFY(person->name().at(person->name().length() - 1) ==
                    person->name().at(person->name().length() - 2));
        }
    }
}

QTEST_GUILESS_MAIN(QueryResultTest)

#include "tst_queryresult.moc"
