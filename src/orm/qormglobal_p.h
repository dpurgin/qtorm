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

namespace QOrmPrivate
{
    Q_REQUIRED_RESULT
    inline QVariant propertyValue(const QObject* object, QString property)
    {
        return object->property(property.toUtf8().data());
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
                                   return propertyMapping.isReference() &&
                                          !propertyMapping.isTransient() &&
                                          propertyMapping.referencedEntity() != nullptr &&
                                          propertyMapping.referencedEntity()->className() ==
                                              reference.enclosingEntity().className();
                               });

        return it == std::end(referencedPropertyMappings) ? nullptr : &(*it);
    }

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
