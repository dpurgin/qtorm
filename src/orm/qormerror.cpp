#include "qormerror.h"

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

QOrmError::QOrmError(QOrm::ErrorType error, const QString& text)
    : m_type{error}
    , m_text{text}
{
}

QOrm::ErrorType QOrmError::type() const
{
    return m_type;
}

QString QOrmError::text() const
{
    return m_text;
}

QDebug operator<<(QDebug dbg, const QOrmError& error)
{
    QDebugStateSaver saver{dbg};

    dbg.nospace().noquote() << "QOrmError(" << error.type() << QStringLiteral(", ") << error.text()
                            << QStringLiteral(")");

    return dbg;
}

QT_END_NAMESPACE
