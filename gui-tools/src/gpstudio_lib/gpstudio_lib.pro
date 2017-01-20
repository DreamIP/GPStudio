QT       += core xml script svg

TARGET = gpstudio_lib
TEMPLATE = lib

OUT_PWD = ../gpstudio_lib/
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
    LIBS += -L../../../bin/
}

DEFINES += GPSTUDIO_LIB_EXPORT_LIB

QMAKE_CFLAGS_RELEASE = -O2

INCLUDEPATH += $$PWD

HEADERS += gpstudio_lib_common.h \
    lib_parser/lib.h \
    lib_parser/blocklib.h \
    lib_parser/boardlib.h \
    lib_parser/ioboardlib.h \
    lib_parser/ioboardlibgroup.h \
    scriptengine/propertyclass.h \
    scriptengine/scriptengine.h \
    datawrapper/datawrapper.h \
    datawrapper/gradiantwrapper.h \
    datawrapper/harriswrapper.h \
    model/model_attribute.h \
    model/model_block.h \
    model/model_board.h \
    model/model_ciblock.h \
    model/model_clock.h \
    model/model_comdriver.h \
    model/model_comconnect.h \
    model/model_comparam.h \
    model/model_fiblock.h \
    model/model_file.h \
    model/model_flow.h \
    model/model_flowconnect.h \
    model/model_io.h \
    model/model_iocom.h \
    model/model_node.h \
    model/model_param.h \
    model/model_parambitfield.h \
    model/model_piblock.h \
    model/model_pin.h \
    model/model_port.h \
    model/model_process.h \
    model/model_reset.h \
    model/model_treeconnect.h \
    model/model_treeitem.h \
    model/model_property.h \
    model/model_propertyenum.h \
    model/model_componentpart.h \
    model/model_componentpartflow.h \
    model/model_componentpartproperty.h \
    model/model_gpviewer.h \
    model/model_viewer.h \
    model/model_viewerflow.h \
    camera/camera.h \
    camera/block.h \
    camera/property.h \
    camera/propertyenum.h \
    camera/flow.h \
    camera/flowmanager.h \
    camera/flowconnection.h \
    camera/flowviewerinterface.h \
    camera/registermanager.h \
    camera/registerbitfield.h \
    camera/register.h

SOURCES += \
    lib_parser/lib.cpp \
    lib_parser/blocklib.cpp \
    lib_parser/boardlib.cpp \
    lib_parser/ioboardlib.cpp \
    lib_parser/ioboardlibgroup.cpp \
    scriptengine/propertyclass.cpp \
    scriptengine/scriptengine.cpp \
    datawrapper/datawrapper.cpp \
    datawrapper/gradiantwrapper.cpp \
    datawrapper/harriswrapper.cpp \
    model/model_attribute.cpp \
    model/model_block.cpp \
    model/model_board.cpp \
    model/model_ciblock.cpp \
    model/model_clock.cpp \
    model/model_comdriver.cpp \
    model/model_comconnect.cpp \
    model/model_comparam.cpp \
    model/model_fiblock.cpp \
    model/model_file.cpp \
    model/model_flow.cpp \
    model/model_flowconnect.cpp \
    model/model_io.cpp \
    model/model_iocom.cpp \
    model/model_node.cpp \
    model/model_param.cpp \
    model/model_parambitfield.cpp \
    model/model_piblock.cpp \
    model/model_pin.cpp \
    model/model_port.cpp \
    model/model_process.cpp \
    model/model_reset.cpp \
    model/model_treeconnect.cpp \
    model/model_treeitem.cpp \
    model/model_property.cpp \
    model/model_propertyenum.cpp \
    model/model_componentpart.cpp \
    model/model_componentpartflow.cpp \
    model/model_componentpartproperty.cpp \
    model/model_gpviewer.cpp \
    model/model_viewer.cpp \
    model/model_viewerflow.cpp \
    camera/camera.cpp \
    camera/block.cpp \
    camera/property.cpp \
    camera/propertyenum.cpp \
    camera/flow.cpp \
    camera/flowmanager.cpp \
    camera/flowconnection.cpp \
    camera/flowviewerinterface.cpp \
    camera/registermanager.cpp \
    camera/registerbitfield.cpp \
    camera/register.cpp

# gpstudio_com lib
INCLUDEPATH += $$PWD/../gpstudio_com
DEPENDPATH += $$PWD/../gpstudio_com
LIBS += -lgpstudio_com

DISTFILES += \
    flowviewermodel.qmodel

RESOURCES += \
    connecttypes.qrc
