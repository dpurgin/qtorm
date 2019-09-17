#include "qormquerybuilder.h"

#include "qormerror.h"
#include "qormquery.h"
#include "qormwhereclause.h"
#include "qormwhereclausebuilder.h"
#include "qormorderclause.h"
#include "qormorderclausebuilder.h"
#include "qormsession.h"
#include "qormqueryresult.h"
#include "qormabstractprovider.h"

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

    std::optional<QOrmWhereClauseBuilder> m_whereClauseBuilder;
    std::optional<QOrmOrderClauseBuilder> m_orderClauseBuilder;
};

QOrmQuery QOrmQueryBuilderPrivate::build(const QMetaObject& projection) const
{
    std::optional<QOrmWhereClause> whereClause =
            m_whereClauseBuilder.has_value()? std::make_optional(m_whereClauseBuilder->build())
                                            : std::nullopt;
    std::optional<QOrmOrderClause> orderClause =
            m_orderClauseBuilder.has_value()? std::make_optional(m_orderClauseBuilder->build())
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

QOrmQueryBuilder& QOrmQueryBuilder::where(QOrmWhereClauseBuilder whereClause)
{
    d->m_whereClauseBuilder = whereClause;
    return *this;
}

QOrmQueryBuilder& QOrmQueryBuilder::order(QOrmOrderClauseBuilder orderClause)
{
    d->m_orderClauseBuilder = orderClause;
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
