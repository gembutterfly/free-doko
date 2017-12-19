DEPTH = ../../../..

include $(DEPTH)/Makefile.os

PACKAGE?=freedoko-soundset-mbrola

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
	cp -a . $(PACKAGE_DIR)/debian
	cp Makefile.install $(PACKAGE_DIR)/Makefile
	cp ../copyright $(PACKAGE_DIR)/
	cp $(DEPTH)/AUTHORS $(PACKAGE_DIR)/
	cp ../Makefile.install.directories $(PACKAGE_DIR)/
	mkdir -p $(PACKAGE_DIR)/data
	cp -a ~/Programme/FreeDoko/data/sounds $(PACKAGE_DIR)/data
	rm -rf `find $(PACKAGE_DIR)/data/sounds -name ".svn"`
	chmod -R u+w,a+rX $(PACKAGE_DIR)

.PHONY: package
package : release_directory
	cd $(PACKAGE_DIR) \
	  && debuild -us -uc -b
