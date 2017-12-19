DEPTH = ../trunk

include $(DEPTH)/Makefile.version
include $(DEPTH)/Makefile.os
include $(DEPTH)/Makefile.install.directories

# the subdirectories
DIRECTORIES = de \
	      en

# make all (nothing)
.PHONY: all
all :

# copy the manual in the release directory
.PHONY: release
release :
	mkdir -p $(RELEASE_TMP)/FreeDoko_$(VERSION)/doc/manual
	tar -c --exclude=.svn --exclude=Makefile . \
	   | tar xC $(RELEASE_TMP)/FreeDoko_$(VERSION)/doc/manual

# update the date in the html files
.PHONY: update_date
update_date :
	for d in $(DIRECTORIES); do \
	  $(MAKE) -C $$d $@; \
	done

# check the html files (uses 'tidy')
.PHONY: check
check :
	for f in *.html; do \
	  if [ -f $$f ]; then \
	    echo $$f; \
	    tidy -utf8 -e $$f 2>/dev/null \
	    || tidy -utf8 -e $$f \
	    || exit 1; \
	  fi \
	done
	for d in $(DIRECTORIES); do \
	  $(MAKE) -C $$d $@; \
	done

ifeq ($(SHELLTYPE), sh)
# this depends on having the source in a git repository
.PHONY: release
release:
	cd .. \
	 && git archive -9 --prefix FreeDoko_$(VERSION)/ -o $(RELEASE_TMP)/FreeDoko_$(VERSION).manual.zip HEAD manual
	cd $(RELEASE_TMP) \
	 && unzip -q FreeDoko_$(VERSION).manual.zip
	rm $(RELEASE_TMP)/FreeDoko_$(VERSION).manual.zip 
	cd $(RELEASE_TMP)/FreeDoko_$(VERSION)/manual/de/ \
	 && ln -s ../../doc/de docs
endif
