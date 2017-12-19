DEPTH = ..
SUBDIR = ./ui

include $(DEPTH)/Makefile.os

DIRECTORIES = 

include Makefile.objects
ifeq ($(USE_UI_TEXT), true)
DIRECTORIES += text
endif
ifeq ($(USE_UI_GTKMM), true)
INCLUDE += $(INCLUDE_GTKMM)
DIRECTORIES += gtkmm
endif

.PHONY: all
all : objects directories


include $(DEPTH)/Makefile.rules
