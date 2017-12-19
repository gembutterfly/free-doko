DEPTH = ../../..
SUBDIR = ./ui/gtkmm/dokolounge

include $(DEPTH)/Makefile.os

include Makefile.objects

LIBS = $(LIBS_GTKMM)
INCLUDE = $(INCLUDE_GTKMM)


.PHONY: all
ifeq ($(USE_UI_GTKMM), true)
ifeq ($(USE_NETWORK_DOKOLOUNGE), true)
all : objects
else
all :
endif
else
all :
endif


include $(DEPTH)/Makefile.rules
