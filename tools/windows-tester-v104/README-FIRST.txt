Racer Revenge PC - Windows x64 experimental tester

START (NEXT-BUILD ENTRY)
1. Extract the entire package directly into the folder containing your intact
   original disc files: SLUS_202.68, APPSTART.RES, LOC.RES, UI, AUDIO and the
   original track/pod folders. Keep RacerRevengePC.exe and all supplied DLLs
   together. Back up an older native package before replacing it. Never execute
   or rename SLUS_202.68.
2. Double-click RacerRevengePC.exe. No CMD launcher or asset argument is needed.
   This normal Windows entry uses the EXE's own folder, not another current
   directory, as its original asset root. Missing original files produce a
   visible error message; no substitute assets are searched for or downloaded.
3. Choose your rider and course through the normal menus. Original resource,
   profile and unlock guards remain. The native-PC 60 Hz Rider deadline policy
   controls host pacing only; it does not unlock riders or replace resources.
4. Arrows navigate, Enter confirms and Escape backs out. In a race, Escape
   opens Pause. Use the menu confirmations for Restart Race or return to Main.
   Driving input follows the accepted Course Confirm device. Use the original
   Controls screen to choose the driving scheme. Close the game when finished.

ADVANCED COMMAND-LINE ENTRY
Normal --assets entry and the optional launchers use the same single log in
the EXE folder. --assets retains its explicit asset selection, environment and
working directory. Other diagnostic CLI modes retain caller-owned output
behavior; they are not the normal player launch. No security policy or global environment setting is changed.
Startup directory discovery, missing-file checks and the log file use wide
Windows APIs. Existing downstream asset loaders are not comprehensively
Unicode-audited by this small startup change; end-to-end Unicode paths and
native Windows operation still require runtime verification.

WINDOW SIZE
Windows starts windowed, not fullscreen, with a fixed 1280x896 client area:
exactly twice the original 640x448 canvas in each dimension. Window borders
and the title bar are additional. The source game Video/camera canvas stays
640x448. Original UI source images are nearest-neighbor scaled; gameplay uses
its existing native rendering at the presentation viewport. This is not a
claim that every scene is a pixel-doubled original-resolution framebuffer.
Windows desktop scaling, remote-display behavior and available screen space
can affect the visible physical size. The logs record actual drawable sizes.
Use a display with room for the full client area plus borders/taskbar.

NATIVE SAVE FILES
Accepted settings and native beta profiles are stored separately from the game
assets at %LOCALAPPDATA%\RacerRevengePC\Saves:
- settings-v1.bin: accepted Game Options and control presets.
- progress-v1.bin: native beta profile/progression supported by this build.
These are native beta files, not original PlayStation 2 memory-card images.
Normal no-argument entry clears inherited diagnostic RR_ overrides, including any custom
RR_NATIVE_SAVE_DIR, so normal launches use the per-user location above.
If no usable per-user directory is available, a write can fail; session state
is not evidence that it was saved. Check the displayed save status before exit.
Original disc assets and imported original memory-card files are not modified.
Back up your native save files separately; do not include them in routine logs.

RETURN DIAGNOSTICS
Normal double-click launch writes one plainly named file in the game folder:
RacerRevenge.log. Startup, stdout, stderr, menu/load diagnostics and normal exit
status all go into this one file. Each run appends a clearly separated UTC
session header; earlier sessions are not deleted or overwritten. No sidecar
logs, timestamped log directories, screenshots or memory dumps are generated
by this entry. Nothing is uploaded automatically.

The file stays locked against concurrent writers while this game process is
alive. Close another running instance before launching again. If the game
folder is not writable, startup shows a visible error and stops; it does not
silently put the log somewhere else.

After closing the game, attach RacerRevenge.log in your conversation. Include
what you selected, what went wrong and whether you used keyboard or controller.
Review the file first: paths, device/driver details and game selections may
appear, including earlier sessions. Do not include original assets, passwords
or save files. A best-effort native fault record captures exception code/address and an
image-relative offset when available, without a crash dump. It never resumes
faulting execution. Some faults, forced kills or runtime aborts bypass this
filter, so the last session may still be partial with no exit marker. Windows loader failures before native startup
may occur before a log can be opened; report the visible Windows error too.

REQUIREMENTS AND LIMITS
64-bit Windows, the included matching x64 SDL2.dll, FFmpeg 7.1.5
avcodec-61.dll and avutil-59.dll, the matching bundled libwinpthread-1.dll,
and a compatible desktop OpenGL driver. Keep
all supplied dependency DLLs beside the EXE. Other FFmpeg ABI versions are not
interchangeable: do not rename another version to one of these filenames. This is an experimental native client, not a finished game.
Read the package build/validation receipt for what was actually built and
checked; compilation is not proof of successful execution on Windows.
The earlier Windows tester reached its menus at 1280x896 on the user PC;
that does not validate race loading or this next integrated build.
Existing Linux smoke coverage is not Windows validation, a full gameplay test,
or proof that all beta features or saves work on your Windows system.
Existing gameplay and audio limitations are not removed by this package.
No installation, administrator access, new credentials or security-setting
changes are required. If Windows blocks an unsigned download,
review the package and your policy; do not disable protections to run it.
