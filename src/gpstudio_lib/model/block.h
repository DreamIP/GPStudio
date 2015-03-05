#ifndef BLOCK_H
#define BLOCK_H

#include "gpstudio_lib_common.h"

#include <QDomElement>
#include <QString>
#include <QList>

#include "file.h"
#include "param.h"
#include "blockproperty.h"
#include "flow.h"
#include "clock.h"
#include "port.h"
#include "pin.h"
#include "reset.h"

class GPSTUDIO_LIB_EXPORT Block
{
protected:
    Block();

public:
    virtual ~Block();

    const QString &name() const;
    void setName(const QString &name);

    bool inLib() const;
    void setInLib(bool inLib);

    qint32 addrAbs() const;
    void setAddrAbs(const qint32 &value);

    quint8 sizeAddrRel() const;
    void setSizeAddrRel(const quint8 &value);

    quint8 masterCount() const;
    void setMasterCount(const quint8 &value);

    const QString &description() const;
    void setDescription(const QString &description);

    virtual QString type() const;

    QList<File *> &files();
    const QList<File *> &files() const;
    void addFile(File *file);

    QList<Param *> &params();
    const QList<Param *> &params() const;
    void addParam(Param *param);

    QList<BlockProperty *> &properties();
    const QList<BlockProperty *> &properties() const;
    void addProperty(BlockProperty *propertyEnum);

    QList<Flow *> &flows();
    const QList<Flow *> &flows() const;
    void addFlow(Flow *flow);

    QList<Clock *> &clocks();
    const QList<Clock *> &clocks() const;
    void addClock(Clock *clock);

    QList<Port *> &ports();
    const QList<Port *> &ports() const;
    void addPort(Port *port);

    QList<Pin *> &pins();
    const QList<Pin *> &pins() const;
    void addPin(Pin *pin);

    QList<Reset *> &resets();
    const QList<Reset *> &resets() const;
    void addReset(Reset *reset);

public:
    static Block *fromNodeGenerated(const QDomElement &domElement);
    static QList<Block *> listFromNodeGenerated(const QDomElement &domElement);

protected:
    QString _name;
    bool _inLib;
    qint32 _addrAbs;
    quint8 _sizeAddrRel;
    quint8 _masterCount;
    QString _description;

    QList<File *> _files;
    QList<Param *> _params;
    QList<BlockProperty *> _properties;
    QList<Flow *> _flows;
    QList<Clock *> _clocks;
    QList<Port *> _ports;
    QList<Pin *> _pins;
    QList<Reset *> _resets;
};

#endif // BLOCK_H
