#ifndef MODEL_VIEWER_H
#define MODEL_VIEWER_H

#include "gpstudio_lib_common.h"

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QDomElement>

class ModelGPViewer;
class ModelViewerFlow;

class GPSTUDIO_LIB_EXPORT ModelViewer
{
public:
    ModelViewer(const QString &name=QString());
    ModelViewer(const ModelViewer &modelViewer);
    ~ModelViewer();

    QString name() const;
    void setName(const QString &name);

    QList<ModelViewerFlow *> &viewerFlows();
    const QList<ModelViewerFlow *> &viewerFlows() const;
    QStringList viewerFlowsName() const;
    void addViewerFlow(ModelViewerFlow *viewerFlow);
    void addViewerFlow(QList<ModelViewerFlow *> viewerFlows);
    void removeViewerFlow(ModelViewerFlow *viewerFlow);
    ModelViewerFlow *getViewerFlow(const QString &flowName) const;

    ModelGPViewer *getParent() const;
    void setParent(ModelGPViewer *parent);

public:
    static ModelViewer *fromNodeGenerated(const QDomElement &domElement);
    static QList<ModelViewer *> listFromNodeGenerated(const QDomElement &domElement);
    QDomElement toXMLElement(QDomDocument &doc);

protected:
    QString _name;

    QList<ModelViewerFlow *> _viewerFlows;
    QMap<QString, ModelViewerFlow*> _viewerFlowsMap;
    friend class ModelViewerFlow;
    void updateKeyViewerFlow(ModelViewerFlow *viewerFlow, const QString &oldKey);

    ModelGPViewer *_parent;
};

#endif // MODEL_VIEWER_H
