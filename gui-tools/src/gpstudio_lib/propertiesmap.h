#ifndef PROPERTIESMAP_H
#define PROPERTIESMAP_H

#include "gpstudio_lib_common.h"

#include <QMap>

class Property;

class GPSTUDIO_LIB_EXPORT PropertiesMap
{
public:
    PropertiesMap();
    ~PropertiesMap();

    void addProperty(Property *property);
    const QList<Property *> &properties() const;
    const QMap<QString, Property *> &propertiesMap() const;

private:
    QMap<QString, Property *> _propertiesMap;
    QList<Property *> _properties;
};

#endif // PROPERTIESMAP_H
