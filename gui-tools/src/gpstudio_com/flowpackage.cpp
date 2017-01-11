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

#include "flowpackage.h"

#include <QDebug>
#include <QDateTime>

FlowPackage::FlowPackage(const QByteArray &data)
    : _data(data), _newPack(true)
{
}

FlowPackage::FlowPackage(const FlowPackage &other)
    : _data(other._data), _newPack(other._newPack)
{
    // qDebug()<<"copy FLOWDATA"<<other._data.size();
}

const FlowPackage &FlowPackage::operator=(const FlowPackage &other)
{
    _data = other._data;
    _newPack = other._newPack;
    return other;
}

FlowPackage::FlowPackage(const QImage &image, const int bitCount, const FlowPackage::ImageMode imageMode)
{
    Q_UNUSED(bitCount);

    if(imageMode==ImageModeGray)
    {
        for(int y=0; y<image.height(); y++)
        {
            for(int x=0; x<image.width(); x++)
            {
                //_data.append((char)0);
                _data.append((unsigned char)qGray(image.pixel(x,y)));
                //_data.append((char)0);
            }
        }
    }
    else
    {
    }

    _newPack = true;
}

const QByteArray &FlowPackage::data() const
{
    return _data;
}

QByteArray FlowPackage::getPart(const int size)
{
    QByteArray part = _data.left(size);
    _data.remove(0,size);
    _newPack = false;
    return part;
}

bool FlowPackage::empty() const
{
    return _data.size()==0;
}

void FlowPackage::clear()
{
    _data.clear();
}

void FlowPackage::appendData(const QByteArray &data)
{
    _data.append(data);
}

QImage FlowPackage::toImage(const int width, const int height, const int dataSize) const
{
    QImage img(width, height, QImage::Format_RGB32);

    if(dataSize==8)
    {
        const char *ptr = _data.data();
        //const char *ptrEnd = _data.data()+_data.size();

        int x, y;
        //int rwidth = width;
        int rheight = _data.size()/width;
        if(rheight > height)
            rheight = height;

        for(y=0; y<rheight; y++)
        {
            QRgb *line = (QRgb*)img.scanLine(y);
            for(x=0; x<width; x++)
            {
                line[x]=qRgb(*ptr,*ptr,*ptr);
                ptr++;
            }
        }
        for(y=rheight; y<height; y++)
        {
            QRgb *line = (QRgb*)img.scanLine(y);
            for(x=0; x<width; x++)
                line[x]=qRgb(10,0,100);
        }
    }

    return img;
}

QImage FlowPackage::toImage(const QSize size, const int dataSize) const
{
    return toImage(size.width(), size.height(), dataSize);
}

bool FlowPackage::isNewPack() const
{
    return _newPack;
}
