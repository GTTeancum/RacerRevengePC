@echo off
setlocal DisableDelayedExpansion
cd /d "%~dp0"
if errorlevel 1 goto nofolder
rem Clear only inherited game/graphics overrides, not our _RR_ launcher variables.
for /f "tokens=1 delims==" %%V in ('set RR_ 2^>nul') do set "%%V="
for /f "tokens=1 delims==" %%V in ('set SDL_ 2^>nul') do set "%%V="
for /f "tokens=1 delims==" %%V in ('set MESA_ 2^>nul') do set "%%V="
for /f "tokens=1 delims==" %%V in ('set LIBGL_ 2^>nul') do set "%%V="
rem A built-in date expression, not execution of a downloaded PowerShell script.
rem No execution-policy override or security-setting change is used.
set "_RR_MISSING="
set "_RR_STAMP="
for /f "delims=" %%T in ('powershell.exe -NoLogo -NoProfile -NonInteractive -Command "[DateTime]::UtcNow.ToString('yyyyMMdd-HHmmss-fff')" 2^>nul') do set "_RR_STAMP=%%T"
rem Local clock fallback if PowerShell itself is unavailable or restricted.
if defined _RR_STAMP goto stampready
set "_RR_STAMP=%DATE%_%TIME%"
set "_RR_STAMP=%_RR_STAMP:/=-%"
set "_RR_STAMP=%_RR_STAMP:\=-%"
set "_RR_STAMP=%_RR_STAMP::=-%"
set "_RR_STAMP=%_RR_STAMP: =0%"
:stampready
set "_RR_RUN=%CD%\RacerRevenge-logs\%_RR_STAMP%-%RANDOM%-%RANDOM%"
if exist "%_RR_RUN%" goto collision
mkdir "%_RR_RUN%" 2>nul
if not errorlevel 1 goto logready
if not defined LOCALAPPDATA goto nolog
set "_RR_RUN=%LOCALAPPDATA%\RacerRevenge\logs\%_RR_STAMP%-%RANDOM%-%RANDOM%"
if exist "%_RR_RUN%" goto collision
mkdir "%_RR_RUN%" 2>nul
if errorlevel 1 goto nolog
:logready
set "_RR_EXIT=1"
echo Diagnostics for this run: "%_RR_RUN%"
>"%_RR_RUN%\launch.txt" echo format=RacerRevenge-Windows-diagnostics-2
>>"%_RR_RUN%\launch.txt" echo started_local=%DATE% %TIME%
>>"%_RR_RUN%\launch.txt" echo timestamp_label=%_RR_STAMP%
>>"%_RR_RUN%\launch.txt" echo asset_root="%CD%"
>>"%_RR_RUN%\launch.txt" echo window=1280x896 windowed client area
>>"%_RR_RUN%\launch.txt" echo original_canvas=640x448
>>"%_RR_RUN%\launch.txt" echo telemetry=false
>>"%_RR_RUN%\launch.txt" ver
>"%_RR_RUN%\HOW-TO-RETURN-LOGS.txt" echo Close the game, then ZIP this whole run folder and attach it in your conversation. Review logs first: paths, devices, graphics details and game selections may appear. Nothing is uploaded. Do not include original disc assets or profile/save files.
for %%F in (RacerRevengePC.exe SDL2.dll avcodec-61.dll avutil-59.dll libwinpthread-1.dll) do if not exist "%CD%\%%F" set "_RR_MISSING=%%F"
if defined _RR_MISSING goto missing
for %%F in (SLUS_202.68 APPSTART.RES LOC.RES UI\UIFONTS.RES UI\UIMAIN\UIMAIN.RES) do if not exist "%CD%\%%F" set "_RR_MISSING=%%F"
if defined _RR_MISSING goto missing
rem Fingerprints are diagnostic evidence, not a substitute for package verification.
for %%F in (RacerRevengePC.exe SDL2.dll avcodec-61.dll avutil-59.dll libwinpthread-1.dll) do certutil.exe -hashfile "%CD%\%%F" SHA256 >>"%_RR_RUN%\file-hashes.txt" 2>&1
set "RR_NATIVE_FIXED_WINDOW=1"
rem Established native PC host pacing only; source resource/profile guards remain.
set "RR_ORIGINAL_RIDER_PRESENTATION=allow-native-pc-deadline60hz"
>>"%_RR_RUN%\launch.txt" echo rider_presentation=allow-native-pc-deadline60hz
echo Opening original Main in a fixed 1280x896 window.
echo Arrows select, Enter confirms, Escape backs out. Close the game to finish.
echo Leave this console open while the game runs.
>>"%_RR_RUN%\launch.txt" echo status=starting_native_process
"%CD%\RacerRevengePC.exe" --assets "%CD%\." >"%_RR_RUN%\stdout.log" 2>"%_RR_RUN%\stderr.log"
set "_RR_EXIT=%ERRORLEVEL%"
>>"%_RR_RUN%\launch.txt" echo status=native_process_returned
>>"%_RR_RUN%\launch.txt" echo native_exit_code=%_RR_EXIT%
goto finished
:missing
echo Required file missing: "%_RR_MISSING%"
echo Extract the whole package into your intact original disc folder.
>"%_RR_RUN%\launcher-error.txt" echo Required file missing: "%_RR_MISSING%"
>>"%_RR_RUN%\launch.txt" echo status=preflight_failed
:finished
>>"%_RR_RUN%\launch.txt" echo finished_local=%DATE% %TIME%
>>"%_RR_RUN%\launch.txt" echo launcher_exit_code=%_RR_EXIT%
echo Exit code: %_RR_EXIT%. This alone does not establish a gameplay pass.
echo Return logs by ZIPping this whole folder: "%_RR_RUN%"
pause
exit /b %_RR_EXIT%
:collision
echo A diagnostics folder with this name already exists. Nothing was overwritten.
pause
exit /b 1
:nolog
echo Cannot create a diagnostics folder beside the package or in LOCALAPPDATA.
echo Choose a writable package folder. The game has not been launched.
pause
exit /b 1
:nofolder
echo Cannot open the package folder. Extract the ZIP before launching.
pause
exit /b 1
