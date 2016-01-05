QT       += core gui xml script scripttools

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gpnode_gui
TEMPLATE = app

OUT_PWD = ../gpnode/
equals(DISTRIB, 1) {
    win32 {
        DESTDIR = ../../bin-win/
        LIBS += -L../../bin-win/
    }
    linux-g++-32 {
		greaterThan(QT_MAJOR_VERSION, 4) {
			DESTDIR = ../../bin-linux32-qt5/
			LIBS += -L../../bin-linux32-qt5/
		} else {
			DESTDIR = ../../bin-linux32-qt4/
			LIBS += -L../../bin-linux32-qt4/
        }
    }
    linux-g++-64 {
		greaterThan(QT_MAJOR_VERSION, 4) {
			DESTDIR = ../../bin-linux64-qt5/
			LIBS += -L../../bin-linux64-qt5/
		} else {
			DESTDIR = ../../bin-linux64-qt4/
			LIBS += -L../../bin-linux64-qt4/
        }
    }
} else {
    DESTDIR = ../../../bin/
    LIBS += -L../../../bin/
}

QMAKE_CFLAGS_RELEASE = -O2

SOURCES += main.cpp\
        mainwindow.cpp \
    confignodedialog.cpp

HEADERS  += mainwindow.h \
    confignodedialog.h

FORMS    += mainwindow.ui \
    confignodedialog.ui

RESOURCES  += icons.qrc

# relative path for shared library in the same directory
LIBS += -Wl,-R.

# gpstudio_lib lib
INCLUDEPATH += $$PWD/../gpstudio_lib
DEPENDPATH += $$PWD/../gpstudio_lib
LIBS += -lgpstudio_lib

# gpstudio_com lib
INCLUDEPATH += $$PWD/../gpstudio_com
DEPENDPATH += $$PWD/../gpstudio_com
LIBS += -lgpstudio_com

# gpstudio_gui lib
INCLUDEPATH += $$PWD/../gpstudio_gui
DEPENDPATH += $$PWD/../gpstudio_gui
LIBS += -lgpstudio_gui

use_open_cv {
    win32 {
        INCLUDEPATH += "E:\opencv\include"
        LIBS += -L"E:\opencv\x86\mingw\bin" -lopencv_core249 -lopencv_highgui249 -lopencv_imgproc249
    }
    unix {
        LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc
    }
}


win32: LIBS += -L$$PWD/../../thirdparts/libusb-1.0/
LIBS += -lusb-1.0
