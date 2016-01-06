
all: doc gui-tools

doc: FORCE
	cd docsrc/ && make -f Makefile

distrib: FORCE
	cd distrib/ && make -f Makefile

distrib-web: distrib
	cp doc/*.pdf /var/www/gpstudio/doc
	cp distrib/gpstudio_linux-`cat version.txt`.tar.gz /var/www/gpstudio/download/
	cp distrib/gpstudio_win-`cat version.txt`.zip /var/www/gpstudio/download/

clean:
	cd docsrc/ && make -f Makefile clean
	cd distrib/ && make -f Makefile clean
	cd gui-tools/ && make -f Makefile clean

lines: FORCE
	wc -l scripts/*.php scripts/*/*.php support/toolchain/*/*.php distrib/*.php gui-tools/src/*/*.h gui-tools/src/*/*.cpp gui-tools/src/*/*/*.h gui-tools/src/*/*/*.cpp gpnode_completion gplib_completion| sort -n -k1

gui-tools: FORCE
	cd gui-tools/ && make -f Makefile

install: gui-tools

FORCE:

