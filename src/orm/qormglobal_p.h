/*
 * Copyright (C) 2019 Dmitriy Purgin <dmitriy.purgin@sequality.at>
 * Copyright (C) 2019 sequality software engineering e.U. <office@sequality.at>
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

#ifndef QORMGLOBAL_P_H
#define QORMGLOBAL_P_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormmetadata.h>

#include <QtCore/qhashfunctions.h>
#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>

#include <variant>

QT_BEGIN_NAMESPACE

class QOrmFilterExpression;
class QOrmRelation;

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
namespace std
{
    template<>
    struct hash<QString>
    {
        size_t operator()(const QString& s) const { return qHash(s); }
    };

    template<>
    struct hash<QByteArray>
    {
        size_t operator()(const QByteArray& ba) const { return qHash(ba); }
    };
} // namespace std
#endif

namespace QOrmPrivate
{
    Q_REQUIRED_RESULT
    inline QVariant propertyValue(const QObject* object, QString property)
    {
        return object->property(property.toUtf8().data());
    }

    Q_REQUIRED_RESULT
    inline QVariant propertyValue(const QObject* object, const QOrmPropertyMapping& mapping)
    {
        return propertyValue(object, mapping.classPropertyName());
    }

    Q_REQUIRED_RESULT
    inline bool setPropertyValue(QObject* object, const QString& property, const QVariant& value)
    {
        return object->setProperty(property.toUtf8().data(), value);
    }

    Q_REQUIRED_RESULT
    inline QVariant objectIdPropertyValue(const QObject* entityInstance, const QOrmMetadata& meta)
    {
        Q_ASSERT(meta.objectIdMapping() != nullptr);
        return propertyValue(entityInstance, meta.objectIdMapping()->classPropertyName());
    }

    Q_REQUIRED_RESULT
    Q_ORM_EXPORT
    extern QOrmFilterExpression resolvedFilterExpression(const QOrmRelation& relation,
                                                         const QOrmFilterExpression& expression);

    Q_REQUIRED_RESULT
    inline const QOrmPropertyMapping* backReference(const QOrmPropertyMapping& reference)
    {
        Q_ASSERT(reference.referencedEntity() != nullptr);

        const auto& referencedPropertyMappings = reference.referencedEntity()->propertyMappings();

        auto it = std::find_if(std::begin(referencedPropertyMappings),
                               std::end(referencedPropertyMappings),
                               [&reference](const QOrmPropertyMapping& propertyMapping) {
                                   Q_ASSERT(!propertyMapping.isReference() ||
                                            propertyMapping.referencedEntity() != nullptr);
                                   return propertyMapping.isReference() &&
                                          propertyMapping.referencedEntity()->className() ==
                                              reference.enclosingEntity().className();
                               });

        return it == std::end(referencedPropertyMappings) ? nullptr : &(*it);
    }

    Q_REQUIRED_RESULT
    Q_ORM_EXPORT
    extern QString entityInstanceRepresentation(const QOrmMetadata& entity,
                                                const QObject* entityInstance);

    Q_REQUIRED_RESULT
    Q_ORM_EXPORT
    extern QString shortPropertyMappingRepresentation(const QOrmPropertyMapping& mapping);

    Q_REQUIRED_RESULT
    Q_ORM_EXPORT
    extern std::optional<QString> crossReferenceError(const QOrmMetadata& entity,
                                                      const QObject* entityInstance);

    template<typename E>
    class Unexpected
    {
    public:
        explicit Unexpected(E val)
            : m_value{std::move(val)}
        {
        }

        const E& value() const { return m_value; }

    private:
        E m_value;
    };

    template<typename E>
    Unexpected<std::decay_t<E>> makeUnexpected(E&& val)
    {
        return Unexpected<std::decay_t<E>>(std::forward<E>(val));
    }

    template<typename T, typename E>
    struct Expected
    {
    public:
        Expected(T&& expected)
            : m_value{expected}
        {
        }

        Expected(Unexpected<E>&& unexpected)
            : m_value{unexpected}
        {
        }

        constexpr bool hasValue() const { return std::holds_alternative<T>(m_value); }
        constexpr const T& value() const { return *std::get_if<T>(&m_value); }
        constexpr const E& error() const { return std::get_if<Unexpected<E>>(&m_value)->value(); }

        constexpr explicit operator bool() const { return hasValue(); }

    private:
        std::variant<T, Unexpected<E>> m_value;
    };

} // namespace QOrmPrivate

#define Q_ORM_UNEXPECTED_STATE (qFatal("QtOrm: %s: unexpected state", __PRETTY_FUNCTION__))
#define Q_ORM_NOT_IMPLEMENTED (qFatal("QtOrm: %s: not implemented", __PRETTY_FUNCTION__))

QT_END_NAMESPACE

#endif
