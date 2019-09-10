#ifndef QORMSQLPROVIDER_H
#define QORMSQLPROVIDER_H

#include <QtOrm/qormglobal.h>
#include <QtOrm/qormabstractprovider.h>

QT_BEGIN_NAMESPACE

class QOrmSqlConfiguration;
class QOrmSqlProviderPrivate;
class QSqlDatabase;

class QOrmSqlProvider : public QOrmAbstractProvider
{
public:
    explicit QOrmSqlProvider(const QOrmSqlConfiguration& sqlConfiguration);
    ~QOrmSqlProvider() override;

    QOrmError connectToBackend() override;
    QOrmError disconnectFromBackend() override;
    bool isConnectedToBackend() override;

    QOrmError beginTransaction() override;
    QOrmError commitTransaction() override;
    QOrmError rollbackTransaction() override;

    QOrmError create(QObject* entityInstance, const QMetaObject& qMetaObject) override;
    QOrmError read(const QOrmQuery& query) override;
    QOrmError update(QObject* entityInstance, const QMetaObject& qMetaObject) override;
    QOrmError remove(QObject* entityInstance, const QMetaObject& qMetaObject) override;

private:
    Q_DECLARE_PRIVATE(QOrmSqlProvider)
    QOrmSqlProviderPrivate* d_ptr{nullptr};
};

QT_END_NAMESPACE

#endif // QORMSQLPROVIDER_H
