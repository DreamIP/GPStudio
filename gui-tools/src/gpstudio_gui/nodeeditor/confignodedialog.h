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

#ifndef CONFIGNODEDIALOG_H
#define CONFIGNODEDIALOG_H

#include "gpstudio_gui_common.h"

#include <QDialog>
#include <QComboBox>
#include <QLayout>
#include <QScrollArea>

#include "gpnodeproject.h"

class GPSTUDIO_GUI_EXPORT ConfigNodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigNodeDialog(QWidget *parent = 0);
    ~ConfigNodeDialog();

    GPNodeProject *project() const;
    void setProject(GPNodeProject *project);

    QString boardName();
    QStringList iosName();

private slots:
    void selectBoard(const QString &boardName);

private:
    void setupWidgets();
    GPNodeProject *_project;

    QComboBox *_boardComboBox;
    QScrollArea *_iosWidget;

    QLayout *_iosLayout;
};

#endif // CONFIGNODEDIALOG_H
