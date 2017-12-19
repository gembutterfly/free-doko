DEPTH = ../..

include $(DEPTH)/Makefile.os
include $(DEPTH)/Makefile.install.directories

#include ../Makefile.latex

.PHONY: all
all: txt pdf

ifeq ($(OPERATING_SYSTEM), Linux)
.PHONY: release
release : all
	mkdir -p $(RELEASE_TMP)/FreeDoko_$(VERSION)/doc/de/
	cp LiesMich.txt LiesMich.html \
	   GPL.de \
	   SuSE \
	   Windows \
	   subversion \
	   FreeDoko_Konzept.pdf \
	   FreeDoko_Regeln.pdf \
	   Kurzregeln.pdf \
	   DDV_Turnierregeln.pdf \
	   DDV_Kurzregeln.pdf \
	   $(RELEASE_TMP)/FreeDoko_$(VERSION)/doc/de/

.PHONY: install
install : all
	mkdir -p $(DESTDIR)$(HELP_DIRECTORY)/de
	cp LiesMich.txt LiesMich.html \
	   GPL.de \
	   SuSE \
	   Windows \
	   subversion \
	   FreeDoko_Konzept.pdf \
	   FreeDoko_Regeln.pdf \
	   Kurzregeln.pdf \
	   DDV_Turnierregeln.pdf \
	   DDV_Kurzregeln.pdf \
	   $(DESTDIR)$(HELP_DIRECTORY)/de/
endif

ifeq ($(OPERATING_SYSTEM), Linux)
.PHONY: txt
txt : LiesMich.txt

LiesMich.txt : LiesMich.html
	w3m -dump $< > $@
	flip -mb $@ || unix2dos $@


.PHONY: pdf
pdf : FreeDoko_Konzept.pdf \
	FreeDoko_Regeln.pdf Kurzregeln.pdf \
	DDV_Turnierregeln.pdf DDV_Kurzregeln.pdf

FreeDoko_Konzept.pdf : FreeDoko_Konzept.tex
	pdflatex $< && pdflatex $<
	$(RM) `basename $< .tex`.{log,toc,aux,out}

FreeDoko_Regeln.pdf : FreeDoko_Regeln.tex
	pdflatex $< && pdflatex $<
	$(RM) `basename $< .tex`.{log,toc,aux,out}

Kurzregeln.pdf : Kurzregeln.tex
	pdflatex $< && pdflatex $<
	$(RM) `basename $< .tex`.{log,toc,aux,out}

DDV_Turnierregeln.pdf : DDV_Turnierregeln.tex
	pdflatex $< && pdflatex $<
	$(RM) `basename $< .tex`.{log,toc,aux,out}

DDV_Kurzregeln.pdf : DDV_Kurzregeln.tex
	pdflatex $< && pdflatex $<
	$(RM) `basename $< .tex`.{log,toc,aux,out}
endif

clean :
	for f in FreeDoko_Konzept FreeDoko_Regeln Kurzregeln DDV_Turnierregeln DDV_Kurzregeln; do \
	  for x in aux log out toc pdf; do \
	    $(RM) $$f.$$x; \
	  done \
	done
	$(RM) FreeDoko_Konzept.pdf \
	      FreeDoko_Regeln.pdf Kurzregeln.pdf \
	      DDV_Turnierregeln.pdf DDV_Kurzregeln.pdf
