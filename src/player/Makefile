DEPTH = ..
SUBDIR = ./player

include $(DEPTH)/Makefile.os

DIRECTORIES = rating human ai ai.dummy ai.random network
#aiconfigs.preset

include Makefile.objects

.PHONY: all
all : directories objects 

$(TARGET_DIR_LOCAL)aiconfig.o : aiconfigs.preset

include $(DEPTH)/Makefile.rules
