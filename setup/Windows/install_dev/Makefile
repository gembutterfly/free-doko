# Laedt Entwicklungsumgebung msys, Bibliotheken gtkmm und gnet sowie FreeDoko
# und erstellt FreeDoko Programm
# Getestet unter Microsoft Windows XP, wird bestimmt nicht unter Windows 98 funktionieren.
#
# 2009-01-18 Diether Knof



#
# Umgebung
#
# uebergeordnetes Verzeichnis
# set FREEDOKO_DIRECTORY=C:\install\FreeDoko.build
#set FREEDOKO_DIRECTORY=%CD%
# Verzeichnis fuer die Programme
#set BIN_DIR=%FREEDOKO_DIRECTORY%\bin
# Verzeichnis fuer die download
#set DOWNLOAD_DIR=%FREEDOKO_DIRECTORY%\download
# Verzeichnis fuer MinGW32
#set MINGW_DIR=%FREEDOKO_DIRECTORY%\MinGW32
# Verzeichnis fuer den FreeDoko Quelltext
#set FREEDOKO_TRUNK_DIR=%FREEDOKO_DIRECTORY%\trunk
# Verzeichnis fuer die weiteren Datendateien von FreeDoko
#set FREEDOKO_DATA_DIR=%FREEDOKO_DIRECTORY%\more_data
# Verzeichnis fuer das FreeDoko Handbuch
#set FREEDOKO_MANUAL_DIRECTORY=%FREEDOKO_DIRECTORY%\manual
# Verzeichnis fuer die FreeDoko Installations-Dateien
#set FREEDOKO_INSTALL_DEV_DIR=%FREEDOKO_DIRECTORY%\install_dev

# Programme
#   wget
#set WGET=%BIN_DIR%\wget.exe --no-clobber --no-directories --directory-prefix=%DOWNLOAD_DIR%
#   Entpacker
#set UNCOMPRESS=%BIN_DIR%\7z.exe x -aoa -y
#   md5
#set MD5SUM=%BIN_DIR%\md5.exe
#   svn
#set SVN=%MINGW_DIR%\bin\svn.exe

VERSION=0.7.9

.PHONY: help
help :
	@echo Aufruf:
	@echo help:  gibt diese Hilfe aus
	@echo info:  gibt Informationen ueber den Sinn aus
	@echo clean: loescht alle erzeugten Dateien
	@echo install: fuehrt die Installation durch
	@echo install_mingw: installiert die MinGW-Umgebung
	@echo install_svn: installiert subversion
	@echo install_pthreads: installiert die Bibliothek pthreads
	@echo install_boost: installiert die Bibliothek boost
	@echo install_gtk: installiert die Bibliothek gtk
	@echo install_gtkmm: installiert die Bibliothek gtkmm
	@echo install_gnet: installiert die Bibliothek gnet
	@echo install_alut: installiert die Bibliothek alut
	@echo install_FreeDoko: installiert FreeDoko
	@echo install_FreeDoko_src: installiert den FreeDoko Quelltext
	@echo install_FreeDoko_nonfree-cardsets: installiert unfreie Kartensaetze für FreeDoko
	@echo install_FreeDoko_more_data: installiert weitere Datendateien von FreeDoko
	@echo install_FreeDoko_more_cardsets: installiert weitere Kartensätze
	@echo install_FreeDoko_manual: installiert das Handbuch von FreeDoko
	@echo install_FreeDoko_shortcuts: erstellt Verknuepfungen von FreeDoko
	@echo install_FreeDoko_update: erstellt das Aktualisierungsskript von FreeDoko
	@echo install_FreeDoko_install_scripts: installiert die Installationsskripte von FreeDoko
	@echo update:  aktualisiert FreeDoko und ggf. die Entwicklungsumgebung
	@echo update_FreeDoko_src: aktualisiert den Quelltext
	@echo update_FreeDoko_more_data: aktualisiert die zusätzlichen Daten
	@echo update_FreeDoko_manual: aktualisiert das Handbuch von FreeDoko
	@echo compile: kompiliert FreeDoko

# Informationen ueber das Skript
.PHONY: info
info :
	echo ----
	echo Dieses Skript laedt die Entwicklungsumgebung msys, die Bibliotheken gtkmm und gnet sowie FreeDoko und erstellt das FreeDoko Programm.
	echo Es werden etwa 80 MB heruntergeladen
	echo Getestet unter Microsoft Windows XP, wird wahrscheinlich nicht unter Windows 98 funktionieren.
	echo ----


# loescht alle erzeugten Dateien
.PHONY: clean
clean :
	if exist $(TARGET_DIR) rmdir /s /q $(TARGET_DIR)
	if exist $(MINGW_DIR) rmdir /s /q $(MINGW_DIR)
	if exist $(FREEDOKO_TRUNK_DIR) rmdir /s /q $(FREEDOKO_TRUNK_DIR)
	if exist $(FREEDOKO_DATA_DIR) rmdir /s /q $(FREEDOKO_DATA_DIR)
	if exist $(FREEDOKO_MANUAL_DIRECTORY) rmdir /s /q $(FREEDOKO_MANUAL_DIRECTORY)
	if exist $(FREEDOKO_INSTALL_DEV_DIR) rmdir /s /q $(FREEDOKO_INSTALL_DEV_DIR)

