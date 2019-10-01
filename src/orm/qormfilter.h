#ifndef QORMFILTER_H
#define QORMFILTER_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormfilterexpression.h>

#include <variant>

QT_BEGIN_NAMESPACE

class Q_ORM_EXPORT QOrmFilter
{
public:
    explicit QOrmFilter(QOrmFilterExpression expression)
        : m_type{QOrm::FilterType::Expression}
        , m_filter{expression}
    {
    }

    Q_REQUIRED_RESULT
    QOrm::FilterType type() const
    {
        return m_type;
    }

    Q_REQUIRED_RESULT
    const QOrmFilterExpression* expression() const
    {
        return std::get_if<QOrmFilterExpression>(&m_filter);
    }

private:
    QOrm::FilterType m_type;
    std::variant<QOrmFilterExpression, void*> m_filter{nullptr};
};

extern Q_ORM_EXPORT QDebug operator<<(QDebug debug, const QOrmFilter& filter);

QT_END_NAMESPACE

#endif
