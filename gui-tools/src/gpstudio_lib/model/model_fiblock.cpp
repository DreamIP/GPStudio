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

#include "model_fiblock.h"

#include <QDebug>

ModelFIBlock::ModelFIBlock()
    : ModelBlock::ModelBlock()
{
    _name = "fi";
}

ModelFIBlock::ModelFIBlock(const ModelFIBlock &modelFIBlock)
    : ModelBlock(modelFIBlock)
{
    for(int i=0; i<modelFIBlock._flowConnects.size(); i++)
        addFlowConnect(new ModelFlowConnect(*modelFIBlock._flowConnects[i]));
    for(int i=0; i<modelFIBlock._treeConnects.size(); i++)
        addTreeConnect(new ModelTreeConnect(*modelFIBlock._treeConnects[i]));
}

ModelFIBlock::~ModelFIBlock()
{
    for(int i=0; i<_flowConnects.size(); i++)
        delete _flowConnects[i];
    for(int i=0; i<_treeConnects.size(); i++)
        delete _treeConnects[i];
}

ModelBlock::Type ModelFIBlock::type() const
{
    return FI;
}

QList<ModelFlowConnect *> &ModelFIBlock::flowConnects()
{
    return _flowConnects;
}

QList<ModelFlowConnect *> ModelFIBlock::flowConnects(const QString blockName)
{
    QList<ModelFlowConnect *> connects;

    for(int i=0; i<_flowConnects.size(); i++)
    {
        if(_flowConnects[i]->fromblock()==blockName || _flowConnects[i]->toblock()==blockName)
        {
            connects.append(_flowConnects[i]);
        }
    }

    return connects;
}

const QList<ModelFlowConnect *> &ModelFIBlock::flowConnects() const
{
    return _flowConnects;
}

void ModelFIBlock::addFlowConnect(ModelFlowConnect *flowConnect)
{
    flowConnect->setParent(this);
    _flowConnects.append(flowConnect);
}

void ModelFIBlock::removeFlowConnect(ModelFlowConnect *flowConnect)
{
    flowConnect->setParent(NULL);
    _flowConnects.removeOne(flowConnect);
}

void ModelFIBlock::addFlowConnects(const QList<ModelFlowConnect *> &flowConnects)
{
    foreach (ModelFlowConnect *flowConnect, flowConnects)
    {
        addFlowConnect(flowConnect);
    }
}

void ModelFIBlock::connectFlow(ModelFlow *fromFlow, ModelFlow *toFlow)
{
    ModelFlowConnect *modelFlowConnect = new ModelFlowConnect();
    modelFlowConnect->setFromblock(fromFlow->parent()->name());
    modelFlowConnect->setFromflow(fromFlow->name());
    modelFlowConnect->setToblock(toFlow->parent()->name());
    modelFlowConnect->setToflow(toFlow->name());
    addFlowConnect(modelFlowConnect);
}

void ModelFIBlock::connectFlow(const ModelFlowConnect &flowConnect)
{
    addFlowConnect(new ModelFlowConnect(flowConnect));
}

void ModelFIBlock::disConnectFlow(ModelFlow *fromFlow, ModelFlow *toFlow)
{
    if(fromFlow==NULL || toFlow==NULL)
        return;

    for(int i=0; i<_flowConnects.size(); i++)
    {
        if(_flowConnects[i]->fromblock()==fromFlow->parent()->name())
        {
            if(_flowConnects[i]->fromflow()==fromFlow->name())
            {
                if(_flowConnects[i]->toblock()==toFlow->parent()->name())
                {
                    if(_flowConnects[i]->toflow()==toFlow->name())
                    {
                        removeFlowConnect(_flowConnects[i]);
                        return;
                    }
                }
            }
        }
    }
}

void ModelFIBlock::disConnectFlow(const ModelFlowConnect &flowConnect)
{
    for(int i=0; i<_flowConnects.size(); i++)
    {
        if(*(_flowConnects[i])==flowConnect)
        {
            removeFlowConnect(_flowConnects[i]);
            return;
        }
    }
}

QList<ModelTreeConnect *> &ModelFIBlock::treeConnects()
{
    return _treeConnects;
}

const QList<ModelTreeConnect *> &ModelFIBlock::treeConnects() const
{
    return _treeConnects;
}

void ModelFIBlock::addTreeConnect(ModelTreeConnect *treeConnect)
{
    _treeConnects.append(treeConnect);
}

void ModelFIBlock::addTreeConnects(const QList<ModelTreeConnect *> &treeConnects)
{
    foreach (ModelTreeConnect *treeConnect, treeConnects)
    {
        addTreeConnect(treeConnect);
    }
}

ModelFIBlock *ModelFIBlock::fromNodeGenerated(const QDomElement &domElement, ModelFIBlock *fiBlock)
{
    if(fiBlock==NULL)
        fiBlock = new ModelFIBlock();

    ModelBlock::fromNodeGenerated(domElement, fiBlock);

    QDomNode n = domElement.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="flow_connects")
                fiBlock->addFlowConnects(ModelFlowConnect::listFromNodeGenerated(e));
            if(e.tagName()=="tree_connects")
                fiBlock->addTreeConnects(ModelTreeConnect::listFromNodeGenerated(e));
        }
        n = n.nextSibling();
    }

    return fiBlock;
}

ModelFIBlock *ModelFIBlock::fromNodeDef(const QDomElement &domElement, ModelFIBlock *fiBlock)
{
    if(fiBlock==NULL)
        fiBlock = new ModelFIBlock();

    fiBlock->setName("fi");

    QDomNode n = domElement.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="connects")
                fiBlock->addFlowConnects(ModelFlowConnect::listFromNodeDef(e));
        }
        n = n.nextSibling();
    }

    return fiBlock;
}

QDomElement ModelFIBlock::toXMLElement(QDomDocument &doc, const QDomElement &other)
{
    Q_UNUSED(other);

    QDomElement element = doc.createElement("flow_interconnect");

    QDomElement connectsList = doc.createElement("connects");
    foreach (ModelFlowConnect *flowConnect, _flowConnects)
    {
        connectsList.appendChild(flowConnect->toXMLElement(doc));
    }
    element.appendChild(connectsList);

    return element;
}
