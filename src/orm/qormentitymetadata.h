#ifndef QORMENTITYMETADATA_H
#define QORMENTITYMETADATA_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormpropertymapping.h>

#include <QtCore/qstring.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>
#include <QtCore/qvector.h>

#include <optional>

QT_BEGIN_NAMESPACE

class QOrmEntityMetadataPrivate;

class Q_ORM_EXPORT QOrmEntityMetadata
{
public:
    QOrmEntityMetadata();
    QOrmEntityMetadata(const QMetaObject& qMetaObject);
    QOrmEntityMetadata(const QOrmEntityMetadata&);
    ~QOrmEntityMetadata();

    QOrmEntityMetadata& operator=(const QOrmEntityMetadata&);

#ifdef Q_COMPILER_RVALUE_REFS
    QOrmEntityMetadata(QOrmEntityMetadata&&);

    QOrmEntityMetadata& operator=(QOrmEntityMetadata&&);
#endif

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
    QSharedDataPointer<QOrmEntityMetadataPrivate> d;
};

QT_END_NAMESPACE

#endif // QORMENTITYMETADATA_H
