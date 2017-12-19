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

%MAKE% install_FreeDoko_more_cardsets || goto ERROR

ENDLOCAL
ECHO .
ECHO Die Installation der zusätzlichen Kartensätze hat anscheinend funktioniert.
ECHO Viel Spass beim Spielen.
ECHO .
PAUSE
goto EOF

:ERROR
echo Fehler, breche ab.
PAUSE
exit /b 1

:EOF
