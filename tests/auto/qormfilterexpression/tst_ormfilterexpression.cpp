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

#include <QtTest>

#include <QOrmFilterExpression>

class QOrmFilterExpressionTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void init();
    void cleanup();

    void testTerminalPredicateGeneration();
    void testTerminalPredicateGenerationLong();
    void testTerminalPredicateGenerationEntity();
    void testUnaryPredicateGeneration();
    void testBinaryPredicateGeneration();
    void testNestedPredicateGeneration();
};

class Entity : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id MEMBER m_id NOTIFY idChanged)

    int m_id{0};

public:
    Entity(QObject* parent)
        : QObject{parent}
    {
    }

signals:
    void idChanged();
};

void QOrmFilterExpressionTest::initTestCase()
{
    qRegisterOrmEntity<Entity>();
}

void QOrmFilterExpressionTest::init()
{
}

void QOrmFilterExpressionTest::cleanup()
{
}

void QOrmFilterExpressionTest::testTerminalPredicateGeneration()
{
    {
        QOrmFilterTerminalPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) == 3);
        QCOMPARE(predicate.classProperty()->descriptor(), "id");
        QCOMPARE(predicate.comparison(), QOrm::Comparison::Equal);
        QCOMPARE(predicate.value(), 3);
    }

    {
        QOrmFilterTerminalPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) != 3);
        QCOMPARE(predicate.classProperty()->descriptor(), "id");
        QCOMPARE(predicate.comparison(), QOrm::Comparison::NotEqual);
        QCOMPARE(predicate.value(), 3);
    }

    {
        QOrmFilterTerminalPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) < 3);
        QCOMPARE(predicate.classProperty()->descriptor(), "id");
        QCOMPARE(predicate.comparison(), QOrm::Comparison::Less);
        QCOMPARE(predicate.value(), 3);
    }

    {
        QOrmFilterTerminalPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) <= 3);
        QCOMPARE(predicate.classProperty()->descriptor(), "id");
        QCOMPARE(predicate.comparison(), QOrm::Comparison::LessOrEqual);
        QCOMPARE(predicate.value(), 3);
    }

    {
        QOrmFilterTerminalPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) > 3);
        QCOMPARE(predicate.classProperty()->descriptor(), "id");
        QCOMPARE(predicate.comparison(), QOrm::Comparison::Greater);
        QCOMPARE(predicate.value(), 3);
    }

    {
        QOrmFilterTerminalPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) >= 3);
        QCOMPARE(predicate.classProperty()->descriptor(), "id");
        QCOMPARE(predicate.comparison(), QOrm::Comparison::GreaterOrEqual);
        QCOMPARE(predicate.value(), 3);
    }
}

void QOrmFilterExpressionTest::testTerminalPredicateGenerationLong()
{
    {
        QOrmFilterTerminalPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) == 3L);
        QCOMPARE(predicate.classProperty()->descriptor(), "id");
        QCOMPARE(predicate.comparison(), QOrm::Comparison::Equal);
        QCOMPARE(predicate.value(), 3);
    }

    {
        QOrmFilterTerminalPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) != 3L);
        QCOMPARE(predicate.classProperty()->descriptor(), "id");
        QCOMPARE(predicate.comparison(), QOrm::Comparison::NotEqual);
        QCOMPARE(predicate.value(), 3);
    }

    {
        QOrmFilterTerminalPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) < 3L);
        QCOMPARE(predicate.classProperty()->descriptor(), "id");
        QCOMPARE(predicate.comparison(), QOrm::Comparison::Less);
        QCOMPARE(predicate.value(), 3);
    }

    {
        QOrmFilterTerminalPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) <= 3L);
        QCOMPARE(predicate.classProperty()->descriptor(), "id");
        QCOMPARE(predicate.comparison(), QOrm::Comparison::LessOrEqual);
        QCOMPARE(predicate.value(), 3);
    }

    {
        QOrmFilterTerminalPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) > 3L);
        QCOMPARE(predicate.classProperty()->descriptor(), "id");
        QCOMPARE(predicate.comparison(), QOrm::Comparison::Greater);
        QCOMPARE(predicate.value(), 3);
    }

    {
        QOrmFilterTerminalPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) >= 3L);
        QCOMPARE(predicate.classProperty()->descriptor(), "id");
        QCOMPARE(predicate.comparison(), QOrm::Comparison::GreaterOrEqual);
        QCOMPARE(predicate.value(), 3);
    }
}

