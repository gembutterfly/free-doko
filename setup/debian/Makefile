DEPTH = ../../..

include $(DEPTH)/Makefile.version
include $(DEPTH)/Makefile.os

PACKAGES = \
	freedoko-nonfree \
	freedoko-all \
	freedoko \
	freedoko-binary \
	freedoko-binary-no-network \
	freedoko-data \
	freedoko-translations \
	freedoko-backgrounds \
	freedoko-icongroups \
	freedoko-cardsets-xskat \
	freedoko-cardset-openclipart \
	freedoko-cardsets-gnome-games \
	freedoko-cardsets-pysol \
	freedoko-cardsets-kdecarddecks \
	freedoko-soundset-mbrola \
	freedoko-manual-base \
	freedoko-manual-de \
	freedoko-manual-en

PACKAGES_TO_CREATE = \
	freedoko-binary \
	freedoko-binary-no-network \
	freedoko-nonfree \
	freedoko-translations \
	freedoko-manual-de

#PACKAGES_TO_CREATE = freedoko-nonfree

#PACKAGES_TO_CREATE = $(PACKAGES)

DEBIAN_VERSION = squeeze

TARGET_DIR = /tmp/FreeDoko.packages
export TARGET_DIR

DEB_DIR = /home/install/mirrors/FreeDoko

.PHONY: all
all :

.PHONY: packages
packages :
	export DEPEND_GTKMM=libgtkmm-2.4-1c2a; \
        export DEPEND_GNET=libgnet2.0-0; \
	for d in $(PACKAGES_TO_CREATE); do \
	  $(MAKE) -C $$d package || exit; \
	  rm -rf $(TARGET_DIR)/$$d-$(VERSION); \
	done || false
	mkdir -p $(DEB_DIR)/pool/main/FreeDoko
	mkdir -p $(DEB_DIR)/pool/non-free/FreeDoko
	cp $(TARGET_DIR)/*.deb $(DEB_DIR)/pool/main/FreeDoko/
	-mv $(DEB_DIR)/pool/main/FreeDoko/freedoko-nonfree* \
	    $(DEB_DIR)/pool/non-free/FreeDoko/
	$(RM) /tmp/packages-i386.db
	mkdir -p $(DEB_DIR)/dists/squeeze/main/binary-i386
	mkdir -p $(DEB_DIR)/dists/squeeze/non-free/binary-i386
	apt-ftparchive generate ./generate.cfg
