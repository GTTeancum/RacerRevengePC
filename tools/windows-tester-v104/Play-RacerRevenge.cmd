@echo off
setlocal DisableDelayedExpansion
rem Optional compatibility wrapper only. The EXE owns startup and the one log.
if not exist "%~dp0RacerRevengePC.exe" goto missing
"%~dp0RacerRevengePC.exe"
exit /b %ERRORLEVEL%
:missing
echo RacerRevengePC.exe is missing. Extract the entire package before launching.
pause
exit /b 1
