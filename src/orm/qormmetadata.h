#ifndef QORMMETADATA_H
#define QORMMETADATA_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormpropertymapping.h>

#include <QtCore/qstring.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>
#include <QtCore/qvector.h>

#include <optional>

QT_BEGIN_NAMESPACE

class QOrmMetadataPrivate;

class Q_ORM_EXPORT QOrmMetadata
{
public:
    QOrmMetadata();
    QOrmMetadata(const QMetaObject& qMetaObject);
    QOrmMetadata(const QOrmMetadata&);
    QOrmMetadata(QOrmMetadata&&);
    ~QOrmMetadata();

    QOrmMetadata& operator=(const QOrmMetadata&);
    QOrmMetadata& operator=(QOrmMetadata&&);

    Q_REQUIRED_RESULT const QMetaObject& qMetaObject() const;

    Q_REQUIRED_RESULT
    QString className() const;
    void setClassName(const QString& className);

    Q_REQUIRED_RESULT
    QString tableName() const;
    void setTableName(const QString& tableName);

    Q_REQUIRED_RESULT
    const QVector<QOrmPropertyMapping>& propertyMappings() const;

    Q_REQUIRED_RESULT
    std::optional<QOrmPropertyMapping> tableFieldMapping(const QString& fieldName) const;

    Q_REQUIRED_RESULT
    std::optional<QOrmPropertyMapping> classPropertyMapping(const QString& classProperty) const;

    void addPropertyMapping(const QOrmPropertyMapping& propertyMapping);

    Q_REQUIRED_RESULT
    std::optional<QOrmPropertyMapping> objectIdMapping() const;
    void setObjectIdMapping(const QOrmPropertyMapping& objectIdMapping);

private:
    QSharedDataPointer<QOrmMetadataPrivate> d;
};

QT_END_NAMESPACE

#endif // QORMMETADATA_H
