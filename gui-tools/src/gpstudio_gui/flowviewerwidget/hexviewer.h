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

#ifndef HEXVIEWER_H
#define HEXVIEWER_H

#include "gpstudio_gui_common.h"

#include "abstractviewer.h"

#include "../../thirdparts/hexedit/qhexedit.h"

class GPSTUDIO_GUI_EXPORT HexViewer : public AbstractViewer
{
    Q_OBJECT
public:
    HexViewer(FlowViewerInterface *flowViewerInterface);
    ~HexViewer();

protected slots:
    void showFlowConnection(int flowId);

protected:
    virtual void setupWidgets();

    QHexEdit *_widget;
};

#endif // HEXVIEWER_H
