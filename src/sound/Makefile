DEPTH = ..
SUBDIR = ./sound

include $(DEPTH)/Makefile.os

ifeq ($(USE_SOUND_ALUT), true)
INCLUDE += $(INCLUDE_SOUND_ALUT)
LIBS += $(LIBS_SOUND_ALUT)
endif

include Makefile.objects

.PHONY: all
all : objects


include $(DEPTH)/Makefile.rules
