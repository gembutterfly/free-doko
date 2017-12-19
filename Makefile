DEPTH = .

include $(DEPTH)/Makefile.version
include $(DEPTH)/Makefile.os

include $(DEPTH)/Makefile.install.directories

ifeq ($(OPERATING_SYSTEM), Linux)
	PROGRAM ?= FreeDoko
endif
ifeq ($(OPERATING_SYSTEM), Windows)
	PROGRAM ?= FreeDoko.exe
endif

export PROGRAM


# Edited for Debian GNU/Linux.
DESTDIR = 


# compile FreeDoko and create the documentation"
.PHONY: all
all:
	@echo "short help"
	@echo ""
	@echo "make compile"
	@echo "   Create the binary in the src-directory."
	@echo ""
	@echo "make documentation"
	@echo "   Create the documentation."
	@echo ""
	@echo "make install"
	@echo "   Create the binary and documentation and install FreeDoko."
	@echo "   The paths can be changed in Makefile.directories."
	@echo ""
	@echo "make uninstall"
	@echo "   Remove the files installed by 'make install'."
	@echo ""
	@echo "make help"
	@echo "   Show help for all make targets."


# print the targets of this makefile
.PHONY: help
help :
	@echo "make targets"
	@echo "  help             this help"
	@echo "  compile          compile FreeDoko"
	@echo "  documentation    create the documentation"
	@echo "  Version          update the Version file"
	@echo "  commit           commit the FreeDoko source by svn"
	@echo "  delete           clean the directory"
	@echo "  clean            clean the directory"
	@echo "  chmod            reset the permission flags"
	@echo "  release          create a release"
	@echo "  release_debian           create the debian packages for a release"
	@echo "  release_Linux            create the Linux archive for a release"
	@echo "  release_Windows          create the Windows installer for a release"
	@echo "  release_directory        create the directory for the release files"
	@echo "  release_data             copies the data files in the release directory"
	@echo "  release_linux_binary     creates the linux binary and copies it into the release directory"
	@echo "  release_windows_binary   copies the windows binary into the release directory"
	@echo "  release_windows_libs     copies the windows libs into the release directory"
	@echo "  release_source   copies all data for the release source zip in the release diretory"
	@echo "  install          creates the program and installs it into the system"
	@echo "  uninstall        delete the by 'make install' installed files"
	@echo "  CD               create the FreeDoko CD"
	@echo "  flip             transforms the text files in DOS-format"

# update the 'Version' file
Version : ChangeLog
ifeq ($(SHELLTYPE), sh)
	@echo "Version: "${VERSION}" (alpha)" >Version
	@echo "Date: "`date +%Y-%m-%d` >>Version
endif

.PHONY: compile
compile:
# compile FreeDoko
	$(MAKE) Version
	$(MAKE) -C src/ FreeDoko
	@echo "---"
	@echo "* Program 'FreeDoko' created"

.PHONY: documentation
documentation:
# create the documentation"
	$(MAKE) -C doc/ all
	@echo "---"
	@echo "* documentation created"

# commit the FreeDoko source by svn
# updates the date of the current version in 'freedoko.cpp'
# private target
.PHONY: commit
commit :
	sed 's/^\(\s*::all_versions.push_back(new Version(.*Date(\).*\(), false));\)/\1'"`date +\"%Y, %_2m, %_2d\"`"'\2/' \
	  src/freedoko.cpp \
	  > src/f.cpp \
	  && mv -f src/f.cpp src/freedoko.cpp
	svn commit

# clean the directory
.PHONY: delete
delete : clean

# clean the directory
ifeq ($(SHELLTYPE), sh)
.PHONY: clean
clean :
	rm -f *~
	-@for d in *; \
	do	if test -d $$d; \
		then	if test -e $$d/Makefile; \
			then	$(MAKE) -C $$d clean; \
			fi \
		fi \
	done
	-find . -name "core" -exec rm -f \{\} \;
#	-find . -name ".thumbnails" -exec rm -rf \{\} \;
endif

