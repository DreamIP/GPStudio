#include "model_viewer.h"

#include "model_viewerflow.h"
#include "model_gpviewer.h"

ModelViewer::ModelViewer(const QString &name)
    : _name(name)
{
    _parent = NULL;
}

ModelViewer::ModelViewer(const ModelViewer &modelViewer)
{
    _name = modelViewer._name;
    for(int i=0; i<modelViewer._viewerFlows.size(); i++)
        addViewerFlow(new ModelViewerFlow(*modelViewer._viewerFlows[i]));
    _parent = NULL;
}

ModelViewer::~ModelViewer()
{
    for(int i=0; i<_viewerFlows.size(); i++)
        delete _viewerFlows[i];
}

QString ModelViewer::name() const
{
    return _name;
}

void ModelViewer::setName(const QString &name)
{
    if(_name != name)
    {
        QString oldName = _name;
        _name = name;
        if(_parent)
            _parent->updateKeyViewer(this, oldName);
    }
}

QList<ModelViewerFlow *> &ModelViewer::viewerFlows()
{
    return _viewerFlows;
}

const QList<ModelViewerFlow *> &ModelViewer::viewerFlows() const
{
    return _viewerFlows;
}

QStringList ModelViewer::viewerFlowsName() const
{
    QStringList names;
    foreach (ModelViewerFlow *viewerFlow, _viewerFlows)
        names << viewerFlow->flowName();
    return names;
}

void ModelViewer::addViewerFlow(ModelViewerFlow *viewerFlow)
{
    _viewerFlows.append(viewerFlow);
    _viewerFlowsMap.insert(viewerFlow->flowName(), viewerFlow);
    viewerFlow->setViewer(this);
}

void ModelViewer::addViewerFlow(QList<ModelViewerFlow *> viewerFlows)
{
    foreach (ModelViewerFlow *viewerFlow, viewerFlows)
        addViewerFlow(viewerFlow);
}

void ModelViewer::removeViewerFlow(ModelViewerFlow *viewerFlow)
{
    _viewerFlows.removeOne(viewerFlow);
    _viewerFlowsMap.remove(viewerFlow->flowName());
}

ModelViewerFlow *ModelViewer::getViewerFlow(const QString &flowName) const
{
    QMap<QString, ModelViewerFlow*>::const_iterator localConstFind = _viewerFlowsMap.constFind(flowName);
    if(localConstFind!=_viewerFlowsMap.constEnd())
        return *localConstFind;
    return NULL;
}

ModelGPViewer *ModelViewer::getParent() const
{
    return _parent;
}

void ModelViewer::setParent(ModelGPViewer *parent)
{
    _parent = parent;
}

ModelViewer *ModelViewer::fromNodeGenerated(const QDomElement &domElement)
{
    ModelViewer *viewer = new ModelViewer();

    viewer->setName(domElement.attribute("name","no_name"));

    QDomNode n = domElement.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="flows")
                viewer->addViewerFlow(ModelViewerFlow::listFromNodeGenerated(e));
        }
        n = n.nextSibling();
    }

    return viewer;
}

QList<ModelViewer *> ModelViewer::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelViewer *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="viewer")
                list.append(ModelViewer::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}

QDomElement ModelViewer::toXMLElement(QDomDocument &doc)
{
    QDomElement element = doc.createElement("viewer");

    element.setAttribute("name", _name);

    QDomElement flowsList = doc.createElement("flows");
    foreach (ModelViewerFlow *viewerFlow, _viewerFlows)
    {
        flowsList.appendChild(viewerFlow->toXMLElement(doc));
    }
    element.appendChild(flowsList);

    return element;
}

void ModelViewer::updateKeyViewerFlow(ModelViewerFlow *viewerFlow, const QString &oldKey)
{
    _viewerFlowsMap.remove(oldKey);
    _viewerFlowsMap.insert(viewerFlow->flowName(), viewerFlow);
}
