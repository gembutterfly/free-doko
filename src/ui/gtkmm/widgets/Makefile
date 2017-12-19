DEPTH = ../../..
SUBDIR = ./ui/gtkmm/widgets

include $(DEPTH)/Makefile.os

include Makefile.objects

LIBS = $(LIBS_GTKMM)
INCLUDE = $(INCLUDE_GTKMM)


.PHONY: all
ifeq ($(USE_UI_GTKMM), true)
all : objects
else
all :
endif


include $(DEPTH)/Makefile.rules
