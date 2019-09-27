#include "qormrelation.h"
#include "qormmetadata.h"
#include "qormquery.h"

#include <QDebug>
#include <variant>

QT_BEGIN_NAMESPACE

class QOrmRelationPrivate : public QSharedData
{
    friend class QOrmRelation;

    using RelationStorageType = std::variant<QOrmMetadata, QOrmQuery>;

    QOrmRelationPrivate(RelationStorageType relation)
        : m_relation{std::move(relation)}
    {
    }

    std::variant<QOrmMetadata, QOrmQuery> m_relation;
};

QOrmRelation::QOrmRelation(const QOrmMetadata& mapping)
    : d{new QOrmRelationPrivate{mapping}}
{
}

QOrmRelation::QOrmRelation(const QOrmQuery& query)
    : d{new QOrmRelationPrivate{query}}
{
}

QOrmRelation::QOrmRelation(const QOrmRelation&) = default;

QOrmRelation::QOrmRelation(QOrmRelation&&) = default;

QOrmRelation::~QOrmRelation() = default;

QOrmRelation& QOrmRelation::operator=(const QOrmRelation&) = default;

QOrmRelation& QOrmRelation::operator=(QOrmRelation&&) = default;

QOrm::RelationType QOrmRelation::type() const
{
    if (std::holds_alternative<QOrmMetadata>(d->m_relation))
        return QOrm::RelationType::Mapping;
    else if (std::holds_alternative<QOrmQuery>(d->m_relation))
        return QOrm::RelationType::Query;

    qFatal("QtORM: Unexpected state of QOrmRelation");
}

const QOrmMetadata* QOrmRelation::mapping() const
{
    return std::get_if<QOrmMetadata>(&d->m_relation);
}

const QOrmQuery* QOrmRelation::query() const
{
    return std::get_if<QOrmQuery>(&d->m_relation);
}

QDebug operator<<(QDebug dbg, const QOrmRelation& relation)
{
    QDebugStateSaver saver{dbg};

    dbg.noquote().nospace() << "QOrmRelation(" << relation.type() << ", ";

    switch (relation.type())
    {
        case QOrm::RelationType::Query:
            dbg << *relation.query();
            break;

        case QOrm::RelationType::Mapping:
            dbg << *relation.mapping();
            break;
    }

    dbg << ")";

    return dbg;
}

QT_END_NAMESPACE