void QOrmFilterExpressionTest::testTerminalPredicateGenerationEntity()
{
    Entity* e = new Entity{this};

    {
        QOrmFilterTerminalPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) == e);
        QCOMPARE(predicate.classProperty()->descriptor(), "id");
        QCOMPARE(predicate.comparison(), QOrm::Comparison::Equal);
        QCOMPARE(predicate.value(), QVariant::fromValue(e));
    }

    {
        QOrmFilterTerminalPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) != e);
        QCOMPARE(predicate.classProperty()->descriptor(), "id");
        QCOMPARE(predicate.comparison(), QOrm::Comparison::NotEqual);
        QCOMPARE(predicate.value(), QVariant::fromValue(e));
    }

    {
        QOrmFilterTerminalPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) < e);
        QCOMPARE(predicate.classProperty()->descriptor(), "id");
        QCOMPARE(predicate.comparison(), QOrm::Comparison::Less);
        QCOMPARE(predicate.value(), QVariant::fromValue(e));
    }

    {
        QOrmFilterTerminalPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) <= e);
        QCOMPARE(predicate.classProperty()->descriptor(), "id");
        QCOMPARE(predicate.comparison(), QOrm::Comparison::LessOrEqual);
        QCOMPARE(predicate.value(), QVariant::fromValue(e));
    }

    {
        QOrmFilterTerminalPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) > e);
        QCOMPARE(predicate.classProperty()->descriptor(), "id");
        QCOMPARE(predicate.comparison(), QOrm::Comparison::Greater);
        QCOMPARE(predicate.value(), QVariant::fromValue(e));
    }

    {
        QOrmFilterTerminalPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) >= e);
        QCOMPARE(predicate.classProperty()->descriptor(), "id");
        QCOMPARE(predicate.comparison(), QOrm::Comparison::GreaterOrEqual);
        QCOMPARE(predicate.value(), QVariant::fromValue(e));
    }
}
void QOrmFilterExpressionTest::testUnaryPredicateGeneration()
{
    {
        QOrmFilterUnaryPredicate predicate = !(Q_ORM_CLASS_PROPERTY(id) == 3);
        QCOMPARE(predicate.logicalOperator(), QOrm::UnaryLogicalOperator::Not);

        QOrmFilterExpression expr = predicate.rhs();
        QCOMPARE(expr.type(), QOrm::FilterExpressionType::TerminalPredicate);
        QVERIFY(expr.terminalPredicate() != nullptr);
        QVERIFY(expr.unaryPredicate() == nullptr);
        QVERIFY(expr.binaryPredicate() == nullptr);
    }
}

void QOrmFilterExpressionTest::testBinaryPredicateGeneration()
{
    {
        QOrmFilterBinaryPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) > 3 &&
                                               Q_ORM_CLASS_PROPERTY(id) < 5);
        QCOMPARE(predicate.logicalOperator(), QOrm::BinaryLogicalOperator::And);

        QCOMPARE(predicate.lhs().type(), QOrm::FilterExpressionType::TerminalPredicate);
        QCOMPARE(predicate.rhs().type(), QOrm::FilterExpressionType::TerminalPredicate);
    }

    {
        QOrmFilterBinaryPredicate predicate = (Q_ORM_CLASS_PROPERTY(id) > 3 ||
                                               Q_ORM_CLASS_PROPERTY(id) < 5);
        QCOMPARE(predicate.logicalOperator(), QOrm::BinaryLogicalOperator::Or);

        QCOMPARE(predicate.lhs().type(), QOrm::FilterExpressionType::TerminalPredicate);
        QCOMPARE(predicate.rhs().type(), QOrm::FilterExpressionType::TerminalPredicate);
    }
}

void QOrmFilterExpressionTest::testNestedPredicateGeneration()
{
    {
        QOrmFilterExpression expr =
                !(Q_ORM_CLASS_PROPERTY(id) > 0 && Q_ORM_CLASS_PROPERTY(id) < 10) ||
                (Q_ORM_CLASS_PROPERTY(name) == "some string");

        QCOMPARE(expr.type(), QOrm::FilterExpressionType::BinaryPredicate);
        QCOMPARE(expr.binaryPredicate()->lhs().type(), QOrm::FilterExpressionType::UnaryPredicate);
        QCOMPARE(expr.binaryPredicate()->rhs().type(), QOrm::FilterExpressionType::TerminalPredicate);

        QCOMPARE(expr.binaryPredicate()->lhs().unaryPredicate()->logicalOperator(), QOrm::UnaryLogicalOperator::Not);
        QCOMPARE(expr.binaryPredicate()->lhs().unaryPredicate()->rhs().type(), QOrm::FilterExpressionType::BinaryPredicate);
        QCOMPARE(expr.binaryPredicate()->lhs().unaryPredicate()->rhs().binaryPredicate()->logicalOperator(), QOrm::BinaryLogicalOperator::And);
        QCOMPARE(expr.binaryPredicate()->lhs().unaryPredicate()->rhs().binaryPredicate()->lhs().type(), QOrm::FilterExpressionType::TerminalPredicate);
        QCOMPARE(expr.binaryPredicate()->lhs().unaryPredicate()->rhs().binaryPredicate()->rhs().type(), QOrm::FilterExpressionType::TerminalPredicate);
    }
}

QTEST_APPLESS_MAIN(QOrmFilterExpressionTest)

#include "tst_ormfilterexpression.moc"
