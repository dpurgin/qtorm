#ifndef QORMMETADATA_H
#define QORMMETADATA_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormpropertymapping.h>

#include <QtCore/qstring.h>
#include <QtCore/qshareddata.h>

#include <vector>

QT_BEGIN_NAMESPACE

class QDebug;
class QOrmMetadataPrivate;

class Q_ORM_EXPORT QOrmMetadata
{
public:
    explicit QOrmMetadata(const QMetaObject& qMetaObject);

    QOrmMetadata(const QOrmMetadata&);
    QOrmMetadata(QOrmMetadata&&);
    ~QOrmMetadata();

    QOrmMetadata& operator=(const QOrmMetadata&);
    QOrmMetadata& operator=(QOrmMetadata&&);

    Q_REQUIRED_RESULT const QMetaObject& qMetaObject() const;

    Q_REQUIRED_RESULT QString className() const;
    Q_REQUIRED_RESULT QString tableName() const;

    Q_REQUIRED_RESULT
    const std::vector<QOrmPropertyMapping>& propertyMappings() const;
    Q_REQUIRED_RESULT
    const QOrmPropertyMapping* tableFieldMapping(const QString& fieldName) const;
    Q_REQUIRED_RESULT
    const QOrmPropertyMapping* classPropertyMapping(const QString& classProperty) const;
    Q_REQUIRED_RESULT
    const QOrmPropertyMapping* objectIdMapping() const;

private:
    QSharedDataPointer<QOrmMetadataPrivate> d;
};

extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, const QOrmMetadata& metadata);

QT_END_NAMESPACE

#endif // QORMMETADATA_H
