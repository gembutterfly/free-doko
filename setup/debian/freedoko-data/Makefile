DEPTH = ../../../..

include $(DEPTH)/Makefile.os

PACKAGE?=freedoko-data

TARGET_DIR?=/tmp/FreeDoko.packages
PACKAGE_DIR?=$(TARGET_DIR)/$(PACKAGE)-$(VERSION)

# Edited for Debian GNU/Linux.
DESTDIR = 

.PHONY: all
all :

.PHONY: release_directory 
release_directory :
	-rm -rf $(PACKAGE_DIR)
	mkdir -p $(PACKAGE_DIR)/data
	cp -a . $(PACKAGE_DIR)/debian
	cp Makefile.install $(PACKAGE_DIR)/Makefile
	cp ../copyright $(PACKAGE_DIR)/copyright
	cp $(DEPTH)/AUTHORS $(PACKAGE_DIR)/AUTHORS
	for d in ai bin rules; do \
	  cp -r $(DEPTH)/data/$$d $(PACKAGE_DIR)/data/; \
	done
	cp $(DEPTH)/src/icon.png $(PACKAGE_DIR)/data/
	cp $(DEPTH)/src/logo.free.png $(PACKAGE_DIR)/data/logo.png
	# remove subversion files
	for d in `find $(PACKAGE_DIR)/ -name ".svn"`; do $(RM) -r $$d; done
	chmod -R u+w,a+rX $(PACKAGE_DIR)

.PHONY: package
package : release_directory
	cd $(PACKAGE_DIR) \
	  && debuild -us -uc -b
