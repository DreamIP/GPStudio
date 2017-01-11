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

#include "layerwidget.h"

#include <QWheelEvent>
#include <qmath.h>

#include <QGraphicsPixmapItem>
#include <QImage>
#include <QBitmap>
#include <QDebug>
#include <QMouseEvent>

LayerWidget::LayerWidget(QWidget *parent) :
    QGraphicsView(parent)
{
    _scene = new QGraphicsScene();
    setScene(_scene);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setRenderHint(QPainter::TextAntialiasing, true);

    _currentZoomLevel = 1;

    _propertyView = All;
    _pixmapItem = _scene->addPixmap(QPixmap());
    _maskItem = _scene->addPixmap(QPixmap());
    _maskItem->setZValue(1);
    _titleItem = _scene->addSimpleText(QString());
}

LayerWidget::~LayerWidget()
{
}

#ifdef __USE_OPEN_CV__
using namespace cv;
void LayerWidget::showImage(const Mat &image, const QString &title)
{

    if(image.type()==CV_8U)
    {
        QImage qimg((uchar*)image.data, image.cols, image.rows, QImage::Format_Indexed8);
        QVector<QRgb> colors;
        for(unsigned short c=0; c<256; c++) colors.append(qRgb(c,c,c));
        qimg.setColorTable(colors);
        showImage(qimg, title);
    }
    if(image.type()==CV_64F || image.type()==CV_32F)
    {
        Mat imageChar;
        double minVal, maxVal;
        minMaxLoc(image, &minVal, &maxVal);
        image.convertTo(imageChar, CV_8U, 255.0/(maxVal - minVal), -minVal * 255.0/(maxVal - minVal));

        /*QImage qimg((uchar*)imageChar.data, imageChar.cols, imageChar.rows, QImage::Format_Indexed8);
        QVector<QRgb> colors;
        for(unsigned short c=0; c<256; c++) colors.append(qRgb(c,c,c));
        qimg.setColorTable(colors);*/

        QImage qimg(imageChar.cols, imageChar.rows, QImage::Format_Indexed8);
        QVector<QRgb> colors;
        for(unsigned short c=0; c<256; c++) colors.append(qRgb(c,c,c));
        qimg.setColorTable(colors);
        for(int x=0; x<imageChar.cols; x++)
        {
            for(int y=0; y<imageChar.rows; y++)
            {
                qimg.setPixel(x, y, imageChar.at<unsigned char>(y,x));
            }
        }
        showImage(qimg, title);
    }
    if(image.type()==CV_8UC3)
    {
        QImage qimg(image.cols, image.rows, QImage::Format_RGB32);
        for(int x=0; x<image.cols; x++)
        {
            for(int y=0; y<image.rows; y++)
            {
                Vec3b pix=image.at<Vec3b>(y,x);
                qimg.setPixel(x, y, qRgb(pix[2],pix[1],pix[0]));
            }
        }
        showImage(qimg, title);
    }
}
#endif

void LayerWidget::showImage(const QImage &image, const QString &title)
{
    showImage(QPixmap::fromImage(image), title);
}

void LayerWidget::showImage(const QPixmap &image, const QString &title)
{
    _pixmapItem->setPixmap(image);

    // add title to image
    _titleItem->setText(title);
    _titleItem->setPos((_pixmapItem->pixmap().width()-_titleItem->boundingRect().width())/2, -_titleItem->boundingRect().height()-10);
}

void LayerWidget::setMask(const QImage &mask)
{
    QPixmap masked = QPixmap::fromImage(mask);
    masked.setMask(masked.createMaskFromColor(QColor(Qt::white)));
    _maskItem->setPixmap(masked);
}

void LayerWidget::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;

    setZoomLevel(numSteps);
}

void LayerWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Plus)
        zoomIn();
    if(event->key()==Qt::Key_Minus)
        zoomOut();
    if(event->key()==Qt::Key_Asterisk)
        zoomFit();
    QGraphicsView::keyPressEvent(event);
}

void LayerWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        QPoint dep = (_startPos-mapToScene(event->pos())).toPoint();
        QRect viewRect(mapToScene(0, 0).toPoint(), mapToScene(frameRect().bottomRight()).toPoint());
        viewRect.translate(dep);
        centerOn(viewRect.center());
        emit viewMoved(viewRect);
    }
    QGraphicsView::mouseMoveEvent(event);
}

void LayerWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        event->accept();
        _startPos = mapToScene(event->pos());
        QGraphicsView::setCursor(Qt::ClosedHandCursor);
    }
    else if(event->button() & Qt::RightButton)
    {
        _startPos = mapToScene(event->pos());
    }

    QGraphicsView::mousePressEvent(event);
}

void LayerWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() & Qt::RightButton)
    {
        emit rectDrawed(QRect(_startPos.toPoint(), mapToScene(event->pos()).toPoint()).normalized());
    }
    QGraphicsView::unsetCursor();
    QGraphicsView::mouseReleaseEvent(event);
}

void LayerWidget::setZoomLevel(int step)
{
    double zoom = qPow(1.25,step);

    _currentZoomLevel *= zoom;

    scale(zoom, zoom);

    QRect viewRect(mapToScene(0, 0).toPoint(), mapToScene(frameRect().bottomRight()).toPoint());
    emit viewMoved(viewRect);
}

int LayerWidget::flowNumber() const
{
    return _flowNumber;
}

void LayerWidget::setFlowNumber(int flowNumber)
{
    _flowNumber = flowNumber;
}

unsigned int LayerWidget::propertyView() const
{
    return _propertyView;
}

void LayerWidget::setPropertyView(unsigned int propertyView)
{
    _propertyView = propertyView;
}

void LayerWidget::setView(const QRect &viewRect)
{
    fitInView(viewRect, Qt::KeepAspectRatio);
}

void LayerWidget::zoomIn()
{
    setZoomLevel(1);
}

void LayerWidget::zoomOut()
{
    setZoomLevel(-1);
}

void LayerWidget::zoomFit()
{
    fitInView(_scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}
