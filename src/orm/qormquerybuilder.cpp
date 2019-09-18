#include "qormquerybuilder.h"

#include "qormabstractprovider.h"
#include "qormerror.h"
#include "qormfilter.h"
#include "qormorder.h"
#include "qormquery.h"
#include "qormqueryresult.h"
#include "qormsession.h"

QT_BEGIN_NAMESPACE

class QOrmQueryBuilderPrivate : public QSharedData
{
    friend class QOrmQueryBuilder;

    QOrmQueryBuilderPrivate(QOrmSession* ormSession, const QMetaObject& relationMetaObject)
        : m_ormSession{ormSession},
          m_relation{relationMetaObject},
          m_projection{relationMetaObject}
    {
        Q_ASSERT(ormSession != nullptr);
    }

    QOrmQuery build(const QMetaObject& projection) const;

    QOrmSession* m_ormSession{nullptr};

    QMetaObject m_relation;
    QMetaObject m_projection;

    std::optional<QOrmFilterBuilder> m_filterBuilder;
    std::optional<QOrmOrderBuilder> m_orderBuilder;
};

QOrmQuery QOrmQueryBuilderPrivate::build(const QMetaObject& projection) const
{
    std::optional<QOrmFilter> whereClause =
            m_filterBuilder.has_value()? std::make_optional(m_filterBuilder->build())
                                       : std::nullopt;
    std::optional<QOrmOrder> orderClause =
            m_orderBuilder.has_value()? std::make_optional(m_orderBuilder->build())
                                      : std::nullopt;

    return QOrmQuery{QOrm::Operation::Read,
                     projection,
                     m_relation,
                     whereClause,
                     orderClause};
}

QOrmQueryBuilder::QOrmQueryBuilder(QOrmSession* ormSession,
                                   const QMetaObject& relationMetaObject)
    : d{new QOrmQueryBuilderPrivate{ormSession, relationMetaObject}}
{
}

QOrmQueryBuilder::QOrmQueryBuilder(const QOrmQueryBuilder&) = default;

QOrmQueryBuilder::QOrmQueryBuilder(QOrmQueryBuilder&&) = default;

QOrmQueryBuilder::~QOrmQueryBuilder() = default;

QOrmQueryBuilder& QOrmQueryBuilder::operator=(const QOrmQueryBuilder&) = default;

QOrmQueryBuilder& QOrmQueryBuilder::operator=(QOrmQueryBuilder&&) = default;

QOrmQueryBuilder& QOrmQueryBuilder::filter(QOrmFilterBuilder filterBuilder)
{
    d->m_filterBuilder = filterBuilder;
    return *this;
}

QOrmQueryBuilder& QOrmQueryBuilder::order(QOrmOrderBuilder orderBuilder)
{
    d->m_orderBuilder = orderBuilder;
    return *this;
}

QOrmQueryBuilder& QOrmQueryBuilder::projection(const QMetaObject& projectionMetaObject)
{
    d->m_projection = projectionMetaObject;
    return *this;
}

QOrmQuery QOrmQueryBuilder::build() const
{
    return d->build(d->m_projection);
}

QOrmQueryResult QOrmQueryBuilder::select()
{
    return d->m_ormSession->execute(build());
}

QOrmQueryResult QOrmQueryBuilder::select(const QMetaObject& projectionMetaObject) const
{
    QOrmQuery query{d->build(projectionMetaObject)};

    return d->m_ormSession->execute(query);
}

QT_END_NAMESPACE
