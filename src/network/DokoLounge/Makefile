DEPTH = ../..
SUBDIR = ./network/DokoLounge

include $(DEPTH)/Makefile.os

DIRECTORIES = parser

include Makefile.objects

INCLUDE += $(INCLUDE_NETWORK)
LIBS += $(LIBS_NETWORK)

.PHONY: all
all : objects directories


include $(DEPTH)/Makefile.rules
