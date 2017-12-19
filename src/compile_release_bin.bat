@echo off

SETLOCAL

REM mingw32
SET MINGW_DIR=Q:/MinGW32
PATH Q:\MinGW32\bin;Q:\MinGW32\lib;%PATH%
REM PATH %PATH%;C:\MinGW\bin;C:\MinGW\lib

REM Sonst Probleme mit Microsoft Visual Studio
SET INCLUDE=

if exist FreeDoko.exe del FreeDoko.exe

REM compile
make release_bin

:end

ENDLOCAL
