@echo off
rem vs.cmd <python script> [args...]: runs a python build helper inside the MSVC x64 environment.
set "TEMP=D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\agent-a7890fc7493d16ae2\work\scratch\145g\temp"
set "TMP=%TEMP%"
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
"C:\Users\smmel\AppData\Local\Programs\Python\Python312\python.exe" -B %*
exit /b %errorlevel%
