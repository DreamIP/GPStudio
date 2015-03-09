QT       += core xml script

TARGET = gpstudio_lib
TEMPLATE = lib

DEFINES += GPSTUDIO_LIB_EXPORT_LIB

QMAKE_CFLAGS_RELEASE = -O2

INCLUDEPATH += $$PWD

HEADERS += gpstudio_lib_common.h \
    lib_parser/processlib.h \
    lib_parser/processlibreader.h \
    lib_parser/lib.h \
    lib_parser/boardlib.h \
    model/node.h \
    model/block.h \
    model/io.h \
    model/process.h \
    model/file.h \
    model/param.h \
    model/flow.h \
    model/clock.h \
    model/reset.h \
    model/port.h \
    model/pin.h \
    model/parambitfield.h \
    model/attribute.h \
    camera.h \
    property.h \
    cameraregister.h \
    propertiesmap.h \
    model/blockpropertyenum.h \
    model/blockproperty.h \
    propertyclass.h \
    cameraregistersmap.h

SOURCES += \
    lib_parser/processlib.cpp \
    lib_parser/processlibreader.cpp \
    lib_parser/lib.cpp \
    lib_parser/boardlib.cpp \
    model/node.cpp \
    model/block.cpp \
    model/io.cpp \
    model/process.cpp \
    model/file.cpp \
    model/param.cpp \
    model/flow.cpp \
    model/clock.cpp \
    model/reset.cpp \
    model/port.cpp \
    model/pin.cpp \
    model/parambitfield.cpp \
    model/attribute.cpp \
    camera.cpp \
    property.cpp \
    cameraregister.cpp \
    propertiesmap.cpp \
    model/blockpropertyenum.cpp \
    model/blockproperty.cpp \
    propertyclass.cpp \
    cameraregistersmap.cpp

# gpstudio_com lib
INCLUDEPATH += $$PWD/../gpstudio_com
