#include "model_gpviewer.h"

#include "model_viewer.h"

ModelGPViewer::ModelGPViewer()
{
}

ModelGPViewer::~ModelGPViewer()
{
    for(int i=0; i<_viewers.size(); i++)
        delete _viewers[i];
}

QString ModelGPViewer::name() const
{
    return _name;
}

void ModelGPViewer::setName(const QString &name)
{
    _name = name;
}

QList<ModelViewer *> &ModelGPViewer::viewers()
{
    return _viewers;
}

const QList<ModelViewer *> &ModelGPViewer::viewers() const
{
    return _viewers;
}

void ModelGPViewer::addViewer(ModelViewer *viewer)
{
    _viewers.append(viewer);
    _viewersMap.insert(viewer->name(), viewer);
    viewer->setParent(this);
}

void ModelGPViewer::addViewer(QList<ModelViewer *> viewers)
{
    foreach (ModelViewer *viewer, viewers)
        addViewer(viewer);
}

void ModelGPViewer::removeViewer(ModelViewer *viewer)
{
    _viewers.removeOne(viewer);
    _viewersMap.remove(viewer->name());
}

ModelViewer *ModelGPViewer::getViewer(const QString &name) const
{
    QMap<QString, ModelViewer*>::const_iterator localConstFind = _viewersMap.constFind(name);
    if(localConstFind!=_viewersMap.constEnd())
        return *localConstFind;
    return NULL;
}

ModelViewerFlow *ModelGPViewer::getViewerFlow(const QString &viewerName, const QString &flowName) const
{
    ModelViewer *viewer = getViewer(viewerName);
    if(!viewer)
        return NULL;
    ModelViewerFlow *viewerFlow = viewer->getViewerFlow(flowName);
    return viewerFlow;
}

ModelGPViewer *ModelGPViewer::fromNodeGenerated(const QDomElement &domElement)
{
    ModelGPViewer *gpviewer = new ModelGPViewer();

    QDomNode n = domElement.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="viewers")
                gpviewer->addViewer(ModelViewer::listFromNodeGenerated(e));
        }
        n = n.nextSibling();
    }

    return gpviewer;
}

QDomElement ModelGPViewer::toXMLElement(QDomDocument &doc)
{
    QDomElement element = doc.createElement("gpviewer");

    QDomElement viewersList = doc.createElement("viewers");
    foreach (ModelViewer *viewer, _viewers)
    {
        viewersList.appendChild(viewer->toXMLElement(doc));
    }
    element.appendChild(viewersList);

    return element;
}

void ModelGPViewer::updateKeyViewer(ModelViewer *viewer, const QString &oldKey)
{
    _viewersMap.remove(oldKey);
    _viewersMap.insert(viewer->name(), viewer);
}