# fuehrt die Installation durch
.PHONY: install
install : directories
	$(MAKE) install_FreeDoko_manual \
		install_FreeDoko \
		compile_FreeDoko \
		install_FreeDoko_shortcuts \
		install_FreeDoko_update

# erstellt die verwendeten Verzeichnisse:
# download, target, MinGW
.PHONY: directories
directories :
	if not exist %DOWNLOAD_DIR% mkdir %DOWNLOAD_DIR%
	if not exist %MINGW_DIR% mkdir %MINGW_DIR%
	if not exist %TARGET_DIR% mkdir %TARGET_DIR%

# installiert die MinGW-Umgebung
.PHONY: install_mingw
install_mingw : $(TARGET_DIR)\install_mingw
$(TARGET_DIR)\install_mingw :
	$(MAKE) "LOCATION=http://%SOURCEFORGE_DOWNLOAD%/mingw" \
	        "FILE=mingwrt-3.15.2-mingw32-dev.tar.gz" \
	        "MD5SUM=F24D63744AF66B54547223BD5476B8F0" \
	        fetch
#	$(MAKE) "LOCATION=http://%SOURCEFORGE_DOWNLOAD%/mingw" \
#	        "FILE=mingw32-make-3.80.0-3.tar.gz" \
#	        "MD5SUM=A68E5A25917EB1BB6AA1D359EEC47B4B" \
#	        fetch
	$(MAKE) "LOCATION=http://%SOURCEFORGE_DOWNLOAD%/mingw" \
	        "FILE=binutils-2.19-mingw32-bin.tar.gz" \
	        "MD5SUM=FF419D2C82028D20EFCC6AC98620F7E3" \
	        fetch
	$(MAKE) "LOCATION=http://%SOURCEFORGE_DOWNLOAD%/mingw" \
	        "FILE=gcc-core-3.4.5-20060117-3.tar.gz" \
	        "MD5SUM=33C9FDF7468A3C410F830471D0E3FFB8" \
	        fetch
	$(MAKE) "LOCATION=http://%SOURCEFORGE_DOWNLOAD%/mingw" \
	        "FILE=gcc-g++-3.4.5-20060117-3.tar.gz" \
	        "MD5SUM=B3E7875D160FD8DF2B13C5448CA78C4E" \
	        fetch
	$(MAKE) "LOCATION=http://%SOURCEFORGE_DOWNLOAD%/mingw" \
	        "FILE=w32api-3.13-mingw32-dev.tar.gz" \
	        "MD5SUM=A50FFF6BC1E1542451722E2650CB53B4" \
	        fetch
	$(MAKE) "FILE=mingwrt-3.15.2-mingw32-dev.tar.gz" \
	        extract_tar_gz
#	$(MAKE) "FILE=mingw32-make-3.80.0-3.tar.gz" \
#	        extract_tar_gz
	$(MAKE) "FILE=binutils-2.19-mingw32-bin.tar.gz" \
	        extract_tar_gz
	$(MAKE) "FILE=gcc-core-3.4.5-20060117-3.tar.gz" \
	        extract_tar_gz
	$(MAKE) "FILE=gcc-g++-3.4.5-20060117-3.tar.gz" \
	        extract_tar_gz
	$(MAKE) "FILE=w32api-3.13-mingw32-dev.tar.gz" \
	        extract_tar_gz
	echo "3.4.5" > $@

# installiert subversion
.PHONY: install_svn
install_svn : $(TARGET_DIR)\install_svn
$(TARGET_DIR)\install_svn :
	$(MAKE) "LOCATION=http://subversion.tigris.org/downloads" \
	        "FILE=svn-win32-1.4.2.zip" \
	        "MD5SUM=32DBFF085330BF23033A5215FB171911" \
	        fetch
	echo   Entpacke subversion: svn-win32-1.4.2.zip
	$(UNCOMPRESS) $(DOWNLOAD_DIR)\svn-win32-1.4.2.zip
	move /Y svn-win32-1.4.2\README.txt $(MINGW_DIR)\Subversion.README.txt >NUL
	xcopy /S /Y svn-win32-1.4.2 $(MINGW_DIR)
	rmdir /s /q svn-win32-1.4.2
	echo "1.4.2" > $@

