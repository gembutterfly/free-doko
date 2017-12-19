include $(DEPTH)/../Makefile.os

include $(DEPTH)/Makefile.local
include $(DEPTH)/Makefile.modules

ifeq ($(OPERATING_SYSTEM), Linux)
	# program name
	PROGRAM ?= FreeDoko
	# compiler
	CXX ?= g++
	# strip program
	STRIP ?= strip
	# flags for the compiler (warnings and optimizations)
	#CXXFLAGS ?= -Wall -W -Wno-unused-parameter -Werror -O2 -march=pentium
	#CXXFLAGS ?= -Wall -Werror -pipe -O0 -ggdb
	#CXXFLAGS ?= -Wall -Werror -pipe -O0
	CXXFLAGS ?= -Wall -Werror -Wno-unused-value -Wno-parentheses -pipe -O2 -std=c++14
	# flags for automatic generating of the dependencies
	#DEPGEN_FLAGS ?= -MMD -MP
	DEPGEN_FLAGS ?= -MMD -MP -MT $@ -MF $(@:.o=.d)
	override CPPFLAGS += -DLINUX
	ifeq ($(USE_UI_GTKMM), true)
		INCLUDE_GTKMM ?= $(shell pkg-config --cflags gtkmm-3.0)
		LIBS_GTKMM ?= $(shell pkg-config --libs gtkmm-3.0)
	endif
	ifeq ($(USE_NETWORK), true)
		INCLUDE_NETWORK += $(shell pkg-config --cflags glib-2.0 gnet-2.0)
		LIBS_NETWORK += $(shell pkg-config --libs glib-2.0 gnet-2.0)
	endif
	ifeq ($(USE_SOUND_ALUT), true)
		INCLUDE_SOUND_ALUT ?= $(shell pkg-config --cflags freealut)
		LIBS_SOUND_ALUT ?= $(shell pkg-config --libs freealut)
	endif
	ifeq ($(USE_THREADS), true)
		override LIBS += -lpthread
	endif
	ifeq ($(USE_BOOST), true)
  		LIBS_BOOST ?= -lboost_filesystem -lboost_system
	endif
endif

ifeq ($(OPERATING_SYSTEM), Linux_to_Windows)
	MINGW_DIR ?= /usr/x86_64-w64-mingw32
	PROGRAM ?= FreeDoko.exe
	CXX ?= i686-w64-mingw32-g++
	STRIP ?= strip
	CXXFLAGS ?= -Wall -Werror -std=c++14 -march=pentium -O2
	DEPGEN_FLAGS = -MMD -MP
	override CPPFLAGS += -DWINDOWS
	#C_INCLUDE_PATH ?= $(MINGW_DIR)/include
	#LIBRARY_PATH ?= $(MINGW_DIR)/lib
	#INCLUDE ?= -I$(MINGW_DIR)/include
	#LIBS ?= -I$(MINGW_DIR)/lib
	ifeq ($(USE_UI_GTKMM), true)
#		INCLUDE_GTKMM ?= -I$(MINGW_DIR)/include/gtkmm-2.0 -I$(MINGW_DIR)/lib/gtkmm-2.0/include -I$(MINGW_DIR)/include/gtk-2.0 -I$(MINGW_DIR)/lib/sigc++-1.2/include -I$(MINGW_DIR)/include/sigc++-1.2 -I$(MINGW_DIR)/include/glib-2.0 -I$(MINGW_DIR)/lib/glib-2.0/include -I$(MINGW_DIR)/lib/gtk-2.0/include -I$(MINGW_DIR)/include/pango-1.0 -I$(MINGW_DIR)/include/atk-1.0
#		LIBS_GTKMM ?= -L$(MINGW_DIR)/lib -lgtkmm-2.0 -lgdkmm-2.0 -latkmm-1.0 -lgtk-win32-2.0 -lpangomm-1.0 -lglibmm-2.0 -lsigc-1.2 -lgdk-win32-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lpangowin32-1.0 -lgdi32 -lpango-1.0 -lgobject-2.0 -lgmodule-2.0 -lglib-2.0
		#INCLUDE_GTKMM ?= $(shell pkg-config --cflags gtkmm-3.0)
		#LIBS_GTKMM ?= $(shell pkg-config --libs gtkmm-3.0)
	endif
	ifeq ($(USE_NETWORK), true)
		# all gnet headers in 'include/gnet/'
		# 'libgnet-2.0.a' in 'lib/'
		# 'gnet-2.0.dll' in 'bin/'
		INCLUDE_NETWORK += -I$(MINGW_DIR)/include/gnet -I$(MINGW_DIR)/include/glib-2.0 -I$(MINGW_DIR)/lib/glib-2.0/include 
		LIBS_NETWORK += -lgnet-2.0
	endif
	ifeq ($(USE_THREADS), true)
		override LIBS += -lpthread
	endif
