include gpstudio.mk

MAKE?=make

all: doc gui-tools

doc: docgps docip docbackend

docgps: FORCE
	@cd docsrc/ && $(MAKE) -f Makefile

docbackend: FORCE
	@cd scripts/ && $(MAKE) -f Makefile

docip: FORCE
	@cd support/ && $(MAKE) -f Makefile

distrib: doc FORCE
	cd distrib/ && $(MAKE) -f Makefile

distrib-web: distrib
	cp doc/*.pdf /var/www/gpstudio/doc
	cp distrib/gpstudio_linux32-qt4-`cat version.txt`.tar.gz /var/www/gpstudio/download/distrib/
	cp distrib/gpstudio_linux64-qt4-`cat version.txt`.tar.gz /var/www/gpstudio/download/distrib/
	cp distrib/gpstudio_linux32-qt5-`cat version.txt`.tar.gz /var/www/gpstudio/download/distrib/
	cp distrib/gpstudio_linux64-qt5-`cat version.txt`.tar.gz /var/www/gpstudio/download/distrib/
	cp distrib/gpstudio_win64-qt5-`cat version.txt`.zip /var/www/gpstudio/download/distrib/
	cp distrib/setup-gpstudio_win64-qt5-`cat version.txt`.exe /var/www/gpstudio/download/distrib/

clean:
	cd docsrc/ && $(MAKE) -f Makefile clean
	cd distrib/ && $(MAKE) -f Makefile clean
	cd gui-tools/ && $(MAKE) -f Makefile clean

lines: FORCE
	@wc -l $(shell find scripts/ support/toolchain/ distrib/ -name '*.php') \
	$(shell find gui-tools/src/ \( -name '*.h' -o -name '*.cpp' \)) \
	$(shell find share/ -name '*_completion' ) | sort -n -k1

gui-tools: FORCE
	cd gui-tools/ && $(MAKE) -f Makefile

install: gui-tools

checklib:
	bin/gplib checklib

checkversion:
	grep -r "1\.21" -I --exclude=*.log --exclude=*.io --exclude-dir=doc

package: gui-tools
	rm -rf distrib/gpstudio_distrib
	php distrib/distrib.php -o distrib/gpstudio_distrib
	tar zcf gpstudio_distrib.tar.gz distrib/gpstudio_distrib/

FORCE:
