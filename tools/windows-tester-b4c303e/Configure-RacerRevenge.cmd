@echo off
rem Called by the launchers inside SETLOCAL. Only this launch environment changes.
for /f "tokens=1 delims==" %%V in ('set RR_ 2^>nul') do set "%%V="
for /f "tokens=1 delims==" %%V in ('set SDL_ 2^>nul') do set "%%V="
for /f "tokens=1 delims==" %%V in ('set MESA_ 2^>nul') do set "%%V="
for /f "tokens=1 delims==" %%V in ('set LIBGL_ 2^>nul') do set "%%V="
if not exist "SLUS_202.68" (
 echo Put this package beside your ORIGINAL SLUS_202.68 and game asset folders.
 exit /b 1
)
if not exist "RacerRevengePC-b4c303e.exe" exit /b 1
if not exist "SDL2.dll" exit /b 1
set "RR_ORIGINAL_ASSET_ROOT=%CD%"
set "RR_ORIGINAL_UI_ROOT=%CD%"
set "RR_GENERATED_RUNTIME_ROOT=%CD%\rr-runtime-b4c303e"
set "RR_ORIGINAL_RIDER_IPUM_CACHE=%CD%\rr-rider-cache-b4c303e"
set "RR_ORIGINAL_MAIN_MENU=1"
set "RR_START_MENU=0"
set "RR_ORIGINAL_MAIN_AUDIO=muted"
set "RR_AUDIO=0"
set "RR_AUDIO_DEVICE=0"
set "RR_AUDIO_ROOT=%CD%\AUDIO"
set "RR_TRACK=TA"
set "RR_RACE=1"
set "RR_RACE_MODE=2"
set "RR_ORIGINAL_AI_ALL_RACERS=1"
set "RR_AUTODRIVE=0"
set "RR_DISABLE_RACER_CONTACTS=0"
set "_RR_CORPUS=%RR_GENERATED_RUNTIME_ROOT%\evidence\staticType2Producer49X\original-word\native-fixtures.json"
if not exist "%_RR_CORPUS%" (
 echo Matching generated runtime is missing. Extract the entire ZIP.
 exit /b 1
)
exit /b 0
