#!/bin/sh

# compile FreeDoko and run the test party:
# *  mutating rules
# *  no end of the party
# *  four different ai players (standard, profi, offensive, profi unfair)

# non-release-version

make \
  USE_UI_GTKMM=false \
  USE_SOUND=false \
  USE_NETWORK=false \
  USE_THREADS=false \
  CXXFLAGS="-Wall -Werror -Wno-parentheses -pipe -O0 -ggdb -std=c++14" \
  FREEDOKO_WORKING_DIRECTORY="/home/tmp/${LOGNAME}/FreeDoko.autotest.heuristics" \
  && ./FreeDoko testparty.random -F 251 --no-automatic-savings "$@"
