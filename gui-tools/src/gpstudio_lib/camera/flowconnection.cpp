#include "flowconnection.h"

FlowConnection::FlowConnection()
{
    _fps=0;
}

int FlowConnection::flowId() const
{
    return _flowId;
}

void FlowConnection::setFlowId(int flowId)
{
    _flowId = flowId;
}

Flow *FlowConnection::flow() const
{
    return _flow;
}

void FlowConnection::setFlow(Flow *flow)
{
    _flow = flow;
}

void FlowConnection::recImg(FlowPackage data)
{
    // fps calculation
    if(_prevImgReceive.isValid())
    {
        _fps = 1.0 / (QDateTime::currentDateTime().toMSecsSinceEpoch() - _prevImgReceive.toMSecsSinceEpoch()) * 1000.0;
    }
    _prevImgReceive = QDateTime::currentDateTime();

    // rec notification
    _lastData = data;
    emit flowReceived(data);
}

float FlowConnection::fps() const
{
    return _fps;
}
