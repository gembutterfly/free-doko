rem @echo off

REM compile FreeDoko and run the test party:
REM *  mutating rules
REM *  no end of the party
REM *  four different ai players (standard, profi, offensive, profi unfair)

SETLOCAL

REM MinGW32
PATH %PATH%;C:\MinGW32\bin;C:\MinGW32\lib

REM override settings from Microsoft visual studio
SET INCLUDE=

if exist FreeDoko.exe del FreeDoko.exe

REM compile
make release_bin

if not exist FreeDoko.exe goto end

REM start from ../data, so that all data files are found
cd ..\data
call ..\src\FreeDoko.exe --auto-bug-reports -F251 ..\src\testparty %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..\src

:end
ENDLOCAL
