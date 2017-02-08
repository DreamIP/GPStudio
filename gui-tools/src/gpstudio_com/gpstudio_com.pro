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
    camerausb_ftd3xx.cpp \
    camerainfo.cpp \
    cameraudp.cpp \
    cameracom.cpp \
    flowcom.cpp \
    flowpackage.cpp \
    camerainfochannel.cpp

HEADERS  += \
    cameraio.h \
    camerausb.h \
    camerausb_ftd3xx.h \
    camerainfo.h \
    cameraudp.h \
    gpstudio_com_common.h \
    cameracom.h \
    flowcom.h \
    flowpackage.h \
    camerainfochannel.h

linux-g++:QMAKE_TARGET.arch = $$QMAKE_HOST.arch
linux-g++-32:QMAKE_TARGET.arch = x86
linux-g++-64:QMAKE_TARGET.arch = x86_64

# libusb
win32 {
    LIBS += -L$$PWD/../../thirdparts/libusb-1.0/

    # copy dll to bin dir
    copylibusb.commands = $(COPY_DIR) $$PWD/../../thirdparts/libusb-1.0/libusb-1.0.dll $$DESTDIR
    first.depends = $(first) copylibusb
    export(first.depends)
    export(copylibusb.commands)
    QMAKE_EXTRA_TARGETS += first copylibusb
}
android {
    LIBS += -L$$PWD/../../../libusb/android/libs/armeabi-v7a/
    CONFIG += android_install unversioned_soname android_deployment_settings
    LIBS += -lusb1.0
    ANDROID_EXTRA_LIBS += $$PWD/../../../libusb/android/obj/local/armeabi-v7a/libusb1.0.so
}
!android: LIBS += -lusb-1.0

# lib ft3xx
win32 {
    LIBS += -L$$PWD/../../thirdparts/ftd3xx/win32/
    FTD3LIB = $$PWD/../../thirdparts/ftd3xx/win32/FTD3XX.dll
}
linux-g++ {
    contains(QMAKE_TARGET.arch, x86_64) {
        LIBS += -L$$PWD/../../thirdparts/ftd3xx/linux64/
        FTD3LIB = $$PWD/../../thirdparts/ftd3xx/linux64/libftd3xx.so
    } else {
        LIBS += -L$$PWD/../../thirdparts/ftd3xx/linux32/
        FTD3LIB = $$PWD/../../thirdparts/ftd3xx/linux32/libftd3xx.so
    }
}
LIBS += -lftd3xx
# copy dll to bin dir
copylibftd3.commands = $(COPY_DIR) $$FTD3LIB $$DESTDIR
first.depends = $(first) copylibftd3
export(first.depends)
export(copylibftd3.commands)
QMAKE_EXTRA_TARGETS += first copylibftd3
