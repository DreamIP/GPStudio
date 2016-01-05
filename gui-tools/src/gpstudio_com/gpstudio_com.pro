QT       += core

TARGET = gpstudio_com
TEMPLATE = lib

OUT_PWD = ../gpstudio_com/
equals(DISTRIB, 1) {
    win32 {
        DESTDIR = ../../bin-win/
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
    flowdata.cpp \
    cameracom.cpp \
    flowcom.cpp

HEADERS  += \
    cameraio.h \
    camerausb.h \
    camerainfo.h \
    cameraudp.h \
    flowdata.h \
    gpstudio_com_common.h \
    cameracom.h \
    flowcom.h

win32: LIBS += -L$$PWD/../../thirdparts/libusb-1.0/

LIBS += -lusb-1.0
