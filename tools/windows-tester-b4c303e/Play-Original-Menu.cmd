@echo off
setlocal DisableDelayedExpansion
cd /d "%~dp0"
if errorlevel 1 goto failed
call "%~dp0Configure-RacerRevenge.cmd"
if errorlevel 1 goto failed
echo Starting original Main. Arrows navigate, Enter confirms, Escape backs out.
echo All racers use original AI. Close the game window to finish.
"%CD%\RacerRevengePC-b4c303e.exe" "%_RR_CORPUS%"
set "_RR_EXIT=%ERRORLEVEL%"
echo Native client exit code: %_RR_EXIT%
pause
exit /b %_RR_EXIT%
:failed
pause
exit /b 1
