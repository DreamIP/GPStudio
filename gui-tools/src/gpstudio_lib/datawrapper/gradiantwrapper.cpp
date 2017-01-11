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

#include "gradiantwrapper.h"

#include <math.h>
#include <QPainter>

GradiantWrapper::GradiantWrapper()
{

}

GradiantWrapper::~GradiantWrapper()
{

}

QImage *GradiantWrapper::transform(const FlowPackage &data)
{
    QImage *visu = new QImage(_wimg, _himg, QImage::Format_RGB16);
    QImage input = data.toImage(_wimg/_cellSize*_nbBins, _himg/_cellSize, 16);

    /*QVector<QRgb> colors;
    for(int i=0; i<256; i++) colors.append(qRgb(i,i,i));
    visu->setColorTable(colors);*/

    int cells_in_x_dir = _wimg / _cellSize;
    int cells_in_y_dir = _himg / _cellSize;
    float radRangeForOneBin = 3.1415/(float)_nbBins; // dividing 180° into 9 bins, how large (in rad) is one bin?

    visu->fill(Qt::black);

    QPainter p(visu);
    p.setRenderHints(QPainter::Antialiasing, false);

    int drawX;
    int drawY;

    int mx;
    int my;

    float currRad;

    float dirVecX;
    float dirVecY;
    float maxVecLen;


    // compute line coordinates
    float x1;
    float y1;
    float x2;
    float y2;

    int currentGradStrength;

    for (int celly=1; celly < cells_in_y_dir-1; celly++)
    {
        for (int cellx=1; cellx < cells_in_x_dir-1; cellx++)
        {
            drawX = cellx * _cellSize;
            drawY = celly * _cellSize;

            mx = drawX + _cellSize/2;
            my = drawY + _cellSize/2;

            // draw in each cell all 9 gradient strengths
            for (int bin=0; bin<_nbBins; bin++)
            {
                currentGradStrength = qGray(
                    input.pixel(cellx*_nbBins + bin , celly));

                // no line to draw?
                if (currentGradStrength == 0)
                    continue;

                currRad = bin * radRangeForOneBin + radRangeForOneBin/2;

                dirVecX = cos( currRad + M_PI_2);
                dirVecY = sin( currRad + M_PI_2);
                maxVecLen = _cellSize/2;


                // compute line coordinates
                x1 = mx - dirVecX * maxVecLen;
                y1 = my - dirVecY * maxVecLen;
                x2 = mx + dirVecX * maxVecLen;
                y2 = my + dirVecY * maxVecLen;

                p.setPen(QPen(QColor(currentGradStrength,currentGradStrength,currentGradStrength,255)));

                p.drawLine(QPoint(x1,y1), QPoint(x2,y2));
            } // for (all bins)
        } // for (cellx)
    } // for (celly)

    p.end();

    return visu;
}

int GradiantWrapper::nbBins() const
{
    return _nbBins;
}

void GradiantWrapper::setNbBins(int nbBins)
{
    _nbBins = nbBins;
}

int GradiantWrapper::cellSize() const
{
    return _cellSize;
}

void GradiantWrapper::setCellSize(int cellSize)
{
    _cellSize = cellSize;
}

int GradiantWrapper::wimg() const
{
    return _wimg;
}

void GradiantWrapper::setWimg(int wimg)
{
    _wimg = wimg;
}
int GradiantWrapper::himg() const
{
    return _himg;
}

void GradiantWrapper::setHimg(int himg)
{
    _himg = himg;
}
