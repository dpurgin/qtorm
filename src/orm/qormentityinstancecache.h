#ifndef QORMENTITYINSTANCECACHE_H
#define QORMENTITYINSTANCECACHE_H

#include <QtCore/qglobal.h>
#include <QtCore/qscopedpointer.h>
#include <QtOrm/qormglobal.h>

QT_BEGIN_NAMESPACE

class QOrmEntityInstanceCachePrivate;
class QOrmMetadata;

class Q_ORM_EXPORT QOrmEntityInstanceCache
{
    Q_DISABLE_COPY(QOrmEntityInstanceCache)

public:
    QOrmEntityInstanceCache();
    ~QOrmEntityInstanceCache();

    bool contains(QObject* instance) const;
    void insert(const QOrmMetadata& meta, QObject* instance);
    QObject* take(QObject* instance);

    bool isModified(QObject* instance) const;
    void markUnmodified(QObject* instance) const;

private:
    QScopedPointer<QOrmEntityInstanceCachePrivate> d;
};

QT_END_NAMESPACE

#endif
