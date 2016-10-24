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

#include "compilelogwidget.h"

#include <QLayout>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QMessageBox>

CompileLogWidget::CompileLogWidget(QWidget *parent) : QWidget(parent)
{
    setupWidgets();
    _process = NULL;
    _project = NULL;

    checkAction();
}

void CompileLogWidget::appendLog(const QString &log)
{
    _textWidget->append(log);
}

void CompileLogWidget::launch(const QString &cmd, const QStringList &args)
{
    _process = new QProcess(this);
    connect(_process, SIGNAL(readyRead()), this, SLOT(readProcess()));
    connect(_process, SIGNAL(readyRead()), this, SLOT(readProcess()));
    connect(_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(exitProcess()));
    connect(_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(errorProcess()));

    checkAction();

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("PATH", QCoreApplication::applicationDirPath()+":"+env.value("PATH"));
    _process->setProcessEnvironment(env);

    // _textWidget->clear();
    _program = cmd;
    _arguments = args;
    _startProcessDate = QDateTime::currentDateTime();
    _process->setWorkingDirectory(QFileInfo(_project->path()).path());
    _process->start(cmd, args);

    appendLog(QString("<p><span style=\"color: blue;\">process '%1' start at %2...</span></p>")
              .arg(_program + " " + _arguments.join(" "))
              .arg(QDateTime::currentDateTime().toString()));
}

void CompileLogWidget::checkAction()
{
    if(_project == NULL)
    {
        emit cleanAvailable(false);
        emit compileAvailable(false);
        emit generateAvailable(false);
        emit sendAvailable(false);
        emit runAvailable(false);
        emit stopAvailable(false);
        return;
    }

    if(_process)
    {
        emit cleanAvailable(false);
        emit compileAvailable(false);
        emit generateAvailable(false);
        emit sendAvailable(false);
        emit runAvailable(false);
        emit stopAvailable(true);
    }
    else
    {
        bool okMakefile = false;
        if(!_project->path().isEmpty())
            okMakefile = QFile::exists(QFileInfo(_project->path()).absolutePath()+"/Makefile");
        emit cleanAvailable(okMakefile);
        emit compileAvailable(okMakefile);

        bool okSof = false;
        if(!_project->path().isEmpty())
            okSof = QFile::exists(QFileInfo(_project->path()).absolutePath()+"/build/output_files/"+_project->name()+".sof");
        emit sendAvailable(okSof);
        emit runAvailable(okSof);
        emit generateAvailable(true);
        emit stopAvailable(false);
    }
}

void CompileLogWidget::readProcess()
{
/*#if defined(Q_OS_WIN)
    QRegExp colorReg("([^\r\n]*)");
    int capint = 0;
#else
    QRegExp colorReg("\\x001b\\[([0-9]+)m([^\r\n\\x001b]*)");
    int capint = 2;
#endif*/

    QString html;
    while(_process->canReadLine())
    {
        QByteArray dataRead = _process->readLine();
        QString stringRead = QString::fromLocal8Bit(dataRead);

        /*int pos = 0;
        while ((pos = colorReg.indexIn(stringRead, pos)) != -1)
        {
            QString colorCode = colorReg.cap(1);
            QString colorHTML = "black";

            if(colorCode == "33")
                colorHTML = "orange";
            if(colorCode == "31")
                colorHTML = "red";
            if(colorCode == "32")
                colorHTML = "green";

            html.append("<p><span style=\"color: "+colorHTML+"\">"+colorReg.cap(capint)+"</span></p>");

            pos += colorReg.matchedLength()+1;
        }*/

        stringRead.replace(QRegExp("\\x001b\\[([0-9]+)m"),"");
        html.append("<p>"+stringRead+"</p>");

        dataRead = _process->readAllStandardError();
        if(!dataRead.isEmpty())
            html.append("<p><span style=\"color: red\">"+QString::fromLocal8Bit(dataRead)+"</span></p>");

        appendLog(html);
    }
}

void CompileLogWidget::launchClean()
{
    QString program = "make";
    QStringList arguments;
    arguments << "clean";

    launch(program, arguments);
}

void CompileLogWidget::launchGenerate()
{
    if(!_project->node()->board())
        _project->configBoard();

    if(!_project->saveProject())
        return;

    QStringList arguments;
#if defined(Q_OS_WIN)
    QString program = "cmd";
    arguments << "/c" << "gpnode.bat";
#else
    QString program = "gpnode";
#endif
    arguments << "generate" << "-o" << "build";

    launch(program, arguments);
}

void CompileLogWidget::launchCompile()
{
    QString program = "make";
    QStringList arguments;
    arguments << "compile";

    launch(program, arguments);
}

void CompileLogWidget::launchSend()
{
    QString program = "make";
    QStringList arguments;
    arguments << "send";

    launch(program, arguments);
}

void CompileLogWidget::launchView()
{
    QString program = "make";
    QStringList arguments;
    arguments << "view";

    launch(program, arguments);
}

void CompileLogWidget::stopAll()
{
    if(_process)
        _process->terminate();
}

void CompileLogWidget::clear()
{
    _textWidget->clear();
}

void CompileLogWidget::exitProcess()
{
    appendLog(QString("<p><span style=\"color: blue;\">process '%1' exit with code %2 at %3, elapsed time: %4s</span></p>")
              .arg(_program + " " + _arguments.join(" "))
              .arg(_process->exitCode())
              .arg(QDateTime::currentDateTime().toString())
              .arg((QDateTime::currentMSecsSinceEpoch() - _startProcessDate.toMSecsSinceEpoch())/1000));

    _process->deleteLater();
    _process = NULL;

    checkAction();
}

void CompileLogWidget::errorProcess()
{
    if(_process)
    {
        appendLog(QString("<p><span style=\"color: red;\">failed exit code:%1 %2</span></p>").arg(_process->exitCode()).arg(_process->errorString()));
        _process->deleteLater();
        _process = NULL;
    }
    else
        appendLog("failed...");

    checkAction();
}

void CompileLogWidget::updatePath(QString path)
{
    Q_UNUSED(path)
    checkAction();
}

void CompileLogWidget::setupWidgets()
{
    if(layout())
        layout()->deleteLater();

    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    _textWidget = new QTextEdit();
    _textWidget->setReadOnly(true);
    _textWidget->document()->setDefaultStyleSheet("p{margin: 0;}");
    layout->addWidget(_textWidget);

    setLayout(layout);
}

GPNodeProject *CompileLogWidget::project() const
{
    return _project;
}

void CompileLogWidget::setProject(GPNodeProject *project)
{
    _project = project;
    connect(project, SIGNAL(nodePathChanged(QString)), this, SLOT(updatePath(QString)));
    checkAction();
}
