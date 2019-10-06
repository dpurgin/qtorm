#ifndef QORMMETADATA_P_H
#define QORMMETADATA_P_H

#include "QtOrm/qormpropertymapping.h"

#include <QtCore/qshareddata.h>

#include <vector>

class QOrmMetadataPrivate : public QSharedData
{
public:
    QOrmMetadataPrivate(const QMetaObject& metaObject)
        : m_qMetaObject{metaObject}
    {
    }

    const QMetaObject& m_qMetaObject;

    QString m_className;
    QString m_tableName;
    std::vector<QOrmPropertyMapping> m_propertyMappings;

    int m_objectIdPropertyMappingIdx{-1};
    QHash<QString, int> m_classPropertyMappingIndex;
    QHash<QString, int> m_tableFieldMappingIndex;
};

#endif
