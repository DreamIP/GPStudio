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

#ifndef PDFVIEWER_H
#define PDFVIEWER_H

#include "gpstudio_gui_common.h"

#include <QGraphicsView>

namespace Poppler {
    class Document;
}

class GPSTUDIO_GUI_EXPORT PdfViewer : public QGraphicsView
{
    Q_OBJECT
public:
    explicit PdfViewer(QWidget *parent = 0, QString file = QString());
    virtual ~PdfViewer();

    static void showDocument(const QString &file);

public slots:
    void showPage(int page);
    void nextPage();
    void previousPage();
    void zoomIn();
    void zoomOut();
    void zoomFit();

protected:
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void setZoomLevel(int step);
    void closeEvent(QCloseEvent *event);

private:
    int _currentPage;
    Poppler::Document *_doc;
};

#endif // PDFVIEWER_H
