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

#include "compilelogwidget.h"

#include <QLayout>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QMessageBox>
#include <QSettings>
#include <QCheckBox>
//#include <QStandardPaths>
#include <QMessageBox>

CompileLogWidget::CompileLogWidget(QWidget *parent) : QWidget(parent)
{
    setupWidgets();
    _process = NULL;
    _project = NULL;
    _allRequest = false;
    _currentAction = ActionNone;

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
    connect(_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(exitProcess()));
    connect(_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(errorProcess()));

    checkAction();

    // environement variables
    _process->setProcessEnvironment(getEnv());

    // _textWidget->clear();
    _program = cmd;
    _arguments = args;
    _startProcessDate = QDateTime::currentDateTime();
    _process->setWorkingDirectory(QFileInfo(_project->path()).path());
    _process->start(cmd, args);

    appendLog(QString("<hr><span style=\"color: blue;\">process '%1' start at %2...</span></hr>")
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

        /*dataRead = _process->readAllStandardError();
        if(!dataRead.isEmpty())
            html.append("<p><span style=\"color: red\">"+QString::fromLocal8Bit(dataRead)+"</span></p>");*/

        appendLog(html);
    }
}

void CompileLogWidget::launchClean()
{
    QString program = "make";
    QStringList arguments;
    arguments << "clean";

    _currentAction = ActionClean;
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
    QString program = QCoreApplication::applicationDirPath()+"/gpnode";
#endif
    arguments << "generate" << "-o" << "build";

    _currentAction = ActionGenerate;
    launch(program, arguments);
}

void CompileLogWidget::launchCompile()
{
    QString program = "make";
    QStringList arguments;
    arguments << "compile";

    _currentAction = ActionCompile;
    launch(program, arguments);
}

void CompileLogWidget::launchSend()
{
    QString program = "make";
    QStringList arguments;
    arguments << "send";

    _currentAction = ActionSend;
    launch(program, arguments);
}

void CompileLogWidget::launchView()
{
    QString program = "make";
    QStringList arguments;
    arguments << "view";

    _currentAction = ActionView;
    launch(program, arguments);
}

