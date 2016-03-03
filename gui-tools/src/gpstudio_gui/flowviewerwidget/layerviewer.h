#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include "gpstudio_gui_common.h"

#include <QGraphicsView>
#include <vector>

#ifdef __USE_OPEN_CV__
#include <opencv2/core/core.hpp>
#endif

#include "abstractviewer.h"

/**
 * @brief The LayerViewer class is a QGraphicsView to which show a cv::Mat with capabilities of zoom and move into image.
 * Signal slot viewMoved and setView can be used to syncronize two viewers.
 */
class GPSTUDIO_GUI_EXPORT LayerViewer : public QGraphicsView, public AbstractViewer
{
    Q_OBJECT
public:
    explicit LayerViewer(QWidget *parent = 0);

    enum ViewProperty {All              = 0xFF};

#ifdef __USE_OPEN_CV__
    void showImage(const cv::Mat &image, const QString &title=QString());
#endif
    void showImage(const QImage &image, const QString &title=QString());
    void showImage(const QPixmap &image, const QString &title=QString());

    unsigned int propertyView() const;
    void setPropertyView(unsigned int propertyView);

    int flowNumber() const;
    void setFlowNumber(int flowNumber);

public slots:
    void setView(const QRect &viewRect);

protected:
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

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
    QGraphicsSimpleTextItem *_titleItem;

    int _flowNumber;
};

#endif // IMAGEVIEW_H
