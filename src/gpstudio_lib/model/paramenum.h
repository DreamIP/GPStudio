#ifndef PARAMENUM_H
#define PARAMENUM_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QVariant>
#include <QDomElement>

class GPSTUDIO_LIB_EXPORT ParamEnum
{
public:
    ParamEnum();
    ~ParamEnum();

    QString name() const;
    void setName(const QString &name);

    QVariant value() const;
    void setValue(const QVariant &value);

    QString description() const;
    void setDescription(const QString &description);

public:
    static ParamEnum *fromNodeGenerated(const QDomElement &domElement);

protected:
    QString _name;
    QVariant _value;
    QString _description;
};

#endif // PARAMENUM_H
