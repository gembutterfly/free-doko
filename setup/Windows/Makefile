DEPTH = ../..

include $(DEPTH)/Makefile.version
include $(DEPTH)/Makefile.os
include $(DEPTH)/Makefile.install.directories

.PHONY: all
all :

ifeq ($(SHELLTYPE), sh)
.PHONY: setup
setup : setup_nsis
#setup_dev_nsis

# create a setup file (using nullsoft nsis)
.PHONY: setup_nsis
setup_nsis :
	-$(RM) FreeDoko_$(VERSION)
	ln -s $(RELEASE_TMP)/FreeDoko_$(VERSION)
	echo 'Delete "$$INSTDIR\uninst.exe"' > FreeDoko.Files.uninstall.nsi
	(cd FreeDoko_$(VERSION) && find -follow -type f) \
	  | sed 's./.\\\\.g' | sed 's/^.\\//'\
	  | tac \
	  | xargs -I {} echo 'Delete "$$INSTDIR\{}"' \
	>> FreeDoko.Files.uninstall.nsi
	(cd FreeDoko_$(VERSION) && find -follow -type d) \
	  | sed 's./.\\\\.g' | sed 's/^.\\//' \
	  | tac \
	  | xargs -I {} echo 'RMDir "$$INSTDIR\{}"' \
	>> FreeDoko.Files.uninstall.nsi
	makensis -V4 FreeDoko.nsi
	$(RM) FreeDoko_$(VERSION)
	$(RM) -r $(RELEASE_TMP)/FreeDoko_$(VERSION)/doc/manual

# create a dev-setup file (using nullsoft nsis)
.PHONY: setup_dev_nsis
setup_dev_nsis :
	mkdir -p install_dev.release
	tar -c --exclude=.svn -C install_dev/ . \
	  | tar -xC install_dev.release/
	makensis -V4 FreeDoko-dev.nsi
	$(RM) -r install_dev.release

# not used anymore
.PHONY: full_setup_nsis
full_setup_nsis :
	sed -e 's/\($${PRODUCT_NAME}_$${PRODUCT_VERSION}\)\(.Setup.exe\)/\1.full\2/' \
	    -e 's/\($${PRODUCT_NAME} $${PRODUCT_VERSION}\)\( installer\)/\1 full\2/' \
	    -e 's/\($${PRODUCT_NAME} $${PRODUCT_VERSION}\)\( Installation\)/\1 Vollinstallation/' \
	    < FreeDoko.nsi \
	    > FreeDoko.full.nsi
	-$(RM) FreeDoko_$(VERSION)
	ln -s $(RELEASE_TMP)/FreeDoko_$(VERSION) FreeDoko_$(VERSION)
	cp -r ~/Daten/FreeDoko/cardsets/origin \
	      ~/Daten/FreeDoko/cardsets/xskat \
	      ~/Daten/FreeDoko/cardsets/kdecarddecks \
	      ~/Daten/FreeDoko/cardsets/pysol \
	   FreeDoko_$(VERSION)/cardsets/
	cp -r ~/Programme/FreeDoko/data/iconsets/* \
	   FreeDoko_$(VERSION)/iconsets/
	cp -r ~/Daten/FreeDoko/backgrounds/* \
	   FreeDoko_$(VERSION)/backgrounds/
	for d in `find $(PACKAGE_DIR)/ -name ".svn"`; do $(RM) -r $$d; done
	echo 'Delete "$$INSTDIR\uninst.exe"' > FreeDoko.Files.uninstall.nsi
	(cd FreeDoko_$(VERSION) && find -follow -type f) \
	  | sed 's./.\\\\.g' | sed 's/^.\\//'\
	  | tac \
	  | xargs -I {} echo 'Delete "$$INSTDIR\{}"' \
	>> FreeDoko.Files.uninstall.nsi
	(cd FreeDoko_$(VERSION) && find -follow -type d) \
	  | sed 's./.\\\\.g' | sed 's/^.\\//' \
	  | tac \
	  | xargs -I {} echo 'RMDir "$$INSTDIR\{}"' \
	>> FreeDoko.Files.uninstall.nsi
	echo 'Delete "$$INSTDIR\uninst.exe"' >> FreeDoko.Files.uninstall.nsi
	makensis -V4 FreeDoko.full.nsi
	$(RM) FreeDoko.full.nsi
	$(RM) FreeDoko_$(VERSION)

.PHONY : install_dev.zip
install_dev.zip :
	tar c install_dev --exclude .svn | tar x -C /tmp
	-$(RM) -r /tmp/FreeDoko.dev FreeDoko.Windows.dev.zip
	cd /tmp && mv install_dev FreeDoko.dev && zip -r FreeDoko.Windows.dev.zip FreeDoko.dev
endif

.PHONY: clean
clean :
	$(RM) FreeDoko_$(VERSION)
	$(RM) FreeDoko_*.Setup.exe
	$(RM) FreeDoko.Files.uninstall.nsi
	$(RM) FreeDoko.full.nsi
