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

#include "gpviewer/viewerwindow.h"
#include <QApplication>

#include <QDebug>
#include <QDir>
#include <QSplashScreen>
#include <QTimer>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ViewerWindow w(a.arguments());

    /*QSplashScreen splashScreen(&w, QPixmap("../gui-tools/src/img/GPS_SS.png"));
    QLabel *label = new QLabel("loading...loading...loading...loading...",&splashScreen);
    label->move(200,360);
    splashScreen.show();
    splashScreen.showMessage("Loaded modules");
    a.processEvents();

    QTimer::singleShot(2000, &splashScreen, SLOT(close()));
    QTimer::singleShot(2000, &w, SLOT(show()));

    //splashScreen.finish(&w);*/

    w.showMaximized();

    return a.exec();
}
