DEPTH = ../../../..
include ../Makefile.install.directories

include $(DEPTH)/Makefile.os

PACKAGE?=freedoko-nonfree

TARGET_DIR?=/tmp/FreeDoko.packages
PACKAGE_DIR?=$(TARGET_DIR)/$(PACKAGE)-$(VERSION)

# Edited for Debian GNU/Linux.
DESTDIR = 

.PHONY: all
all :

.PHONY: release_directory
release_directory :
	-rm -rf $(PACKAGE_DIR)
	mkdir -p $(PACKAGE_DIR)
	# included packages
	for p in freedoko-binary freedoko-data freedoko-backgrounds freedoko-translations freedoko-manual-base freedoko-manual-de freedoko-manual-en; do \
	  echo; \
	  echo "Subpackage $$p"; \
	  echo; \
	  export PACKAGE_DIR=$(PACKAGE_DIR)/subpackage \
	    && $(MAKE) -C ../$$p release_directory \
	    && cp -a $(PACKAGE_DIR)/subpackage/* $(PACKAGE_DIR) \
	    && rm -rf $(PACKAGE_DIR)/subpackage; \
	done
	# own files
	rm -rf $(PACKAGE_DIR)/debian
	cp -a . $(PACKAGE_DIR)/debian
	cp Makefile.install $(PACKAGE_DIR)/Makefile
	for f in FreeDoko.6 README.Debian conffiles docs freedoko.asciidoc; do \
	  cp -a ../freedoko-binary/$$f $(PACKAGE_DIR)/debian/; \
	done
	sed "s/PACKAGE=freedoko-binary/PACKAGE=$(PACKAGE)/g" ../freedoko-binary/menu > $(PACKAGE_DIR)/debian/menu
	# logo
	cp $(DEPTH)/src/logo.nonfree.png $(PACKAGE_DIR)/data/logo.png
	#   cardsets (Altenburg)
	mkdir -p $(PACKAGE_DIR)/data/cardsets
	for cardset in Altenburg; do \
	  cp -a ~/Daten/FreeDoko/cardsets/$$cardset $(PACKAGE_DIR)/data/cardsets/$$cardset; \
	done
	#   sounds
	mkdir -p $(PACKAGE_DIR)/data/sounds
	cp -a $(DEPTH)/data/sounds/* $(PACKAGE_DIR)/data/sounds/;
	# remove subversion files
	for d in `find $(PACKAGE_DIR)/ -name ".svn"`; do $(RM) -r $$d; done
	chmod -R u+w,a+rX $(PACKAGE_DIR)

.PHONY: package
package : release_directory
	cd $(PACKAGE_DIR) \
	  && debuild --preserve-envvar CXX -us -uc -b