# installiert die Bibliothek pthreads
.PHONY: install_pthreads
install_pthreads : $(TARGET_DIR)\install_pthreads
$(TARGET_DIR)\install_pthreads :
	$(MAKE) "LOCATION=ftp://sourceware.org/pub/pthreads-win32" \
	        "FILE=pthreads-w32-2-8-0-release.tar.gz" \
	        "MD5SUM=6D30C693233B1464EF8983FEDD8CCB22" \
	        fetch
	echo   Erstelle und installiere die pthreads-Bibliothek
	$(UNCOMPRESS) $(DOWNLOAD_DIR)\pthreads-w32-2-8-0-release.tar.gz
	$(UNCOMPRESS) pthreads-w32-2-8-0-release.tar
	del pthreads-w32-2-8-0-release.tar
	$(MAKE) --silent -C pthreads-w32-2-8-0-release GC-inlined
	copy pthreads-w32-2-8-0-release\pthread.h $(MINGW_DIR)\include 
	copy pthreads-w32-2-8-0-release\libpthreadGC2.a $(MINGW_DIR)\lib 
	copy pthreads-w32-2-8-0-release\pthreadGC2.dll $(MINGW_DIR)\bin 
	rmdir /s /q pthreads-w32-2-8-0-release
	echo "2.8.0" > $@

# installiert die Bibliothek boost
.PHONY: install_boost
install_boost : $(TARGET_DIR)\install_boost
$(TARGET_DIR)\install_boost :
	$(MAKE) install_mingw
	$(MAKE) install_pthreads
	$(MAKE) "LOCATION=http://%SOURCEFORGE_DOWNLOAD%/boost" \
	        "FILE=boost_1_42_0.zip" \
	        "MD5SUM=CEB78ED309C867E49DC29F60BE841B64" \
	        fetch
	echo   Erstelle und installiere Teile der boost-Bibliothek
	echo   Fehlermeldungen koennen ignoriert werden, solange es keinen Absturz gibt.
	$(MAKE) "FILE=boost_1_42_0.zip" extract_zip
	echo cd $(MINGW_DIR)\boost_1_42_0\tools\jam >create_bjam.bat
	echo call build_dist.bat gcc >>create_bjam.bat
	echo call build.bat gcc >>create_bjam.bat
	echo copy  bin.ntx86\bjam.exe $(MINGW_DIR)\bin>>create_bjam.bat
	call create_bjam.bat
	del create_bjam.bat
	echo cd $(MINGW_DIR)\boost_1_42_0 >create_boost.bat
#	echo $(MINGW_DIR)\bin\bjam.exe --show-libraries >>create_boost.bat
	echo $(MINGW_DIR)\bin\bjam.exe -q \
	     --with-date_time \
	     --with-filesystem \
	     --with-iostreams \
	     --with-program_options \
	     --with-signals \
	     --with-system \
	     --with-thread \
	     toolset=gcc \
	     link=shared \
	     runtime-link=shared \
	     threading=single \
	     release \
	     >>create_boost.bat
	echo   Auch Warnungen koennen ignoriert werden, solange es keinen Absturz gibt.
	call create_boost.bat
	del create_boost.bat
	copy $(MINGW_DIR)\boost_1_42_0\LICENSE_1_0.txt $(MINGW_DIR)\LICENSE_boost_1_0.txt
	-mkdir $(MINGW_DIR)\lib\boost
	copy /Y $(MINGW_DIR)\boost_1_42_0\stage\lib\*.lib $(MINGW_DIR)\lib\boost
	copy /Y $(MINGW_DIR)\boost_1_42_0\stage\lib\*.dll $(MINGW_DIR)\bin
	-mkdir $(MINGW_DIR)\include\boost
	xcopy /S /Y $(MINGW_DIR)\boost_1_42_0\boost $(MINGW_DIR)\include\boost
	rmdir /s /q $(MINGW_DIR)\boost_1_42_0
	echo "1.42.0" > $@

