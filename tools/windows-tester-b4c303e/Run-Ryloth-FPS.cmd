@echo off
setlocal DisableDelayedExpansion
cd /d "%~dp0"
if errorlevel 1 goto failed
call "%~dp0Configure-RacerRevenge.cmd"
if errorlevel 1 goto failed
:newfolder
set "_RR_RESULTS=%CD%\RacerRevenge-results\Ryloth-b4c303e-%RANDOM%-%RANDOM%"
if exist "%_RR_RESULTS%" goto newfolder
mkdir "%_RR_RESULTS%"
if errorlevel 1 goto failed
rem Common rewrite B-C21: the per-course route main-mawhonic09-course-ra, its test profile
rem (RR_ORIGINAL_TEST_PROFILE) and RR_RA_NATIVE_WORLD are retired. Ryloth runs through the one
rem free-play route: the Course screen picks RA (RR_TEST_COURSE). The save in use must offer
rem Ryloth on the Course screen (tools\capture\run_windows_takeover.py --seed-save seeds one).
set "RR_UNATTENDED_UI_ROUTE=main-rider-course"
set "RR_TEST_COURSE=RA"
set "RR_ORIGINAL_RIDER_PREVIEWS=3,7,9,23,24"
set "RR_WINDOW_CAPTURE_DIR=%_RR_RESULTS%"
set "RR_WINDOW_CAPTURE_AUTO=1"
set "RR_UNATTENDED_CAPTURE=1"
set "RR_TRACK_FPS_BASELINE=1"
set "RR_UNATTENDED_SECONDS=300"
set "RR_TRACE_PHASES=1"
set "RR_TRACE_RENDER=1"
set "RR_TRACE_ORIGINAL_UI_STAGES=1"
set "RR_PIXEL_PROVENANCE=1"
echo Running the bounded original-menu-to-Ryloth FPS route with all eight original AI racers.
echo Keep the real game window visible and focused. No driving input is needed.
echo Results: "%_RR_RESULTS%"
"%CD%\RacerRevengePC-b4c303e.exe" "%_RR_CORPUS%" >"%_RR_RESULTS%\stdout.log" 2>"%_RR_RESULTS%\stderr.log"
set "_RR_EXIT=%ERRORLEVEL%"
echo Native client exit code: %_RR_EXIT%
echo Read track-fps-baseline.json and logs in: "%_RR_RESULTS%"
echo Exit code alone does not establish a gameplay pass.
pause
exit /b %_RR_EXIT%
:failed
pause
exit /b 1
