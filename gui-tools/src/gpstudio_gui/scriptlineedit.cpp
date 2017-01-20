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

#include "scriptlineedit.h"

#include <QKeyEvent>
#include <QAbstractItemView>
#include <QDebug>

#include "scriptengine/scriptengine.h"

#include <camera/camera.h>
#include "itemmodel/propertycompleter.h"

ScriptLineEdit::ScriptLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    _completer = NULL;
}

ScriptLineEdit::~ScriptLineEdit()
{
}

void ScriptLineEdit::setRootProperty(const Property *property)
{
    _completer = new PropertyCompleter(this, property);
    _completer->setCompletionMode(QCompleter::PopupCompletion);
    setCompleter(_completer);
    _completer->setWidget(this);
}

void ScriptLineEdit::keyPressEvent(QKeyEvent *event)
{
    if (_completer && _completer->popup()->isVisible())
    {
        switch (event->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            event->ignore();
            return; // let the completer do default behavior
        default:
           break;
       }
    }

    if(event->key() == Qt::Key_Space && event->modifiers() | Qt::ControlModifier)
        completer()->complete();
    if(event->key() == Qt::Key_Up)
        emit up();
    if(event->key() == Qt::Key_Down)
        emit down();

    QLineEdit::keyPressEvent(event);
}

void ScriptLineEdit::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
}
