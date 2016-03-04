#include "registermanager.h"

#include "register.h"
#include "property.h"
#include "cameracom.h"
#include "camera.h"

#include <QObject>
#include <QDebug>

RegisterManager::RegisterManager(Camera *camera)
    : _camera(camera)
{
}

RegisterManager::~RegisterManager()
{
    QMapIterator<uint, Register *> it(_registersMap);
    while (it.hasNext())
    {
        it.next();
        delete it.value();
    }
}

Register *RegisterManager::operator[](const uint addr)
{
    if(!_registersMap.contains(addr)) return NULL;
    return _registersMap[addr];
}

void RegisterManager::addRegister(Register *cameraRegister)
{
    if(!cameraRegister) return;
    cameraRegister->setCamera(_camera);
    _registersMap.insert(cameraRegister->addr(), cameraRegister);
}

const QMap<uint, Register *> &RegisterManager::registersMap() const
{
    return _registersMap;
}

QByteArray RegisterManager::registerData() const
{
    return _registerData;
}

void RegisterManager::evalAll()
{
    QMapIterator<uint, Register *> it(_registersMap);
    while (it.hasNext())
    {
        it.next();
        Register *cameraRegister = it.value();

        if(cameraRegister->bitFields().empty())
        {
            cameraRegister->eval();
        }
        else
        {
            foreach (RegisterBitField *bitField, cameraRegister->bitFields())
            {
                bitField->eval();
            }
        }
    }
}

void RegisterManager::start()
{
    if(!_camera) return;

    uint maxAddr=0;
    QMapIterator<uint, Register *> it(_registersMap);
    while (it.hasNext())
    {
        it.next();
        Register *cameraRegister = it.value();

        if(cameraRegister->addr()>maxAddr) maxAddr=cameraRegister->addr();

        //if(cameraRegister->bitFields().count())
        {
            const QStringList &deps = cameraRegister->dependsProperties();
            foreach (QString propName, deps)
            {
                const Property *prop = _camera->rootProperty()->path(propName);
                if(prop) QObject::connect(prop, SIGNAL(bitsChanged(uint)), cameraRegister, SLOT(eval()));
            }
        }
        //else
        {
            foreach (RegisterBitField *bitField, cameraRegister->bitFields())
            {
                const QStringList &deps = bitField->dependsProperties();
                foreach (QString propName, deps)
                {
                    const Property *prop = _camera->rootProperty()->path(propName);
                    if(prop) QObject::connect(prop, SIGNAL(bitsChanged(uint)), bitField, SLOT(eval()));
                }
            }
        }

        _registerData.fill(0,(maxAddr+1)*4);
        QObject::connect(cameraRegister, SIGNAL(registerChange(uint,uint)), _camera, SLOT(setRegister(uint,uint)));
    }
}

void RegisterManager::setRegister(uint addr, uint value)
{
    if(!_camera) return;
    if(addr>=_registerData.size()) return;

    _registerData.data()[addr*4+0]=value>>24;
    _registerData.data()[addr*4+1]=value>>16;
    _registerData.data()[addr*4+2]=value>>8;
    _registerData.data()[addr*4+3]=value;

    // TODO [LOG] log this
    qDebug()<<"setReg"<<addr<<value;

    if(!_camera->com()) return;
    if(!_camera->com()->isConnected()) return;

    // TODO process _regToSend
    _camera->com()->writeParam(addr, value);
}
