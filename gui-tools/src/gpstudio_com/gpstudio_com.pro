QT       += core network

TARGET = gpstudio_com
TEMPLATE = lib

OUT_PWD = ../gpstudio_com/
equals(DISTRIB, 1) {
    win32 {
        DESTDIR = ../../bin-win64-qt5/
        LIBS += -L../../bin-win64-qt5/
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
}

DEFINES += GPSTUDIO_COM_EXPORT_LIB

QMAKE_CFLAGS_RELEASE = -O2

SOURCES += cameraio.cpp \
    camerausb.cpp \
    camerainfo.cpp \
    cameraudp.cpp \
    cameracom.cpp \
    flowcom.cpp \
    flowpackage.cpp \
    camerainfochannel.cpp

HEADERS  += \
    cameraio.h \
    camerausb.h \
    camerainfo.h \
    cameraudp.h \
    gpstudio_com_common.h \
    cameracom.h \
    flowcom.h \
    flowpackage.h \
    camerainfochannel.h

use_ft3xx {
    SOURCES += camerausb_ftd3xx.cpp
    HEADERS += camerausb_ftd3xx.h
}

include(gpstudio_com.pri)
