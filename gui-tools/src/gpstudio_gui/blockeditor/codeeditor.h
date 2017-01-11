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

#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include "gpstudio_gui_common.h"

#include <QWidget>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>

class GPSTUDIO_GUI_EXPORT CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit CodeEditor(QWidget *parent = 0);
    QString textUnderMouse(QPoint pos) const;

public slots:
    void loadFileCode(QString file);

    void highlightCurrentLine();

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    bool event(QEvent *event);

    QSyntaxHighlighter *highlight;
};

#endif // CODEEDITOR_H
