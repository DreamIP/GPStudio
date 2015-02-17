#include "camera.h"

// camera io driver
#include "camerausb.h"

#include <QDebug>

Camera::Camera(const CameraInfo &cameraInfo)
{
    _cameraIO = NULL;
    qDebug()<<cameraInfo.driverType();

    //if(cameraInfo.driverType().contains("USB", Qt::CaseInsensitive))
    {
        _cameraIO = new CameraUSB();
    }

    if(_cameraIO) _cameraIO->connect(cameraInfo);

    // TODO pass this part dynamic
    _outputFlow.append(new Flow(15));   // set param
    _paramFlow = _outputFlow[0];

    _outputFlow.append(new Flow(1));
    _outputFlow.append(new Flow(2));

    _inputFlow.append(new Flow(0x80));
    _inputFlow.append(new Flow(0x81));

    _start=true;
    start(QThread::NormalPriority);
}

Camera::~Camera()
{
    terminate();
    delete _cameraIO;
}

bool Camera::isConnected() const
{
    if(_cameraIO) return _cameraIO->isConnected();
    else return false;
}

void Camera::stop()
{
    _start=false;
    this->wait();
}

QVector<CameraInfo> Camera::avaibleCams()
{
    QVector<CameraInfo> avaibleCams;

    avaibleCams += CameraUSB::avaibleCams();

    return avaibleCams;
}

void Camera::run()
{
    int prev_numpacket=-1;
    bool succes;

    while(_start)
    {
        const QByteArray &received = _cameraIO->read(512*128, 10, &succes);

        if(!succes)
        {
            qDebug()<<"fail";
            terminate();
        }

        int start=0;
        while(start<received.size())
        {
            const QByteArray &packet = received.mid(start, 512);

            unsigned char idFlow = packet[0];
            unsigned char flagFlow = packet[1];
            unsigned short numpacket = ((unsigned short)((unsigned char)packet[2])*256)+(unsigned char)packet[3];

            for(int i=0; i<_inputFlow.size(); i++)
            {
                if(_inputFlow[i]->idFlow()==idFlow)
                {
                    if(prev_numpacket+1 != numpacket)
                    {
                        int miss_packet = numpacket-prev_numpacket-1;
                        qDebug()<<"miss"<<miss_packet<<numpacket;
                        for(int j=0; j<miss_packet; j++) _inputFlow[i]->appendData(QByteArray(512,0));
                    }
                    _inputFlow[i]->appendData(packet);
                    prev_numpacket = numpacket;

                    if(flagFlow==0xBA)      // end of flow
                    {
                        prev_numpacket=-1;
                        _inputFlow[i]->validate();
                        emit flowReadyToRead(i);
                    }
                }
            }
            if(idFlow == 0xFD)
            {
                //emit flowReadyToRead(received);
            }

            start+=512;
        }
        for(int i=0; i<_outputFlow.size(); i++)
        {
            if(_outputFlow[i]->readyToSend())
            {
                const QByteArray data = _outputFlow[i]->dataToSend(_cameraIO->sizePacket());
                _cameraIO->write(data);
            }
        }

        //QThread::sleep(100);
    }
}

CameraIO *Camera::cameraIO() const
{
    return _cameraIO;
}

void Camera::writeParam(const unsigned int addr, const unsigned int value)
{
    QByteArray paramFlow;

    // addr
    paramFlow.append((char)(addr >> 24));
    paramFlow.append((char)(addr >> 16));
    paramFlow.append((char)(addr >> 8));
    paramFlow.append((char)addr);
    // data
    paramFlow.append((char)(value >> 24));
    paramFlow.append((char)(value >> 16));
    paramFlow.append((char)(value >> 8));
    paramFlow.append((char)value);

    _paramFlow->send(paramFlow);
    //qDebug() << "param_trame: "<< byte.toHex();
}

void Camera::writeParam(const unsigned int addr, const unsigned int *data, const unsigned size)
{
    QByteArray paramFlow;
    unsigned int addrCurrent = addr;

    for(unsigned int i=0; i<size; i++)
    {
        unsigned int valueCurrent = data[i];

        // addr
        paramFlow.append((char)(addrCurrent >> 24));
        paramFlow.append((char)(addrCurrent >> 16));
        paramFlow.append((char)(addrCurrent >> 8));
        paramFlow.append((char)addrCurrent);
        // data
        paramFlow.append((char)(valueCurrent >> 24));
        paramFlow.append((char)(valueCurrent >> 16));
        paramFlow.append((char)(valueCurrent >> 8));
        paramFlow.append((char)valueCurrent);

        addrCurrent++;
    }

    _paramFlow->send(paramFlow);
}

void Camera::askStatus()
{
    // TODO to be removed
    QByteArray byte;
    byte.append((char)0x00);
    byte.append((char)0xFD);
    _cameraIO->write(byte);
}

const QList<Flow*> &Camera::outputFlow() const
{
    return _outputFlow;
}

QList<Flow*> &Camera::outputFlow()
{
    return _outputFlow;
}

const QList<Flow*> &Camera::inputFlow() const
{
    return _inputFlow;
}

QList<Flow*> &Camera::inputFlow()
{
    return _inputFlow;
}