# installiert die Bibliothek gtk
.PHONY: install_gtk
install_gtk : $(TARGET_DIR)\install_gtk
$(TARGET_DIR)\install_gtk :
	$(MAKE) "LOCATION=ftp://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/old" \
	        "FILE=pkg-config-0.23-2.zip" \
	        "MD5SUM=33482D62CA4A07DC7A13AF4324999170" \
	        fetch
	$(MAKE) "LOCATION=ftp://ftp.gnome.org/pub/gnome/binaries/win32/glib/2.12" \
	        "FILE=glib-2.12.11.zip" \
	        "MD5SUM=04F58BB1C6DA05B33AF6B4596B47ED33" \
	        fetch
	$(MAKE) "LOCATION=ftp://ftp.gnome.org/pub/gnome/binaries/win32/glib/2.12" \
	        "FILE=glib-dev-2.12.11.zip" \
	        "MD5SUM=B9905CBC058BEB83A1D07DBC2EA8E874" \
	        fetch
	$(MAKE) "LOCATION=ftp://ftp.gnome.org/pub/gnome/binaries/win32/atk/1.18" \
	        "FILE=atk-1.18.0.zip" \
	        "MD5SUM=090C5FA305D7C542C136B14AD39E00EA" \
	        fetch
	$(MAKE) "LOCATION=ftp://ftp.gnome.org/pub/gnome/binaries/win32/atk/1.18" \
	        "FILE=atk-dev-1.18.0.zip" \
	        "MD5SUM=E8CE4864D2D9FE822C4009A575A00712" \
	        fetch
	$(MAKE) "LOCATION=ftp://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/old" \
	        "FILE=cairo-1.4.6.zip" \
	        "MD5SUM=B206371D6D6E90E238C4EEABF7493E60" \
	        fetch
	$(MAKE) "LOCATION=ftp://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/old" \
	        "FILE=cairo-dev-1.4.6.zip" \
	        "MD5SUM=EE443122980A340AA59EE25741D9FE3E" \
	        fetch
	$(MAKE) "LOCATION=ftp://ftp.gnome.org/pub/gnome/binaries/win32/pango/1.16" \
	        "FILE=pango-1.16.1.zip" \
	        "MD5SUM=4DF41C118B56B15B1764297B3153DF27" \
	        fetch
	$(MAKE) "LOCATION=ftp://ftp.gnome.org/pub/gnome/binaries/win32/pango/1.16" \
	        "FILE=pango-dev-1.16.1.zip" \
	        "MD5SUM=3408544BCCA1B47D2308DA919B55ADDA" \
	        fetch
	$(MAKE) "LOCATION=ftp://ftp.gnome.org/pub/gnome/binaries/win32/gtk+/2.10" \
	        "FILE=gtk+-2.10.11.zip" \
	        "MD5SUM=3FC6B1287300C5E6E9D5DE2C4CCE1BB4" \
	       fetch
	$(MAKE) "LOCATION=ftp://ftp.gnome.org/pub/gnome/binaries/win32/gtk+/2.10" \
	        "FILE=gtk+-dev-2.10.11.zip" \
	        "MD5SUM=4AF595A2C61AA4198D77E7C228A0B19F" \
	       fetch
	$(MAKE) "LOCATION=ftp://ftp.gnome.org/pub/gnome/binaries/win32/dependencies" \
		"FILE=libiconv-1.9.1.bin.woe32.zip" \
	        "MD5SUM=A18E9420D6A354B585A77230996B4171" \
	       fetch
	$(MAKE) "LOCATION=ftp://ftp.gnome.org/pub/gnome/binaries/win32/dependencies" \
		"FILE=gettext-runtime_0.18.1.1-2_win32.zip" \
	        "MD5SUM=C2AA52EB2DE5B3BA6659A22835B6B563" \
	       fetch
	$(MAKE) "LOCATION=ftp://ftp.gnome.org/pub/gnome/binaries/win32/dependencies" \
		"FILE=gettext-runtime-dev_0.18.1.1-2_win32.zip" \
	        "MD5SUM=A3969CAF51BCC92C58FF30FABD1B707B" \
	       fetch
	$(MAKE) "LOCATION=ftp://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/old" \
		"FILE=libpng-1.2.8.zip" \
	        "MD5SUM=1450F8B4DDB7159A05D779F4B887A494" \
	       fetch
	$(MAKE) "LOCATION=ftp://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/old" \
		"FILE=libpng-dev-1.2.8.zip" \
	        "MD5SUM=A1F7FB1628D1C4670E1A5B18D942B31C" \
	       fetch
	$(MAKE) "LOCATION=ftp://ftp.gnome.org/pub/gnome/binaries/win32/dependencies" \
		"FILE=libjpeg-6b-4.zip" \
	        "MD5SUM=DD7E44F7225DD9063A98D01AD2E49022" \
	       fetch
	$(MAKE) "LOCATION=ftp://ftp.gnome.org/pub/gnome/binaries/win32/dependencies" \
		"FILE=zlib-1.2.3.zip" \
	        "MD5SUM=02D56C7570A67DE8552527DAAF742F62" \
	       fetch
	$(MAKE) "FILE=pkg-config-0.23-2.zip" \
	        extract_zip
	$(MAKE) "FILE=glib-2.12.11.zip" \
	        extract_zip
	$(MAKE) "FILE=glib-dev-2.12.11.zip" \
	        extract_zip
	$(MAKE) "FILE=cairo-1.4.6.zip" \
	        extract_zip
	$(MAKE) "FILE=cairo-dev-1.4.6.zip" \
	        extract_zip
	$(MAKE) "FILE=atk-1.18.0.zip" \
	        extract_zip
	$(MAKE) "FILE=atk-dev-1.18.0.zip" \
	        extract_zip
	$(MAKE) "FILE=pango-1.16.1.zip" \
	        extract_zip
	$(MAKE) "FILE=pango-dev-1.16.1.zip" \
	        extract_zip
	$(MAKE) "FILE=gtk+-2.10.11.zip" \
	        extract_zip
	$(MAKE) "FILE=gtk+-dev-2.10.11.zip" \
	        extract_zip
	$(MAKE) "FILE=libiconv-1.9.1.bin.woe32.zip" \
	        extract_zip
	$(MAKE) "FILE=gettext-runtime_0.18.1.1-2_win32.zip" \
	        extract_zip
	$(MAKE) "FILE=gettext-runtime-dev_0.18.1.1-2_win32.zip" \
	        extract_zip
	$(MAKE) "FILE=libpng-1.2.8.zip" \
	        extract_zip
	$(MAKE) "FILE=libpng-dev-1.2.8.zip" \
	        extract_zip
	$(MAKE) "FILE=libjpeg-6b-4.zip" \
	        extract_zip
	$(MAKE) "FILE=zlib-1.2.3.zip" \
	        extract_zip
