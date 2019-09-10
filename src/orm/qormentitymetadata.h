#ifndef QORMENTITYMETADATA_H
#define QORMENTITYMETADATA_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormpropertymapping.h>

#include <QString>
#include <QVariant>
#include <QVector>

QT_BEGIN_NAMESPACE


class Q_ORM_EXPORT QOrmEntityMetadata
{
public:
    Q_REQUIRED_RESULT
    QString className() const;
    void setClassName(const QString& className);

    Q_REQUIRED_RESULT
    QString tableName() const;
    void setTableName(const QString& tableName);

    Q_REQUIRED_RESULT
    const QVector<QOrmPropertyMapping>& propertyMappings() const;

    void addPropertyMapping(const QOrmPropertyMapping& propertyMapping);
    void addPropertyMapping(QOrmPropertyMapping&& propertyMapping);

    Q_REQUIRED_RESULT
    const QOrmPropertyMapping& objectIdPropertyMapping() const;
    void setObjectIdPropertyMapping(const QOrmPropertyMapping& objectIdPropertyMapping);

private:
    QString m_className;
    QString m_tableName;
    QVector<QOrmPropertyMapping> m_propertyMappings;
    QOrmPropertyMapping m_objectIdPropertyMapping;
};

QT_END_NAMESPACE

#endif // QORMENTITYMETADATA_H
