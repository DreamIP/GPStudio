#ifndef CAMERAREGISTER_H
#define CAMERAREGISTER_H

#include "gpstudio_lib_common.h"

#include <QObject>
#include <QString>
#include <QList>

#include "registerbitfield.h"

class ModelParam;
class Camera;

class GPSTUDIO_LIB_EXPORT Register : public QObject
{
    Q_OBJECT
public:
    Register(const QString &name=QString(), const uint &addr=0);
    ~Register();

    QString name() const;
    void setName(const QString &name);

    uint addr() const;
    void setAddr(const uint &addr);

    uint value() const;

    QString blockName() const;
    void setBlockName(const QString &blockName);

    QString propertyMap() const;
    void setPropertyMap(const QString &propertyMap);
    QStringList dependsProperties() const;

    Camera *camera() const;
    void setCamera(Camera *camera);

    void addBitField(RegisterBitField *bitField);
    const QList<RegisterBitField *> &bitFields() const;

    const ModelParam *modelParam() const;

public:
    static Register *fromParam(const ModelParam *modelParam);

signals:
    void registerChange(uint addr, uint value);

public slots:
    void setValue(const uint &value);
    void setValue(const QVariant &value);
    void eval();

    void setField(uint value, uint mask);

private:
    QString _name;
    uint _addr;
    uint _value;
    QString _blockName;
    QString _propertyMap;

    Camera *_camera;
    QList<RegisterBitField *> _bitFields;

    const ModelParam *_modelParam;
};

#endif // CAMERAREGISTER_H
