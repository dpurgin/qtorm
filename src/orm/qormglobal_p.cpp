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

#include "qormglobal_p.h"
#include "qormfilterexpression.h"
#include "qormglobal.h"
#include "qormquery.h"
#include "qormrelation.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

namespace QOrmPrivate
{
    QOrmFilterExpression resolvedFilterExpression(const QOrmRelation& relation,
                                                  const QOrmFilterExpression& expression)
    {
        switch (expression.type())
        {
            case QOrm::FilterExpressionType::TerminalPredicate:
            {
                const QOrmFilterTerminalPredicate* predicate = expression.terminalPredicate();

                if (predicate->isResolved())
                    return *predicate;

                const QOrmPropertyMapping* propertyMapping = nullptr;

                switch (relation.type())
                {
                    case QOrm::RelationType::Mapping:
                        propertyMapping = relation.mapping()->classPropertyMapping(
                            predicate->classProperty()->descriptor());
                        break;

                    case QOrm::RelationType::Query:
                        Q_ASSERT(relation.query()->projection().has_value());

                        propertyMapping = relation.query()->projection()->classPropertyMapping(
                            predicate->classProperty()->descriptor());
                        break;
                }

                if (propertyMapping == nullptr)
                {
                    qCritical() << "QtOrm: Unable to resolve filter expression for class property"
                                << predicate->classProperty()->descriptor() << ", relation"
                                << relation;
                    qFatal("QtOrm: Malformed query filter");
                }

                return QOrmFilterTerminalPredicate{*propertyMapping,
                                                   predicate->comparison(),
                                                   predicate->value()};
            }

            case QOrm::FilterExpressionType::BinaryPredicate:
            {
                const QOrmFilterBinaryPredicate* predicate = expression.binaryPredicate();
                return QOrmFilterBinaryPredicate{resolvedFilterExpression(relation,
                                                                          predicate->lhs()),
                                                 predicate->logicalOperator(),
                                                 resolvedFilterExpression(relation,
                                                                          predicate->rhs())};
            }

            case QOrm::FilterExpressionType::UnaryPredicate:
            {
                const QOrmFilterUnaryPredicate* predicate = expression.unaryPredicate();
                return QOrmFilterUnaryPredicate{predicate->logicalOperator(),
                                                resolvedFilterExpression(relation,
                                                                         predicate->rhs())};
            }
        }

        Q_ORM_UNEXPECTED_STATE;
    }

    QString entityInstanceRepresentation(const QOrmMetadata& entity, const QObject* entityInstance)
    {
        QString repr;
        QDebug dbg{&repr};

        dbg.nospace().noquote() << entity.className() << "{";

        for (const QOrmPropertyMapping& mapping : entity.propertyMappings())
        {
            dbg << mapping.classPropertyName() << " = " << propertyValue(entityInstance, mapping)
                << ", ";
        }

        dbg << "}";

        return repr;
    }

    QString shortPropertyMappingRepresentation(const QOrmPropertyMapping& mapping)
    {
        QString repr;
        QDebug dbg{&repr};

        dbg.nospace().noquote() << mapping.enclosingEntity().className()
                                << "::" << mapping.classPropertyName();

        return repr;
    }

    std::optional<QString> crossReferenceError(const QOrmMetadata& entity,
                                               const QObject* entityInstance)
    {
        for (const QOrmPropertyMapping& mapping : entity.propertyMappings())
        {
            if (!mapping.isReference())
                continue;

            Q_ASSERT(mapping.referencedEntity() != nullptr);

            const QOrmPropertyMapping* backReference = QOrmPrivate::backReference(mapping);
            if (backReference == nullptr)
                continue;

            Q_ASSERT(backReference->referencedEntity()->className() == entity.className());

            // if reference to a single instance, make sure that our instance is listed on the other
            // side
            if (!mapping.isTransient())
            {
                QObject* referencedEntity =
                    propertyValue(entityInstance, mapping).value<QObject*>();

                if (referencedEntity == nullptr)
                    continue;

                // T* <-> QVector<T*>
                // Check that the right side contains a reference to this entity instance
                if (backReference->isTransient())
                {
                    QVector<QObject*> backReferencedInstances =
                        propertyValue(referencedEntity, *backReference).value<QVector<QObject*>>();

                    if (std::find(std::cbegin(backReferencedInstances),
                                  std::cend(backReferencedInstances),
                                  entityInstance) == std::cend(backReferencedInstances))
                    {
                        QString errorText;
                        QDebug dbg{&errorText};
                        dbg.noquote().nospace()
                            << entityInstanceRepresentation(entity, entityInstance)
                            << " references "
                            << entityInstanceRepresentation(*mapping.referencedEntity(),
                                                            referencedEntity)
                            << " but its back-reference "
                            << shortPropertyMappingRepresentation(*backReference)
                            << " does not contain the original instance. ";
                        return std::make_optional(errorText);
                    }
                }
                // T* <-> T*
                // check that both sides are equal
                else
                {
                    QObject* backReferencedInstance =
                        propertyValue(referencedEntity, *backReference).value<QObject*>();

                    if (backReferencedInstance != entityInstance)
                    {
                        QString errorText;
                        QDebug dbg{&errorText};
                        dbg.noquote().nospace()
                            << entityInstanceRepresentation(entity, entityInstance)
                            << " references "
                            << entityInstanceRepresentation(*mapping.referencedEntity(),
                                                            referencedEntity)
                            << " but its back-reference "
                            << shortPropertyMappingRepresentation(*backReference)
                            << " is set to something else.";
                        return std::make_optional(errorText);
                    }
                }
            }
            // If list of referenced instances, check that each one has a back reference to our
            // instance on the other side
            else
            {
                // for now, do not support n:m
                if (backReference->isTransient())
                {
                    qCritical()
                        << "QtOrm: Many-to-many relation is not supported in related entities"
                        << mapping << "<->" << *backReference;
                    Q_ORM_NOT_IMPLEMENTED;
                }

                QVector<QObject*> referencedInstances =
                    propertyValue(entityInstance, mapping).value<QVector<QObject*>>();

                for (const QObject* referencedInstance : referencedInstances)
                {
                    // QVector<T*> <-> T*
                    // check that the entity on the other side references this one
                    QObject* backReferencedEntity =
                        propertyValue(referencedInstance, *backReference).value<QObject*>();

                    if (backReferencedEntity != entityInstance)
                    {
                        QString errorText;
                        QDebug dbg{&errorText};
                        dbg.noquote().nospace()
                            << entityInstanceRepresentation(entity, entityInstance)
                            << " references "
                            << entityInstanceRepresentation(*mapping.referencedEntity(),
                                                            referencedInstance)
                            << " but its back-reference "
                            << shortPropertyMappingRepresentation(*backReference)
                            << " is set to something else.";

                        return std::make_optional(errorText);
                    }
                }
            }
        }

        return std::nullopt;
    }
} // namespace QOrmPrivate

#ifdef QT_NO_DEBUG
Q_LOGGING_CATEGORY(qtorm, "qtorm", QtMsgType::QtInfoMsg)
#else
Q_LOGGING_CATEGORY(qtorm, "qtorm", QtMsgType::QtDebugMsg)
#endif

QT_END_NAMESPACE
