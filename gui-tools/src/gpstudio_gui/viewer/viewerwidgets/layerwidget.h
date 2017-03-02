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

#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include "gpstudio_gui_common.h"

#include <QGraphicsView>
#include <QList>
#include <QMap>

#include "featureitem.h"

#ifdef __USE_OPEN_CV__
#include <opencv2/core/core.hpp>
#endif

/**
 * @brief The LayerViewer class is a QGraphicsView to which show a cv::Mat with capabilities of zoom and move into image.
 * Signal slot viewMoved and setView can be used to syncronize two viewers.
 */
class GPSTUDIO_GUI_EXPORT LayerWidget : public QGraphicsView
{
    Q_OBJECT
public:
    LayerWidget(QWidget *parent = 0);
    ~LayerWidget();

    enum ViewProperty {All              = 0xFF};

#ifdef __USE_OPEN_CV__
    void showImage(const cv::Mat &image, const QString &title=QString());
#endif
    void showImage(const QImage &image, const QString &title=QString());
    void showImage(const QPixmap &image, const QString &title=QString());

    void setRectSize(QSize size);
    QRect rectSize() const;

    void setMask(const QImage &mask);

    void setFeatures(uint layer, const QList<Feature> &features);
    void clearAllLayers();

    unsigned int propertyView() const;
    void setPropertyView(unsigned int propertyView);


public slots:
    void setView(const QRect &viewRect);
    void zoomIn();
    void zoomOut();
    void zoomFit();

protected:
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void setZoomLevel(int step);

signals:
    void viewMoved(const QRect &viewRect);
    void rectDrawed(const QRect &viewRect);

private slots:

private:
    QGraphicsScene *_scene;
    double _currentZoomLevel;
    QPointF _startPos;
    unsigned int _propertyView;

    QGraphicsPixmapItem *_pixmapItem;
    QGraphicsPixmapItem *_maskItem;
    QGraphicsSimpleTextItem *_titleItem;
    QGraphicsRectItem *_rectItem;
    QRect _rect;

    QMap<uint, QList<FeatureItem *> > _featureItems;
};

#endif // IMAGEVIEW_H
