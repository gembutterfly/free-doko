@ECHO off 
SETLOCAL 
call init.bat  
%SVN% update %FREEDOKO_INSTALL_DEV_DIR% 
if ERRORLEVEL 1 (
  echo Fehler beim Aktualisieren von %FREEDOKO_INSTALL_DEV_DIR%
  call :error_message "Fehler beim Aktualisieren von %FREEDOKO_INSTALL_DEV_DIR%"
  goto ERROR
)
copy %FREEDOKO_INSTALL_DEV_DIR%\Makefile . 
copy %FREEDOKO_INSTALL_DEV_DIR%\init.bat . 
ENDLOCAL 

SETLOCAL 
call init.bat
if exist FreeDoko.exe copy FreeDoko.exe FreeDoko.bak.exe
%MAKE% update 
if ERRORLEVEL 1 (
  echo Fehler beim Aktualisieren von FreeDoko
  call :error_message "Fehler beim Aktualisieren von FreeDoko"
  goto ERROR
)

echo Aktualisierung erfolgreich.
echo MsgBox "Aktualisierung erfolgreich :-)", 64, "Aktualisierung erfolgreich" > message.vbs
cscript //nologo message.vbs
PAUSE
goto :EOF

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
ENDLOCAL 
