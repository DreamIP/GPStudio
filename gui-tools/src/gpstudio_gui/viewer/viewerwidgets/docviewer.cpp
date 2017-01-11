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

#include "docviewer.h"

#include <QLayout>
#include <QDebug>
#include <QFileInfo>
#include <QKeyEvent>
#include <QPlainTextEdit>

DocViewer::DocViewer(QWidget *parent) : QWidget(parent)
{
}

void DocViewer::loadFileCode(QString file)
{
    QFileInfo info(file);

    if(layout())
    {
        while (layout()->count())
        {
            delete layout()->takeAt(0)->widget();
        }
        delete layout();
    }

    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    if((QStringList()<<"html"<<"htm").contains(info.suffix()))
    {
        QPlainTextEdit *htmlViewer = new QPlainTextEdit();
        layout->addWidget(htmlViewer);

        QFile fileIO(file);
        if(fileIO.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            htmlViewer->appendHtml(fileIO.readAll());
            fileIO.close();
        }
    }
    else if((QStringList()<<"txt"<<"").contains(info.suffix()))
    {
        QPlainTextEdit *textViewer = new QPlainTextEdit();
        layout->addWidget(textViewer);

        QFile fileIO(file);
        if(fileIO.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            textViewer->appendPlainText(fileIO.readAll());
            fileIO.close();
        }
    }
    else if((QStringList()<<"pdf").contains(info.suffix()))
    {
        //
    }

    setLayout(layout);
}
