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

#include "pdfviewer.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
 #include <poppler/qt5/poppler-qt5.h>
#else
 #include <poppler/qt4/poppler-qt4.h>
#endif

#include <QKeyEvent>
#include <QGraphicsPixmapItem>
#include <qmath.h>
#include <QDebug>

PdfViewer::PdfViewer(QWidget *parent, QString file)
    : QGraphicsView(parent)
{
    _currentPage = 0;
    setScene(new QGraphicsScene());
    scale(0.15, 0.15);

    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setRenderHint(QPainter::TextAntialiasing, true);
    setRenderHint(QPainter::HighQualityAntialiasing, true);

    setDragMode(QGraphicsView::ScrollHandDrag);

    _doc = Poppler::Document::load(file);
    showPage(0);
}

PdfViewer::~PdfViewer()
{
    delete _doc;
}

void PdfViewer::showDocument(const QString &file)
{
    PdfViewer *pdfViewer = new PdfViewer(NULL, file);
    pdfViewer->show();
}

void PdfViewer::showPage(int page)
{
    if(!_doc)
        return;
    if(_doc->page(page))
    {
        _currentPage = page;
        scene()->clear();
        QImage image = _doc->page(_currentPage)->renderToImage(
                1200,
                1200);
        scene()->addPixmap(QPixmap::fromImage(image))->setTransformationMode(Qt::SmoothTransformation);
    }
}

void PdfViewer::nextPage()
{
    showPage(_currentPage+1);
}

void PdfViewer::previousPage()
{
    showPage(_currentPage-1);
}

void PdfViewer::zoomIn()
{
    setZoomLevel(1);
}

void PdfViewer::zoomOut()
{
    setZoomLevel(-1);
}

void PdfViewer::zoomFit()
{
    fitInView(scene()->itemsBoundingRect().adjusted(-20, -20, 20, 20), Qt::KeepAspectRatio);
}
void PdfViewer::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Plus)
        zoomIn();
    if(event->key()==Qt::Key_Minus)
        zoomOut();
    if(event->key()==Qt::Key_Asterisk)
        zoomFit();
    if(event->key()==Qt::Key_Right || event->key()==Qt::Key_Down)
        nextPage();
    if(event->key()==Qt::Key_Left || event->key()==Qt::Key_Up)
        previousPage();
}

void PdfViewer::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;

    setZoomLevel(numSteps);
}

void PdfViewer::setZoomLevel(int step)
{
    double scaleLvl = qPow(1.2, step);
    double zoom = transform().m22();
    if(scaleLvl>1 && zoom>5)
        return;
    if(scaleLvl<1 && zoom<0.02)
        return;
    scale(scaleLvl, scaleLvl);
}

void PdfViewer::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    if(!parent())
        deleteLater();
}
