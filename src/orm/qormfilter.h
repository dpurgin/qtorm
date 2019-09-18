#ifndef QORMFILTER_H
#define QORMFILTER_H

#include <QtOrm/qormglobal.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOrmClassProperty;
class QOrmFilterPrivate;
class QVariant;

class Q_ORM_EXPORT QOrmFilter
{
public:
    explicit QOrmFilter(const QOrmClassProperty& property,
                             QOrm::Comparison comparison,
                             const QVariant& value);
    QOrmFilter(const QOrmFilter&);
    QOrmFilter(QOrmFilter&&);
    ~QOrmFilter();

    QOrmFilter& operator=(const QOrmFilter&);
    QOrmFilter& operator=(QOrmFilter&&);

    Q_REQUIRED_RESULT
    QOrmClassProperty property() const;
    Q_REQUIRED_RESULT
    QOrm::Comparison comparison() const;
    Q_REQUIRED_RESULT
    QVariant value() const;

private:
    QSharedDataPointer<QOrmFilterPrivate> d;
};

QT_END_NAMESPACE

#endif
