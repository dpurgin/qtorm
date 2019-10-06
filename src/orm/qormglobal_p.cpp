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
} // namespace QOrmPrivate

QT_END_NAMESPACE