# ToDo: write the gtkrc
# echo gtk-theme-name = "Default" > $$_OUTDIR\etc\gtk-2.0\gtkrc
	echo "2.10.11" > $@

# installiert die Bibliothek gtkmm
.PHONY: install_gtkmm
install_gtkmm : $(TARGET_DIR)\install_gtkmm
$(TARGET_DIR)\install_gtkmm :
	$(MAKE) install_gtk
	$(MAKE) "LOCATION=ftp://ftp.gnome.org/pub/gnome/binaries/win32/gtkmm/2.10" \
               "FILE=	gtkmm-win32-devel-2.10.8-1.exe" \
	       "MD5SUM=8BCE3BC5ADC755A1B162FF93066CC1F4" \
	       fetch
	echo   Entpacke gtkmm: gtkmm-win32-devel-2.10.8-1.exe
	$(UNCOMPRESS) -o$(MINGW_DIR) $(DOWNLOAD_DIR)\gtkmm-win32-devel-2.10.8-1.exe
	move  $(MINGW_DIR)\$$_OUTDIR\*.lib $(MINGW_DIR)\lib >NUL
	move  $(MINGW_DIR)\$$_OUTDIR\atkmm-1.6 $(MINGW_DIR)\include >NUL
	move  $(MINGW_DIR)\\$$_OUTDIR\cairomm-1.0 $(MINGW_DIR)\include >NUL
	mkdir $(MINGW_DIR)\lib\gdkmm-2.4
	move  $(MINGW_DIR)\\$$_OUTDIR\gdkmm-2.4\include $(MINGW_DIR)\lib\gdkmm-2.4 >NUL
	move  $(MINGW_DIR)\$$_OUTDIR\gdkmm-2.4 $(MINGW_DIR)\include >NUL
	mkdir $(MINGW_DIR)\lib\glibmm-2.4
	move  $(MINGW_DIR)\$$_OUTDIR\glibmm-2.4\include $(MINGW_DIR)\lib\glibmm-2.4 >NUL
	move  $(MINGW_DIR)\$$_OUTDIR\glibmm-2.4\proc $(MINGW_DIR)\lib\glibmm-2.4 >NUL
	rmdir /s /q $(MINGW_DIR)\$$_OUTDIR\glibmm-2.4\docs
	rmdir /s /q $(MINGW_DIR)\$$_OUTDIR\glibmm-2.4\examples
	move  $(MINGW_DIR)\$$_OUTDIR\glibmm-2.4 $(MINGW_DIR)\include >NUL
	mkdir $(MINGW_DIR)\lib\gtkmm-2.4
	move  $(MINGW_DIR)\$$_OUTDIR\gtkmm-2.4\include $(MINGW_DIR)\lib\gtkmm-2.4 >NUL
	move  $(MINGW_DIR)\$$_OUTDIR\gtkmm-2.4\proc $(MINGW_DIR)\lib\gtkmm-2.4 >NUL
	rmdir /s /q $(MINGW_DIR)\$$_OUTDIR\gtkmm-2.4\docs
	rmdir /s /q $(MINGW_DIR)\$$_OUTDIR\gtkmm-2.4\tests
	move  $(MINGW_DIR)\$$_OUTDIR\gtkmm-2.4 $(MINGW_DIR)\include >NUL
	mkdir $(MINGW_DIR)\lib\libglademm-2.4
	move  $(MINGW_DIR)\$$_OUTDIR\libglademm-2.4\include $(MINGW_DIR)\lib\libglademm-2.4 >NUL
	move  $(MINGW_DIR)\$$_OUTDIR\libglademm-2.4\proc $(MINGW_DIR)\lib\libglademm-2.4 >NUL
	rmdir /s /q $(MINGW_DIR)\$$_OUTDIR\libglademm-2.4\docs
	del /q $(MINGW_DIR)\$$_OUTDIR\libglademm-2.4\libglademm-2.4.devhelp
	move  $(MINGW_DIR)\$$_OUTDIR\libglademm-2.4 $(MINGW_DIR)\include >NUL
	mkdir $(MINGW_DIR)\lib\libxml++-2.6
	move  $(MINGW_DIR)\$$_OUTDIR\libxml++-2.6\include $(MINGW_DIR)\lib\libxml++-2.6 >NUL
	rmdir /s /q $(MINGW_DIR)\$$_OUTDIR\libxml++-2.6\docs
	move  $(MINGW_DIR)\$$_OUTDIR\libxml++-2.6 $(MINGW_DIR)\include >NUL
	move  $(MINGW_DIR)\$$_OUTDIR\pangomm-1.4 $(MINGW_DIR)\include >NUL
	mkdir $(MINGW_DIR)\lib\sigc++-2.0
	move  $(MINGW_DIR)\$$_OUTDIR\sigc++-2.0\include $(MINGW_DIR)\lib\sigc++-2.0 >NUL
	move  $(MINGW_DIR)\$$_OUTDIR\sigc++-2.0 $(MINGW_DIR)\include >NUL
	rmdir /s /q $(MINGW_DIR)\$$PLUGINSDIR
	rmdir /s /q $(MINGW_DIR)\$$_OUTDIR
	echo "2.10.8" > $@

