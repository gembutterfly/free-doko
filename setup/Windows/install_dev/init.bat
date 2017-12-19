@echo off
REM
REM  Installation batch file for FreeDoko, a Doppelkopf-Game
REM   
REM  Copyright (C) 2006  by Diether Knof
REM
REM  This program is free software; you can redistribute it and/or 
REM  modify it under the terms of the GNU General Public License as 
REM  published by the Free Software Foundation; either version 2 of 
REM  the License, or (at your option) any later version.
REM
REM  This program is distributed in the hope that it will be useful,
REM  but WITHOUT ANY WARRANTY; without even the implied warranty of
REM  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
REM  GNU General Public License for more details. 
REM  You can find this license in the file 'GPL'.
REM
REM  You should have received a copy of the GNU General Public License
REM  along with this program; if not, write to the Free Software
REM  Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
REM  MA  02111-1307  USA
REM
REM Contact:
REM   Diether Knof dknof@gmx.de
REM   Borg Enders  borg@borgsoft.de
REM

REM Initialisiert die Umgebung
REM Getestet unter Microsoft Windows XP,
REM funktioniert nicht unter Windows 98.
REM
REM 2006-2008 Diether Knof

echo.
echo Initialisiere die Umgebung
REM uebergeordnetes Verzeichnis
set FREEDOKO_DIRECTORY=%CD%
REM Verzeichnis fuer die Programme
set BIN_DIR=%FREEDOKO_DIRECTORY%\bin
REM Verzeichnis fuer die download
set DOWNLOAD_DIR=%FREEDOKO_DIRECTORY%\download
REM Verzeichnis fuer die Ziele
set TARGET_DIR=%FREEDOKO_DIRECTORY%\targets
REM Verzeichnis fuer MinGW32
set MINGW_DIR=%FREEDOKO_DIRECTORY%\MinGW32
REM Verzeichnis fuer den FreeDoko Quelltext
set FREEDOKO_TRUNK_DIR=%FREEDOKO_DIRECTORY%\trunk
REM Verzeichnis fuer weitere FreeDoko Datendateien
set FREEDOKO_MORE_DATA_DIRECTORY=%FREEDOKO_DIRECTORY%\more_data
REM Verzeichnis fuer das FreeDoko Handbuch
set FREEDOKO_MANUAL_DIRECTORY=%FREEDOKO_DIRECTORY%\manual
REM Verzeichnis fuer die FreeDoko Installations-Dateien
set FREEDOKO_INSTALL_DEV_DIR=%FREEDOKO_DIRECTORY%\install_dev

REM Programme
REM   wget
set WGET=%BIN_DIR%\wget.exe --no-clobber --no-directories --directory-prefix=%DOWNLOAD_DIR%
REM   Entpacker
REM set UNCOMPRESS=%BIN_DIR%\7z.exe x -aoa -y
set UNCOMPRESS=%BIN_DIR%\7z.exe x -y
REM   md5
set MD5=%BIN_DIR%\md5.exe
REM   svn
set SVN=%MINGW_DIR%\bin\svn.exe
REM   make
set MAKE=%MINGW_DIR%\bin\mingw32-make.exe

REM  Download-Server von sourceforge
set SOURCEFORGE_DOWNLOAD=heanet.dl.sourceforge.net/sourceforge

REM Umgebungsvariablen für Programme
set PKG_CONFIG_PATH=%MINGW_DIR%\lib\pkgconfig

REM Pfad
PATH %MINGW_DIR%\bin;%MINGW_DIR%\lib;%PATH% 

REM Variablen zurÃ¼cksetzen
set INCLUDE=
set LIBS=
