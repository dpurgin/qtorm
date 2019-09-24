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
    QOrmQuery build(const QOrmMetadata& projection) const;

    Q_REQUIRED_RESULT
    QOrmFilterExpression resolvedFilterExpression(const QOrmMetadata& relation,
                                                  const QOrmFilterExpression& expression) const;

    QOrmSession* m_session{nullptr};

    QOrmMetadata m_relation;
    QOrmMetadata m_projection;

    QVector<QOrmFilter> m_filters;
    std::optional<QOrmOrderBuilder> m_orderBuilder;
};

QOrmQuery QOrmQueryBuilderPrivate::build(const QOrmMetadata& projection) const
{    
    QOrmFilter filter;

    // Fold filter expressions
    if (!m_filters.empty())
    {
        decltype(m_filters)::const_iterator it = std::cbegin(m_filters);

        // skip to first non-empty filter
        while (it != std::cend(m_filters) && it->type() == QOrm::FilterType::Empty)
            ++it;

        // currently only filter expressions are supported
        Q_ASSERT(it->type() == QOrm::FilterType::Expression);
        filter = *it;

        for (it = it + 1; it != std::cend(m_filters); ++it)
        {
            Q_ASSERT(it->type() == QOrm::FilterType::Expression);
            filter = QOrmFilter{*filter.expression() && *it->expression()};
        }

        Q_ASSERT(filter.type() == QOrm::FilterType::Expression);
        filter = QOrmFilter{resolvedFilterExpression(m_relation, *filter.expression())};
    }

    QOrmOrder order;

    return QOrmQuery{QOrm::Operation::Read,
                     projection,
                     m_relation,
                     filter,
                     order};
}

QOrmFilterExpression QOrmQueryBuilderPrivate::resolvedFilterExpression(
    const QOrmMetadata& relation,
    const QOrmFilterExpression& expression) const
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

QOrmQuery QOrmQueryBuilder::build() const
{
    return d->build(d->m_projection);
}

QOrmQueryResult QOrmQueryBuilder::select()
{
    return d->m_session->execute(build());
}

QOrmQueryResult QOrmQueryBuilder::select(const QMetaObject& projectionMetaObject) const
{
    const QOrmMetadata& meta = (*d->m_session->metadataCache())[projectionMetaObject];

    QOrmQuery query{d->build(meta)};

    return d->m_session->execute(query);
}

QT_END_NAMESPACE
