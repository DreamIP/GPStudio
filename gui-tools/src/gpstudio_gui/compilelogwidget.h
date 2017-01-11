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

#ifndef COMPILELOGWIDGET_H
#define COMPILELOGWIDGET_H

#include "gpstudio_gui_common.h"

#include <QWidget>
#include <QTextEdit>
#include <QProcess>
#include <QStringList>
#include <QDateTime>

#include "nodeeditor/gpnodeproject.h"

class GPSTUDIO_GUI_EXPORT CompileLogWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CompileLogWidget(QWidget *parent = 0);

    GPNodeProject *project() const;
    void setProject(GPNodeProject *project);

    bool checkProgramm(const QString &programm);
    bool checkPhp();

    static QProcessEnvironment getEnv();

protected:
    void launch(const QString &cmd, const QStringList &args);
    void checkAction();

signals:
    void generateAvailable(bool available);
    void cleanAvailable(bool available);
    void compileAvailable(bool available);
    void sendAvailable(bool available);
    void runAvailable(bool available);
    void stopAvailable(bool available);

public slots:
    void readProcess();
    void appendLog(const QString &log);

    void launchClean();
    void launchGenerate();
    void launchCompile();
    void launchSend();
    void launchView();
    void launchAll();

    void stopAll();
    void clear();

protected slots:
    void exitProcess();
    void errorProcess();
    void updatePath(QString path);

private:
    void setupWidgets();

    bool _allRequest;
    enum Action {
        ActionNone,
        ActionClean,
        ActionGenerate,
        ActionCompile,
        ActionSend,
        ActionView
    };
    Action _currentAction;

    QTextEdit *_textWidget;

    QString _program;
    QStringList _arguments;
    QProcess *_process;
    QDateTime _startProcessDate;

    GPNodeProject *_project;
};

#endif // COMPILELOGWIDGET_H
