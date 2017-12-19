DEPTH = ../..
SUBDIR = ./player/aiconfigs.preset

include $(DEPTH)/Makefile.os

.PHONY: all
all : aiconfigs


.PHONY: aiconfigs
aiconfigs: standard.defensive standard.offensive profi profi.aggressive profi.unfair

standard.defensive : standard.defensive.saved
ifeq ($(SHELLTYPE), sh)
	sh ./create_standard_aiconfigs
endif

standard.offensive : standard.defensive.saved
profi : standard.defensive.saved
profi.aggressive : standard.defensive.saved
profi.unfair : standard.defensive.saved
novice : standard.defensive.saved

