#ifndef QORMSESSIONCONFIGURATION_H
#define QORMSESSIONCONFIGURATION_H

#include <QtOrm/qormglobal.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qstring.h>
#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOrmAbstractProvider;
class QOrmSessionConfigurationData;

class Q_ORM_EXPORT QOrmSessionConfiguration
{
public:
    static QOrmSessionConfiguration defaultConfiguration();

public:
    QOrmSessionConfiguration();
    QOrmSessionConfiguration(const QOrmSessionConfiguration&);
    QOrmSessionConfiguration(QOrmSessionConfiguration&&);
    ~QOrmSessionConfiguration();

    QOrmSessionConfiguration& operator=(const QOrmSessionConfiguration&);
    QOrmSessionConfiguration& operator=(QOrmSessionConfiguration&&);

    Q_REQUIRED_RESULT
    QOrmAbstractProvider* provider() const;

    Q_REQUIRED_RESULT
    bool isVerbose() const;

    Q_REQUIRED_RESULT
    bool isValid() const;

protected:
    void setProvider(QOrmAbstractProvider* provider);
    void setVerbose(bool isVerbose);

private:
    QSharedDataPointer<QOrmSessionConfigurationData> d;
};

QT_END_NAMESPACE

#endif // QORMSESSIONCONFIGURATION_H
