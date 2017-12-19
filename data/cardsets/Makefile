DEPTH = ../..

include $(DEPTH)/Makefile.os
include $(DEPTH)/Makefile.install.directories

.PHONY: all
all :

ifeq ($(SHELLTYPE), sh)
.PHONY: release
release :
	mkdir -p $(RELEASE_TMP)/FreeDoko_$(VERSION)/cardsets/
	for d in xskat; do \
	  if [ -d $$d ]; then \
	    tar -c --exclude=.svn $$d \
	      | tar -xC $(RELEASE_TMP)/FreeDoko_$(VERSION)/cardsets/; \
	  fi \
	done
	@# In order to add all cardsets to the release
	@# uncomment the following line, add a trailing backslash
	@# and remove the line after
	@#for d in freedoko-pingucards gnome-games kdecarddecks openclipart pysol; do
	for d in openclipart freedoko-pingucards; do \
	  tar -c --exclude=.svn $$d \
	    | tar -xC $(RELEASE_TMP)/FreeDoko_$(VERSION)/cardsets/; \
	done
	@# Special code for the InnoCard cardset
	mkdir -p $(RELEASE_TMP)/FreeDoko_$(VERSION)/cardsets/InnoCard
	-cp InnoCard/Lizenz* $(RELEASE_TMP)/FreeDoko_$(VERSION)/cardsets/InnoCard/
	-cp -r InnoCard/english InnoCard/french $(RELEASE_TMP)/FreeDoko_$(VERSION)/cardsets/InnoCard/
	chmod -R u+w $(RELEASE_TMP)/FreeDoko_$(VERSION)/cardsets/

.PHONY: install
install :
	mkdir -p $(DESTDIR)$(DATA_DIRECTORY)/cardsets
	for f in LiesMich ReadMe origin back xskat openclipart freedoko-pingucards gnome-games kdecarddecks pysol; do \
	  cp -a $$f $(DESTDIR)$(DATA_DIRECTORY)/cardsets/; \
	done
endif
