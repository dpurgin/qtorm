#ifndef QORMSESSIONCONFIGURATION_H
#define QORMSESSIONCONFIGURATION_H

#include <QtOrm/qormglobal.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qstring.h>
#include <QtCore/qshareddata.h>

#include <memory>

QT_BEGIN_NAMESPACE

class QOrmAbstractProvider;
class QOrmSessionConfigurationData;

class Q_ORM_EXPORT QOrmSessionConfiguration
{
public:
    static QOrmSessionConfiguration defaultConfiguration();

public:
    QOrmSessionConfiguration();
    QOrmSessionConfiguration(const QOrmSessionConfiguration&) = delete;
    QOrmSessionConfiguration(QOrmSessionConfiguration&&);
    ~QOrmSessionConfiguration();

    QOrmSessionConfiguration& operator=(const QOrmSessionConfiguration&) = delete;
    QOrmSessionConfiguration& operator=(QOrmSessionConfiguration&&);

    Q_REQUIRED_RESULT
    QOrmAbstractProvider* provider() const;
    void setProvider(QOrmAbstractProvider* provider);

    Q_REQUIRED_RESULT
    bool isVerbose() const;
    void setVerbose(bool isVerbose);

    Q_REQUIRED_RESULT
    bool isValid() const;

private:
    std::unique_ptr<QOrmSessionConfigurationData> d;
    //    QSharedDataPointer<QOrmSessionConfigurationData> d;
};

QT_END_NAMESPACE

#endif // QORMSESSIONCONFIGURATION_H
