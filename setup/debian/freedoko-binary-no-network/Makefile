DEPTH = ../../../..

include $(DEPTH)/Makefile.os

PACKAGE?=freedoko-binary-no-network

TARGET_DIR?=/tmp/FreeDoko.packages
PACKAGE_DIR?=$(TARGET_DIR)/$(PACKAGE)-$(VERSION)

export PACKAGE TARGET_DIR PACKAGE_DIR

# Edited for Debian GNU/Linux.
DESTDIR = 

.PHONY: all
all :

.PHONY: release_directory 
release_directory :
	$(MAKE) -C ../freedoko-binary $@
	cd $(PACKAGE_DIR)/; \
	  sed -e "s/PACKAGE=freedoko-binary/PACKAGE=$(PACKAGE)/" \
	    < Makefile \
	    > Makefile.bak \
	  && mv Makefile.bak Makefile
	# copy this debian files
	cp * $(PACKAGE_DIR)/debian/
	# remove the network module
	cd $(PACKAGE_DIR)/src/; \
	  sed -e "/USE_NETWORK/s/true/false/" \
	    < Makefile.modules \
	    > Makefile.modules.tmp \
	    && mv Makefile.modules.tmp \
	          Makefile.modules; \
	  sed -e "s/\(VERSION_DESCRIPTION\)='.*'/\1='\"debian linux binary (no network)\"'/" \
	    < Makefile.local \
	    > Makefile.local.tmp \
	    && mv Makefile.local.tmp \
	          Makefile.local
	chmod -R u+w,a+rX $(PACKAGE_DIR)

.PHONY: package
package : release_directory
	cd $(PACKAGE_DIR) \
	  && debuild --preserve-envvar CXX -us -uc -b
