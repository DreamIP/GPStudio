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

#include "nodesettings.h"

#include "pathssettingspage.h"

#include <QLayout>
#include <QTabWidget>

NodeSettings::NodeSettings(QWidget *parent) : QDialog(parent)
{
    setupWidgets();
}

void NodeSettings::setupWidgets()
{
    QLayout *layout = new QVBoxLayout();

    QTabWidget *tabWidget = new QTabWidget(this);

    _pathPage = new PathsSettingsPage();
    tabWidget->addTab(_pathPage, "paths");
    layout->addWidget(tabWidget);

    setLayout(layout);
}

void NodeSettings::closeEvent(QCloseEvent *)
{
    _pathPage->saveSettings();
}
