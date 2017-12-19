DEPTH = ../..
SUBDIR = ./player/ai

include $(DEPTH)/Makefile.os

include Makefile.objects

.PHONY: all
all : objects


ifeq ($(SHELLTYPE), sh)
.PHONY: documentation
documentation:
	echo '# FreeDoko translation: part aiconfig heuristics description' \
	  >  ../../../data/translations/en/aiconfig.heuristics.description
	echo '# language: english' \
	  >> ../../../data/translations/en/aiconfig.heuristics.description
	echo '' \
	  >> ../../../data/translations/en/aiconfig.heuristics.description

	egrep '^[[:space:]]*// @heuristic' heuristics.cpp \
	  | sed -e 's|[[:space:]]*// @heuristic::name[ ]*\(.*\)|\nAiConfig::Heuristic::Description::\1 =\\|' \
	        -e 's|[[:space:]]*// @heuristic::idea[ ]*\(.*\)|\t\1\\n\\n\\|' \
	        -e 's|[[:space:]]*// @heuristic::comment[ ]*\(.*\)|\t\1\\|' \
	        -e 's|[[:space:]]*// @heuristic::condition[ ]*\(.*\)|\t* \1\\n\\|' \
	        -e 's|[[:space:]]*// @heuristic::action[ ]*\(.*\)|\t\\nAction: \1\\|' \
	  >> ../../../data/translations/en/aiconfig.heuristics.description
endif


include $(DEPTH)/Makefile.rules
