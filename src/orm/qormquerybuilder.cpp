#include "qormquerybuilder.h"

#include "qormabstractprovider.h"
#include "qormerror.h"
#include "qormfilter.h"
#include "qormfilterexpression.h"
#include "qormmetadatacache.h"
#include "qormorder.h"
#include "qormquery.h"
#include "qormqueryresult.h"
#include "qormsession.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

class QOrmQueryBuilderPrivate : public QSharedData
{
    friend class QOrmQueryBuilder;

    QOrmQueryBuilderPrivate(QOrmSession* ormSession, const QOrmMetadata& relationMeta)
        : m_session{ormSession}, m_relation{relationMeta}, m_projection{relationMeta}
    {
        Q_ASSERT(ormSession != nullptr);
    }

    Q_REQUIRED_RESULT
    static QOrmFilter foldFilters(const QOrmMetadata& relation, const QVector<QOrmFilter>& filters);

    Q_REQUIRED_RESULT
    static QOrmFilterExpression resolvedFilterExpression(const QOrmMetadata& relation,
                                                         const QOrmFilterExpression& expression);

    QOrmSession* m_session{nullptr};

    QOrm::Operation m_operation;

    QOrmMetadata m_relation;
    QOrmMetadata m_projection;

    QVector<QOrmFilter> m_filters;
    std::optional<QOrmOrderBuilder> m_orderBuilder;
};

QOrmFilter QOrmQueryBuilderPrivate::foldFilters(const QOrmMetadata& relation,
                                                const QVector<QOrmFilter>& filters)
{
    QOrmFilter filter;

    // Fold filter expressions
    if (!filters.empty())
    {
        auto it = std::cbegin(filters);

        // skip to first non-empty filter
        while (it != std::cend(filters) && it->type() == QOrm::FilterType::Empty)
            ++it;

        // currently only filter expressions are supported
        Q_ASSERT(it->type() == QOrm::FilterType::Expression);
        filter = *it;

        for (it = it + 1; it != std::cend(filters); ++it)
        {
            Q_ASSERT(it->type() == QOrm::FilterType::Expression);
            filter = QOrmFilter{*filter.expression() && *it->expression()};
        }

        Q_ASSERT(filter.type() == QOrm::FilterType::Expression);
        filter = QOrmFilter{resolvedFilterExpression(relation, *filter.expression())};
    }

    return filter;
}

QOrmFilterExpression
QOrmQueryBuilderPrivate::resolvedFilterExpression(const QOrmMetadata& relation,
                                                  const QOrmFilterExpression& expression)
{
    switch (expression.type())
    {
        case QOrm::FilterExpressionType::TerminalPredicate:
        {
            const QOrmFilterTerminalPredicate* predicate = expression.terminalPredicate();

            if (predicate->isResolved())
                return *predicate;

            const QOrmPropertyMapping* propertyMapping =
                relation.classPropertyMapping(predicate->classProperty()->descriptor());

            if (propertyMapping == nullptr)
            {
                qCritical() << "QtOrm: Unable to resolve filter expression for class property"
                            << predicate->classProperty()->descriptor()
                            << ", relation" << relation;
                qFatal("QtOrm: Malformed query filter");
            }

            return QOrmFilterTerminalPredicate{
                *propertyMapping, predicate->comparison(), predicate->value()};
        }

        case QOrm::FilterExpressionType::BinaryPredicate:
        {
            const QOrmFilterBinaryPredicate* predicate = expression.binaryPredicate();
            return QOrmFilterBinaryPredicate{resolvedFilterExpression(relation, predicate->lhs()),
                                             predicate->logicalOperator(),
                                             resolvedFilterExpression(relation, predicate->rhs())};
        }

        case QOrm::FilterExpressionType::UnaryPredicate:
        {
            const QOrmFilterUnaryPredicate* predicate = expression.unaryPredicate();
            return QOrmFilterUnaryPredicate{predicate->logicalOperator(),
                                            resolvedFilterExpression(relation, predicate->rhs())};
        }
    }

    qFatal("QtOrm: Unexpected state in %s", __PRETTY_FUNCTION__);
}

QOrmQueryBuilder::QOrmQueryBuilder(QOrmSession* ormSession,
                                   const QOrmMetadata& relationMetaObject)
    : d{new QOrmQueryBuilderPrivate{ormSession, relationMetaObject}}
{
}

QOrmQueryBuilder::QOrmQueryBuilder(const QOrmQueryBuilder&) = default;

QOrmQueryBuilder::QOrmQueryBuilder(QOrmQueryBuilder&&) = default;

QOrmQueryBuilder::~QOrmQueryBuilder() = default;

QOrmQueryBuilder& QOrmQueryBuilder::operator=(const QOrmQueryBuilder&) = default;

QOrmQueryBuilder& QOrmQueryBuilder::operator=(QOrmQueryBuilder&&) = default;

QOrmQueryBuilder& QOrmQueryBuilder::filter(QOrmFilterExpression expression)
{
    d->m_filters.push_back(QOrmFilter{expression});
    return *this;
}

QOrmQueryBuilder& QOrmQueryBuilder::order(QOrmOrderBuilder orderBuilder)
{
    d->m_orderBuilder = orderBuilder;
    return *this;
}

QOrmQueryBuilder& QOrmQueryBuilder::projection(const QMetaObject& projectionMetaObject)
{
    d->m_projection = (*d->m_session->metadataCache())[projectionMetaObject];
    return *this;
}

QOrmQuery QOrmQueryBuilder::build(QOrm::Operation operation) const
{
    return QOrmQuery{operation,
                     d->m_projection,
                     d->m_relation,
                     d->foldFilters(d->m_relation, d->m_filters),
                     d->m_orderBuilder->build()};
}

QOrmQueryResult QOrmQueryBuilder::select()
{
    QOrmFilter filter = QOrmQueryBuilderPrivate::foldFilters(d->m_relation, d->m_filters);

    QOrmQuery query{
        QOrm::Operation::Read, d->m_projection, d->m_relation, filter, d->m_orderBuilder->build()};

    return d->m_session->execute(query);
}

QOrmQueryResult QOrmQueryBuilder::select(const QMetaObject& projectionMetaObject) const
{
    const QOrmMetadata& projection = (*d->m_session->metadataCache())[projectionMetaObject];

    QOrmFilter filter = QOrmQueryBuilderPrivate::foldFilters(d->m_relation, d->m_filters);

    QOrmQuery query{
        QOrm::Operation::Read, projection, d->m_relation, filter, d->m_orderBuilder->build()};

    return d->m_session->execute(query);
}

QOrmQueryResult QOrmQueryBuilder::remove(QOrm::RemoveMode removeMode) const
{
    if (removeMode != QOrm::RemoveMode::ForceRemoveAll && d->m_filters.isEmpty())
    {
        qCritical() << "QtORM: Attempting to remove all entries in" << d->m_relation
                    << ". Either provide a filter or pass QOrm::RemoveMode::ForceRemoveAll";
        qFatal("QtORM: Security check failure");
    }

    QOrmFilter filter = QOrmQueryBuilderPrivate::foldFilters(d->m_relation, d->m_filters);

    QOrmQuery query{QOrm::Operation::Delete,
                    d->m_projection,
                    d->m_relation,
                    filter,
                    d->m_orderBuilder->build()};

    return d->m_session->execute(query);
}

QT_END_NAMESPACE
