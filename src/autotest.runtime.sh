#!/bin/sh

# compile FreeDoko and run the runtime test party:
# *  standard rules
# *  limited rounds
# *  profi players


# create program in release version
	  
mkdir -p "/home/tmp/${LOGNAME}/FreeDoko.runtime/src"
rm ./FreeDoko
make "CXXFLAGS=-Wall -Werror -Wno-parentheses -pipe -O2 -std=c++14" \
  "USE_UI_GTKMM=false" \
  "USE_NETWORK=false" \
  "FREEDOKO_WORKING_DIRECTORY=/home/tmp/${LOGNAME}/FreeDoko.runtime/src"

nice -20 ./FreeDoko ./testparty.runtime -F 123 --ui=none "$@"
