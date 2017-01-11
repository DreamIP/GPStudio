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

#ifndef FLOWVIEWERWIDGET_H
#define FLOWVIEWERWIDGET_H

#include "gpstudio_gui_common.h"

#include <QWidget>
#include <QSharedPointer>
#include <QMutex>
#include <QMutexLocker>

#include <QLabel>
#include <QComboBox>

#include "camera/flowviewerinterface.h"
#include "abstractviewer.h"

class GPSTUDIO_GUI_EXPORT FlowViewerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FlowViewerWidget(const QSharedPointer<FlowViewerInterface> &flowViewerInterface);
    FlowViewerWidget(FlowViewerInterface *flowViewerInterface);
    FlowViewerWidget(const FlowViewerWidget &other);
    ~FlowViewerWidget();

    FlowViewerWidget &operator= (const FlowViewerWidget &other);

signals:

protected slots:
    void changeType();
    void changeViewer();
    void dataReceive();

protected:
    void setupWidgets();
    void setupViewer(AbstractViewer *viewer);

    void setFlowViewerInterface(const QSharedPointer<FlowViewerInterface> &flowViewerInterface);

protected:
    QSharedPointer<FlowViewerInterface> _flowViewerInterface;
    QMutex _viewerMutex;

    AbstractViewer *_viewer;

    QLayout *_statusLayout;
    QLabel *_statusLabel;
    QComboBox *_typeComboBox;
};

#endif // FLOWVIEWERWIDGET_H
