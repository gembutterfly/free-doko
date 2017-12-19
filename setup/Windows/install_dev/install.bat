@echo off
REM
REM  Installation batch file for FreeDoko, a Doppelkopf-Game
REM   
REM  Copyright (C) 2009  by Diether Knof
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

REM Laedt Entwicklungsumgebung msys, Bibliotheken gtkmm und gnet sowie FreeDoko
REM und erstellt FreeDoko Programm
REM Es werden etwa 80 MB heruntergeladen
REM Getestet unter Microsoft Windows XP, wird wahrscheinlich nicht unter Windows 98 funktionieren.
REM
REM 2008-01-19 Diether Knof

SETLOCAL

REM initialize the environment
call init.bat

REM if exist %MINGW_DIR% rmdir /s /q %MINGW_DIR%
REM if exist %TARGET_DIR% rmdir /s /q %TARGET_DIR%

REM fetch and extract the make program
if not exist %DOWNLOAD_DIR% mkdir %DOWNLOAD_DIR%
if exist %MAKE% goto :make_installed
call :fetch http://%SOURCEFORGE_DOWNLOAD%/mingw mingw32-make-3.80.0-3.tar.gz A68E5A25917EB1BB6AA1D359EEC47B4B || exit /b 1
if not exist %BIN_DIR% mkdir %BIN_DIR%
call :extract_tar_gz mingw32-make-3.80.0-3       || exit /b 1

:make_installed

REM %MAKE% clean
%MAKE% directories || goto ERROR
%MAKE% install_FreeDoko_install_scripts || goto ERROR
%SVN% update %FREEDOKO_INSTALL_DEV_DIR% 
copy %FREEDOKO_INSTALL_DEV_DIR%\Makefile . 
copy %FREEDOKO_INSTALL_DEV_DIR%\init.bat . 
%MAKE% install || goto ERROR

ENDLOCAL
ECHO .
ECHO Die Installation hat anscheinend funktioniert.
ECHO Viel Spass beim Spielen.
ECHO .
PAUSE
goto EOF



:fetch
echo   Lade %1/%2 aus dem Internet herunter
%WGET% %1/%2 >NUL
if ERRORLEVEL 1 (
  echo Fehler beim Herunterladen von %1/%2
  call :error_message "Fehler beim Herunterladen von %1/%2."
  exit /b 1
) else (
  echo   Ueberpruefe MD5SUM von %2 ...
  %MD5% -c%3 %DOWNLOAD_DIR%\%2
  if ERRORLEVEL 1 (
    echo   ... MD5SUM stimmt nicht ueberein mit %3
    echo   Loesche %DOWNLOAD_DIR%\%2.
    call :error_message "Fehler beim Herunterladen von %1/%2: MD5SUM stimmt nicht."
    exit /b 1
  ) else (
    echo   ... IO
    echo.
  )
)
goto EOF

REM entpackt die angegebene tar.gz-Datei
REM Argumente:
REM 1   Datei (ohne Endung '.tar.gz')
:extract_tar_gz
echo   Entpacke %1.tar.gz
%UNCOMPRESS% -o%MINGW_DIR% %DOWNLOAD_DIR%\%1.tar.gz >NUL
if ERRORLEVEL 1 (
  echo Fehler beim Entpacken des Archives %1.tar.gz
  call :error_message "Fehler beim Entpacken des Archives %1.tar.gz"
  goto ERROR
)
%UNCOMPRESS% -o%MINGW_DIR% %MINGW_DIR%\%1.tar >NUL
if ERRORLEVEL 1 (
  echo Fehler beim Entpacken des Archives %1.tar
  call :error_message "Fehler beim Entpacken des Archives %1.tar"
  goto ERROR
)
del %MINGW_DIR%\%1.tar
goto EOF

REM Zeige eine Fehlernachricht (nutze Visual Basic Script)
:error_message
  echo MsgBox %1, 16, "Fehler" > message.vbs
  cscript //nologo message.vbs
  del message.vbs
goto ERROR

:ERROR
echo Fehler, breche ab.
PAUSE
exit /b 1


:EOF
