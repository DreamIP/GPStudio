
MAKE?=make

WIN_CROSS_QT:=/opt/Qt5.7_win64/

NPROC:=$(shell nproc || echo 2)

all: gui-tools

distrib: gui-tools-win64-qt5 gui-tools-linux32-qt4 gui-tools-linux64-qt4 gui-tools-linux32-qt5 gui-tools-linux64-qt5

gui-tools-win64-qt5: gpnode-win64-qt5 gpblock-win64-qt5 gpviewer-win64-qt5
	#cd bin-win64-qt5/ && $(WIN_CROSS_QT)windeployqt .
	cp `i686-w64-mingw32-gcc -print-file-name=`libgcc_s_sjlj-1.dll bin-win64-qt5/
	cp `i686-w64-mingw32-gcc -print-file-name=`libstdc++-6.dll bin-win64-qt5/
	cp /usr/i686-w64-mingw32/lib/libwinpthread-1.dll bin-win64-qt5/
	cp $(WIN_CROSS_QT)bin/Qt5Core.dll bin-win64-qt5/
	cp $(WIN_CROSS_QT)bin/Qt5Gui.dll bin-win64-qt5/
	cp $(WIN_CROSS_QT)bin/Qt5PrintSupport.dll bin-win64-qt5/
	cp $(WIN_CROSS_QT)bin/Qt5Script.dll bin-win64-qt5/
	cp $(WIN_CROSS_QT)bin/Qt5Svg.dll bin-win64-qt5/
	cp $(WIN_CROSS_QT)bin/Qt5Xml.dll bin-win64-qt5/
	cp $(WIN_CROSS_QT)bin/Qt5Widgets.dll bin-win64-qt5/
	mkdir -p bin-win64-qt5/platforms/
	cp $(WIN_CROSS_QT)plugins/platforms/qminimal.dll bin-win64-qt5/platforms/
	cp $(WIN_CROSS_QT)plugins/platforms/qwindows.dll bin-win64-qt5/platforms/
	mkdir -p bin-win64-qt5/printsupport/
	cp $(WIN_CROSS_QT)plugins/printsupport/windowsprintersupport.dll bin-win64-qt5/printsupport/
	mkdir -p bin-win64-qt5/iconengines/
	cp $(WIN_CROSS_QT)plugins/iconengines/qsvgicon.dll bin-win64-qt5/iconengines/
	cp -r $(WIN_CROSS_QT)plugins/imageformats/ bin-win64-qt5/
	rm -f bin-win64-qt5/*.a

build-win64-qt5/:
	mkdir build-win64-qt5
build-win64-qt5/Makefile: build-win64-qt5/ src/gpstudio.pro
	cd build-win64-qt5/ && $(WIN_CROSS_QT)bin/qmake -r ../src "DISTRIB=1"
gpstudio_com-win64-qt5: build-win64-qt5/Makefile
	cd build-win64-qt5/ && $(MAKE) -j$(NPROC) -f Makefile sub-gpstudio_com-release
gpstudio_lib-win64-qt5: build-win64-qt5/Makefile gpstudio_com-win64-qt5
	cd build-win64-qt5/ && $(MAKE) -j$(NPROC) -f Makefile sub-gpstudio_lib-release
gpstudio_gui-win64-qt5: build-win64-qt5/Makefile gpstudio_com-win64-qt5 gpstudio_lib-win64-qt5
	cd build-win64-qt5/ && $(MAKE) -j$(NPROC) -f Makefile sub-gpstudio_gui-release
gpnode-win64-qt5: build-win64-qt5/Makefile gpstudio_com-win64-qt5 gpstudio_lib-win64-qt5 gpstudio_gui-win64-qt5
	cd build-win64-qt5/ && $(MAKE) -j$(NPROC) -f Makefile sub-gpnode-release
gpblock-win64-qt5: build-win64-qt5/Makefile gpstudio_com-win64-qt5 gpstudio_lib-win64-qt5 gpstudio_gui-win64-qt5
	cd build-win64-qt5/ && $(MAKE) -j$(NPROC) -f Makefile sub-gpblock-release
gpviewer-win64-qt5: build-win64-qt5/Makefile gpstudio_com-win64-qt5 gpstudio_lib-win64-qt5 gpstudio_gui-win64-qt5
	cd build-win64-qt5/ && $(MAKE) -j$(NPROC) -f Makefile sub-gpviewer-release


gui-tools-linux32-qt4: gpnode-linux32-qt4 gpblock-linux32-qt4 gpviewer-linux32-qt4
build-linux32-qt4/:
	mkdir build-linux32-qt4
build-linux32-qt4/Makefile: build-linux32-qt4/ src/gpstudio.pro
	cd build-linux32-qt4/ && /opt/Qt4.8_32/bin/qmake -r ../src "DISTRIB=1" -spec linux-g++-32
gpstudio_com-linux32-qt4: build-linux32-qt4/Makefile
	cd build-linux32-qt4/ && make -j$(NPROC) -f Makefile sub-gpstudio_com
gpstudio_lib-linux32-qt4: build-linux32-qt4/Makefile gpstudio_com-linux32-qt4
	cd build-linux32-qt4/ && make -j$(NPROC) -f Makefile sub-gpstudio_lib
gpstudio_gui-linux32-qt4: build-linux32-qt4/Makefile gpstudio_com-linux32-qt4 gpstudio_lib-linux32-qt4
	cd build-linux32-qt4/ && make -j$(NPROC) -f Makefile sub-gpstudio_gui
gpnode-linux32-qt4: build-linux32-qt4/Makefile gpstudio_com-linux32-qt4 gpstudio_lib-linux32-qt4 gpstudio_gui-linux32-qt4
	cd build-linux32-qt4/ && make -j$(NPROC) -f Makefile sub-gpnode
gpblock-linux32-qt4: build-linux32-qt4/Makefile gpstudio_com-linux32-qt4 gpstudio_lib-linux32-qt4 gpstudio_gui-linux32-qt4
	cd build-linux32-qt4/ && make -j$(NPROC) -f Makefile sub-gpblock
gpviewer-linux32-qt4: build-linux32-qt4/Makefile gpstudio_com-linux32-qt4 gpstudio_lib-linux32-qt4 gpstudio_gui-linux32-qt4
	cd build-linux32-qt4/ && make -j$(NPROC) -f Makefile sub-gpviewer


gui-tools-linux64-qt4: gpnode-linux64-qt4 gpblock-linux64-qt4 gpviewer-linux64-qt4
build-linux64-qt4/:
	mkdir build-linux64-qt4
build-linux64-qt4/Makefile: build-linux64-qt4/ src/gpstudio.pro
	cd build-linux64-qt4/ && qmake-qt4 -r ../src "DISTRIB=1" -spec linux-g++-64
gpstudio_com-linux64-qt4: build-linux64-qt4/Makefile
	cd build-linux64-qt4/ && make -j$(NPROC) -f Makefile sub-gpstudio_com
gpstudio_lib-linux64-qt4: build-linux64-qt4/Makefile gpstudio_com-linux64-qt4
	cd build-linux64-qt4/ && make -j$(NPROC) -f Makefile sub-gpstudio_lib
gpstudio_gui-linux64-qt4: build-linux64-qt4/Makefile gpstudio_com-linux64-qt4 gpstudio_lib-linux64-qt4
	cd build-linux64-qt4/ && make -j$(NPROC) -f Makefile sub-gpstudio_gui
gpnode-linux64-qt4: build-linux64-qt4/Makefile gpstudio_com-linux64-qt4 gpstudio_lib-linux64-qt4 gpstudio_gui-linux64-qt4
	cd build-linux64-qt4/ && make -j$(NPROC) -f Makefile sub-gpnode
gpblock-linux64-qt4: build-linux64-qt4/Makefile gpstudio_com-linux64-qt4 gpstudio_lib-linux64-qt4 gpstudio_gui-linux64-qt4
	cd build-linux64-qt4/ && make -j$(NPROC) -f Makefile sub-gpblock
gpviewer-linux64-qt4: build-linux64-qt4/Makefile gpstudio_com-linux64-qt4 gpstudio_lib-linux64-qt4 gpstudio_gui-linux64-qt4
	cd build-linux64-qt4/ && make -j$(NPROC) -f Makefile sub-gpviewer


gui-tools-linux32-qt5: gpnode-linux32-qt5 gpblock-linux32-qt5 gpviewer-linux32-qt5
build-linux32-qt5/:
	mkdir build-linux32-qt5
build-linux32-qt5/Makefile: build-linux32-qt5/ src/gpstudio.pro
	cd build-linux32-qt5/ && /opt/Qt5.2_32/bin/qmake -r ../src "DISTRIB=1" -spec linux-g++-32
gpstudio_com-linux32-qt5: build-linux32-qt5/Makefile
	cd build-linux32-qt5/ && make -j$(NPROC) -f Makefile sub-gpstudio_com
gpstudio_lib-linux32-qt5: build-linux32-qt5/Makefile gpstudio_com-linux32-qt5
	cd build-linux32-qt5/ && make -j$(NPROC) -f Makefile sub-gpstudio_lib
gpstudio_gui-linux32-qt5: build-linux32-qt5/Makefile gpstudio_com-linux32-qt5 gpstudio_lib-linux32-qt5
	cd build-linux32-qt5/ && make -j$(NPROC) -f Makefile sub-gpstudio_gui
gpnode-linux32-qt5: build-linux32-qt5/Makefile gpstudio_com-linux32-qt5 gpstudio_lib-linux32-qt5 gpstudio_gui-linux32-qt5
	cd build-linux32-qt5/ && make -j$(NPROC) -f Makefile sub-gpnode
gpblock-linux32-qt5: build-linux32-qt5/Makefile gpstudio_com-linux32-qt5 gpstudio_lib-linux32-qt5 gpstudio_gui-linux32-qt5
	cd build-linux32-qt5/ && make -j$(NPROC) -f Makefile sub-gpblock
gpviewer-linux32-qt5: build-linux32-qt5/Makefile gpstudio_com-linux32-qt5 gpstudio_lib-linux32-qt5 gpstudio_gui-linux32-qt5
	cd build-linux32-qt5/ && make -j$(NPROC) -f Makefile sub-gpviewer


gui-tools-linux64-qt5: gpnode-linux64-qt5 gpblock-linux64-qt5 gpviewer-linux64-qt5
build-linux64-qt5/:
	mkdir build-linux64-qt5
build-linux64-qt5/Makefile: build-linux64-qt5/ src/gpstudio.pro
	cd build-linux64-qt5/ && qmake -r ../src "DISTRIB=1" -spec linux-g++-64
gpstudio_com-linux64-qt5: build-linux64-qt5/Makefile
	cd build-linux64-qt5/ && make -j$(NPROC) -f Makefile sub-gpstudio_com
gpstudio_lib-linux64-qt5: build-linux64-qt5/Makefile gpstudio_com-linux64-qt5
	cd build-linux64-qt5/ && make -j$(NPROC) -f Makefile sub-gpstudio_lib
gpstudio_gui-linux64-qt5: build-linux64-qt5/Makefile gpstudio_com-linux64-qt5 gpstudio_lib-linux64-qt5
	cd build-linux64-qt5/ && make -j$(NPROC) -f Makefile sub-gpstudio_gui
gpnode-linux64-qt5: build-linux64-qt5/Makefile gpstudio_com-linux64-qt5 gpstudio_lib-linux64-qt5 gpstudio_gui-linux64-qt5
	cd build-linux64-qt5/ && make -j$(NPROC) -f Makefile sub-gpnode
gpblock-linux64-qt5: build-linux64-qt5/Makefile gpstudio_com-linux64-qt5 gpstudio_lib-linux64-qt5 gpstudio_gui-linux64-qt5
	cd build-linux64-qt5/ && make -j$(NPROC) -f Makefile sub-gpblock
gpviewer-linux64-qt5: build-linux64-qt5/Makefile gpstudio_com-linux64-qt5 gpstudio_lib-linux64-qt5 gpstudio_gui-linux64-qt5
	cd build-linux64-qt5/ && make -j$(NPROC) -f Makefile sub-gpviewer


gui-tools: gpnode gpblock gpviewer
build/:
	mkdir build
build/Makefile: build/ src/gpstudio.pro
	#cd build/ && qmake ../src -spec `qmake -query QMAKE_SPEC`
	cd build/ && qmake ../src
gpstudio_com: build/Makefile
	cd build/ && $(MAKE) -j$(NPROC) -f Makefile sub-gpstudio_com
gpstudio_lib: build/Makefile gpstudio_com
	cd build/ && $(MAKE) -j$(NPROC) -f Makefile sub-gpstudio_lib
gpstudio_gui: build/Makefile gpstudio_com gpstudio_lib
	cd build/ && $(MAKE) -j$(NPROC) -f Makefile sub-gpstudio_gui
gpnode: build/Makefile gpstudio_com gpstudio_lib gpstudio_gui
	cd build/ && $(MAKE) -j$(NPROC) -f Makefile sub-gpnode
gpblock: build/Makefile gpstudio_com gpstudio_lib gpstudio_gui
	cd build/ && $(MAKE) -j$(NPROC) -f Makefile sub-gpblock
gpviewer: build/Makefile gpstudio_com gpstudio_lib gpstudio_gui
	cd build/ && $(MAKE) -j$(NPROC) -f Makefile sub-gpviewer


clean:
	rm -rf build/
	rm -rf build-linux32-qt4/
	rm -rf build-linux32-qt5/
	rm -rf build-linux64-qt4/
	rm -rf build-linux64-qt5/
	rm -rf build-win64-qt5/
	rm -f ../bin/*.so* ../bin/*.dll

FORCE:

