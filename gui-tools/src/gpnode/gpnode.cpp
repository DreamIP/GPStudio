/****************************************************************************
** Copyright (C) 2016 Dream IP
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

#include "nodeeditor/nodeeditorwindows.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GPNodeProject *nodeProject = new GPNodeProject();

    if(a.arguments().size()>1)
    {
        QString nodeFileName = a.arguments()[1];
        if(!nodeProject->openProject(nodeFileName))
            qDebug()<<"Cannot open file"<<nodeFileName;
    }

    NodeEditorWindows nodeEditorWindows(NULL, nodeProject);
    nodeEditorWindows.show();

    return a.exec();
}
