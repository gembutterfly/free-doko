DEPTH = ../..

include $(DEPTH)/Makefile.os
include $(DEPTH)/Makefile.install.directories

#include ../Makefile.latex

.PHONY: all
all: txt pdf

ifeq ($(OPERATING_SYSTEM), Linux)
.PHONY: release
release : all
	mkdir -p $(RELEASE_TMP)/FreeDoko_$(VERSION)/doc/en/
	cp README.txt README.html \
	   Windows \
	   FreeDoko_concept.pdf \
	   FreeDoko_rules.pdf \
	   shortrules.pdf \
	   $(RELEASE_TMP)/FreeDoko_$(VERSION)/doc/en/

.PHONY: install
install : all
	mkdir -p $(DESTDIR)$(HELP_DIRECTORY)/en
	cp README.txt README.html \
	   Windows \
	   FreeDoko_concept.pdf \
	   FreeDoko_rules.pdf \
	   shortrules.pdf \
	   $(DESTDIR)$(HELP_DIRECTORY)/en/
endif

ifeq ($(OPERATING_SYSTEM), Linux)
.PHONY: txt
txt : README.txt

README.txt : README.html
	w3m -dump $< > $@
	flip -mb $@ || unix2dos $@


.PHONY: pdf
pdf : FreeDoko_concept.pdf FreeDoko_rules.pdf shortrules.pdf

FreeDoko_concept.pdf : FreeDoko_concept.tex
	pdflatex $< && pdflatex $<
	$(RM) `basename $< .tex`.{log,toc,aux,out}

FreeDoko_rules.pdf : FreeDoko_rules.tex
	pdflatex $< && pdflatex $<
	$(RM) `basename $< .tex`.{log,toc,aux,out}

shortrules.pdf : shortrules.tex
	pdflatex $< && pdflatex $<
	$(RM) `basename $< .tex`.{log,toc,aux,out}
endif

clean :
	$(RM) FreeDoko_concept.pdf FreeDoko_rules.pdf shortrules.pdf
