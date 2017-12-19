DEPTH = ../..

include $(DEPTH)/Makefile.os
include $(DEPTH)/Makefile.install.directories

.PHONY: all
all :

ifeq ($(SHELLTYPE), sh)
.PHONY: release
release :
	mkdir -p $(RELEASE_TMP)/FreeDoko_$(VERSION)/backgrounds/
	cp table.png $(RELEASE_TMP)/FreeDoko_${VERSION}/backgrounds/
	for d in christmas wood; do \
	  tar -c --exclude=.svn $$d \
	    | tar -xC $(RELEASE_TMP)/FreeDoko_$(VERSION)/backgrounds/; \
	done

.PHONY: install
install :
	mkdir -p $(DESTDIR)$(DATA_DIRECTORY)/backgrounds
	for f in origin table.png wood christmas; do \
	  cp -a $$f $(DESTDIR)$(DATA_DIRECTORY)/backgrounds/; \
	done
endif