# installiert die Bibliothek gnet
.PHONY: install_gnet
install_gnet : $(TARGET_DIR)\install_gnet
$(TARGET_DIR)\install_gnet :
	$(MAKE) install_mingw
	$(MAKE) "LOCATION=http://free-doko.sourceforge.net/download" \
	        "FILE=gnet-src-2.0-20030531.zip" \
	        "MD5SUM=A8E82916DCC3085B1BB0F901A426A43D" \
	        fetch
	echo   Erstelle und installiere die gnet-Bibliothek
	echo     (die 'dllwrap'-Ausgabe kann ignoriert werden)
	$(UNCOMPRESS) $(DOWNLOAD_DIR)\gnet-src-2.0-20030531.zip
	echo /**/>>gnet\gnetconfig.h
	$(MAKE) CC=$(MINGW_DIR)\bin\gcc DLLWRAP=$(MINGW_DIR)\bin\dllwrap FLAGS="-g -Wall -mno-cygwin -DGNET_EXPERIMENTAL=1" INCLUDE="-I. -I$(MINGW_DIR)/include/glib-2.0 -I$(MINGW_DIR)/lib/glib-2.0/include" LIBS="-L$(MINGW_DIR)/lib -lglib-2.0 -lws2_32" -C gnet
	mkdir $(MINGW_DIR)\include\gnet
	copy gnet\*.h $(MINGW_DIR)\include\gnet
	copy gnet\libgnet-2.0.a $(MINGW_DIR)\lib
	copy gnet\gnet-2.0.dll $(MINGW_DIR)\bin
	rmdir /s /q gnet
	echo "2.0-20030531" > $@

# installiert die Bibliothek alut
.PHONY: install_alut
install_alut : $(TARGET_DIR)\install_alut
$(TARGET_DIR)\install_alut :
	$(MAKE) "LOCATION=http://connect.creativelabs.com/openal/Downloads/ALUT" \
	        "FILE=freealut-1.1.0-bin.zip" \
	        "MD5SUM=4B3BBCAB56B47F7BBF75BE89D593C1BD" \
	        fetch
	echo   Entpacke alut: freealut-1.1.0-bin.zip
	$(UNCOMPRESS) $(DOWNLOAD_DIR)\freealut-1.1.0-bin.zip
	-mkdir $(MINGW_DIR)\include\AL
	copy freealut-1.1.0-bin\include\AL\*.* $(MINGW_DIR)\include\AL
	copy freealut-1.1.0-bin\lib\*.* $(MINGW_DIR)\lib
	-mkdir $(MINGW_DIR)\doc\alut
	copy freealut-1.1.0-bin\doc\*.* $(MINGW_DIR)\doc\alut
	-mkdir $(MINGW_DIR)\examples\alut
	copy freealut-1.1.0-bin\examples\* $(MINGW_DIR)\examples\alut
	rmdir /s /q freealut-1.1.0-bin
	echo "1.1.0" > $@

# installiert FreeDoko
.PHONY: install_FreeDoko
install_FreeDoko : $(TARGET_DIR)\install_FreeDoko
$(TARGET_DIR)\install_FreeDoko : \
                   install_FreeDoko_manual \
	           install_FreeDoko_src \
                   install_FreeDoko_update \
                   install_FreeDoko_nonfree_cardsets \
		   install_FreeDoko_more_data
	echo $(VERSION) > $@

# installiert den FreeDoko Quelltext
.PHONY: install_FreeDoko_src
install_FreeDoko_src : $(TARGET_DIR)\install_FreeDoko_src
$(TARGET_DIR)\install_FreeDoko_src :
	$(MAKE) install_svn
	echo Lade FreeDoko aus dem Internet herunter
	echo   Lade den Quelltext: svn://svn.gna.org/svn/freedoko/trunk
	$(SVN) checkout svn://svn.gna.org/svn/freedoko/trunk $(FREEDOKO_TRUNK_DIR)
#	$(UNCOMPRESS) $(DOWNLOAD_DIR)\FreeDoko.trunk.zip
	echo $(VERSION) > $@

# installiert den FreeDoko Quelltext
.PHONY: install_FreeDoko_install_scripts
install_FreeDoko_install_scripts : $(TARGET_DIR)\install_FreeDoko_install_scripts
$(TARGET_DIR)\install_FreeDoko_install_scripts :
	$(MAKE) install_svn
	echo   Lade Installationsskripte: svn://svn.gna.org/svn/freedoko/more_data/setup/Windows/install_dev
	$(SVN) checkout svn://svn.gna.org/svn/freedoko/more_data/setup/Windows/install_dev $(FREEDOKO_INSTALL_DEV_DIR)
	echo $(VERSION) > $@

