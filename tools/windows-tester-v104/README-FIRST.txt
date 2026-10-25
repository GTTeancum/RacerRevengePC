Racer Revenge PC - Windows x64 experimental tester

START
1. Extract the entire package, including RacerRevengePC.exe, SDL2.dll,
   avcodec-61.dll, avutil-59.dll, libwinpthread-1.dll,
   Launch-RacerRevenge.ps1 and Play-RacerRevenge.cmd, directly into the folder
   containing your intact original disc files: SLUS_202.68, APPSTART.RES, LOC.RES,
   UI, AUDIO, and the original track/pod folders. Back up any older native
   package before replacing it. Never execute or rename SLUS_202.68.
2. Double-click Play-RacerRevenge.cmd. It opens original Main, not a test route.
   This is an ordinary CMD launcher with native stdout/stderr redirection. It
   does not execute a downloaded PowerShell script. It asks built-in PowerShell
   only for a timestamp, with a local-clock fallback if that command is blocked.
   No execution-policy bypass or security-setting change is used.
   The established native-PC 60 Hz Rider deadline policy is enabled for host
   pacing only; original Rider, resource, profile and unlock guards remain.
   Choose your rider and course through the normal menus. The original
   unlock/profile rules apply; no fixture, unlock override or asset pack is
   included. Unsupported or missing resources report an error, not a substitute.
3. Arrows navigate; Enter confirms; Escape backs out. In a race, Escape opens
   Pause. Use the menu's confirmation for Restart Race or return to Main.
   Use the original Controls screen for the selected driving scheme. Keyboard
   versus controller driving follows the accepted Course Confirm device.
4. Close the game window when finished. Leave the launcher console open while
   the game runs so it can finish the diagnostic receipt.

ASSETS IN ANOTHER DIRECTORY
From Windows PowerShell, invoke the included Launch-RacerRevenge.ps1 with
-OriginalAssetRoot "D:\Your original disc folder". The executable, SDL2.dll, avcodec-61.dll
avutil-59.dll and libwinpthread-1.dll stay together in this package. This optional PowerShell script
may be restricted by your policy; the default CMD launch does not require it. The launcher does not copy or modify disc files.
No generated runtime/corpus, Rider image cache, emulator or bundled game assets
are required for normal --assets entry. Keep the original files intact.

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
The launcher clears inherited diagnostic RR_ overrides, including any custom
RR_NATIVE_SAVE_DIR, so normal launches use the per-user location above.
If no usable per-user directory is available, a write can fail; session state
is not evidence that it was saved. Check the displayed save status before exit.
Original disc assets and imported original memory-card files are not modified.
Back up your native save files separately; do not include them in routine logs.

RETURN DIAGNOSTICS
Every launcher run creates a fresh timestamped folder under RacerRevenge-logs
beside the package. If that location is not writable, logs go under
%LOCALAPPDATA%\RacerRevenge\logs. The console prints the exact folder.

After closing the game, ZIP the whole run folder and attach it in your
conversation. Include what you selected, what went wrong and whether you used
a keyboard or controller. The default CMD folder contains launch.txt (startup time, OS, path and exit
status), file-hashes.txt (executable and required DLL SHA-256 output from Windows
certutil), stdout.log, stderr.log, and any launcher error. The optional advanced
PowerShell launcher writes equivalent metadata and hashes in launch.json. Runtime logs include menu selection changes, cold-load state,
graphics details and errors. There is no automatic upload or telemetry.

Review the logs before sharing: local folder names, device/driver details and
game selections may appear. Do not send original game assets, passwords or
save/profile files. If the process is forcibly killed or the launcher closes,
logs may be partial and the final exit receipt may be absent; mention that fact.
No screenshot recording or crash memory dump is enabled by this launcher.

REQUIREMENTS AND LIMITS
64-bit Windows, the included matching x64 SDL2.dll, FFmpeg 7.1.5
avcodec-61.dll and avutil-59.dll, the matching bundled libwinpthread-1.dll,
and a compatible desktop OpenGL driver. Keep
all supplied dependency DLLs beside the EXE. Other FFmpeg ABI versions are not
interchangeable: do not rename another version to one of these filenames. This is an experimental native client, not a finished game.
Read the package build/validation receipt for what was actually built and
checked; compilation is not proof of successful execution on Windows.
This native Windows runtime has not been launched or tested on Windows.
Existing Linux smoke coverage is not Windows validation, a full gameplay test,
or proof that all beta features or saves work on your Windows system.
Existing gameplay and audio limitations are not removed by this package.
No installation, administrator access, new credentials or security-setting
changes are required. If Windows blocks an unsigned download,
review the package and your policy; do not disable protections to run it.
