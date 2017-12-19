DEPTH = ../..

include $(DEPTH)/Makefile.version
include $(DEPTH)/Makefile.os
include $(DEPTH)/Makefile.install.directories

.PHONY: all
all :

ifeq ($(SHELLTYPE), sh)
.PHONY: release
release :
	mkdir -p $(RELEASE_TMP)/FreeDoko_$(VERSION)/sounds
	cp Makefile $(RELEASE_TMP)/FreeDoko_$(VERSION)/sounds/
	for d in de de-alt -en; do \
	  if [ -d $$d ]; then \
	    tar -c --exclude=.svn $$d \
	      | tar -xC $(RELEASE_TMP)/FreeDoko_$(VERSION)/sounds; \
	  fi \
	done
	chmod -R u+w $(RELEASE_TMP)/FreeDoko_$(VERSION)/sounds/

.PHONY: install
install :
	mkdir -p $(DESTDIR)$(DATA_DIRECTORY)/sounds
	for f in de de-alt en; do \
	  cp -a $$f $(DESTDIR)$(DATA_DIRECTORY)/sounds/; \
	done
endif