endif

ifeq ($(OPERATING_SYSTEM), FreeBSD)
	PROGRAM ?= FreeDoko
	CXX ?= c++
	STRIP ?= strip
	CXXFLAGS ?= -Wall -Werror -O2 -std=c++14
	DEPGEN_FLAGS ?= -MMD -MP
	override CPPFLAGS += -DLINUX
	ifeq ($(USE_UI_GTKMM), true)
		INCLUDE_GTKMM ?= $(shell pkg-config --cflags gtkmm-3.0)
		LIBS_GTKMM ?= $(shell pkg-config --libs gtkmm-3.0)
	endif
	ifeq ($(USE_NETWORK), true)
		INCLUDE_NETWORK += $(shell pkg-config --cflags glib-2.0 gnet-2.0)
		LIBS_NETWORK += $(shell pkg-config --libs glib-2.0 gnet-2.0)
	endif
	ifeq ($(USE_THREADS), true)
		override LIBS += -lpthread
	endif
endif

ifeq ($(OPERATING_SYSTEM), HPUX)
	PROGRAM ?= FreeDoko
	CXX ?= g++
	STRIP ?= strip
	CXXFLAGS ?= -Wall -Werror -O2
	DEPGEN_FLAGS ?= -MMD -MP
	LIBS ?= -Wl,+s
	override CPPFLAGS += -DHPUX
	ifeq ($(USE_UI_GTKMM), true)
		INCLUDE_GTKMM ?= $(shell pkg-config --cflags gtkmm-3.0)
		LIBS_GTKMM ?= $(shell pkg-config --libs gtkmm-3.0)
	endif
	ifeq ($(USE_NETWORK), true)
		INCLUDE_NETWORK += $(shell pkg-config --cflags glib-2.0 gnet-2.0)
		LIBS_NETWORK += $(shell pkg-config --libs glib-2.0 gnet-2.0)
	endif
	ifeq ($(USE_THREADS), true)
		override LIBS += -lpthread
	endif
endif

ifeq ($(OPERATING_SYSTEM), MACOSX)
	PROGRAM ?= FreeDoko
	CXX ?= g++
	STRIP ?= strip
	CXXFLAGS ?= -Wall -Werror -O2
	DEPGEN_FLAGS ?= -MMD -MP
	override CPPFLAGS += -DLINUX
	override CPPFLAGS += -DOSX
	ifeq ($(USE_UI_GTKMM), true)
		INCLUDE_GTKMM ?= $(shell pkg-config --cflags gtkmm-3.0)
		LIBS_GTKMM ?= $(shell pkg-config --libs gtkmm-3.0)
	endif
	ifeq ($(USE_NETWORK), true)
		INCLUDE_NETWORK += $(shell pkg-config --cflags glib-2.0 gnet-2.0)
		LIBS_NETWORK += $(shell pkg-config --libs glib-2.0 gnet-2.0)
	endif
	ifeq ($(USE_THREADS), true)
		override LIBS += -lpthread
	endif
