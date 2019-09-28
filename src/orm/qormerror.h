#ifndef QORMERROR_H
#define QORMERROR_H

#include <QtOrm/qormglobal.h>

#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

class QDebug;

class Q_ORM_EXPORT QOrmError
{
public:
    QOrmError(QOrm::ErrorType type, const QString& text);

    Q_REQUIRED_RESULT QOrm::ErrorType type() const;
    Q_REQUIRED_RESULT QString text() const;

private:
    QOrm::ErrorType m_type{QOrm::ErrorType::None};
    QString m_text;
};

extern Q_ORM_EXPORT QDebug operator<<(QDebug dbg, const QOrmError& error);

QT_END_NAMESPACE

#endif // QORMERROR_H
