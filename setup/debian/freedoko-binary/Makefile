DEPTH = ../../../..

include $(DEPTH)/Makefile.os
include ../Makefile.install.directories

PACKAGE?=freedoko-binary

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
	# copy some files from the base directory
	cp Makefile.install $(PACKAGE_DIR)/Makefile
	cp ../copyright $(PACKAGE_DIR)/copyright
	for f in Version ChangeLog AUTHORS Makefile.os Makefile.version README LIESMICH data/bin/FreeDoko.desktop; do \
	  cp $(DEPTH)/$$f $(PACKAGE_DIR)/; \
	done
	cp ../Makefile.install.directories $(PACKAGE_DIR)/
	# copy the documentation
	mkdir -p $(PACKAGE_DIR)/doc
	for f in Bugs Roadmap GPL hpux.required_libs-ia64.txt; do \
	  cp $(DEPTH)/doc/$$f $(PACKAGE_DIR)/doc/; \
	done
	cp -a $(DEPTH)/doc/Implementation $(PACKAGE_DIR)/doc/; \
	mkdir $(PACKAGE_DIR)/doc/de
	cd $(DEPTH)/doc/de \
	  && cp GPL.de LiesMich* $(PACKAGE_DIR)/doc/de/
	mkdir $(PACKAGE_DIR)/doc/en
	cd $(DEPTH)/doc/en \
	  && cp README* $(PACKAGE_DIR)/doc/de/
	# copy the source
	tar -c --exclude=.svn \
	    -C $(DEPTH) src/ \
	  | tar -x -C $(PACKAGE_DIR)/
	cd $(PACKAGE_DIR)/src/; \
	  sed -e "s/\(VERSION_DESCRIPTION\)='.*'/\1='\"debian linux binary\"'/" \
	      -e "s|\(PUBLIC_DATA_DIRECTORY_VALUE\)='.*'|\1='\"$(DATA_DIRECTORY)\"'|" \
	      -e "s|\(MANUAL_DIRECTORY_VALUE\)='.*'|\1='\"$(HELP_DIRECTORY)\"'|" \
	    < Makefile.local.template \
	    > Makefile.local || exit; \
	  echo >> Makefile.local; \
	  echo "override CPPFLAGS += -DRELEASE" \
	    >> Makefile.local; \
	  echo "override CXXFLAGS = -Wall -Werror -Wno-parentheses -pipe -O2" \
	    >> Makefile.local
	$(MAKE) -C $(PACKAGE_DIR)/src/ clean
	# remove subversion files
	for d in `find $(PACKAGE_DIR)/ -name ".svn"`; do $(RM) -r $$d; done
	chmod -R u+w,a+rX $(PACKAGE_DIR)

.PHONY: package
package : release_directory
	cd $(PACKAGE_DIR) \
	  && debuild --preserve-envvar CXX -us -uc -b
