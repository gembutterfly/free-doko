# Windows
ifeq ($(OS), Windows_NT)
  	ifeq ($(OSTYPE), msys	)
    	  OPERATING_SYSTEM = Cygwin
  	else
	  OPERATING_SYSTEM = Windows
	endif
	OPERATING_SYSTEM_NAME = Microsoft Windows
endif

# debian, RedHat, Mandrake
ifeq ($(OSTYPE), linux-gnu)
	OPERATING_SYSTEM = Linux
	OPERATING_SYSTEM_NAME = GNU/Linux
endif
# SuSE
ifeq ($(OSTYPE), linux)
	OPERATING_SYSTEM = Linux
	OPERATING_SYSTEM_NAME = GNU/Linux
endif
# Caldera
ifeq ($(OSTYPE), Linux)
	OPERATING_SYSTEM = Linux
	OPERATING_SYSTEM_NAME = GNU/Linux
endif


# for creating windows binary under linux
ifeq ($(OSTYPE), Linux_to_Windows)
	OPERATING_SYSTEM = Linux_to_Windows
	OPERATING_SYSTEM_NAME = Microsoft Windows
endif


# HPUX
ifeq ($(OSTYPE), hpux11.00)
	OPERATING_SYSTEM = HPUX
	OPERATING_SYSTEM_NAME = HPUX 11.00
endif
ifeq ($(OSTYPE), hpux11.11)
	OPERATING_SYSTEM = HPUX
	OPERATING_SYSTEM_NAME = HPUX 11.11
endif
ifeq ($(OSTYPE), hpux11.31)
	OPERATING_SYSTEM = HPUX
	OPERATING_SYSTEM_NAME = HPUX 11.31
endif

# Apple
ifeq ($(OSTYPE), darwin)
	OPERATING_SYSTEM = MACOSX
	OPERATING_SYSTEM_NAME = Mac OS X
endif

# FreeBSD
ifeq ($(OS), FreeBSD)
	OPERATING_SYSTEM = FreeBSD
	OPERATING_SYSTEM_NAME = FreeBSD
endif

# Windows msys
ifeq ($(MSYSTEM), MSYS)
	OPERATING_SYSTEM = MSYS
	OPERATING_SYSTEM_NAME = Microsoft Windows
endif


# Unknown: take linux
ifeq ($(OPERATING_SYSTEM), )
	OPERATING_SYSTEM = Linux
	OPERATING_SYSTEM_NAME = GNU/Linux
endif


# the shelltype
ifeq ($(OPERATING_SYSTEM), Linux)
	SHELLTYPE = sh
endif
ifeq ($(OPERATING_SYSTEM), Linux_to_Windows)
	SHELLTYPE = sh
endif
ifeq ($(OPERATING_SYSTEM), FreeBSD)
 	SHELLTYPE = sh
endif
ifeq ($(OPERATING_SYSTEM), HPUX)
	SHELLTYPE = sh
endif
ifeq ($(OPERATING_SYSTEM), MSYS)
	SHELLTYPE = sh
endif
ifeq ($(OPERATING_SYSTEM), Windows)
ifeq ($(TERM), vt100)
# qnd zsh detection
	SHELLTYPE = sh
	RM = rm
else
	SHELLTYPE = COMMAND.COM
	RM = del
endif
endif
ifeq ($(OPERATING_SYSTEM), Cygwin)
	SHELLTYPE = sh
	OPERATING_SYSTEM = Windows
endif
