#include "qormwhereclause.h"

#include <QOrmField>
#include <QVariant>

class QOrmWhereClausePrivate : public QSharedData
{
    friend class QOrmWhereClause;

    QOrmWhereClausePrivate(const QOrmField& field,
                           QOrm::Comparison comparison,
                           const QVariant& value)
        : m_field{field},
          m_comparison{comparison},
          m_value{value}
    {
    }

    QOrmField m_field;
    QOrm::Comparison m_comparison;
    QVariant m_value;
};

QOrmWhereClause::QOrmWhereClause(const QOrmField& field,
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

QOrmField QOrmWhereClause::field() const
{
    return d->m_field;
}

QOrm::Comparison QOrmWhereClause::comparison() const
{
    return d->m_comparison;
}

QVariant QOrmWhereClause::value() const
{
    return d->m_value;
}
