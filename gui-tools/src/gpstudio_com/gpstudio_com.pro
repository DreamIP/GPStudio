QT       += core

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
    flowpackage.cpp

HEADERS  += \
    cameraio.h \
    camerausb.h \
    camerainfo.h \
    cameraudp.h \
    gpstudio_com_common.h \
    cameracom.h \
    flowcom.h \
    flowpackage.h

# libusb
win32: LIBS += -L$$PWD/../../thirdparts/libusb-1.0/
android {
    LIBS += -L$$PWD/../../../libusb/android/libs/armeabi-v7a/
    CONFIG += android_install unversioned_soname android_deployment_settings
    LIBS += -lusb1.0
    ANDROID_EXTRA_LIBS += $$PWD/../../../libusb/android/obj/local/armeabi-v7a/libusb1.0.so
}
!android: LIBS += -lusb-1.0

LIBS += -lusb-1.0
