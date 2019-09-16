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

    QOrmSession* m_ormSession{nullptr};

    QMetaObject m_relation;
    QMetaObject m_projection;

    QOrmWhereClauseBuilder m_whereClauseBuilder;
    QOrmOrderClauseBuilder m_orderClauseBuilder;
};

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
    return QOrmQuery{QOrm::Operation::Read,
                     d->m_projection,
                     d->m_relation,
                     d->m_whereClauseBuilder.build(),
                     d->m_orderClauseBuilder.build()};
}

QOrmQueryResult QOrmQueryBuilder::select()
{
    return d->m_ormSession->execute(build());
}

QOrmQueryResult QOrmQueryBuilder::select(const QMetaObject& projectionMetaObject) const
{
    QOrmQuery query{QOrm::Operation::Read,
                    projectionMetaObject,
                    d->m_relation,
                    d->m_whereClauseBuilder.build(),
                    d->m_orderClauseBuilder.build()};

    return d->m_ormSession->execute(query);
}
QT_END_NAMESPACE
