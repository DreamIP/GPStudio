#include <QApplication>
#include <QDebug>
#include <QTimer>

#include "camera/camera.h"
#include "camera/flowmanager.h"
#include "camera/flowconnection.h"
#include "cameracom.h"

#include <QSignalSpy>

class SleeperThread : public QThread
{
    public:
    static void msleep(unsigned long msecs)
    {
        QThread::msleep(msecs);
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // load camera config file (node_generated.xml)
    if(a.arguments().size()<2)
    {
        qDebug()<<"Please specify node_generated.xml as first argument";
        return 1;
    }
    QString configFileName = a.arguments()[1];
    Camera cam(configFileName);
    if(!cam.node())
    {
        qDebug()<<"Invalid node_generated.xml file";
        return 1;
    }

    // check for available camera, if present, connect to the first one
    QVector<CameraInfo> avaibleCams = CameraCom::avaibleCams();
    if(avaibleCams.empty())
    {
        qDebug()<<"No camera available";
        return 1;
    }
    cam.connectCam(avaibleCams[0]);
    qDebug()<<"connected to camera"<<avaibleCams[0].driverType()<<avaibleCams[0].addr();
    SleeperThread::msleep(500);
    cam.registermanager().evalAll();

    //set parameters
    cam.rootProperty().path("threshold_1.threshold")->setValue(200);

    // send image
    QImage image("/home/seb/Seafile/GPStudio_dev/hwloop/cat.jpg");
    cam.com()->outputFlows()[1]->send(image);

    QSignalSpy spy(cam.com(), SIGNAL(flowReadyToRead(int)));
    qDebug()<<spy.wait(5000);

    // save result
    cam.flowManager()->flowConnection(128)->lastData().toImage(image.size(), 8).save("/home/seb/catcat.jpg");

    return a.exec();
}
