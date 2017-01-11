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

#include "pathssettingspage.h"

#include <QFormLayout>
#include <QLabel>
#include <QSettings>

#include "compilelogwidget.h"

PathsSettingsPage::PathsSettingsPage(QWidget *parent) : QWidget(parent)
{
    setupWidgets();

    QSettings settings("GPStudio", "gpnode");
    settings.beginGroup("paths");
    _phpPathEdit->setPath(settings.value("php", "").toString());
    settings.endGroup();
}

void PathsSettingsPage::saveSettings()
{
    QSettings settings("GPStudio", "gpnode");
    settings.beginGroup("paths");
    settings.setValue("php", _phpPathEdit->path());
    settings.setValue("make", _makePathEdit->path());
    settings.setValue("quartus", _quartusPathEdit->path());
    settings.endGroup();
}

void PathsSettingsPage::setupWidgets()
{
    QFormLayout *layout = new QFormLayout(this);

    layout->addWidget(new QLabel("Paths for external tools, leave empty to take defaut path system"));

    _phpPathEdit = new PathEditWidget();
    _phpPathEdit->setProgramm("php");
    _phpPathEdit->setEnv(CompileLogWidget::getEnv());
    layout->addRow("php path (php exe)", _phpPathEdit);

    _makePathEdit = new PathEditWidget();
    _makePathEdit->setProgramm("make");
    _makePathEdit->setEnv(CompileLogWidget::getEnv());
    layout->addRow("make path (make exe)", _makePathEdit);

    _quartusPathEdit = new PathEditWidget();
    _quartusPathEdit->setProgramm("quartus_sh");
    _quartusPathEdit->setEnv(CompileLogWidget::getEnv());
    layout->addRow("quartus path (quartus exe)", _quartusPathEdit);

    setLayout(layout);
}
