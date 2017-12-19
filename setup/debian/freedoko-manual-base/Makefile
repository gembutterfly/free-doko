DEPTH = ../../../..

include $(DEPTH)/Makefile.os

PACKAGE?=freedoko-manual-base

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
	mkdir -p $(PACKAGE_DIR)/doc/manual/
	cp -a $(DEPTH)/doc/manual/*.html $(DEPTH)/doc/manual/*.css $(DEPTH)/doc/manual/pictures \
       		$(PACKAGE_DIR)/doc/manual/
	# remove subversion files
	for d in `find $(PACKAGE_DIR)/ -name ".svn"`; do $(RM) -r $$d; done
	chmod -R u+w,a+rX $(PACKAGE_DIR)

.PHONY: package
package : release_directory
	cd $(PACKAGE_DIR) \
	  && debuild -us -uc -b
