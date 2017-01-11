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

/*#include "harriswrapper.h"
#include <QDebug>
Harriswrapper::Harriswrapper()
{
}

//QImage *Harriswrapper::transform(const FlowData &data)
//{
//    QImage *visu = new QImage(_wimg, _himg, QImage::Format_RGB16);

//    visu->fill(Qt::black);

//    QRect imageRect = QRect(QPoint(0,0), visu->size());

////    QPainter p;
////    p.begin(&_harrisimage);

//    qDebug() << data.data().size() << data.data().mid(0,32).toHex();

//    if(data.data().size() <7) return visu;
////    unsigned nb_patch = (unsigned char)flow.data()[3];
//    unsigned nb_patch = (unsigned char)data.data()[1]*255 + (unsigned char)data.data()[3];
//    bool partial_packet = (data.data()[1] & 0x80);

//    unsigned patch_size= (unsigned char)data.data()[5];

//    qDebug() << "NPI: "<< nb_patch <<
//                "patch_size: "<< patch_size <<
//                "partial_packet: "<< partial_packet;

//    if(nb_patch == 0 || nb_patch > 200) return visu;

//    int offset = 7;
//    for (int cnt=0; cnt < nb_patch-1; cnt++)
//    {

//        if(offset+8+31*31*2+1>data.data().size()) return visu;
//        unsigned patch_y = (unsigned char) (data.data()[offset]*255) + (unsigned char) (data.data()[offset+2]) ;
//        offset+=4;
//        unsigned patch_x = (unsigned char) (data.data()[offset]*255) + (unsigned char) (data.data()[offset+2]);
//        offset+=4;
//        //qDebug() << "X" << patch_x << "Y" << patch_y;

//        QRect patchRect = QRect(patch_x-15, patch_y-15, 31, 31);
//        if(imageRect.contains(patchRect))
//        {
//            for(int y=0; y<31; y++)
//            {
//                for(int x=0; x<31; x++)
//                {
//                    QPoint p(x,y);
//                    unsigned pix = (unsigned char) (data.data()[offset]);
//                    visu->setPixel(p+patchRect.topLeft(),qRgb(pix,pix,pix));
//                    offset+=2;
//                }
//            }
//        }
//    }
//    return visu;

//}

QImage *Harriswrapper::transform(const FlowPackage &data)
{
    QImage *visu = new QImage(_wimg, _himg, QImage::Format_RGB16);

    visu->fill(Qt::black);

    QRect imageRect = QRect(QPoint(0,0), visu->size());

//    QPainter p;
//    p.begin(&_harrisimage);

//    qDebug() << data.data().size() << data.data().mid(0,64).toHex();


    if(data.data().size() <7)
    {

        qDebug()<< "trame de merde";
        return visu;
    }


//      Sur 8 bits !
//    unsigned nb_patch = (unsigned char)data.data()[0]*255 + (unsigned char)data.data()[1];
//    unsigned descripteur_size = (unsigned char)data.data()[2];
//    unsigned xcoord = (unsigned char)data.data()[3]*255 + (unsigned char)data.data()[4];
//    unsigned ycoord = (unsigned char)data.data()[5]*255 + (unsigned char)data.data()[6];

        int offset = 2;
        unsigned nb_patch = (unsigned char)data.data()[2]*256 + (unsigned char)data.data()[3];
        unsigned descripteur_size = (unsigned char)data.data()[4]*256 +  (unsigned char)data.data()[5];


//    qDebug() << "NPI: "<< nb_patch <<
//                "descripteur size: "<< descripteur_size;


    if(nb_patch == 0 || nb_patch > 200)
    {
        qDebug()<< "trame de merde";
        return visu;
    }
    if(data.data().size() != (nb_patch*(2*descripteur_size*descripteur_size+8)+6) )
    {

        qDebug()<< "trame de merde";
        return visu;
    }


     offset = 7;
    for (int cnt=0; cnt < nb_patch; cnt++)
    {

         // ATTENTION COOR X ET Y peut etre inverse dans le vhdl
        unsigned patch_y = ((unsigned) (data.data()[offset])<<8 )  + (unsigned char) (data.data()[offset+2]) ;
        offset+=4;
        unsigned patch_x = ((unsigned) (data.data()[offset])<<8)  + (unsigned char) (data.data()[offset+2]);
        offset+=2;
//        for (int ii=0;ii<25;ii++)
//        {
//            qDebug()<< "ii:" << ii <<":" << (unsigned) ((unsigned char) data.data()[ii]) ;

//        }
//        qDebug() << "X" << patch_x << "Y" << patch_y;

        QRect patchRect = QRect(patch_x-15, patch_y-15, 31, 31);
        if(imageRect.contains(patchRect))
        {
            for(int y=0; y<31; y++)
            {
                for(int x=0; x<31; x++)
                {
                    QPoint p(x,y);
                    //unsigned pix = (unsigned char) (data.data()[offset]);
                    unsigned pix = (unsigned char) (data.data()[offset]);
                    visu->setPixel(p+patchRect.topLeft(),qRgb(pix,pix,pix));
                    offset+=2;
                }
            }
        }
       offset+=2;
//
    }
    return visu;

}

int Harriswrapper::wimg() const
{
    return _wimg;
}

void Harriswrapper::setWimg(int wimg)
{
    _wimg = wimg;
}

int Harriswrapper::himg() const
{
    return _himg;
}

void Harriswrapper::setHimg(int himg)
{
    _himg = himg;
}
*/
