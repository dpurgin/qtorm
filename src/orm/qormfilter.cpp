#include "qormfilter.h"
#include "qormclassproperty.h"

#include <QVariant>

class QOrmFilterPrivate : public QSharedData
{
    friend class QOrmFilter;

    QOrmFilterPrivate(const QOrmClassProperty& property,
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

QOrmFilter::QOrmFilter(const QOrmClassProperty& field,
                                 QOrm::Comparison comparison,
                                 const QVariant& value)
    : d{new QOrmFilterPrivate{field, comparison, value}}
{
}

QOrmFilter::QOrmFilter(const QOrmFilter&) = default;

QOrmFilter::QOrmFilter(QOrmFilter&&) = default;

QOrmFilter::~QOrmFilter() = default;

QOrmFilter& QOrmFilter::operator=(const QOrmFilter&) = default;

QOrmFilter& QOrmFilter::operator=(QOrmFilter&&) = default;

QOrmClassProperty QOrmFilter::property() const
{
    return d->m_property;
}

QOrm::Comparison QOrmFilter::comparison() const
{
    return d->m_comparison;
}

QVariant QOrmFilter::value() const
{
    return d->m_value;
}
