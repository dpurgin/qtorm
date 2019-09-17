#include "qormwhereclause.h"
#include "qormwhereclausebuilder.h"
#include "qormclassproperty.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

class QOrmWhereClauseBuilderPrivate : public QSharedData
{
    friend class QOrmWhereClauseBuilder;

    QOrmWhereClauseBuilderPrivate(const QOrmClassProperty& property,
                                  QOrm::Comparison comparison,
                                  const QVariant& value)
        : m_classProperty{property},
          m_comparison{comparison},
          m_value{value}
    {
    }

    QOrmClassProperty m_classProperty;
    QOrm::Comparison m_comparison{QOrm::Comparison::Invalid};
    QVariant m_value;        
};

QOrmWhereClauseBuilder::QOrmWhereClauseBuilder(const QOrmClassProperty& field,
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
    return QOrmWhereClause{d->m_classProperty, d->m_comparison, d->m_value};
}

QOrmWhereClauseBuilder operator==(const QOrmClassProperty& property, const QVariant& value)
{
    return QOrmWhereClauseBuilder{property, QOrm::Comparison::Equal, value};
}

QOrmWhereClauseBuilder operator!=(const QOrmClassProperty& property, const QVariant& value)
{
    return QOrmWhereClauseBuilder{property, QOrm::Comparison::NotEqual, value};
}

QOrmWhereClauseBuilder operator<(const QOrmClassProperty& property, const QVariant& value)
{
    return QOrmWhereClauseBuilder{property, QOrm::Comparison::Less, value};
}

QOrmWhereClauseBuilder operator<=(const QOrmClassProperty& property, const QVariant& value)
{
    return QOrmWhereClauseBuilder{property, QOrm::Comparison::LessOrEqual, value};
}

QOrmWhereClauseBuilder operator>(const QOrmClassProperty& property, const QVariant& value)
{    
    return QOrmWhereClauseBuilder{property, QOrm::Comparison::Greater, value};
}

QOrmWhereClauseBuilder operator>=(const QOrmClassProperty& property, const QVariant& value)
{
    return QOrmWhereClauseBuilder{property, QOrm::Comparison::GreaterOrEqual, value};
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
