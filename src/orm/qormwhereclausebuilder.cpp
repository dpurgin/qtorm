#include "qormwhereclause.h"
#include "qormwhereclausebuilder.h"
#include "qormfield.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

class QOrmWhereClauseBuilderPrivate : public QSharedData
{
    friend class QOrmWhereClauseBuilder;

    QOrmWhereClauseBuilderPrivate(const QOrmField& field,
                                  QOrm::Comparison comparison,
                                  const QVariant& value)
        : m_field{field},
          m_comparison{comparison},
          m_value{value}
    {
    }

    QOrmField m_field;
    QOrm::Comparison m_comparison{QOrm::Comparison::Equals};
    QVariant m_value;        
};

QOrmWhereClauseBuilder::QOrmWhereClauseBuilder()
{
}

QOrmWhereClauseBuilder::QOrmWhereClauseBuilder(const QOrmField& field,
                                 QOrm::Comparison comparison,
                                 const QVariant& value)
    : d{new QOrmWhereClauseBuilderPrivate{field, comparison, value}}
{
}

QOrmWhereClauseBuilder::QOrmWhereClauseBuilder(const QOrmWhereClauseBuilder&) = default;

QOrmWhereClauseBuilder::QOrmWhereClauseBuilder(QOrmWhereClauseBuilder&&) = default;

QOrmWhereClauseBuilder::~QOrmWhereClauseBuilder() = default;

QOrmWhereClauseBuilder& QOrmWhereClauseBuilder::operator=(const QOrmWhereClauseBuilder&) = default;

QOrmWhereClauseBuilder& QOrmWhereClauseBuilder::operator=(QOrmWhereClauseBuilder&&) = default;

QOrmWhereClause QOrmWhereClauseBuilder::build() const
{
    return QOrmWhereClause{d->m_field, d->m_comparison, d->m_value};
}

QOrmWhereClauseBuilder operator==(const QOrmField& field, const QVariant& value)
{
    return QOrmWhereClauseBuilder{field, QOrm::Comparison::Equals, value};
}

QOrmWhereClauseBuilder operator!=(const QOrmField& field, const QVariant& value)
{
    return QOrmWhereClauseBuilder{field, QOrm::Comparison::NotEquals, value};
}

QOrmWhereClauseBuilder operator<(const QOrmField& field, const QVariant& value)
{
    return QOrmWhereClauseBuilder{field, QOrm::Comparison::Less, value};
}

QOrmWhereClauseBuilder operator<=(const QOrmField& field, const QVariant& value)
{
    return QOrmWhereClauseBuilder{field, QOrm::Comparison::LessOrEquals, value};
}

QOrmWhereClauseBuilder operator>(const QOrmField& field, const QVariant& value)
{    
    return QOrmWhereClauseBuilder{field, QOrm::Comparison::Greater, value};
}

QOrmWhereClauseBuilder operator>=(const QOrmField& field, const QVariant& value)
{
    return QOrmWhereClauseBuilder{field, QOrm::Comparison::GreaterOrEquals, value};
}

//QOrmWhereClause operator||(const QOrmWhereClause& lhs, const QOrmWhereClause& rhs)
//{
//    return QOrmWhereClause{};
//}

//QOrmWhereClause operator&&(const QOrmWhereClause& lhs, const QOrmWhereClause& rhs)
//{
//    return QOrmWhereClause{};
//}

QT_END_NAMESPACE