void CompileLogWidget::launchAll()
{
    _allRequest = true;
    launchGenerate();
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
    int exitCode = 1;

    if(_process)
    {
        exitCode = _process->exitCode();
        if(exitCode==0)
            appendLog(QString("<p><span style=\"color: blue;\">process '%1' exit with code %2 at %3, elapsed time: %4s</span></p>")
                  .arg(_program + " " + _arguments.join(" "))
                  .arg(exitCode)
                  .arg(QDateTime::currentDateTime().toString())
                  .arg((QDateTime::currentMSecsSinceEpoch() - _startProcessDate.toMSecsSinceEpoch())/1000));
        else
        {
            appendLog(QString("<p><span style=\"color: red;\">%1</span></p>").arg(QString::fromLocal8Bit(_process->readAllStandardError())));
            appendLog(QString("<p><span style=\"color: red;\">failed exit code:%1 %2</span></p>").arg(exitCode).arg(_process->errorString()));
        }
        _process->deleteLater();
        _process = NULL;
    }

    if(exitCode != 0)
        _allRequest = false;

    if(_allRequest)
    {
        switch(_currentAction)
        {
        case CompileLogWidget::ActionNone:
        case CompileLogWidget::ActionClean:
        case CompileLogWidget::ActionView:
            _allRequest = false;
            checkAction();
            break;
        case CompileLogWidget::ActionGenerate:
            launchCompile();
            break;
        case CompileLogWidget::ActionCompile:
            launchSend();
            break;
        case CompileLogWidget::ActionSend:
            launchView();
            break;
        }
    }
    else
    {
        QMessageBox messageBox(this);
        QString title, message;
        bool error;
        checkAction();
        switch(_currentAction)
        {
        case CompileLogWidget::ActionNone:
        case CompileLogWidget::ActionView:
            break;
        case CompileLogWidget::ActionClean:
            if(exitCode==0)
            {
                title = "Clean successfully";
                message = "Cleaning of built files terminated successfully.";
                error = false;
            }
            else
            {
                title = "Clean failed...";
                message = "Cleaning step failed.\nPlease check your make exe path or delete the Makefile file if it is corrupted and launch generate.";
                error = true;
            }
            break;
        case CompileLogWidget::ActionGenerate:
            if(exitCode==0)
            {
                title = "Generate successfully";
                message = "Generation of project terminated successfully.";
                error = false;
            }
            else
            {
                title = "Generate failed...";
                message = "Generate step failed.";
                error = true;
            }
            break;
        case CompileLogWidget::ActionCompile:
            if(exitCode==0)
            {
                title = "Synthesys successfully";
                message = "Synthesys of project terminated successfully.";
                error = false;
            }
            else
            {
                title = "Synthesys failed...";
                message = "Synthesys step failed.\nPlease check your quartus exe path or re-generate your project.";
                error = true;
            }
            break;
        case CompileLogWidget::ActionSend:
            if(exitCode==0)
            {
                title = "Send successfully";
                message = "Node programmation terminated successfully.";
                error = false;
            }
            else
            {
                title = "Send failed...";
                message = "Send step failed.\nPlease check your quartus exe or your camera connection. If you use it for the first time on windows, launch the Altera graphical tool to programm yor node";
                error = true;
            }
            break;
        }
        messageBox.setWindowTitle(title);
        messageBox.setText(message);
        if(error)
            messageBox.setIconPixmap(QMessageBox::standardIcon(QMessageBox::Critical));
        else
            messageBox.setIconPixmap(QMessageBox::standardIcon(QMessageBox::Information));
        messageBox.addButton(QMessageBox::Ok);
        QCheckBox dontShowCheckBox("don't show this message again");
        messageBox.addButton(&dontShowCheckBox, QMessageBox::ActionRole);

        QSettings settings("GPStudio", "gpnode");
        settings.beginGroup("compilelog");
        if(settings.value("showmessage", true).toBool())
        {
            if(messageBox.exec() == 0)
                settings.setValue("showmessage", false);
        }
        settings.endGroup();
        emit messageSended(message);
    }
}

void CompileLogWidget::errorProcess()
{
    if(_process)
        appendLog(QString("<p><span style=\"color: red;\">failed exit code:%1 %2</span></p>").arg(_process->exitCode()).arg(_process->errorString()));
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

/*bool CompileLogWidget::checkProgramm(const QString &programm)
{
    QString programmPath = programm;
    QProcess *process = new QProcess(this);
    QProcessEnvironment env = getEnv();
    process->setProcessEnvironment(env);
    QString path = QStandardPaths::findExecutable(programm, env.value("PATH").split(QDir::listSeparator()));
    if(!path.isEmpty())
        programmPath = path;
    QStringList args;
    args.append("-v");
    process->start(programmPath, args);
    process->waitForFinished(3000);
    QProcess::ExitStatus exitStatus = process->exitStatus();
    delete process;
    return (exitStatus==QProcess::NormalExit);
}

bool CompileLogWidget::checkPhp()
{
    return checkProgramm("php");
}*/

QProcessEnvironment CompileLogWidget::getEnv()
{
#if defined(Q_OS_WIN)
    char listSep = ';';
#else
    char listSep = ':';
#endif

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString path;
    path += QDir::toNativeSeparators(QCoreApplication::applicationDirPath()) + listSep;

    // php path from settings
    QSettings settings("GPStudio", "gpnode");
    settings.beginGroup("paths");

    QString phpPath = settings.value("php", "").toString();
    if(!phpPath.isEmpty())
        path += phpPath + listSep;

    QString makePath = settings.value("make", "").toString();
    if(!makePath.isEmpty())
        path += makePath + listSep;

    QString quartusPath = settings.value("quartus", "").toString();
    if(!quartusPath.isEmpty())
        path += quartusPath + listSep;

    settings.endGroup();

    path += env.value("PATH");
    env.insert("PATH", path);
    return env;
}
