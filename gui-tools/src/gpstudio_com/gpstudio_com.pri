
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
use_ft3xx {
    win32 {
        LIBS += -L$$PWD/../../thirdparts/ftd3xx/win32/
        FTD3LIB = $$PWD/../../thirdparts/ftd3xx/win32/FTD3XX.dll
        LIBS += -lftd3xx
    }
    linux-g++ {
        contains(QMAKE_TARGET.arch, x86_64) {
            LIBS += -L$$PWD/../../thirdparts/ftd3xx/linux64/
            FTD3LIB = $$PWD/../../thirdparts/ftd3xx/linux64/libftd3xx.so
        } else {
            LIBS += -L$$PWD/../../thirdparts/ftd3xx/linux32/
            FTD3LIB = $$PWD/../../thirdparts/ftd3xx/linux32/libftd3xx.so
        }
        LIBS += -lftd3xx
    }
    # copy dll to bin dir
    copylibftd3.commands = $(COPY_DIR) $$FTD3LIB $$DESTDIR
    first.depends = $(first) copylibftd3
    export(first.depends)
    export(copylibftd3.commands)
    QMAKE_EXTRA_TARGETS += first copylibftd3
}
