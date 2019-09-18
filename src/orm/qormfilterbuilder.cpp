#include "qormfilter.h"
#include "qormfilterbuilder.h"
#include "qormclassproperty.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

class QOrmFilterBuilderPrivate : public QSharedData
{
    friend class QOrmFilterBuilder;

    QOrmFilterBuilderPrivate(const QOrmClassProperty& property,
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

QOrmFilterBuilder::QOrmFilterBuilder(const QOrmClassProperty& field,
                                 QOrm::Comparison comparison,
                                 const QVariant& value)
    : d{new QOrmFilterBuilderPrivate{field, comparison, value}}
{
}

QOrmFilterBuilder::QOrmFilterBuilder(const QOrmFilterBuilder&) = default;

QOrmFilterBuilder::QOrmFilterBuilder(QOrmFilterBuilder&&) = default;

QOrmFilterBuilder::~QOrmFilterBuilder() = default;

QOrmFilterBuilder& QOrmFilterBuilder::operator=(const QOrmFilterBuilder&) = default;

QOrmFilterBuilder& QOrmFilterBuilder::operator=(QOrmFilterBuilder&&) = default;

QOrmFilter QOrmFilterBuilder::build() const
{
    return QOrmFilter{d->m_classProperty, d->m_comparison, d->m_value};
}

QOrmFilterBuilder operator==(const QOrmClassProperty& property, const QVariant& value)
{
    return QOrmFilterBuilder{property, QOrm::Comparison::Equal, value};
}

QOrmFilterBuilder operator!=(const QOrmClassProperty& property, const QVariant& value)
{
    return QOrmFilterBuilder{property, QOrm::Comparison::NotEqual, value};
}

QOrmFilterBuilder operator<(const QOrmClassProperty& property, const QVariant& value)
{
    return QOrmFilterBuilder{property, QOrm::Comparison::Less, value};
}

QOrmFilterBuilder operator<=(const QOrmClassProperty& property, const QVariant& value)
{
    return QOrmFilterBuilder{property, QOrm::Comparison::LessOrEqual, value};
}

QOrmFilterBuilder operator>(const QOrmClassProperty& property, const QVariant& value)
{    
    return QOrmFilterBuilder{property, QOrm::Comparison::Greater, value};
}

QOrmFilterBuilder operator>=(const QOrmClassProperty& property, const QVariant& value)
{
    return QOrmFilterBuilder{property, QOrm::Comparison::GreaterOrEqual, value};
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