# installiert unfreie Kartensaetze
.PHONY: install_FreeDoko_nonfree_cardsets
install_FreeDoko_nonfree_cardsets : $(TARGET_DIR)\install_FreeDoko_nonfree_cardsets
$(TARGET_DIR)\install_FreeDoko_nonfree_cardsets :
	$(MAKE) install_FreeDoko_src
	echo   Lade zusaetzliche Kartensaetze: http://%SOURCEFORGE_DOWNLOAD%/free-doko/FreeDoko_0.7.8.Windows.zip
	$(MAKE) "LOCATION=http://%SOURCEFORGE_DOWNLOAD%/free-doko" \
		"MD5SUM=1F3AAD7A4279E4A81B6A9E4153FE0D50" \
	        "FILE=FreeDoko_0.7.8.Windows.zip" \
	        fetch
	echo   Entpacke zusaetzliche Kartensaetze: FreeDoko_0.7.8.Windows.zip
	$(UNCOMPRESS) $(DOWNLOAD_DIR)\FreeDoko_0.7.8.Windows.zip
	echo move /y FreeDoko_0.7.8\cardsets\Altenburg $(FREEDOKO_TRUNK_DIR)\data\cardsets >t.bat
	echo move /y FreeDoko_0.7.8\logo.png $(FREEDOKO_TRUNK_DIR)\src\logo.png >>t.bat
	echo rmdir /s /q FreeDoko_0.7.8 >>t.bat
	call t.bat
	del t.bat
	echo "0.7.8" > $@

# installiert weitere Datendateien
.PHONY: install_FreeDoko_more_data
install_FreeDoko_more_data : $(TARGET_DIR)\install_FreeDoko_more_data
$(TARGET_DIR)\install_FreeDoko_more_data :
	$(MAKE) install_svn
	echo   Lade zusaetzliche Datendateien: svn://svn.gna.org/svn/freedoko/more_data
	-mkdir $(FREEDOKO_MORE_DATA_DIRECTORY)
	$(SVN) checkout svn://svn.gna.org/svn/freedoko/more_data/backgrounds $(FREEDOKO_MORE_DATA_DIRECTORY)/backgrounds
	$(SVN) checkout svn://svn.gna.org/svn/freedoko/more_data/sounds $(FREEDOKO_MORE_DATA_DIRECTORY)/sounds
	echo $(VERSION) > $@

# installiert weitere Datendateien
.PHONY: install_FreeDoko_more_cardsets
install_FreeDoko_more_cardsets : $(TARGET_DIR)\install_FreeDoko_more_cardsets
$(TARGET_DIR)\install_FreeDoko_more_cardsets :
	$(MAKE) install_svn
	echo   Lade zusaetzliche Datendateien: svn://svn.gna.org/svn/freedoko/more_data/cardsets
	$(SVN) checkout svn://svn.gna.org/svn/freedoko/more_data/cardsets $(FREEDOKO_MORE_DATA_DIRECTORY)/cardsets
	echo $(VERSION) > $@

# installiert das Handbuch von FreeDoko
.PHONY: install_FreeDoko_manual
install_FreeDoko_manual : $(TARGET_DIR)\install_FreeDoko_manual
$(TARGET_DIR)\install_FreeDoko_manual :
	$(MAKE) install_svn
	echo   Lade das Handbuch: svn://svn.gna.org/svn/freedoko/manual
	$(SVN) checkout svn://svn.gna.org/svn/freedoko/manual $(FREEDOKO_MANUAL_DIRECTORY)
#	$(UNCOMPRESS) $(DOWNLOAD_DIR)\FreeDoko.manual.zip
	echo Zeit, das Handbuch unter $(FREEDOKO_MANUAL_DIRECTORY)\de\overview.html zu lesen
	-explorer $(FREEDOKO_MANUAL_DIRECTORY)\de\overview.html
	echo $(VERSION) > $@

# erstellt Verknuepfungen von FreeDoko
.PHONY: install_FreeDoko_shortcuts
install_FreeDoko_shortcuts : $(TARGET_DIR)\install_FreeDoko_shortcuts
$(TARGET_DIR)\install_FreeDoko_shortcuts :
	echo   Erstelle Verknuepfungen fuer FreeDoko
	echo @ECHO off >FreeDoko.bat
	echo REM If your path variable is full and you do use the DOS-Box only for FreeDoko >>FreeDoko.bat
	echo REM PATH C:\Windows\Command >>FreeDoko.bat
	echo SETLOCAL >>FreeDoko.bat
	echo call init.bat >>FreeDoko.bat
	echo cd %%FREEDOKO_TRUNK_DIR%%\src >>FreeDoko.bat
	echo echo Starte FreeDoko >>FreeDoko.bat
	echo if exist .\FreeDoko.exe .\FreeDoko.exe %%1 %%2 %%3 %%4 %%5 %%6 %%7 %%8 %%9 >>FreeDoko.bat
	echo if not exist .\FreeDoko.exe .\FreeDoko.bak.exe %%1 %%2 %%3 %%4 %%5 %%6 %%7 %%8 %%9 >>FreeDoko.bat
	echo ENDLOCAL >> FreeDoko.bat
	cscript //nologo create_shortcut_on_desktop.vbs FreeDoko "$(FREEDOKO_DIRECTORY)\FreeDoko.bat" "$(FREEDOKO_DIRECTORY)" "FreeDoko spielen" "$(FREEDOKO_DIRECTORY)\trunk\src\FreeDoko.ico" 
	cscript //nologo create_shortcut_on_desktop.vbs "FreeDoko aktualisieren" "$(FREEDOKO_DIRECTORY)\update.bat" "$(FREEDOKO_DIRECTORY)" "FreeDoko aktualisieren" "$(FREEDOKO_DIRECTORY)\trunk\src\FreeDoko.update.ico" 
	echo $(VERSION) > $@

