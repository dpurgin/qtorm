/*
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

#include <QOrmEntityInstanceCache>

#include <QOrmMetadataCache>
#include <QtTest>

#include "domain/province.h"
#include "domain/town.h"

// add necessary includes here

class EntityInstanceCache : public QObject
{
    Q_OBJECT

public:
    EntityInstanceCache();
    ~EntityInstanceCache();

private slots:
    void init();

    void testWithObjectId();
    void testModificationTracked();
};

EntityInstanceCache::EntityInstanceCache()
{

}

EntityInstanceCache::~EntityInstanceCache()
{

}

void EntityInstanceCache::init()
{
    qRegisterOrmEntity<Province, Town>();
}

void EntityInstanceCache::testWithObjectId()
{
    QOrmMetadataCache metadataCache;
    QOrmEntityInstanceCache instanceCache;

    std::unique_ptr<Province> upperAustria{new Province(1, QString::fromUtf8("Oberösterreich"))};

    instanceCache.insert(metadataCache.get<Province>(), upperAustria.get());
    instanceCache.finalize(metadataCache.get<Province>(), upperAustria.get());

    QVERIFY(instanceCache.contains(upperAustria.get()));
    QVERIFY(!instanceCache.isModified(upperAustria.get()));
    QCOMPARE(instanceCache.get(metadataCache.get<Province>(), 1), upperAustria.get());
    QCOMPARE(instanceCache.get(metadataCache.get<Province>(), 2), nullptr);
    QCOMPARE(instanceCache.get(metadataCache.get<Town>(), 1), nullptr);
    QCOMPARE(instanceCache.get(metadataCache.get<Town>(), 2), nullptr);

    std::unique_ptr<Town> hagenberg{new Town(1, QString::fromUtf8("Hagenberg"), nullptr)};
    instanceCache.insert(metadataCache.get<Town>(), hagenberg.get());
    instanceCache.finalize(metadataCache.get<Town>(), hagenberg.get());

    QVERIFY(instanceCache.contains(upperAustria.get()));
    QVERIFY(instanceCache.contains(hagenberg.get()));
    QVERIFY(!instanceCache.isModified(upperAustria.get()));
    QVERIFY(!instanceCache.isModified(hagenberg.get()));
    QCOMPARE(instanceCache.get(metadataCache.get<Province>(), 1), upperAustria.get());
    QCOMPARE(instanceCache.get(metadataCache.get<Province>(), 2), nullptr);
    QCOMPARE(instanceCache.get(metadataCache.get<Town>(), 1), hagenberg.get());
    QCOMPARE(instanceCache.get(metadataCache.get<Town>(), 2), nullptr);

    QCOMPARE(instanceCache.take(upperAustria.get()), upperAustria.get());
    QCOMPARE(instanceCache.get(metadataCache.get<Province>(), 1), nullptr);
    QVERIFY(!instanceCache.contains(upperAustria.get()));

    QCOMPARE(instanceCache.take(hagenberg.get()), hagenberg.get());
    QCOMPARE(instanceCache.get(metadataCache.get<Town>(), 1), nullptr);
    QVERIFY(!instanceCache.contains(hagenberg.get()));
}

void EntityInstanceCache::testModificationTracked()
{
    QOrmMetadataCache metadataCache;
    QOrmEntityInstanceCache instanceCache;

    Province* upperAustria = new Province(1, QString::fromUtf8("Oberösterreich"));
    instanceCache.insert(metadataCache.get<Province>(), upperAustria);
    instanceCache.finalize(metadataCache.get<Province>(), upperAustria);

    QVERIFY(!instanceCache.isModified(upperAustria));

    upperAustria->setName(QString::fromUtf8("Upper Austria"));
    QVERIFY(instanceCache.isModified(upperAustria));

    instanceCache.markUnmodified(upperAustria);
    QVERIFY(!instanceCache.isModified(upperAustria));
}

QTEST_APPLESS_MAIN(EntityInstanceCache)

#include "tst_entityinstancecache.moc"
