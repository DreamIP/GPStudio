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

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NodeEditorWindows *nodeEditorWindows = NULL;

    if(a.arguments().size()>1)
    {
        QString nodeFileName = a.arguments()[1];
        if(QFile::exists(nodeFileName))
        {
            ModelNode *node = ModelNode::readFromFile(nodeFileName);
            nodeEditorWindows = new NodeEditorWindows(NULL, node);
        }
    }

    if(!nodeEditorWindows)
        nodeEditorWindows = new NodeEditorWindows(NULL);

    nodeEditorWindows->show();

    return a.exec();
}