.PHONY: install_FreeDoko_update
install_FreeDoko_update : $(TARGET_DIR)\install_FreeDoko_update
$(TARGET_DIR)\install_FreeDoko_update :
	$(MAKE) install_FreeDoko_install_scripts 
	xcopy /y $(FREEDOKO_INSTALL_DEV_DIR)\update.bat .
	echo $(VERSION) > $@

#  aktualisiert FreeDoko und ggf. die Entwicklungsumgebung
.PHONY: compile_FreeDoko
compile_FreeDoko : install_mingw install_pthreads install_boost install_gtkmm install_gnet install_alut install_FreeDoko_src 
	echo Kompiliere FreeDoko
	$(MAKE) -C $(FREEDOKO_TRUNK_DIR)\src

#  aktualisiert FreeDoko und ggf. die Entwicklungsumgebung
.PHONY: update
update : update_FreeDoko_src \
	 update_FreeDoko_more_data \
	 update_FreeDoko_manual \
	 compile_FreeDoko
	copy $(FREEDOKO_INSTALL_DEV_DIR)\init.bat . 
	 

# aktualisiert den FreeDoko Quelltext
.PHONY: update_FreeDoko_src
update_FreeDoko_src : install_svn
	echo Aktualisiere FreeDoko Quelltext
	$(SVN) update $(FREEDOKO_TRUNK_DIR)

# aktualisiert die weiteren FreeDoko Datendateien
.PHONY: update_FreeDoko_more_data
update_FreeDoko_more_data : install_svn
	echo Aktualisiere FreeDoko Datendateien
	if exist $(FREEDOKO_MORE_DATA_DIRECTORY)\.svn $(SVN) update $(FREEDOKO_MORE_DATA_DIRECTORY)
	$(SVN) update $(FREEDOKO_MORE_DATA_DIRECTORY)\backgrounds
	$(SVN) update $(FREEDOKO_MORE_DATA_DIRECTORY)\sounds
	if exist $(FREEDOKO_MORE_DATA_DIRECTORY)\cardsets\.svn $(SVN) update $(FREEDOKO_MORE_DATA_DIRECTORY)\cardsets

# aktualisiert das Handbuch von FreeDoko
.PHONY: update_FreeDoko_manual
update_FreeDoko_manual : install_svn
	echo Aktualisiere FreeDoko Handbuch
	$(SVN) update $(FREEDOKO_MANUAL_DIRECTORY)


# Laedt die angegebene Datei aus dem Internet
# LOCATION: Internetadresse (ohne Datei)
# FILE:     Dateiname
# MD5SUM:   md5sum
.PHONY: fetch
fetch :
	if exist $(DOWNLOAD_DIR)\$(FILE) echo   Datei $(FILE) schon heruntergeladen
	if not exist $(DOWNLOAD_DIR)\$(FILE) echo   Lade $(LOCATION)/$(FILE) aus dem Internet herunter
	if not exist $(DOWNLOAD_DIR)\$(FILE) $(WGET) $(LOCATION)/$(FILE)
	echo   Ueberpruefe MD5SUM von $(FILE) ...
	echo $(MD5SUM) (soll)
	$(MD5) $(DOWNLOAD_DIR)\$(FILE)
	$(MD5) -c$(MD5SUM) $(DOWNLOAD_DIR)\$(FILE)
	echo   ... IO

# entpackt die angegebene zip-Datei
# FILE: Datei
.PHONY: extract_zip
extract_zip :
	echo   Entpacke $(FILE)
	$(UNCOMPRESS) -o$(MINGW_DIR) $(DOWNLOAD_DIR)\$(FILE)

# entpackt die angegebene tar.gz-Datei
# FILE: Datei
.PHONY: extract_tar_gz
extract_tar_gz :
	echo   Entpacke $(FILE)
	$(UNCOMPRESS) -so $(DOWNLOAD_DIR)\$(FILE) > $(MINGW_DIR)\$(FILE:.tar.gz=.tar)
	$(UNCOMPRESS) -o$(MINGW_DIR) $(MINGW_DIR)\$(FILE:.tar.gz=.tar)
	del $(MINGW_DIR)\$(FILE:.tar.gz=.tar)

# Zeigt eine Fehlernachricht (nutze Visual Basic Script)
# MESSAGE: Fehlernachricht
.PHONY: error_message
error_message :
	echo MsgBox %1, 16, "Fehler" > message.vbs
	cscript //nologo message.vbs
	del message.vbs
	exit 1