# fixes the permission flags
ifeq ($(SHELLTYPE), sh)
.PHONY: chmod
chmod :
	find -type d -exec chmod +x \{\} \;
	chmod -R go-w .
	chmod -R a+rX .
	find . ! -type d ! -type l -exec chmod -x \{\} \;
	find -name "c" -exec chmod +x \{\} \;
	find -name "FreeDoko" -exec chmod +x \{\} \;
	find -name "*.bat" -exec chmod +x \{\} \;
	find -name "*.exe" -exec chmod +x \{\} \;
	chmod a+x setup/debian/*/rules
	for f in bin/*; do \
	  (head -n 1 $$f | grep -q '^#!') && chmod +x $$f; \
	done
	@#find . -name "*.cpp" -exec chmod -x \{\} \;
	@#find . -name "*.h" -exec chmod -x \{\} \;
endif

# create a release
# destination is ../FreeDoko_$(VERSION).release
# * source zip
# * linux tgz archive
# * windows zip archive
# * windows setup exe (see setup/Windows/Makefile)
# * debian package (needs debian programs) (see setup/debian/)
# * rpm package (from debian archive)
ifeq ($(OPERATING_SYSTEM), Linux)
.PHONY: release
release : clean
	-chmod -R +w $(RELEASE_DEST)
	-rm -r $(RELEASE_DEST)/*
	mkdir -p $(RELEASE_DEST)
#	source
	$(MAKE) release_source
#	Linux
	$(MAKE) release_Linux
	$(RM) $(RELEASE_TMP)/FreeDoko_$(VERSION)/FreeDoko
#	Windows
#	$(MAKE) release_Windows
#	$(RM) $(RELEASE_TMP)/FreeDoko_$(VERSION)/FreeDoko
#	packages
#	$(MAKE) release_debian

# create the debian packages
.PHONY: release_debian
release_debian:
	$(MAKE) debian_packages
	cd $(RELEASE_DEST) \
	  && rm -rf debian/ \
	  && mkdir -p debian/pool/main/FreeDoko/ \
	  && cp -a /home/install/mirrors/FreeDoko/pool/main/FreeDoko/*$(VERSION)*.deb debian/pool/main/FreeDoko/ \
	  && mkdir -p debian/pool/non-free/FreeDoko/ \
	  && cp -a /home/install/mirrors/FreeDoko/pool/non-free/FreeDoko/*$(VERSION)*.deb debian/pool/non-free/FreeDoko/
	cd $(RELEASE_DEST) \
	  && cp debian/pool/non-free/FreeDoko/freedoko-nonfree_$(VERSION)*.deb .
# Probleme mit dem Setzen der Dateizugehörigkeit auf root -- manuell aufrufen
#	cd $(RELEASE_DEST) \
	  && fakeroot alien --to-rpm freedoko-nonfree_$(VERSION)*.deb
	$(RM) -r $(RELEASE_TMP)


# create the windows release
.PHONY: release_Windows
release_Windows:
	-test -f $(RELEASE_DEST)/FreeDoko_$(VERSION).Windows.zip && rm $(RELEASE_DEST)/FreeDoko_$(VERSION).Windows.zip 
	$(MAKE) release_directory
	fuse-zip $(RELEASE_DEST)/FreeDoko_$(VERSION).Windows.zip $(RELEASE_TMP)
	mkdir $(RELEASE_TMP)/FreeDoko_$(VERSION)
	$(MAKE) release_data
	$(MAKE) release_windows_binary
	$(MAKE) release_windows_libs
	fusermount -u $(RELEASE_TMP)
	#$(RM) -r `find $(RELEASE_TMP)/FreeDoko_$(VERSION)/ -name ".svn"`
	while ! test -f $(RELEASE_DEST)/FreeDoko_$(VERSION).Windows.zip; do sleep 1;done
	fuse-zip -r $(RELEASE_DEST)/FreeDoko_$(VERSION).Windows.zip $(RELEASE_TMP)
	$(MAKE) -C setup/Windows setup
	fusermount -u $(RELEASE_TMP)
	mv setup/Windows/FreeDoko_$(VERSION).Setup.exe $(RELEASE_DEST)/
	-test -f setup/Windows/FreeDoko_$(VERSION)-dev.Setup.exe && mv setup/Windows/FreeDoko_$(VERSION)-dev.Setup.exe $(RELEASE_DEST)/
	chmod +x $(RELEASE_DEST)/*.exe

# create the directory for the release files
# called by target 'release'
.PHONY: release_directory
release_directory :
	-test -d $(RELEASE_TMP) \
	  && ( chmod -R +w $(RELEASE_TMP); \
	  rm -r $(RELEASE_TMP) )
	mkdir -p $(RELEASE_TMP)

# copies the data files in the release directory
# 'release_directory' should be called sometime before
# called by target 'release'
.PHONY: release_data
release_data :
	$(MAKE) Version
	cp AUTHORS COPYING ChangeLog README LIESMICH Version $(RELEASE_TMP)/FreeDoko_$(VERSION)/
	$(MAKE) -C data release
	$(MAKE) -C doc release
	$(MAKE) -C ../manual release

# create the Linux release
.PHONY: release_Linux
release_Linux:
	$(MAKE) release_directory
	echo "UEsFBgAAAAAAAAAAAAAAAAAAAAAAAA==" | base64 -d > $(RELEASE_DEST)/FreeDoko_$(VERSION).Linux.zip 
	fuse-zip $(RELEASE_DEST)/FreeDoko_$(VERSION).Linux.zip $(RELEASE_TMP)
	mkdir $(RELEASE_TMP)/FreeDoko_$(VERSION)
	$(MAKE) release_data
	$(MAKE) release_linux_binary
	#tar czf $(RELEASE_DEST)/FreeDoko_$(VERSION).Linux.tgz -C $(RELEASE_TMP) FreeDoko_$(VERSION)
	fusermount -u $(RELEASE_TMP)

# creates the linux binary and copies it into the release directory
# 'release_directory' should be called sometime before
# called by target 'release'
.PHONY: release_linux_binary
release_linux_binary :
	$(MAKE) -C src release_bin
	cp src/$(PROGRAM) $(RELEASE_TMP)/FreeDoko_$(VERSION)/

# copies the windows binary into the release directory
# 'release_directory' should be called sometime before
# called by target 'release'
# the windows binary is not created, it is only being copied
.PHONY: release_windows_binary
release_windows_binary :
	cp $(FREEDOKO_EXE) $(RELEASE_TMP)/FreeDoko_$(VERSION)/

# copy the libraries for the release files
# called by target 'release'
.PHONY: release_windows_libs
release_windows_libs :
	for d in setup/dll/gtkmm_2.4.dll setup/dll/gnet.dll; do \
	  cd "$$d"; \
	  git ls-files . \
	    | while read f; do \
	        cp --parents "$$f" $(RELEASE_TMP)/FreeDoko_$(VERSION)/; \
	      done; \
	  cd -; \
	done

# copies all data for the release source zip in the release diretory
# called by target 'release'
# this depends on having the source in a git repository
.PHONY: release_source
release_source : release_directory
	-test -f $(RELEASE_DEST)/FreeDoko_$(VERSION).src.zip  && rm $(RELEASE_DEST)/FreeDoko_$(VERSION).src.zip 
	git archive --prefix FreeDoko_$(VERSION)/ -o $(RELEASE_DEST)/FreeDoko_$(VERSION).src.zip HEAD .
	fuse-zip $(RELEASE_DEST)/FreeDoko_$(VERSION).src.zip $(RELEASE_TMP)
	cd "$(RELEASE_TMP)/FreeDoko_$(VERSION)/data/" \
	  && rm -r "cardsets/InnoCard" "cardsets/gnome-games" "cardsets/kdecarddecks" "cardsets/pysol" "iconsets/Georg"
	rm -r "$(RELEASE_TMP)/FreeDoko_$(VERSION)/setup/dll"
	cd ../ \
	  && git ls-files manual \
	  | while read f; do \
	      cp --parents "$$f" $(RELEASE_TMP)/FreeDoko_$(VERSION)/; \
	    done
	fusermount -u $(RELEASE_TMP)
endif

# compiles the program and installs it into the system
# The directories can be changed in 'Makefile.install.directories'
ifeq ($(OPERATING_SYSTEM), Linux)
.PHONY: install
install :
	$(MAKE) \
	  --always-make \
	  "USE_UI_TEXT=false" \
	  "USE_UI_GTKMM=true" \
	  "USE_NETWORK=true" \
	  "CXXFLAGS=-Wall -Werror -Wno-parentheses -pipe -Os -std=c++14" \
	  "CPPFLAGS=-DRELEASE -DVERSION_DESCRIPTION='\"$(OPERATING_SYSTEM_NAME) binary\"' -DPUBLIC_DATA_DIRECTORY_VALUE='\"$(DATA_DIRECTORY)\"' -DMANUAL_DIRECTORY_VALUE='\"$(HELP_DIRECTORY)/manual\"'" \
	  -C src \
	  $(PROGRAM)
	$(MAKE) -C src strip
#	Copying the program
	echo "Copying the program"
	mkdir -p $(DESTDIR)$(BIN_DIRECTORY)
	cp src/FreeDoko $(DESTDIR)$(BIN_DIRECTORY)
	-chgrp games $(DESTDIR)$(BIN_DIRECTORY)/FreeDoko
	chmod 750 $(DESTDIR)$(BIN_DIRECTORY)/FreeDoko
	cd $(DESTDIR)$(BIN_DIRECTORY) && ln -sf FreeDoko freedoko
#	Copying the data files
	echo "Copying the data files"
	$(MAKE) -C data install
#	Copying the documentation
#	SuSE does want the documentation in the directory
#	'/usr/share/doc/packages/freedoko' so adjust it here
	echo "Copying the documentation"
	mkdir -p $(DESTDIR)$(HELP_DIRECTORY)
	cp AUTHORS COPYING ChangeLog $(DESTDIR)$(HELP_DIRECTORY)/
	$(MAKE) -C doc install
#	update the manual page
	echo "update the manual page"
	a2x -f manpage doc/freedoko.asciidoc
	mkdir -p $(DESTDIR)$(MAN_DIRECTORY)
	cp doc/freedoko.6 $(DESTDIR)$(MAN_DIRECTORY)/
	-chgrp games $(DESTDIR)$(MAN_DIRECTORY)/freedoko.6
	chmod 640 $(DESTDIR)$(MAN_DIRECTORY)/freedoko.6
	cd $(DESTDIR)$(MAN_DIRECTORY)/ && ln -sf freedoko.6 FreeDoko.6
#	copy the icon
	echo "copy the icon"
	mkdir -p $(DESTDIR)$(ICON_DIRECTORY)
	cp src/FreeDoko.png $(DESTDIR)$(ICON_DIRECTORY)/
	-chgrp games $(DESTDIR)$(ICON_DIRECTORY)/FreeDoko.png
	chmod 640 $(DESTDIR)$(ICON_DIRECTORY)/FreeDoko.png

.PHONY: uninstall
uninstall :
	@test -f $(DESTDIR)$(BIN_DIRECTORY)/FreeDoko \
	  || (( echo "FreeDoko does not seems to be installed." && false ))
	@echo "Deleting the following files and directories:"
	@ls $(DESTDIR)$(BIN_DIRECTORY)/FreeDoko
	@ls $(DESTDIR)$(BIN_DIRECTORY)/freedoko
	@ls -d $(DESTDIR)$(DATA_DIRECTORY)
	@ls -d $(DESTDIR)$(HELP_DIRECTORY)
	@ls $(DESTDIR)$(HELP_DIRECTORY)/../FreeDoko
	@ls $(DESTDIR)$(MAN_DIRECTORY)/freedoko.6
	@ls $(DESTDIR)$(MAN_DIRECTORY)/FreeDoko.6
	@ls $(DESTDIR)$(ICON_DIRECTORY)/FreeDoko.png
	@echo "Waiting 10 seconds"
	@sleep 10
	@echo "Deleting files.."
	$(RM) $(DESTDIR)$(BIN_DIRECTORY)/FreeDoko
	$(RM) $(DESTDIR)$(BIN_DIRECTORY)/freedoko
	$(RM) $(DESTDIR)$(ICON_DIRECTORY)/FreeDoko.png
	$(RM) $(DESTDIR)$(MAN_DIRECTORY)/freedoko.6
	$(RM) $(DESTDIR)$(MAN_DIRECTORY)/FreeDoko.6
	$(RM) $(DESTDIR)$(HELP_DIRECTORY)/../FreeDoko
	$(RM) -r $(DESTDIR)$(HELP_DIRECTORY)
	$(RM) -r $(DESTDIR)$(DATA_DIRECTORY)
	@echo "...finished uninstall."
endif

# create the debian packages
# see 'setup/debian/Makefile'
ifeq ($(OPERATING_SYSTEM), Linux)
.PHONY: debian_packages
debian_packages :
	$(MAKE) -C setup/debian packages
endif

# create the FreeDoko CD
# uses a script
# needs the release files in ../FreeDoko_$(VERSION).release
ifeq ($(OPERATING_SYSTEM), Linux)
.PHONY: CD
CD :
	./bin/create_cd ${VERSION}
endif

ifeq ($(OPERATING_SYSTEM), Linux)
# copies the whole directory on an usb drive
# private target
# has absolute paths
.PHONY: usb_copy
usb_copy :
	$(MAKE) clean
	-mount /media/usbstick
	tar cz --exclude doxygen \
	     -f /media/usbstick/FreeDoko.tgz -C ../ FreeDoko

# copies the whole directory in the windows partition
# private target
# has absolute paths
.PHONY: Windows_copy
Windows_copy : clean
	-$(RM) -r /home/Daten/Daten.Windows/FreeDoko.bak
	-mv /home/Daten/Daten.Windows/FreeDoko /home/Daten/Daten.Windows/FreeDoko.bak
	-tar chC .. \
	     --exclude .svn \
	     --exclude doxygen \
	     --exclude FreeDoko/homepage/download \
	     FreeDoko \
	   | tar xC /home/Daten/Daten.Windows/
	#touch /home/Daten/Daten.Windows/FreeDoko -t `parsetest now - 2hours`
	cd /home/Daten/Daten.Windows/FreeDoko/src; \
	  for f in `find -name "*.cpp" -o -name "*.h"`; do \
	    if diff -q $$f ../../FreeDoko.bak/src/$$f >/dev/null; then \
	      touch $$f -r ../../FreeDoko; \
	    else \
	      touch $$f -r ../../FreeDoko.bak/src/$$f; \
	    fi \
	  done
	chmod g+w /home/Daten/Daten.Windows/FreeDoko -R
	cd /home/Daten/Daten.Windows/FreeDoko/src; \
	  cp logo.nonfree.png logo.png

# update the FreeDoko directory on the windows partition
# private target
# has absolute paths
.PHONY: Windows_update
Windows_update :
	@for f in $$(find src/  -name "*.cpp" -o -name "*.h"); do \
	  if ! diff -bB $$f /home/Systeme/qemu/FreeDoko/$$f >/dev/null; then \
	    echo "copying $$f"; \
	    cp $$f /home/Systeme/qemu/FreeDoko/$$f; \
	  fi; \
	done
	chmod g+w /home/Systeme/qemu/FreeDoko -R

# create a backup of the whole directory
# private target
# has absolute paths
.PHONY: backup
backup : clean chmod
	@echo tar czf  /Sicherung/FreeDoko/$(VERSION)/FreeDoko_$(VERSION)_`date +%Y-%m-%d_%0k`.dk.tgz FreeDoko
	@cd -P ..;\
	mkdir -p /Sicherung/FreeDoko/$(VERSION);\
	tar czf /Sicherung/FreeDoko/$(VERSION)/FreeDoko_$(VERSION)_`date +%Y-%m-%d_%0k`.dk.tgz \
		--exclude "FreeDoko/src/Makefile.local" \
		FreeDoko
	@chmod a+r-w /Sicherung/FreeDoko/$(VERSION)/FreeDoko_$(VERSION)_`date +%Y-%m-%d_%0k`.dk.tgz

# create a backup of the whole directory
# private target
# has absolute paths
.PHONY: tgz
tgz : clean chmod
	tar -C .. -czf FreeDoko.tgz FreeDoko

# create a backup of the source
# private target
# has absolute paths
.PHONY: src.tgz
src.tgz : clean chmod
	@echo tar czf /Sicherung/FreeDoko/$(VERSION)/FreeDoko_$(VERSION)_`date +%Y-%m-%d_%0k`.dk.src.tgz src ChangeLog Makefile*
	@cd -P ..;\
	mkdir -p /Sicherung/FreeDoko/$(VERSION); \
	tar czf /Sicherung/FreeDoko/$(VERSION)/FreeDoko_$(VERSION)_`date +%Y-%m-%d_%0k`.dk.src.tgz \
	     --exclude 'FreeDoko/src/Makefile.local' \
	     --exclude '.svn' \
	     FreeDoko/Makefile* \
	     FreeDoko/src/ \
	     FreeDoko/ChangeLog
	@chmod a+r-w /Sicherung/FreeDoko/$(VERSION)/FreeDoko_$(VERSION)_`date +%Y-%m-%d_%0k`.dk.src.tgz

# transforms the text files in DOS-format
.PHONY: flip
flip :
	flip -mb AUTHORS COPYING ChangeLog README* Version \
	 || unix2dos AUTHORS COPYING ChangeLog README* Version
	$(MAKE) -C src/ flip
endif
