/*
 * Copyright (C) 2019-2024 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2019-2024 sequality software engineering e.U. <office@sequality.at>
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

#include <QFile>
#include <QtTest>

#include <QOrmSession>

class DomainClassesTest : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void testBidirectionalReferenceQVector();
    void testBidirectionalReferenceQSet();
    void testBidirectionalReferenceStdVector();
    void testNullValueToQDateTime();
    void testUnidirectionalReferenceOnNSide();
    void testUnidirectionalReferenceOn1Side();
    void testEnumInProperty();
};

void DomainClassesTest::init()
{
    QFile db{"testdb.db"};

    if (db.exists())
        QVERIFY(db.remove());
}

class WithQVectorReference;

class ReferencedByQVector : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)
    Q_PROPERTY(WithQVectorReference* otherSide MEMBER m_otherSide NOTIFY otherSideChanged);

public:
    Q_INVOKABLE ReferencedByQVector() = default;

    int m_id{0};
    WithQVectorReference* m_otherSide{nullptr};

signals:
    void idChanged();
    void otherSideChanged();
};

class WithQVectorReference : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)
    Q_PROPERTY(int dummy MEMBER m_dummy NOTIFY dummyChanged)
    Q_PROPERTY(
        QVector<ReferencedByQVector*> references MEMBER m_references NOTIFY referencesChanged)

public:
    Q_INVOKABLE WithQVectorReference() = default;

    int m_id{0};
    int m_dummy{42};
    QVector<ReferencedByQVector*> m_references;

signals:
    void idChanged();
    void referencesChanged();
    void dummyChanged();
};

void DomainClassesTest::testBidirectionalReferenceQVector()
{
    qRegisterOrmEntity<WithQVectorReference, ReferencedByQVector>();

    {
        auto* parent = new WithQVectorReference;

        auto* ref1 = new ReferencedByQVector;
        ref1->m_otherSide = parent;

        auto* ref2 = new ReferencedByQVector;
        ref2->m_otherSide = parent;

        parent->m_references.push_back(ref1);
        parent->m_references.push_back(ref2);

        QOrmSession session;
        QVERIFY(session.merge(parent, ref1, ref2));
    }

    {
        QOrmSession session{QOrmSessionConfiguration::fromFile(":/qtorm_bypass_schema.json")};

        auto result = session.from<WithQVectorReference>().select().toVector();

        QCOMPARE(result.size(), 1);
        QCOMPARE(result.first()->m_references.size(), 2);
    }
}

class ReferencedByQSet;

class WithQSetReference : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)
    Q_PROPERTY(int dummy MEMBER m_dummy NOTIFY dummyChanged)
    Q_PROPERTY(QSet<ReferencedByQSet*> references MEMBER m_references NOTIFY referencesChanged)

public:
    Q_INVOKABLE WithQSetReference() = default;

    int m_id{0};
    int m_dummy{42};
    QSet<ReferencedByQSet*> m_references;

signals:
    void idChanged();
    void referencesChanged();
    void dummyChanged();
};

class ReferencedByQSet : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)
    Q_PROPERTY(WithQSetReference* otherSide MEMBER m_otherSide NOTIFY otherSideChanged);

public:
    Q_INVOKABLE ReferencedByQSet() = default;

    int m_id{0};
    WithQSetReference* m_otherSide{nullptr};

signals:
    void idChanged();
    void otherSideChanged();
};

void DomainClassesTest::testBidirectionalReferenceQSet()
{
    QSKIP("QSet<T*> reference is not implemented");

    qRegisterOrmEntity<WithQSetReference, ReferencedByQSet>();

    {
        auto* parent = new WithQSetReference;

        auto* ref1 = new ReferencedByQSet;
        ref1->m_otherSide = parent;

        auto* ref2 = new ReferencedByQSet;
        ref2->m_otherSide = parent;

        parent->m_references.insert(ref1);
        parent->m_references.insert(ref2);

        QOrmSession session;
        QVERIFY(session.merge(parent, ref1, ref2));
    }

    {
        QOrmSession session{QOrmSessionConfiguration::fromFile(":/qtorm_bypass_schema.json")};

        auto result = session.from<WithQSetReference>().select().toVector();

        QCOMPARE(result.size(), 1);
        QCOMPARE(result.first()->m_references.size(), 2);
    }
}

class WithStdVectorReference;

class ReferencedByStdVector : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)
    Q_PROPERTY(WithStdVectorReference* otherSide MEMBER m_otherSide NOTIFY otherSideChanged);

public:
    Q_INVOKABLE ReferencedByStdVector() = default;

    int m_id{0};
    WithStdVectorReference* m_otherSide{nullptr};

signals:
    void idChanged();
    void otherSideChanged();
};

class WithStdVectorReference : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)
    Q_PROPERTY(int dummy MEMBER m_dummy NOTIFY dummyChanged)
    Q_PROPERTY(QSet<ReferencedByStdVector*> references MEMBER m_references NOTIFY referencesChanged)

public:
    Q_INVOKABLE WithStdVectorReference() = default;

    int m_id{0};
    int m_dummy{42};
    QSet<ReferencedByStdVector*> m_references;

signals:
    void idChanged();
    void referencesChanged();
    void dummyChanged();
};

void DomainClassesTest::testBidirectionalReferenceStdVector()
{
    QSKIP("std::vector<T*> reference is not implemented");

    qRegisterOrmEntity<WithStdVectorReference, ReferencedByStdVector>();

    {
        auto* parent = new WithStdVectorReference;

        auto* ref1 = new ReferencedByStdVector;
        ref1->m_otherSide = parent;

        auto* ref2 = new ReferencedByStdVector;
        ref2->m_otherSide = parent;

        parent->m_references.insert(ref1);
        parent->m_references.insert(ref2);

        QOrmSession session;
        QVERIFY(session.merge(parent, ref1, ref2));
    }

    {
        QOrmSession session{QOrmSessionConfiguration::fromFile(":/qtorm_bypass_schema.json")};

        auto result = session.from<WithStdVectorReference>().select().toVector();

        QCOMPARE(result.size(), 1);
        QCOMPARE(result.first()->m_references.size(), 2);
    }
}

class WithQDateTime : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)
    Q_PROPERTY(QDateTime date MEMBER m_date NOTIFY dateChanged)

public:
    Q_INVOKABLE WithQDateTime() = default;

    int m_id{0};
    QDateTime m_date{QDateTime::currentDateTime()};

signals:
    void idChanged();
    void dateChanged();
};

void DomainClassesTest::testNullValueToQDateTime()
{
    qRegisterOrmEntity<WithQDateTime>();

    {
        auto entity = new WithQDateTime;
        entity->m_date = QDateTime{};
        QOrmSession session;
        QVERIFY(session.merge(entity));
    }

    {
        QOrmSession session{QOrmSessionConfiguration::fromFile(":/qtorm_bypass_schema.json")};
        auto result = session.from<WithQDateTime>().select().toVector();
        QCOMPARE(result.size(), 1);
        QVERIFY(result.first()->m_date.isNull());
    }
}

class ProvinceN : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)
    Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)

public:
    Q_INVOKABLE ProvinceN() = default;

    int m_id{0};
    QString m_name;

signals:
    void idChanged();
    void nameChanged();
};

class TownN : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)
    Q_PROPERTY(ProvinceN* province MEMBER m_province NOTIFY provinceChanged)

public:
    Q_INVOKABLE TownN() = default;

    int m_id;
    ProvinceN* m_province{nullptr};

signals:
    void idChanged();
    void provinceChanged();
};

void DomainClassesTest::testUnidirectionalReferenceOnNSide()
{
    qRegisterOrmEntity<ProvinceN, TownN>();

    {
        ProvinceN* province = new ProvinceN;
        TownN* town = new TownN;
        town->m_province = province;

        QOrmSession session;
        QVERIFY(session.merge(province, town));
    }

    {
        QOrmSession session{QOrmSessionConfiguration::fromFile(":/qtorm_bypass_schema.json")};

        ProvinceN* province = nullptr;
        {
            auto result = session.from<ProvinceN>().select().toVector();
            QCOMPARE(result.size(), 1);
            province = result.first();
        }

        TownN* town = nullptr;
        {
            auto result = session.from<TownN>().select().toVector();
            QCOMPARE(result.size(), 1);
            town = result.first();
        }

        QVERIFY(province != nullptr);
        QCOMPARE(province->m_id, 1);

        QVERIFY(town != nullptr);
        QCOMPARE(town->m_id, 1);
        QCOMPARE(town->m_province, province);
    }
}

class Town1;

class Province1 : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)
    Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)
    Q_PROPERTY(QVector<Town1*> towns MEMBER m_towns NOTIFY townsChanged)

public:
    Q_INVOKABLE Province1() = default;

    int m_id{0};
    QVector<Town1*> m_towns;
    QString m_name;

signals:
    void idChanged();
    void nameChanged();
    void townsChanged();
};

class Town1 : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)
    Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)

public:
    Q_INVOKABLE Town1() = default;

    int m_id;
    QString m_name;

signals:
    void idChanged();
    void nameChanged();
};

void DomainClassesTest::testUnidirectionalReferenceOn1Side()
{
    QSKIP("Unidirectional references on the 1 side of 1:n relationships are not supported");

    qRegisterOrmEntity<Province1, Town1>();

    {
        Province1* province = new Province1;
        Town1* town = new Town1;
        province->m_towns.push_back(town);

        QOrmSession session;
        QVERIFY(session.merge(province, town));
    }

    {
        QOrmSession session{QOrmSessionConfiguration::fromFile(":/qtorm_bypass_schema.json")};

        Province1* province = nullptr;
        {
            auto result = session.from<Province1>().select().toVector();
            QCOMPARE(result.size(), 1);
            province = result.first();
        }

        Town1* town = nullptr;
        {
            auto result = session.from<Town1>().select().toVector();
            QCOMPARE(result.size(), 1);
            town = result.first();
        }

        QVERIFY(province != nullptr);
        QCOMPARE(province->m_id, 1);
        QCOMPARE(province->m_towns.size(), 1);
        QVERIFY(province->m_towns.contains(town));

        QVERIFY(town != nullptr);
        QCOMPARE(town->m_id, 1);
    }
}

enum class MyEnum
{
    Value1,
    Value2
};
Q_DECLARE_METATYPE(MyEnum);

namespace Namespace
{
    enum class MyEnum
    {
        Value1 = 10,
        Value2
    };
};
Q_DECLARE_METATYPE(Namespace::MyEnum);

class WithEnum : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)
    Q_PROPERTY(MyEnum myEnum MEMBER m_myEnum NOTIFY myEnumChanged)
    Q_PROPERTY(
        Namespace::MyEnum myNamespaceEnum MEMBER m_myNamespaceEnum NOTIFY myNamespaceEnumChanged)

public:
    Q_INVOKABLE WithEnum() = default;

    [[nodiscard]] int id() const { return m_id; }
    void setId(int id)
    {
        if (m_id != id)
        {
            m_id = id;
            emit idChanged();
        }
    }

    [[nodiscard]] MyEnum myEnum() const { return m_myEnum; }
    void setMyEnum(MyEnum myEnum)
    {
        if (m_myEnum != myEnum)
        {
            m_myEnum = myEnum;
            emit myEnumChanged();
        }
    }

    [[nodiscard]] Namespace::MyEnum myNamespaceEnum() const { return m_myNamespaceEnum; }
    void setMyNamespaceEnum(Namespace::MyEnum myNamespaceEnum)
    {
        if (m_myNamespaceEnum != myNamespaceEnum)
        {
            m_myNamespaceEnum = myNamespaceEnum;
            emit myNamespaceEnumChanged();
        }
    }

signals:
    void idChanged();
    void myEnumChanged();
    void myNamespaceEnumChanged();

private:
    int m_id{-1};
    MyEnum m_myEnum{MyEnum::Value1};
    Namespace::MyEnum m_myNamespaceEnum{Namespace::MyEnum::Value1};
};

void DomainClassesTest::testEnumInProperty()
{
    qRegisterOrmEntity<WithEnum>();
    qRegisterOrmEnum<MyEnum, Namespace::MyEnum>();

    {
        WithEnum* withEnum = new WithEnum;
        withEnum->setMyEnum(MyEnum::Value2);
        withEnum->setMyNamespaceEnum(Namespace::MyEnum::Value2);

        QOrmSession session;
        QVERIFY(session.merge(withEnum));
    }

    {
        QOrmSession session{QOrmSessionConfiguration::fromFile(":/qtorm_bypass_schema.json")};

        WithEnum* withEnum = nullptr;
        {
            auto result = session.from<WithEnum>().select().toVector();
            QCOMPARE(result.size(), 1);
            withEnum = result.first();
        }

        QVERIFY(withEnum != nullptr);
        QCOMPARE(withEnum->myEnum(), MyEnum::Value2);
        QCOMPARE(withEnum->myNamespaceEnum(), Namespace::MyEnum::Value2);

        withEnum->setMyEnum(MyEnum::Value1);
        withEnum->setMyNamespaceEnum(Namespace::MyEnum::Value1);
        QVERIFY(session.merge(withEnum));
    }

    {
        QOrmSession session{QOrmSessionConfiguration::fromFile(":/qtorm_bypass_schema.json")};

        WithEnum* withEnum = nullptr;
        {
            auto result = session.from<WithEnum>().select().toVector();
            QCOMPARE(result.size(), 1);
            withEnum = result.first();
        }

        QVERIFY(withEnum != nullptr);
        QCOMPARE(withEnum->myEnum(), MyEnum::Value1);
        QCOMPARE(withEnum->myNamespaceEnum(), Namespace::MyEnum::Value1);
    }
}

QTEST_GUILESS_MAIN(DomainClassesTest)

#include "tst_domainclasses.moc"
