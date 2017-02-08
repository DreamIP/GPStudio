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

#ifndef SCRIPTWIDGET_H
#define SCRIPTWIDGET_H

#include "gpstudio_gui_common.h"

#include <QWidget>

#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>

#include "scriptlineedit.h"
#include "scriptengine/scriptengine.h"

class GPSTUDIO_GUI_EXPORT ScriptWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ScriptWidget(QWidget *parent = 0);
    virtual ~ScriptWidget();

    void setRootProperty(const Property *property);

signals:

public slots:
    void exec();
    void clearHisto();
    void appendEcho(QString text);

private slots:
    void up();
    void down();

private:
    QPlainTextEdit *_textEdit;
    ScriptLineEdit *_lineEdit;
    QPushButton *_execButton;

    QStringList _histo;
    int _histoIndex;

    // QWidget interface
public:
    QSize sizeHint() const;
};

#endif // SCRIPTWIDGET_H
