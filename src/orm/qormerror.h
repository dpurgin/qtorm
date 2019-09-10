#ifndef QORMERROR_H
#define QORMERROR_H

#include <QtOrm/qormglobal.h>

#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

class QDebug;

class Q_ORM_EXPORT QOrmError
{
public:
    QOrmError(QOrm::Error error, const QString& errorText);

    Q_REQUIRED_RESULT QOrm::Error error() const;
    Q_REQUIRED_RESULT QString errorText() const;

private:
    QOrm::Error m_error{QOrm::Error::None};
    QString m_errorText;
};

extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, const QOrmError& error);

QT_END_NAMESPACE

#endif // QORMERROR_H
