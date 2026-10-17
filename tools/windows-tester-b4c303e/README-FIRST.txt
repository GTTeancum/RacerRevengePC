Racer Revenge PC - Windows x64 tester - b4c303e - 2026-10-17

STATUS
The Windows executable was cross-compiled successfully with MinGW-w64 GCC14.2
in the cloud. It has NOT been launched or smoke-tested on Windows. This is an
experimental tester build, not a Windows validation pass or a finished game.
The same game source passed the Linux basic smoke at 0.795 FPS using software
rendering; that number is not a baseline for your PC/GPU. No Mesa software
renderer is bundled or forced here. Your Windows OpenGL driver is used.

INSTALL
1. Back up your existing native EXE and SDL2.dll before replacing anything.
2. Extract ALL ZIP contents into your original game asset folder, directly
   beside SLUS_202.68, APPSTART.RES, LOC.RES, UI, AUDIO, track and pod folders.
   Do not put the EXE in a nested folder. Never execute or rename SLUS.
3. Keep rr-runtime-b4c303e, rr-rider-cache-b4c303e and SDL2.dll with this package.
   Original disc assets are not included. Use your existing intact assets.
4. Double-click Play-Original-Menu.cmd for the original menus and ALL8 AI races.
   Arrows navigate, Enter confirms, Escape backs out. Close the window to quit.
5. For a comparable Ryloth measurement, double-click Run-Ryloth-FPS.cmd. It runs
   the authenticated menu/character/course route and exits automatically after
   its bounded FPS capture (maximum 300 seconds). Keep the window focused.
   See RacerRevenge-results/Ryloth-b4c303e-*/track-fps-baseline.json and logs.

LIMITS
All racers, including the followed player, use original in-game AI. This package
has no human-driving mode or temporary driver. Audio is muted. Menu/gameplay
coverage remains incomplete; unsupported entries can be rejected. The benchmark
includes existing tracing and captures, with capture intervals excluded from FPS.
No emulator or guest instructions are executed. There are no installers, admin
steps, credential requests or security-setting changes. If Windows blocks this
unsigned experimental build, review it rather than disabling protections.

DEPENDENCIES
Use 64-bit Windows with a working desktop OpenGL4.0+ graphics driver.
Official SDL2 2.32.10 x64 DLL is included. GCC runtime components are statically
linked; no separate Visual C++ runtime is required for this cross-build.
Only normal Windows system DLL imports were found by static PE inspection.
This does not replace running it on Windows. Source is b4c303e / tree9afdc22f.
See build-receipt.json, package-manifest.json and licenses/ for provenance.