endif

ifeq ($(OPERATING_SYSTEM), MSYS)
	# program name
	PROGRAM ?= FreeDoko
	# compiler
	CXX ?= g++
	# strip program
	STRIP ?= strip
	# flags for the compiler (warnings and optimizations)
	CXXFLAGS ?= -Wall -Werror -Wno-unused-command-line-argument -Wno-unused-value -Wno-parentheses -pipe -O2 -std=c++14
	# flags for automatic generating of the dependencies
	DEPGEN_FLAGS ?= -MMD -MP -MT $@ -MF $(@:.o=.d)
	ifeq ($(USE_UI_GTKMM), true)
		INCLUDE_GTKMM ?= $(shell pkg-config --cflags gtkmm-3.0)
		LIBS_GTKMM ?= $(shell pkg-config --libs gtkmm-3.0)
	endif
	ifeq ($(USE_NETWORK), true)
		INCLUDE_NETWORK += $(shell pkg-config --cflags glib-2.0 gnet-2.0)
		LIBS_NETWORK += $(shell pkg-config --libs glib-2.0 gnet-2.0)
	endif
	ifeq ($(USE_SOUND_ALUT), true)
	# does not exists
		#INCLUDE_SOUND_ALUT ?= $(shell pkg-config --cflags freealut)
		#LIBS_SOUND_ALUT ?= $(shell pkg-config --libs freealut)
	endif
	ifeq ($(USE_THREADS), true)
		override LIBS += -lpthread
	endif
	ifeq ($(USE_BOOST), true)
  		LIBS_BOOST ?= -lboost_filesystem -lboost_system
	endif
endif

ifeq ($(OPERATING_SYSTEM), MSYS)
	MINGW_DIR ?= C:/MinGW32
	PROGRAM ?= FreeDoko.exe
	CXX ?= $(MINGW_DIR)/bin/g++
	STRIP ?= $(MINGW_DIR)/bin/strip
	CXXFLAGS ?= -Wall -Werror -O2 -march=pentium -mno-cygwin -mms-bitfields
	DEPGEN_FLAGS ?= -MMD -MP
	override CPPFLAGS += -DWINDOWS
	C_INCLUDE_PATH ?= D:/mingw32/include
	LIBRARY_PATH ?= D:/mingw32/lib
	ifeq ($(USE_UI_GTKMM), true)
		INCLUDE_GTKMM ?= $(shell pkg-config --cflags gtkmm-3.0)
		LIBS_GTKMM ?= $(shell pkg-config --libs gtkmm-3.0)
		# because of error with auto-import (see 'ld'-documentation for 'enable-auto-import'
		LIBS_GTKMM += -Wl,-enable-runtime-pseudo-reloc
	endif
	ifeq ($(USE_NETWORK), true)
#		# all gnet headers in 'include/gnet/'
#		# 'libgnet-2.0.a' in 'lib/'
#		# 'gnet-2.0.dll' in 'bin/'
		INCLUDE_NETWORK += \
			-I$(MINGW_DIR)/include/gnet \
			-I$(MINGW_DIR)/include/glib-2.0 \
			-I$(MINGW_DIR)/lib/glib-2.0/include 
		LIBS_NETWORK += -lgnet-2.0
	endif
	ifeq ($(USE_THREADS), true)
		override LIBS += -lpthread
	endif
endif

ifeq ($(OPERATING_SYSTEM), Windows)
	MINGW_DIR ?= C:\MinGW32
	PROGRAM ?= FreeDoko.exe
	CXX ?= $(MINGW_DIR)\bin\g++
	STRIP ?= $(MINGW_DIR)\bin\strip
#	# the '-w' is used instead of '-Wall' because else I get warnings like
#	#     choosing ... over ...
#	#     because conversion sequence for the argument jis better
	CXXFLAGS ?= -w -Werror -O2 -march=pentium -mno-cygwin -mms-bitfields
