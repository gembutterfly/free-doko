DEPTH = ../../..

include $(DEPTH)/Makefile.version


# make all (nothing)
.PHONY: all
all :

# edit all files
.PHONY: edit
edit :
	xvim \
	  overview.html \
	  &

# update the date in all html files
.PHONY: update_date
update_date :
	for f in `find . -maxdepth 2 -name "*.html" -a -type f`; do \
	  sed "s/<meta name=\"date\" content=\"[0-9\-]*T[0-9:+\-]*\">/<meta name=\"date\" content=\"`date -Iseconds`\">/" $$f > t.out \
	  && mv t.out $$f; \
	done

# check all html files (uses 'tidy')
.PHONY: check
check :
	for f in *.html */*.html; do \
	  if [ -f $$f ]; then \
	    echo $$f; \
	    tidy -utf8 -e $$f 2>/dev/null \
	    || tidy -utf8 -e $$f \
	    || exit; \
	  fi \
	done
