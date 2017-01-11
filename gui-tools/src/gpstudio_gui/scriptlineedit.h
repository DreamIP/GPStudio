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

#ifndef SCRIPTLINEEDIT_H
#define SCRIPTLINEEDIT_H

#include "gpstudio_gui_common.h"

#include <QLineEdit>
#include <QCompleter>

class Property;

class GPSTUDIO_GUI_EXPORT ScriptLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit ScriptLineEdit(QWidget *parent = 0);
    virtual ~ScriptLineEdit();

    void setRootProperty(const Property *property);

signals:
    void up();
    void down();

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event);
    void focusInEvent(QFocusEvent *e);

public slots:

private:
    QCompleter* _completer;
};

#endif // SCRIPTLINEEDIT_H
