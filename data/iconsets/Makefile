DEPTH = ../..

include $(DEPTH)/Makefile.os
include $(DEPTH)/Makefile.install.directories

.PHONY: all
all :

ifeq ($(SHELLTYPE), sh)
.PHONY: release
release :
	mkdir -p $(RELEASE_TMP)/FreeDoko_$(VERSION)/iconsets/
	for d in default symbols chips; do \
	  if [ -d $$d ]; then \
	    tar -c --exclude=.svn $$d \
	      | tar -xC $(RELEASE_TMP)/FreeDoko_$(VERSION)/iconsets/; \
	  fi \
	done
	chmod -R u+w $(RELEASE_TMP)/FreeDoko_$(VERSION)/iconsets/

.PHONY: install
install :
	mkdir -p $(DESTDIR)$(DATA_DIRECTORY)/iconsets
	for d in default chips symbols; do \
	  cp -a $$d $(DESTDIR)$(DATA_DIRECTORY)/iconsets/; \
	done
endif
