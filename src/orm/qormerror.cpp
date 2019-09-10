#include "qormerror.h"

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

QOrmError::QOrmError(QOrm::Error error, const QString& errorText)
    : m_error{error},
      m_errorText{errorText}
{
}

QOrm::Error QOrmError::error() const
{
    return m_error;
}

QString QOrmError::errorText() const
{
    return m_errorText;
}

QDebug operator<<(QDebug dbg, const QOrmError& error)
{
    QDebugStateSaver saver{dbg};

    dbg.nospace().noquote() << "QOrmError("
                            << error.error()
                            << QStringLiteral(", ")
                            << error.errorText() << QStringLiteral(")");

    return dbg;
}

QT_END_NAMESPACE
