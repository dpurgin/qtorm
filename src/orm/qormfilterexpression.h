/*
 * Copyright (C) 2019-2022 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2019-2022 sequality software engineering e.U. <office@sequality.at>
 *
 * This file is part of QtOrm library.
 *
 * QtOrm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QtOrm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with QtOrm.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef QORMFILTERBUILDER_H
#define QORMFILTERBUILDER_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormclassproperty.h>
#include <QtOrm/qormpropertymapping.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

#include <variant>

QT_BEGIN_NAMESPACE

class QOrmFilterExpressionPrivate;
class QVariant;

class QOrmFilterTerminalPredicate;
class QOrmFilterBinaryPredicate;
class QOrmFilterUnaryPredicate;

class Q_ORM_EXPORT QOrmFilterExpression
{
public:    
    QOrmFilterExpression(const QOrmFilterTerminalPredicate& terminalPredicate);
    QOrmFilterExpression(const QOrmFilterBinaryPredicate& binaryPredicate);
    QOrmFilterExpression(const QOrmFilterUnaryPredicate& unaryPredicate);
    QOrmFilterExpression(const QOrmFilterExpression&);
    QOrmFilterExpression(QOrmFilterExpression&&);
    ~QOrmFilterExpression();

    QOrmFilterExpression& operator=(const QOrmFilterExpression&);
    QOrmFilterExpression& operator=(QOrmFilterExpression&&);

    Q_REQUIRED_RESULT QOrm::FilterExpressionType type() const;

    Q_REQUIRED_RESULT const QOrmFilterTerminalPredicate* terminalPredicate() const;
    Q_REQUIRED_RESULT const QOrmFilterBinaryPredicate* binaryPredicate() const;
    Q_REQUIRED_RESULT const QOrmFilterUnaryPredicate* unaryPredicate() const;

private:
    QSharedDataPointer<QOrmFilterExpressionPrivate> d;
};

class Q_ORM_EXPORT QOrmFilterTerminalPredicate
{
public:
    using FilterProperty = std::variant<QOrmClassProperty, QOrmPropertyMapping>;

    QOrmFilterTerminalPredicate(FilterProperty filterProperty,
                                QOrm::Comparison comparison,
                                QVariant value);

    Q_REQUIRED_RESULT bool isResolved() const;

    Q_REQUIRED_RESULT const QOrmClassProperty* classProperty() const;
    Q_REQUIRED_RESULT const QOrmPropertyMapping* propertyMapping() const;

    Q_REQUIRED_RESULT QOrm::Comparison comparison() const;

    Q_REQUIRED_RESULT QVariant value() const;

private:
    std::variant<QOrmClassProperty, QOrmPropertyMapping> m_filterProperty;
    QOrm::Comparison m_comparison;
    QVariant m_value;
};

class Q_ORM_EXPORT QOrmFilterBinaryPredicate
{
public:
    QOrmFilterBinaryPredicate(QOrmFilterExpression lhs,
                              QOrm::BinaryLogicalOperator logicalOperator,
                              QOrmFilterExpression rhs);

    Q_REQUIRED_RESULT const QOrmFilterExpression& lhs() const;
    Q_REQUIRED_RESULT QOrm::BinaryLogicalOperator logicalOperator() const;
    Q_REQUIRED_RESULT const QOrmFilterExpression& rhs() const;

private:
    QOrmFilterExpression m_lhs;
    QOrm::BinaryLogicalOperator m_logicalOperator;
    QOrmFilterExpression m_rhs;
};

class Q_ORM_EXPORT QOrmFilterUnaryPredicate
{
public:
    QOrmFilterUnaryPredicate(QOrm::UnaryLogicalOperator logicalOperator,
                             QOrmFilterExpression rhs);

    Q_REQUIRED_RESULT QOrm::UnaryLogicalOperator logicalOperator() const;
    Q_REQUIRED_RESULT const QOrmFilterExpression& rhs() const;

private:
    QOrm::UnaryLogicalOperator m_logicalOperator;
    QOrmFilterExpression m_rhs;
};

extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, const QOrmFilterExpression& expression);
extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, const QOrmFilterTerminalPredicate& predicate);
extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, const QOrmFilterBinaryPredicate& predicate);
extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, const QOrmFilterUnaryPredicate& predicate);

namespace QtOrmPrivate
{
    template<typename T>
    struct IsList : std::false_type
    {
    };

    template<typename T>
    struct IsList<QVector<T>> : std::true_type
    {
    };

    template<typename T>
    struct IsList<QList<T>> : std::true_type
    {
    };

    template<typename T>
    struct IsList<QSet<T>> : std::true_type
    {
    };

    template<>
    struct IsList<QStringList> : std::true_type
    {
    };

    template<typename T, bool Convertible = std::is_convertible_v<std::decay_t<T>, QVariant>>
    struct ValueConverter;

    template<typename T>
    struct ValueConverter<T, false>
    {
        static QVariant convert(T&& value) { return QVariant::fromValue(std::forward<T>(value)); }
    };

    template<typename T>
    struct ValueConverter<T, true>
    {
        static QVariant convert(const T& value) { return QVariant{value}; }
    };

    template<typename T, bool ConvertToQVariantList = IsList<std::decay_t<T>>::value>
    struct FilterTerminalPredicateFactory;

    template<typename T>
    struct FilterTerminalPredicateFactory<T, false>
    {
        [[nodiscard]] static QOrmFilterTerminalPredicate create(
            const QOrmFilterTerminalPredicate::FilterProperty& property,
            QOrm::Comparison comparison,
            T&& value)
        {
            return {property, comparison, ValueConverter<T>::convert(std::forward<T>(value))};
        }
    };

    template<typename T>
    struct FilterTerminalPredicateFactory<T, true>
    {
        [[nodiscard]] static QOrmFilterTerminalPredicate create(
            const QOrmFilterTerminalPredicate::FilterProperty& property,
            QOrm::Comparison comparison,
            T&& value)
        {
            QVariantList list;
            using ValueType = typename std::decay_t<T>::value_type;

            std::transform(std::cbegin(std::forward<T>(value)),
                           std::cend(std::forward<T>(value)),
                           std::back_inserter(list),
                           [](const ValueType& value)
                           { return ValueConverter<ValueType>::convert(value); });

            return {property, comparison, list};
        }
    };

    template<typename T, bool IsList = IsList<std::decay_t<T>>::value>
    struct EqualsComparator;

    template<typename T>
    struct EqualsComparator<T, false>
    {
        [[nodiscard]] static QOrm::Comparison comparator() { return QOrm::Comparison::Equal; }
    };

    template<typename T>
    struct EqualsComparator<T, true>
    {
        [[nodiscard]] static QOrm::Comparison comparator() { return QOrm::Comparison::InList; }
    };

    template<typename T, bool IsList = IsList<std::decay_t<T>>::value>
    struct NotEqualsComparator;

    template<typename T>
    struct NotEqualsComparator<T, false>
    {
        [[nodiscard]] static QOrm::Comparison comparator() { return QOrm::Comparison::NotEqual; }
    };

    template<typename T>
    struct NotEqualsComparator<T, true>
    {
        [[nodiscard]] static QOrm::Comparison comparator() { return QOrm::Comparison::NotInList; }
    };
} // namespace QtOrmPrivate

template<typename T>
[[nodiscard]] inline QOrmFilterTerminalPredicate
operator==(const QOrmFilterTerminalPredicate::FilterProperty& property, T&& value)
{
    return QtOrmPrivate::FilterTerminalPredicateFactory<T>::create(
        property, QtOrmPrivate::EqualsComparator<T>::comparator(), std::forward<T>(value));
}

template<typename T>
[[nodiscard]] inline QOrmFilterTerminalPredicate
operator!=(const QOrmFilterTerminalPredicate::FilterProperty& property, T&& value)
{
    return QtOrmPrivate::FilterTerminalPredicateFactory<T>::create(
        property, QtOrmPrivate::NotEqualsComparator<T>::comparator(), std::forward<T>(value));
}

template<typename T>
[[nodiscard]] inline QOrmFilterTerminalPredicate
operator<(const QOrmFilterTerminalPredicate::FilterProperty& property, T&& value)
{
    return QtOrmPrivate::FilterTerminalPredicateFactory<T>::create(property,
                                                                   QOrm::Comparison::Less,
                                                                   std::forward<T>(value));
}

template<typename T>
[[nodiscard]] inline QOrmFilterTerminalPredicate
operator<=(const QOrmFilterTerminalPredicate::FilterProperty& property, T&& value)
{
    return QtOrmPrivate::FilterTerminalPredicateFactory<T>::create(property,
                                                                   QOrm::Comparison::LessOrEqual,
                                                                   std::forward<T>(value));
}

template<typename T>
[[nodiscard]] inline QOrmFilterTerminalPredicate
operator>(const QOrmFilterTerminalPredicate::FilterProperty& property, T&& value)
{
    return QtOrmPrivate::FilterTerminalPredicateFactory<T>::create(property,
                                                                   QOrm::Comparison::Greater,
                                                                   std::forward<T>(value));
}

template<typename T>
[[nodiscard]] inline QOrmFilterTerminalPredicate
operator>=(const QOrmFilterTerminalPredicate::FilterProperty& property, T&& value)
{
    return QtOrmPrivate::FilterTerminalPredicateFactory<T>::create(property,
                                                                   QOrm::Comparison::GreaterOrEqual,
                                                                   std::forward<T>(value));
}

Q_REQUIRED_RESULT
Q_ORM_EXPORT
QOrmFilterUnaryPredicate operator!(const QOrmFilterExpression& operand);

Q_REQUIRED_RESULT
Q_ORM_EXPORT
QOrmFilterBinaryPredicate operator||(const QOrmFilterExpression& lhs,
                                     const QOrmFilterExpression& rhs);

Q_REQUIRED_RESULT
Q_ORM_EXPORT
QOrmFilterBinaryPredicate operator&&(const QOrmFilterExpression& lhs,
                                     const QOrmFilterExpression& rhs);

QT_END_NAMESPACE

#endif // QORMFILTERBUILDER_H
