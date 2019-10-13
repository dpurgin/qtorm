#ifndef QORMSQLITEPROVIDER_H
#define QORMSQLITEPROVIDER_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormabstractprovider.h>

QT_BEGIN_NAMESPACE

class QOrmEntityInstanceCache;
class QOrmQueryResult;
class QOrmSqliteConfiguration;
class QOrmSqliteProviderPrivate;
class QSqlDatabase;

class Q_ORM_EXPORT QOrmSqliteProvider : public QOrmAbstractProvider
{
public:
    explicit QOrmSqliteProvider(const QOrmSqliteConfiguration& sqlConfiguration);
    ~QOrmSqliteProvider() override;

    QOrmError connectToBackend() override;
    QOrmError disconnectFromBackend() override;
    bool isConnectedToBackend() override;

    QOrmError beginTransaction() override;
    QOrmError commitTransaction() override;
    QOrmError rollbackTransaction() override;

    QOrmQueryResult execute(const QOrmQuery& query,
                            QOrmEntityInstanceCache& entityInstanceCache) override;

    QOrmSqliteConfiguration configuration() const;
    QSqlDatabase database() const;

private:
    Q_DECLARE_PRIVATE(QOrmSqliteProvider)
    QOrmSqliteProviderPrivate* d_ptr{nullptr};
};

QT_END_NAMESPACE

#endif // QORMSQLITEPROVIDER_H
