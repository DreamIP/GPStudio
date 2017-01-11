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

#ifndef MODEL_BLOCK_H
#define MODEL_BLOCK_H

#include "gpstudio_lib_common.h"

#include <QObject>
#include <QDomElement>
#include <QString>
#include <QList>
#include <QMap>
#include <QPoint>

#include "model_file.h"
#include "model_param.h"
#include "model_property.h"
#include "model_flow.h"
#include "model_clock.h"
#include "model_port.h"
#include "model_pin.h"
#include "model_reset.h"
#include "model_componentpart.h"

class ModelNode;

class GPSTUDIO_LIB_EXPORT ModelBlock
{
public:
    ModelBlock();
    ModelBlock(const ModelBlock &modelBlock);
    virtual ~ModelBlock();

    const QString &name() const;
    void setName(const QString &name);

    bool inLib() const;
    void setInLib(bool inLib);

    const QString &driver() const;
    void setDriver(const QString &driver);

    QString path() const;
    void setPath(const QString &path);

    const QString &categ() const;
    void setCateg(const QString &categ);

    qint32 addrAbs() const;
    void setAddrAbs(const qint32 &value);

    quint8 sizeAddrRel() const;
    void setSizeAddrRel(const quint8 &value);

    quint8 masterCount() const;
    void setMasterCount(const quint8 &value);

    const QString &description() const;
    void setDescription(const QString &description);

    enum Type
    {
        Block,
        Process,
        IO,
        IOCom,
        PI,
        FI,
        CI
    };
    virtual Type type() const;
    bool isIO() const;

    ModelNode *node() const;
    void setNode(ModelNode *node);

    QList<ModelFile *> &files();
    const QList<ModelFile *> &files() const;
    void addFile(ModelFile *file);
    void addFiles(const QList<ModelFile *> &files);
    ModelFile *getFile(const QString &name) const;
    ModelFile *getDefFile() const;

    QList<ModelParam *> &params();
    const QList<ModelParam *> &params() const;
    void addParam(ModelParam *param);
    void addParams(const QList<ModelParam *> &params);
    ModelParam *getParam(const QString &name) const;

    QList<ModelProperty *> &properties();
    const QList<ModelProperty *> &properties() const;
    void addProperty(ModelProperty *property);
    void addProperties(const QList<ModelProperty *> &properties);
    ModelProperty *getProperty(const QString &name) const;
    ModelProperty *getPropertyPath(const QString &path) const;

    QList<ModelFlow *> &flows();
    const QList<ModelFlow *> &flows() const;
    QList<ModelFlow *> flowsIn() const;
    QList<ModelFlow *> flowsOut() const;
    void addFlow(ModelFlow *flow);
    void addFlows(const QList<ModelFlow *> &flows);
    ModelFlow *getFlow(const QString &name) const;

    QList<ModelClock *> &clocks();
    const QList<ModelClock *> &clocks() const;
    void addClock(ModelClock *clock);
    void addClocks(const QList<ModelClock *> &clocks);
    ModelClock *getClock(const QString &name) const;

    QList<ModelPort *> &ports();
    const QList<ModelPort *> &ports() const;
    void addPort(ModelPort *port);
    void addPorts(const QList<ModelPort *> &ports);
    ModelPort *getPort(const QString &name) const;

    QList<ModelPin *> &pins();
    const QList<ModelPin *> &pins() const;
    void addPin(ModelPin *pin);
    void addPins(const QList<ModelPin *> &pins);
    ModelPin *getPin(const QString &name) const;

    QList<ModelReset *> &resets();
    const QList<ModelReset *> &resets() const;
    void addReset(ModelReset *reset);
    void addResets(const QList<ModelReset *> &resets);
    ModelReset *getReset(const QString &name) const;

    QList<ModelComponentPart *> &parts();
    const QList<ModelComponentPart *> &parts() const;
    void addPart(ModelComponentPart *part);
    void addParts(const QList<ModelComponentPart *> &parts);
    ModelComponentPart *getPart(const QString &name) const;

    QStringList getPdfDoc() const;

public:
    static ModelBlock *readFromFile(const QString &fileName);

    static ModelBlock *fromNodeGenerated(const QDomElement &domElement, ModelBlock *block=NULL);
    static ModelBlock *fromNodeDef(const QDomElement &domElement, ModelBlock *block=NULL);
    static QList<ModelBlock *> listFromNodeGenerated(const QDomElement &domElement);
    static QList<ModelBlock *> listFromNodeDef(const QDomElement &domElement);
    virtual QDomElement toXMLElement(QDomDocument &doc, const QDomElement &other=QDomElement());

protected:
    QString _name;
    bool _inLib;
    QString _driver;
    QString _path;
    QString _categ;
    qint32 _addrAbs;
    quint8 _sizeAddrRel;
    quint8 _masterCount;
    QString _description;

    QList<ModelFile *> _files;
    QMap<QString, ModelFile*> _filesMap;

    void redefParams(QList<ModelParam *> params);
    QList<ModelParam *> _params;
    QMap<QString, ModelParam*> _paramsMap;

    void redefProperties(QList<ModelProperty *> properties);
    QList<ModelProperty *> _properties;
    QMap<QString, ModelProperty*> _propertiesMap;

    void redefFlows(QList<ModelFlow *> flows);
    QList<ModelFlow *> _flows;
    QMap<QString, ModelFlow*> _flowsMap;

    void redefClocks(QList<ModelClock *> clocks);
    QList<ModelClock *> _clocks;
    QMap<QString, ModelClock*> _clocksMap;

    QList<ModelPort *> _ports;
    QMap<QString, ModelPort*> _portsMap;

    QList<ModelPin *> _pins;
    QMap<QString, ModelPin*> _pinsMap;

    QList<ModelReset *> _resets;
    QMap<QString, ModelReset*> _resetsMap;

    QList<ModelComponentPart *> _parts;
    QMap<QString, ModelComponentPart*> _partsMap;

    ModelNode *_node;
};

#endif // MODEL_BLOCK_H