#	#CXXFLAGS ?= -Wall -Werror -O2 -march=pentium -mno-cygwin -mms-bitfields
	DEPGEN_FLAGS ?= -MMD -MP
	override CPPFLAGS += -DWINDOWS
	C_INCLUDE_PATH ?= $(MINGW_DIR)/include
	LIBRARY_PATH ?= $(MINGW_DIR)/lib
#	# for a windows application (without DOS-prompt)
#	#LIBS ?= -mwindows
	ifeq ($(USE_UI_GTKMM), true)
#		# version 2.8
#		# adjusted output of 'pkg-config --cflags gtkmm-2.4'
		INCLUDE_GTKMM ?= $(shell pkg-config --cflags gtkmm-2.4)
		LIBS_GTKMM ?= $(shell pkg-config --libs gtkmm-2.4)
#		# because of error with auto-import (see 'ld'-documentation for 'enable-auto-import'
		LIBS_GTKMM += -Wl,-enable-runtime-pseudo-reloc
	endif
	ifeq ($(USE_NETWORK), true)
#		# all gnet headers in 'include/gnet/'
#		# 'libgnet-2.0.a' in 'lib/'
#		# 'gnet-2.0.dll' in 'bin/'
		INCLUDE_NETWORK += \
			-I$(MINGW_DIR)/include/gnet \
			-I$(MINGW_DIR)/include/glib-2.0 \
			-I$(MINGW_DIR)/lib/glib-2.0/include 
		LIBS_NETWORK += -L$(MINGW_DIR)/lib \
			  	-lgnet-2.0
#		LIBS_NETWORK += -rpath gnet.dll
	endif
	ifeq ($(USE_SOUND_ALUT), true)
		INCLUDE_SOUND_ALUT ?= -I$(MINGW_DIR)/include/alut
		LIBS_SOUND_ALUT ?= -L$(MINGW_DIR)/lib \
				   -lalut -lopenal32
	endif
	ifeq ($(USE_SOUND_PLAYSOUND), true)
		LIBS_SOUND_PLAYSOUND ?= -L$(MINGW_DIR)/lib \
				        -lwinmm
	endif
	ifeq ($(USE_THREADS), true)
		override LIBS += -lpthreadGCE2
	endif
	ifeq ($(USE_BOOST), true)
		INCLUDE_BOOST ?= -I$(MINGW_DIR)/include/boost
		LIBS_BOOST ?= -L$(MINGW_DIR)/lib/boost -lboost_filesystem-mgw34-1_42 -lboost_system-mgw34-1_42
	endif
endif
ifeq ($(USE_UI_TEXT), true)
	override CPPFLAGS += -DUSE_UI_TEXT
endif
ifeq ($(USE_UI_GTKMM), true)
	override CPPFLAGS += -DUSE_UI_GTKMM
endif
ifeq ($(USE_SOUND), true)
	override CPPFLAGS += -DUSE_SOUND
ifeq ($(USE_SOUND_ALUT), true)
	override CPPFLAGS += -DUSE_SOUND_ALUT
endif
ifeq ($(USE_SOUND_COMMAND), true)
	override CPPFLAGS += -DUSE_SOUND_COMMAND
endif
ifeq ($(USE_SOUND_PLAYSOUND), true)
	override CPPFLAGS += -DUSE_SOUND_PLAYSOUND
endif
endif
ifeq ($(USE_NETWORK), true)
	override CPPFLAGS += -DUSE_NETWORK
ifeq ($(USE_NETWORK_DOKOLOUNGE), true)
	override CPPFLAGS += -DUSE_NETWORK_DOKOLOUNGE
endif
endif
ifeq ($(USE_THREADS), true)
	override CPPFLAGS += -DUSE_THREADS
endif
ifeq ($(USE_BOOST), true)
	override CPPFLAGS += -DBOOST
endif

# path to the generated .o and .d files
# should be changed in 'Makefile.local'
WORKING_DIR ?= $(DEPTH)
