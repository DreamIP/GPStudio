QT       += core gui xml script svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = gpstudio_gui
TEMPLATE = lib

OUT_PWD = ../gpstudio_gui/
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

DEFINES += GPSTUDIO_GUI_EXPORT_LIB

QMAKE_CFLAGS_RELEASE = -O2

SOURCES += \
    propertywidgets/propertywidget.cpp \
    propertywidgets/propertygroupwidget.cpp \
    propertywidgets/propertyintwidget.cpp \
    propertywidgets/propertysintwidget.cpp \
    propertywidgets/propertyclockwidget.cpp \
    propertywidgets/propertystringwidget.cpp \
    propertywidgets/propertyboolwidget.cpp \
    propertywidgets/propertyenumwidget.cpp \
    scriptwidget.cpp \
    scriptlineedit.cpp \
    itemmodel/caminfoitemmodel.cpp \
    itemmodel/libitem.cpp \
    itemmodel/libitemmodel.cpp \
    itemmodel/libtreeview.cpp \
    itemmodel/cameraitemmodel.cpp \
    itemmodel/cameraitem.cpp \
    itemmodel/propertyitemmodel.cpp \
    propertywidgets/propertymatrixwidget.cpp \
    flowviewerwidget/flowviewerwidget.cpp \
    flowviewerwidget/abstractviewer.cpp \
    flowviewerwidget/layerviewer.cpp \
    flowviewerwidget/hexviewer.cpp \
    flowviewerwidget/plotviewer.cpp \
    blockview/blockitem.cpp \
    blockview/blockscene.cpp \
    blockview/blockconnectoritem.cpp \
    blockview/blockportitem.cpp \
    blockview/blockview.cpp \
    blockeditor/blockeditorwindow.cpp \
    blockeditor/codeeditor.cpp \
    blockeditor/syntaxhighlight/abstractsyntax.cpp \
    blockeditor/syntaxhighlight/vhdlsyntax.cpp \
    blockeditor/syntaxhighlight/xmlsyntax.cpp \
    blockeditor/syntaxhighlight/verilogsyntax.cpp \
    blockeditor/syntaxhighlight/tclsyntax.cpp \
    nodeeditor/gpnodeproject.cpp \
    nodeeditor/nodeeditorwindows.cpp \
    nodeeditor/confignodedialog.cpp \
    nodeeditor/undostack/nodecommands.cpp \
    nodeeditor/undostack/blockcommands.cpp \
    nodeeditor/undostack/viewercommands.cpp \
    viewer/camexplorerwidget.cpp \
    viewer/viewerwidgets/docviewer.cpp \
    viewer/viewerwidgets/pdfviewer.cpp \
    viewer/viewerwidgets/layerwidget.cpp \
    compilelogwidget.cpp \
    viewer/viewerexplorerwidget.cpp \
    itemmodel/propertycompleter.cpp \
    itemmodel/viewertreeview.cpp \
    viewer/viewerwidgets/feature.cpp \
    viewer/viewerwidgets/featureitem.cpp \
    itemmodel/caminfotreeview.cpp \
    nodeeditor/settings/pathssettingspage.cpp \
    nodeeditor/settings/patheditwidget.cpp \
    nodeeditor/settings/nodesettings.cpp

HEADERS  += \
    gpstudio_gui_common.h \
    propertywidgets/propertywidgets.h \
    propertywidgets/propertywidget.h \
    propertywidgets/propertygroupwidget.h \
    propertywidgets/propertyintwidget.h \
    propertywidgets/propertysintwidget.h \
    propertywidgets/propertyclockwidget.h \
    propertywidgets/propertystringwidget.h \
    propertywidgets/propertyboolwidget.h \
    propertywidgets/propertyenumwidget.h \
    scriptwidget.h \
    scriptlineedit.h \
    itemmodel/caminfoitemmodel.h \
    itemmodel/libitemmodel.h \
    itemmodel/libitem.h \
    itemmodel/libtreeview.h \
    itemmodel/cameraitemmodel.h \
    itemmodel/cameraitem.h \
    itemmodel/propertyitemmodel.h \
    propertywidgets/propertymatrixwidget.h \
    flowviewerwidget/flowviewerwidget.h \
    flowviewerwidget/abstractviewer.h \
    flowviewerwidget/layerviewer.h \
    flowviewerwidget/hexviewer.h \
    flowviewerwidget/plotviewer.h \
    blockview/blockitem.h \
    blockview/blockscene.h \
    blockview/blockconnectoritem.h \
    blockview/blockportitem.h \
    blockview/blockview.h \
    blockeditor/blockeditorwindow.h \
    blockeditor/codeeditor.h \
    blockeditor/syntaxhighlight/abstractsyntax.h \
    blockeditor/syntaxhighlight/vhdlsyntax.h \
    blockeditor/syntaxhighlight/xmlsyntax.h \
    blockeditor/syntaxhighlight/verilogsyntax.h \
    blockeditor/syntaxhighlight/tclsyntax.h \
    nodeeditor/gpnodeproject.h \
    nodeeditor/nodeeditorwindows.h \
    nodeeditor/confignodedialog.h \
    nodeeditor/undostack/nodecommands.h \
    nodeeditor/undostack/blockcommands.h \
    nodeeditor/undostack/viewercommands.h \
    viewer/camexplorerwidget.h \
    viewer/viewerwidgets/docviewer.h \
    viewer/viewerwidgets/pdfviewer.h \
    viewer/viewerwidgets/layerwidget.h \
    compilelogwidget.h \
    viewer/viewerexplorerwidget.h \
    itemmodel/propertycompleter.h \
    itemmodel/viewertreeview.h \
    viewer/viewerwidgets/feature.h \
    viewer/viewerwidgets/featureitem.h \
    itemmodel/caminfotreeview.h \
    nodeeditor/settings/pathssettingspage.h \
    nodeeditor/settings/patheditwidget.h \
    nodeeditor/settings/nodesettings.h

RESOURCES += \
    icons.qrc \
    flowvieweractions.qrc \
    flowviewertypes.qrc

include(../../thirdparts/hexedit/hexedit.pri)
include(../../thirdparts/qcustomplot/qcustomplot.pri)

# gpstudio_lib lib
INCLUDEPATH += $$PWD/../gpstudio_lib
DEPENDPATH += $$PWD/../gpstudio_lib
LIBS += -lgpstudio_lib

# gpstudio_com lib
INCLUDEPATH += $$PWD/../gpstudio_com
DEPENDPATH += $$PWD/../gpstudio_com
LIBS += -lgpstudio_com

use_open_cv {
    DEFINES += __USE_OPEN_CV__
    win32 {
        INCLUDEPATH += "E:\opencv\include"
        LIBS += -L"E:\opencv\x86\mingw\bin" -lopencv_core249 -lopencv_highgui249 -lopencv_imgproc249
    }
}
