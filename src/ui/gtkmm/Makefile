DEPTH = ../..
SUBDIR = ./ui/gtkmm

include $(DEPTH)/Makefile.os

ifeq ($(USE_UI_GTKMM), true)
INCLUDE += $(INCLUDE_GTKMM)
LIBS += $(LIBS_GTKMM)
endif

DIRECTORIES = widgets
ifeq ($(USE_NETWORK_DOKOLOUNGE), true)
  DIRECTORIES += dokolounge
endif

include Makefile.objects

.PHONY: all
ifeq ($(USE_UI_GTKMM), true)
all : objects directories
else
all :
endif

# private
.PHONY: convert_doxygen
convert_doxygen :
	for f in *.cpp; do \
	sed -e "s/@param\t/@param     /g" \
	    -e "s/@return\t/@return    /g" \
	    -e "s/@author\t/@author    /g" \
	    -e "s/@version\t/@version   /g" \
	    -e "s/\(@param     [a-z_]\+\)\t/\1   /g" \
	    -e "s/\(@param     [a-z_]\+\)  \([a-z_]\)/\1   \2/g" \
	    < $$f > t.cpp \
	  && mv t.cpp $$f; \
	done


include $(DEPTH)/Makefile.rules
