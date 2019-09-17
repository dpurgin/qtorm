#include "qormwhereclause.h"
#include "qormclassproperty.h"

#include <QVariant>

class QOrmWhereClausePrivate : public QSharedData
{
    friend class QOrmWhereClause;

    QOrmWhereClausePrivate(const QOrmClassProperty& property,
                           QOrm::Comparison comparison,
                           const QVariant& value)
        : m_property{property},
          m_comparison{comparison},
          m_value{value}
    {
    }

    QOrmClassProperty m_property;
    QOrm::Comparison m_comparison;
    QVariant m_value;
};

QOrmWhereClause::QOrmWhereClause(const QOrmClassProperty& field,
                                 QOrm::Comparison comparison,
                                 const QVariant& value)
    : d{new QOrmWhereClausePrivate{field, comparison, value}}
{
}

QOrmWhereClause::QOrmWhereClause(const QOrmWhereClause&) = default;

QOrmWhereClause::QOrmWhereClause(QOrmWhereClause&&) = default;

QOrmWhereClause::~QOrmWhereClause() = default;

QOrmWhereClause& QOrmWhereClause::operator=(const QOrmWhereClause&) = default;

QOrmWhereClause& QOrmWhereClause::operator=(QOrmWhereClause&&) = default;

QOrmClassProperty QOrmWhereClause::property() const
{
    return d->m_property;
}

QOrm::Comparison QOrmWhereClause::comparison() const
{
    return d->m_comparison;
}

QVariant QOrmWhereClause::value() const
{
    return d->m_value;
}
