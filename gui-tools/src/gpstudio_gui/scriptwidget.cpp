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

#include "scriptwidget.h"

#include <QDebug>
#include <QBoxLayout>

#include <QScriptEngine>

ScriptWidget::ScriptWidget(QWidget *parent) :
    QWidget(parent)
{
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    _textEdit = new QPlainTextEdit();
    _textEdit->setReadOnly(true);
    layout->addWidget(_textEdit);

    QLayout *layout2 = new QHBoxLayout();
    layout2->setContentsMargins(0,0,0,0);
    layout2->setSpacing(6);
    _lineEdit = new ScriptLineEdit();
    connect(_lineEdit, SIGNAL(returnPressed()), this, SLOT(exec()));
    connect(_lineEdit, SIGNAL(up()), this, SLOT(up()));
    connect(_lineEdit, SIGNAL(down()), this, SLOT(down()));
    layout2->addWidget(_lineEdit);

    _execButton = new QPushButton("exec");
    connect(_execButton, SIGNAL(clicked()), this, SLOT(exec()));
    layout2->addWidget(_execButton);

    layout->addItem(layout2);
    setLayout(layout);

    connect(&ScriptEngine::getEngine(), SIGNAL(echoAppended(QString)), this, SLOT(appendEcho(QString)));

    _histoIndex = -1;
}

ScriptWidget::~ScriptWidget()
{
}

void ScriptWidget::setRootProperty(const Property *property)
{
    _lineEdit->setRootProperty(property);
}

void ScriptWidget::exec()
{
    _textEdit->appendPlainText("> "+_lineEdit->text());
    QScriptValue value = ScriptEngine::getEngine().eval(_lineEdit->text());
    if(value.isError())
    {
        _textEdit->appendHtml("<pre style=\"color:red\">"+value.toString()/*.toHtmlEscaped()*/+"</pre>");
    }
    else
    {
        if(!value.isUndefined())
            _textEdit->appendPlainText(value.toString());
    }

    _histo.append(_lineEdit->text());
    _histoIndex = _histo.count();
    _lineEdit->clear();
}

void ScriptWidget::clearHisto()
{
    _textEdit->clear();
}

void ScriptWidget::appendEcho(QString text)
{
    _textEdit->appendPlainText(text);
}

void ScriptWidget::up()
{
    if(_histoIndex<=0) return;
    _histoIndex--;
    _lineEdit->setText(_histo[_histoIndex]);
}

void ScriptWidget::down()
{
    if(_histo.empty()) return;
    if(_histoIndex>=_histo.count()-1) return;
    _histoIndex++;
    _lineEdit->setText(_histo[_histoIndex]);
}


QSize ScriptWidget::sizeHint() const
{
    return QSize(300,100);
}
