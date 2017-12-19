rem @echo off

REM compile FreeDoko and check the references

SETLOCAL

REM MinGW32
PATH %PATH%;C:\MinGW32\bin;C:\MinGW32\lib

REM override settings from Microsoft visual studio
SET INCLUDE=

if exist FreeDoko.exe del FreeDoko.exe

REM compile
REM Note: The boost library must be active
make "USE_BOOST=true"

if not exist FreeDoko.exe goto end
FreeDoko.exe -r ../References --ui=none %1 %2 %3 %4 %5 %6 %7 %8 %9

@REM grep -v OK ../References/References.report.csv | grep FreeDoko; \
@REM tail -n 5 ../References/References.report.csv  | sed "s/;/   /g"
@REM echo "checks entries "`grep "^[ \t]*check" ../References/*.FreeDoko | wc -l`

@PAUSE
