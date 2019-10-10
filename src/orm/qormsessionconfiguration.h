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
    QOrmSessionConfiguration(QOrmAbstractProvider* provider, bool isVerbose);
    QOrmSessionConfiguration(const QOrmSessionConfiguration&);
    QOrmSessionConfiguration(QOrmSessionConfiguration&&);
    ~QOrmSessionConfiguration();

    QOrmSessionConfiguration& operator=(const QOrmSessionConfiguration&);
    QOrmSessionConfiguration& operator=(QOrmSessionConfiguration&&);

    Q_REQUIRED_RESULT
    QOrmAbstractProvider* provider() const;

    Q_REQUIRED_RESULT
    bool isVerbose() const;

private:
    QSharedDataPointer<QOrmSessionConfigurationData> d;
};

QT_END_NAMESPACE

#endif // QORMSESSIONCONFIGURATION_H
