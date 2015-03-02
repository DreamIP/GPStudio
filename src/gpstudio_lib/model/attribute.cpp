#include "attribute.h"

Attribute::Attribute()
{
}

QString Attribute::name() const
{
    return _name;
}

void Attribute::setName(const QString &name)
{
    _name = name;
}

QString Attribute::value() const
{
    return _value;
}

void Attribute::setValue(const QString &value)
{
    _value = value;
}

QString Attribute::type() const
{
    return _type;
}

void Attribute::setType(const QString &type)
{
    _type = type;
}

Attribute *Attribute::fromNodeGenerated(const QDomElement &domElement)
{
    Attribute *attribute=new Attribute();

    attribute->setName(domElement.attribute("name","no_name"));
    attribute->setValue(domElement.attribute("value",""));
    attribute->setType(domElement.attribute("type",""));

    return attribute;
}
