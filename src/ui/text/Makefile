DEPTH = ../..
SUBDIR = ./ui/text

include $(DEPTH)/Makefile.os

DIRECTORIES = 

include Makefile.objects

.PHONY: all
ifeq ($(USE_UI_TEXT), true)
all : objects
else
all :
endif


include $(DEPTH)/Makefile.rules
