DEPTH = ..
SUBDIR = ./network

include $(DEPTH)/Makefile.os

DIRECTORIES = actor gnet file FreeDoko
ifeq ($(USE_NETWORK_DOKOLOUNGE), true)
  DIRECTORIES += DokoLounge
endif

include Makefile.objects

INCLUDE += $(INCLUDE_NETWORK)
LIBS += $(LIBS_NETWORK)

.PHONY: all
all : objects directories


include $(DEPTH)/Makefile.rules
