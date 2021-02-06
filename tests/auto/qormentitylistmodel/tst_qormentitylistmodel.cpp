/*
 * Copyright (C) 2020-2021 Dmitriy Purgin <dpurgin@gmail.com>
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

#include <QtTest>

#include <QOrmEntityListModel>
#include <QOrmSession>
#include <QOrmSessionConfiguration>

#include "domain/province.h"
#include "domain/town.h"

class EntityListModelTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void testQVectorTInData();
};

void EntityListModelTest::initTestCase()
{
    qRegisterOrmEntity<Province, Town>();
}

// Source: https://github.com/dpurgin/qtorm/issues/15
void EntityListModelTest::testQVectorTInData()
{
    QOrmSession session;

    {
        Province* upperAustria = new Province(QString::fromUtf8("Oberösterreich"));
        Province* lowerAustria = new Province(QString::fromUtf8("Niederösterreich"));
        Province* tirol = new Province(QString::fromUtf8("Tirol"));

        Town* hagenberg = new Town(QString::fromUtf8("Hagenberg"), upperAustria);
        Town* pregarten = new Town(QString::fromUtf8("Pregarten"), upperAustria);
        Town* melk = new Town(QString::fromUtf8("Melk"), lowerAustria);

        upperAustria->setTowns({hagenberg, pregarten});
        lowerAustria->setTowns({melk});

        QVERIFY(session.merge(hagenberg, pregarten, melk, upperAustria, lowerAustria, tirol));
    }

    QOrmEntityListModel<Province> provinces{session};
    QCOMPARE(provinces.rowCount(), 3);

    // Roles are generated automatically in the same order as QObject properties
    // Province::id: UserRole
    // Province::name: UserRole+1
    // Province::rowns: UserRole+2
    QCOMPARE(provinces.data(provinces.index(0), Qt::UserRole).type(), QVariant::Int);
    QCOMPARE(provinces.data(provinces.index(0), Qt::UserRole).toInt(), 1);

    QCOMPARE(provinces.data(provinces.index(0), Qt::UserRole + 1).type(), QVariant::String);
    QCOMPARE(provinces.data(provinces.index(0), Qt::UserRole + 1).toString(),
             QString::fromUtf8("Oberösterreich"));

    QCOMPARE(provinces.data(provinces.index(0), Qt::UserRole + 2).type(), QVariant::List);

    auto val = provinces.data(provinces.index(0), Qt::UserRole + 2).toList();
    QCOMPARE(val.size(), 2);

    Town* hagenberg = qobject_cast<Town*>(val.first().value<QObject*>());
    QVERIFY(hagenberg != nullptr);
    QCOMPARE(hagenberg->id(), 1);
    QCOMPARE(hagenberg->name(), QString::fromUtf8("Hagenberg"));
}

QTEST_GUILESS_MAIN(EntityListModelTest)

#include "tst_qormentitylistmodel.moc"
