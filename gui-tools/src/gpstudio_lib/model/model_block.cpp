/****************************************************************************
** Copyright (C) 2014-2017 Dream IP
** 
** This file is part of GPStudio.
**
** GPStudio is a free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "model_block.h"

#include <QDebug>
#include <QFile>
#include <QStringList>

#include "model_node.h"

#include "model_io.h"
#include "model_iocom.h"
#include "model_process.h"
#include "model_piblock.h"
#include "model_fiblock.h"
#include "model_ciblock.h"

#include "lib_parser/lib.h"

ModelBlock::ModelBlock()
{
    _node = NULL;
}

ModelBlock::ModelBlock(const ModelBlock &modelBlock)
{
    _name = modelBlock._name;
    _inLib = modelBlock._inLib;
    _driver = modelBlock._driver;
    _categ = modelBlock._categ;
    _path = modelBlock._path;
    _addrAbs = modelBlock._addrAbs;
    _sizeAddrRel = modelBlock._sizeAddrRel;
    _masterCount = modelBlock._masterCount;
    _description = modelBlock._description;
    _node = NULL;

    for(int i=0; i<modelBlock._files.size(); i++)
        addFile(new ModelFile(*modelBlock._files[i]));
    for(int i=0; i<modelBlock._params.size(); i++)
        addParam(new ModelParam(*modelBlock._params[i]));
    for(int i=0; i<modelBlock._properties.size(); i++)
        addProperty(new ModelProperty(*modelBlock._properties[i]));
    for(int i=0; i<modelBlock._flows.size(); i++)
        addFlow(new ModelFlow(*modelBlock._flows[i]));
    for(int i=0; i<modelBlock._clocks.size(); i++)
        addClock(new ModelClock(*modelBlock._clocks[i]));
    for(int i=0; i<modelBlock._ports.size(); i++)
        addPort(new ModelPort(*modelBlock._ports[i]));
    for(int i=0; i<modelBlock._pins.size(); i++)
        addPin(new ModelPin(*modelBlock._pins[i]));
    for(int i=0; i<modelBlock._resets.size(); i++)
        addReset(new ModelReset(*modelBlock._resets[i]));
    for(int i=0; i<modelBlock._parts.size(); i++)
        addPart(new ModelComponentPart(*modelBlock._parts[i]));
}

ModelBlock::~ModelBlock()
{
    for(int i=0; i<_files.size(); i++)
        delete _files[i];
    for(int i=0; i<_params.size(); i++)
        delete _params[i];
    for(int i=0; i<_properties.size(); i++)
        delete _properties[i];
    for(int i=0; i<_flows.size(); i++)
        delete _flows[i];
    for(int i=0; i<_clocks.size(); i++)
        delete _clocks[i];
    for(int i=0; i<_ports.size(); i++)
        delete _ports[i];
    for(int i=0; i<_pins.size(); i++)
        delete _pins[i];
    for(int i=0; i<_resets.size(); i++)
        delete _resets[i];
    for(int i=0; i<_parts.size(); i++)
        delete _parts[i];
}

const QString &ModelBlock::name() const
{
    return _name;
}

void ModelBlock::setName(const QString &name)
{
    if(_name != name)
    {
        QString oldName = _name;
        _name = name;
        if(_node)
            _node->updateKeyBlock(this, oldName);
    }
}

bool ModelBlock::inLib() const
{
    return _inLib;
}

void ModelBlock::setInLib(bool inLib)
{
    _inLib = inLib;
}

const QString &ModelBlock::driver() const
{
    return _driver;
}

void ModelBlock::setDriver(const QString &driver)
{
    _driver = driver;
}

QString ModelBlock::path() const
{
    return _path;
}

void ModelBlock::setPath(const QString &path)
{
    _path = path;
}

const QString &ModelBlock::categ() const
{
    return _categ;
}

void ModelBlock::setCateg(const QString &categ)
{
    _categ = categ;
}

qint32 ModelBlock::addrAbs() const
{
    return _addrAbs;
}

void ModelBlock::setAddrAbs(const qint32 &value)
{
    _addrAbs = value;
}

quint8 ModelBlock::sizeAddrRel() const
{
    return _sizeAddrRel;
}

void ModelBlock::setSizeAddrRel(const quint8 &value)
{
    _sizeAddrRel = value;
}

quint8 ModelBlock::masterCount() const
{
    return _masterCount;
}

void ModelBlock::setMasterCount(const quint8 &value)
{
    _masterCount = value;
}

const QString &ModelBlock::description() const
{
    return _description;
}

void ModelBlock::setDescription(const QString &description)
{
    _description = description;
}

ModelBlock::Type ModelBlock::type() const
{
    return Block;
}

bool ModelBlock::isIO() const
{
    if(type()==IO || type()==IOCom)
        return true;
    return false;
}

ModelNode *ModelBlock::node() const
{
    return _node;
}

void ModelBlock::setNode(ModelNode *node)
{
    _node = node;
}

QList<ModelFile *> &ModelBlock::files()
{
    return _files;
}

const QList<ModelFile *> &ModelBlock::files() const
{
    return _files;
}

void ModelBlock::addFile(ModelFile *file)
{
    file->setParent(this);
    _files.append(file);
    _filesMap.insert(file->name(), file);
}

void ModelBlock::addFiles(const QList<ModelFile *> &files)
{
    foreach (ModelFile *file, files)
    {
        addFile(file);
    }
}

ModelFile *ModelBlock::getFile(const QString &name) const
{
    QMap<QString, ModelFile*>::const_iterator localConstFind = _filesMap.constFind(name);
    if(localConstFind!=_filesMap.constEnd())
        return *localConstFind;
    return NULL;
}

ModelFile *ModelBlock::getDefFile() const
{
    for(int i=0; i<this->files().size(); i++)
    {
        ModelFile *file = this->files().at(i);
        if(file->group()=="blockdef")
            return file;
    }
    return NULL;
}

QList<ModelParam *> &ModelBlock::params()
{
    return _params;
}

const QList<ModelParam *> &ModelBlock::params() const
{
    return _params;
}

void ModelBlock::addParam(ModelParam *param)
{
    param->setParent(this);
    _params.append(param);
    _paramsMap.insert(param->name(), param);
}

void ModelBlock::addParams(const QList<ModelParam *> &params)
{
    foreach (ModelParam *param, params)
    {
        addParam(param);
    }
}

ModelParam *ModelBlock::getParam(const QString &name) const
{
    QMap<QString, ModelParam*>::const_iterator localConstFind = _paramsMap.constFind(name);
    if(localConstFind!=_paramsMap.constEnd())
        return *localConstFind;
    return NULL;
}

QList<ModelProperty *> &ModelBlock::properties()
{
    return _properties;
}

const QList<ModelProperty *> &ModelBlock::properties() const
{
    return _properties;
}

void ModelBlock::addProperty(ModelProperty *property)
{
    property->setParent(this);
    _properties.append(property);
    _propertiesMap.insert(property->name(), property);
}

void ModelBlock::addProperties(const QList<ModelProperty *> &properties)
{
    foreach (ModelProperty *property, properties)
    {
        addProperty(property);
    }
}

ModelProperty *ModelBlock::getProperty(const QString &name) const
{
    QMap<QString, ModelProperty*>::const_iterator localConstFind = _propertiesMap.constFind(name);
    if(localConstFind!=_propertiesMap.constEnd())
        return *localConstFind;
    return NULL;
}

ModelProperty *ModelBlock::getPropertyPath(const QString &path) const
{
    if(path.isEmpty())
        return NULL;
    int index = path.indexOf(".");
    if(index==-1)
    {
        if(_propertiesMap.contains(path))
            return _propertiesMap[path];
        else return NULL;
    }
    if(_propertiesMap.contains(path.left(index)))
        return _propertiesMap[path.left(index)]->getPropertyPath(path.mid(index+1));
    return NULL;
}

QList<ModelFlow *> &ModelBlock::flows()
{
    return _flows;
}

const QList<ModelFlow *> &ModelBlock::flows() const
{
    return _flows;
}

QList<ModelFlow *> ModelBlock::flowsIn() const
{
    QList<ModelFlow *> lflowsIn;
    foreach (ModelFlow *flow, _flows)
    {
        if(flow->type()=="in")
            lflowsIn.append(flow);
    }
    return lflowsIn;
}

QList<ModelFlow *> ModelBlock::flowsOut() const
{
    QList<ModelFlow *> lflowsOut;
    foreach (ModelFlow *flow, _flows)
    {
        if(flow->type()=="out")
            lflowsOut.append(flow);
    }
    return lflowsOut;
}

void ModelBlock::addFlow(ModelFlow *flow)
{
    flow->setParent(this);
    _flows.append(flow);
    _flowsMap.insert(flow->name(), flow);
}

void ModelBlock::addFlows(const QList<ModelFlow *> &flows)
{
    foreach (ModelFlow *flow, flows)
    {
        addFlow(flow);
    }
}

ModelFlow *ModelBlock::getFlow(const QString &name) const
{
    QMap<QString, ModelFlow*>::const_iterator localConstFind = _flowsMap.constFind(name);
    if(localConstFind!=_flowsMap.constEnd())
        return *localConstFind;
    return NULL;
}

QList<ModelClock *> &ModelBlock::clocks()
{
    return _clocks;
}

const QList<ModelClock *> &ModelBlock::clocks() const
{
    return _clocks;
}

void ModelBlock::addClock(ModelClock *clock)
{
    clock->setParent(this);
    _clocks.append(clock);
    _clocksMap.insert(clock->name(), clock);
}

void ModelBlock::addClocks(const QList<ModelClock *> &clocks)
{
    foreach (ModelClock *clock, clocks)
    {
        addClock(clock);
    }
}

ModelClock *ModelBlock::getClock(const QString &name) const
{
    QMap<QString, ModelClock*>::const_iterator localConstFind = _clocksMap.constFind(name);
    if(localConstFind!=_clocksMap.constEnd())
        return *localConstFind;
    return NULL;
}

QList<ModelPort *> &ModelBlock::ports()
{
    return _ports;
}

const QList<ModelPort *> &ModelBlock::ports() const
{
    return _ports;
}

void ModelBlock::addPort(ModelPort *port)
{
    port->setParent(this);
    _ports.append(port);
    _portsMap.insert(port->name(), port);
}

void ModelBlock::addPorts(const QList<ModelPort *> &ports)
{
    foreach (ModelPort *port, ports)
    {
        addPort(port);
    }
}

ModelPort *ModelBlock::getPort(const QString &name) const
{
    QMap<QString, ModelPort*>::const_iterator localConstFind = _portsMap.constFind(name);
    if(localConstFind!=_portsMap.constEnd())
        return *localConstFind;
    return NULL;
}

QList<ModelPin *> &ModelBlock::pins()
{
    return _pins;
}

const QList<ModelPin *> &ModelBlock::pins() const
{
    return _pins;
}

void ModelBlock::addPin(ModelPin *pin)
{
    pin->setParent(this);
    _pins.append(pin);
    _pinsMap.insert(pin->name(), pin);
}

void ModelBlock::addPins(const QList<ModelPin *> &pins)
{
    foreach (ModelPin *pin, pins)
    {
        addPin(pin);
    }
}

ModelPin *ModelBlock::getPin(const QString &name) const
{
    QMap<QString, ModelPin*>::const_iterator localConstFind = _pinsMap.constFind(name);
    if(localConstFind!=_pinsMap.constEnd())
        return *localConstFind;
    return NULL;
}

QList<ModelReset *> &ModelBlock::resets()
{
    return _resets;
}

const QList<ModelReset *> &ModelBlock::resets() const
{
    return _resets;
}

void ModelBlock::addReset(ModelReset *reset)
{
    reset->setParent(this);
    _resets.append(reset);
    _resetsMap.insert(reset->name(), reset);
}

void ModelBlock::addResets(const QList<ModelReset *> &resets)
{
    foreach (ModelReset *reset, resets)
    {
        addReset(reset);
    }
}

ModelReset *ModelBlock::getReset(const QString &name) const
{
    QMap<QString, ModelReset*>::const_iterator localConstFind = _resetsMap.constFind(name);
    if(localConstFind!=_resetsMap.constEnd())
        return *localConstFind;
    return NULL;
}

QList<ModelComponentPart *> &ModelBlock::parts()
{
    return _parts;
}

const QList<ModelComponentPart *> &ModelBlock::parts() const
{
    return _parts;
}

void ModelBlock::addPart(ModelComponentPart *part)
{
    part->setParent(this);
    _parts.append(part);
    _partsMap.insert(part->name(), part);
}

void ModelBlock::addParts(const QList<ModelComponentPart *> &parts)
{
    foreach (ModelComponentPart *part, parts)
    {
        addPart(part);
    }
}

ModelComponentPart *ModelBlock::getPart(const QString &name) const
{
    QMap<QString, ModelComponentPart*>::const_iterator localConstFind = _partsMap.constFind(name);
    if(localConstFind!=_partsMap.constEnd())
        return *localConstFind;
    return NULL;
}

QStringList ModelBlock::getPdfDoc() const
{
    QStringList docFile;

    for(int i=0; i<this->files().size(); i++)
    {
        ModelFile *file = this->files().at(i);
        if(file->group()=="doc" && file->name().endsWith(".pdf"))
            docFile.append(_path + "/" + file->path());
    }

    return docFile;
}

ModelBlock *ModelBlock::readFromFile(const QString &fileName)
{
    QDomDocument doc;
    ModelBlock *modelBlock;
    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug()<<"Cannot open"<<file.fileName();
    else
    {
        if(!doc.setContent(&file))
        {
            qDebug()<<"Cannot parse"<<file.fileName();
            return NULL;
        }
        modelBlock = ModelBlock::fromNodeGenerated(doc.documentElement());
        file.close();
        return modelBlock;
    }
    return NULL;
}

ModelBlock *ModelBlock::fromNodeGenerated(const QDomElement &domElement, ModelBlock *block)
{
    if(block==NULL)
        block = new ModelBlock();

    bool ok=false;

    block->setName(domElement.attribute("name","no_name"));

    block->setInLib((domElement.attribute("inlib","")=="1" || domElement.attribute("inlib","")=="true"));

    block->setDriver(domElement.attribute("driver",""));
    block->setPath(domElement.attribute("path",""));
    block->setCateg(domElement.attribute("categ",""));

    int addrAbs = domElement.attribute("addr_abs","-1").toInt(&ok);
    if(ok)
        block->setAddrAbs(addrAbs);
    else
        block->setAddrAbs(-1);

    int sizeAddrRel = domElement.attribute("pi_size_addr_rel","0").toInt(&ok);
    if(ok && sizeAddrRel>=0)
        block->setSizeAddrRel(sizeAddrRel);
    else
        block->setSizeAddrRel(0);

    int masterCount = domElement.attribute("master_count","0").toInt(&ok);
    if(ok && masterCount>=0)
        block->setMasterCount(masterCount);
    else
        block->setMasterCount(0);

    block->setDescription(domElement.attribute("desc",""));

    QDomNode n = domElement.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="files")
                block->addFiles(ModelFile::listFromNodeGenerated(e));
            if(e.tagName()=="params")
                block->addParams(ModelParam::listFromNodeGenerated(e));
            if(e.tagName()=="properties")
                block->addProperties(ModelProperty::listFromNodeGenerated(e));
            if(e.tagName()=="flows")
                block->addFlows(ModelFlow::listFromNodeGenerated(e));
            if(e.tagName()=="clocks")
                block->addClocks(ModelClock::listFromNodeGenerated(e));
            if(e.tagName()=="resets")
                block->addResets(ModelReset::listFromNodeGenerated(e));
            if(e.tagName()=="parts")
                block->addParts(ModelComponentPart::listFromNodeGenerated(e));
        }
        n = n.nextSibling();
    }

    return block;
}

ModelBlock *ModelBlock::fromNodeDef(const QDomElement &domElement, ModelBlock *block)
{
    if(block==NULL)
        block = new ModelBlock();

    QDomNode n = domElement.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="parts")
            {
                foreach(ModelComponentPart *nodePart, ModelComponentPart::listFromNodeGenerated(e))
                {
                    ModelComponentPart *part = block->getPart(nodePart->name());
                    if(!part)
                    {
                        part = nodePart;
                        block->addPart(part);
                    }
                    part->setPos(nodePart->pos());
                }
            }
            if(e.tagName()=="properties")
                block->redefProperties(ModelProperty::listFromNodeGenerated(e));
            if(e.tagName()=="clocks")
                block->redefClocks(ModelClock::listFromNodeGenerated(e));
            if(e.tagName()=="params")
                block->redefParams(ModelParam::listFromNodeGenerated(e));
            if(e.tagName()=="flows")
                block->redefFlows(ModelFlow::listFromNodeGenerated(e));
        }
        n = n.nextSibling();
    }

    block->setName(domElement.attribute("name","no_name"));
    block->setDriver(domElement.attribute("driver",""));
    block->setPath(domElement.attribute("path",""));

    // compatibility mode
    if(domElement.hasAttribute("x_pos") || domElement.hasAttribute("y_pos"))
    {
        QPoint pos;
        pos.setX(domElement.attribute("x_pos","0").toInt());
        pos.setX(domElement.attribute("y_pos","0").toInt());

        if(block->parts().empty())
        {
            ModelComponentPart *part = new ModelComponentPart();
            part->setName("main");
            block->addPart(part);
        }

        foreach (ModelComponentPart *part, block->parts())
            part->setPos(pos);
    }

    return block;
}

QList<ModelBlock *> ModelBlock::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelBlock *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            ModelBlock *block=NULL;
            QString typeBlock = e.attribute("type");
            if(e.tagName()=="block")
            {
                if(typeBlock=="process")
                    block = ModelProcess::fromNodeGenerated(e);
                if(typeBlock=="io")
                    block = ModelIO::fromNodeGenerated(e);
                if(typeBlock=="iocom")
                    block = ModelIOCom::fromNodeGenerated(e);
                if(typeBlock=="pi")
                    block = ModelPIBlock::fromNodeGenerated(e);
                if(typeBlock=="fi")
                    block = ModelFIBlock::fromNodeGenerated(e);
                if(typeBlock=="ci")
                    block = ModelCIBlock::fromNodeGenerated(e);
                if(block==NULL)
                    block = ModelBlock::fromNodeGenerated(e);

                list.append(block);
            }
        }
        n = n.nextSibling();
    }
    return list;
}

QList<ModelBlock *> ModelBlock::listFromNodeDef(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelBlock *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            ModelBlock *block=NULL;
            if(e.tagName()=="process")
                block = ModelProcess::fromNodeDef(e);
            if(e.tagName()=="io" || e.tagName()=="iocom")
            {
                QString driver = e.attribute("driver","");

                BlockLib *ioLib = Lib::getLib().io(driver);
                if(ioLib)
                {
                    if(ioLib->type() == BlockLib::IOCom)
                        block = ModelIOCom::fromNodeDef(e);
                    else
                        block = ModelIO::fromNodeDef(e);
                }
            }
            if(block==NULL)
                block = ModelBlock::fromNodeGenerated(e);

            if(block->parts().isEmpty())
            {
                block->addPart(new ModelComponentPart());
            }

            list.append(block);
        }
        n = n.nextSibling();
    }
    return list;
}

QDomElement ModelBlock::toXMLElement(QDomDocument &doc, const QDomElement &other)
{
    QDomElement element;
    if(other.isNull())
    {
        if(type()==IO)
            element = doc.createElement("io");
        else if(type()==IOCom)
            element = doc.createElement("iocom");
        else
            element = doc.createElement("process");
    }
    else
        element = other;

    element.setAttribute("name", _name);
    element.setAttribute("driver", _driver);

    if(!_inLib)
        element.setAttribute("path", _path);

    if(!isIO())
        element.setAttribute("inlib", _inLib ? "true" : "false");

    QDomElement paramList = doc.createElement("params");
    foreach (ModelParam *param, _params)
    {
        if(param->isHard())
            paramList.appendChild(param->toXMLElement(doc));
    }
    if(!paramList.childNodes().isEmpty())
        element.appendChild(paramList);

    QDomElement clockList = doc.createElement("clocks");
    foreach (ModelClock *clock, _clocks)
    {
        if(clock->group().isEmpty() && clock->direction()==ModelClock::In)
            clockList.appendChild(clock->toXMLElement(doc));
    }
    if(!clockList.childNodes().isEmpty())
        element.appendChild(clockList);

    QDomElement propertyList = doc.createElement("properties");
    foreach (ModelProperty *property, _properties)
    {
        propertyList.appendChild(property->toXMLElement(doc));
    }
    if(!propertyList.childNodes().isEmpty())
        element.appendChild(propertyList);

    QDomElement flowList = doc.createElement("flows");
    foreach (ModelFlow *flow, _flows)
    {
        flowList.appendChild(flow->toXMLElement(doc));
    }
    if(!flowList.childNodes().isEmpty())
        element.appendChild(flowList);

    QDomElement partList = doc.createElement("parts");
    foreach (ModelComponentPart *part, _parts)
    {
        partList.appendChild(part->toXMLElement(doc));
    }
    if(!partList.childNodes().isEmpty())
        element.appendChild(partList);

    return element;
}

void ModelBlock::redefParams(QList<ModelParam *> params)
{
    foreach(ModelParam *nodeParam, params)
    {
        ModelParam *param = getParam(nodeParam->name());
        if(param)
            param->setValue(nodeParam->value());
    }
}

void ModelBlock::redefProperties(QList<ModelProperty *> properties)
{
    foreach(ModelProperty *nodeProperty, properties)
    {
        ModelProperty *property = getPropertyPath(nodeProperty->getPath());
        if(property)
            property->setValue(nodeProperty->value());
        redefProperties(nodeProperty->properties());
    }
}

void ModelBlock::redefFlows(QList<ModelFlow *> flows)
{
    foreach(ModelFlow *nodeFlow, flows)
    {
        ModelFlow *flow = getFlow(nodeFlow->name());
        if(flow)
            flow->setSize(nodeFlow->size());
    }
}

void ModelBlock::redefClocks(QList<ModelClock *> clocks)
{
    foreach(ModelClock *nodeClock, clocks)
    {
        ModelClock *clock = getClock(nodeClock->name());
        if(clock)
            clock->setTypical(nodeClock->typical());
    }
}
