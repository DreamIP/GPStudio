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

#ifndef PATHEDITWIDGET_H
#define PATHEDITWIDGET_H

#include "gpstudio_gui_common.h"

#include <QLabel>
#include <QLineEdit>
#include <QProcess>
#include <QToolButton>
#include <QWidget>
#include <QFileSystemModel>

class GPSTUDIO_GUI_EXPORT PathEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PathEditWidget(QWidget *parent = 0);

    QString programm() const;
    void setProgramm(const QString &programm);

    QString path() const;
    void setPath(const QString &path);
    bool isValid() const;

    void setEnv(QProcessEnvironment env);
    QString findExecutable(const QString &executableName, const QStringList &paths = QStringList());

signals:

private slots:
    void buttonClicked();
    void checkProgramm();
    void checkLineEdit();

protected:
    QString _programm;
    QString _path;

    void setupWidgets();
    QLineEdit *_pathLineEdit;
    QToolButton *_button;
    QLabel *_labelVersion;

    QFileSystemModel *_filesModel;
    QProcessEnvironment _env;
};

#endif // PATHEDITWIDGET_H
