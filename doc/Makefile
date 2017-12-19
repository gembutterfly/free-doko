DEPTH = ..

DIRECTORIES = de en

include $(DEPTH)/Makefile.version
include $(DEPTH)/Makefile.os
include $(DEPTH)/Makefile.install.directories

.PHONY: all
all: $(DIRECTORIES)

.PHONY : $(DIRECTORIES)
$(DIRECTORIES) :
	$(MAKE) -C $@


ifeq ($(SHELLTYPE), sh)
.PHONY: release
release :
	mkdir -p $(RELEASE_TMP)/FreeDoko_$(VERSION)/doc
	$(MAKE) -C en release 
	$(MAKE) -C de release 
	tar -c --exclude=.svn Implementation \
	   | tar xC $(RELEASE_TMP)/FreeDoko_$(VERSION)/doc/
	cp Bugs Roadmap GPL hpux.required_libs-ia64.txt freedoko.asciidoc \
	  $(RELEASE_TMP)/FreeDoko_$(VERSION)/doc/

.PHONY: install
install :
	mkdir -p $(DESTDIR)$(HELP_DIRECTORY)
	if test `basename $(HELP_DIRECTORY)` = "freedoko"; then \
	  cd $(DESTDIR)$(HELP_DIRECTORY)/.. && ln -sf freedoko FreeDoko; \
	fi
	if test `basename $(HELP_DIRECTORY)` = "FreeDoko"; then \
	  cd $(DESTDIR)$(HELP_DIRECTORY)/.. && ln -sf FreeDoko freedoko; \
	fi
	cp -a Bugs GPL Roadmap hpux.required_libs-ia64.txt $(DESTDIR)$(HELP_DIRECTORY)/
	for d in de en; do $(MAKE) -C $$d install; done
	-chgrp games $(DESTDIR)$(HELP_DIRECTORY) && chgrp -R games $(DESTDIR)$(HELP_DIRECTORY)
	chmod -R o-rwx,g+rX,g-w $(DESTDIR)$(HELP_DIRECTORY)
	cd $(DESTDIR)$(DATA_DIRECTORY) && ln -sf $(HELP_DIRECTORY)/ doc
endif

ifeq ($(SHELLTYPE), sh)
.PHONY: clean
clean :
	-find . -name "*~" -exec rm \{\} \;
	-rm -f a2out.ps
	-rm -rf doxygen
	for d in *; do\
	  if ( test -d "$$d" && test -f "$$d"/Makefile ); then \
	    $(MAKE) -C "$$d" clean; \
	  fi; \
	done
endif
