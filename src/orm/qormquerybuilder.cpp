#include "qormquerybuilder.h"

#include "qormerror.h"
#include "qormquery.h"
#include "qormwhereclause.h"
#include "qormwhereclausebuilder.h"
#include "qormorderclause.h"
#include "qormorderclausebuilder.h"
#include "qormqueryresult.h"
#include "qormabstractprovider.h"

QT_BEGIN_NAMESPACE

class QOrmQueryBuilderPrivate : public QSharedData
{
    friend class QOrmQueryBuilder;

    QOrmQueryBuilderPrivate(QOrmAbstractProvider* provider, const QMetaObject& resultMetaObject)
        : m_provider{provider},
          m_projection{resultMetaObject}
    {
    }

    QOrmAbstractProvider* m_provider{nullptr};
    const QMetaObject& m_projection;

    int m_firstN{-1};
    int m_lastN{-1};
    QOrmWhereClauseBuilder m_whereClauseBuilder;
    QOrmOrderClauseBuilder m_orderClauseBuilder;
};

QOrmQueryBuilder::QOrmQueryBuilder(QOrmAbstractProvider* provider, const QMetaObject& resultMetaObject)
    : d{new QOrmQueryBuilderPrivate{provider, resultMetaObject}}
{
}

QOrmQueryBuilder::QOrmQueryBuilder(const QOrmQueryBuilder&) = default;

#ifdef Q_COMPILER_RVALUE_REFS
QOrmQueryBuilder::QOrmQueryBuilder(QOrmQueryBuilder&&) = default;
#endif

QOrmQueryBuilder::~QOrmQueryBuilder() = default;

#ifdef Q_COMPILER_RVALUE_REFS
QOrmQueryBuilder& QOrmQueryBuilder::operator=(QOrmQueryBuilder&&) = default;
#endif

QOrmQueryBuilder& QOrmQueryBuilder::first(int n)
{
    d->m_firstN = n;
    return *this;
}

QOrmQueryBuilder& QOrmQueryBuilder::last(int n)
{
    d->m_lastN = n;
    return *this;
}

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

QOrmQuery QOrmQueryBuilder::build() const
{
    return QOrmQuery{d->m_projection,
                     d->m_firstN,
                     d->m_lastN,
                     d->m_whereClauseBuilder.build(),
                     d->m_orderClauseBuilder.build()};
}

QVector<QObject*> QOrmQueryBuilder::toVector()
{        
    return d->m_provider->read(build()).toVector();
}

QT_END_NAMESPACE
