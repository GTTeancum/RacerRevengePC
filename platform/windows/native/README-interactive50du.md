# Native Windows x64 integrated client: build only

This is a source-reviewed build entry, not a validated Windows release. It builds
`tests/native/terrain_interactive_drive50du.cpp`, the existing integrated original
frontend/gameplay development client. It does not build the historical D3D asset
viewer, emulator, original-instruction oracle, aggregate tests or other programs.
It never launches the client or runs a test. Root owns all later acceptance work.

## Prerequisites

- An approved, already configured x64 MSVC developer shell and Windows SDK.
- Explicit paths to its `cl.exe` and `link.exe`; no discovery, installation,
  CMake/Ninja invocation or attempted workaround of denied tools occurs here.
- A consolidated source tree containing the Windows window/audio/GameHost port,
  including `platform/linux/native/native_dynamic_library50win.h`.
- An approved official SDL2 development include directory containing `SDL_events.h`,
  `SDL_audio.h` and `SDL_version.h` with their sibling headers. SDL2 >= 2.0.22 is
  required by the retained event ABI. SDL3 is not compatible.
- An optional approved AMD64 `SDL2.dll` for copying beside the output executable.
  Omission permits compilation only and is reported in the receipt and warning;
  it does not yield a runnable package. SDL2 headers are required even when the
  runtime DLL is omitted. No SDL2 import library or SDL2main library is used.
- A new output directory. Existing outputs are never overwritten/reused/deleted.

The vendored native-json headers and six native-zlib inflate C sources are used
from this source tree. No historical prebuilt zlib library, zlib DLL, external
package manager or system zlib is needed. Those C files are exactly the source
list of `cmake/NativePc48C.cmake`; they use the same `/MD` runtime as the client.

## Entry

Invoke `Build-Interactive50DU.ps1` only after source/dependency approval, supplying
`-SourceRoot`, `-BuildDirectory`, `-Compiler`, `-Linker`, `-SDL2IncludeDirectory`,
optionally `-SDL2RuntimeDll`, and the consolidated commit in `-SourceRevision`.
All paths are supplied by the caller; none depend on a particular user's machine.
The revision string is metadata, not a substitute for independently verifying
Git HEAD, local changes and assets. The script explicitly records that limitation.

The build defaults to serial Release C++20 `/O2 /MD /fp:strict`. Optional
`-MaxCompileJobs` accepts 1 through 4 (default 1), bounding simultaneous direct
MSVC compiler invocations. This remains a full cold compilation in a new output
directory; there is no cache, incremental reuse or `/MP` nesting. Unique
per-source object names, explicit response files and manifest link order remain
the same regardless of completion order. The linker reserves 8 MiB of stack for
the existing development-client workspaces, rather than relying on the smaller
Windows default. This is bounded platform headroom, not proof of stack sufficiency.
x86, ARM64 and ARM64EC are excluded.
No `/fp:fast`, GNU link switches, fast-math fallback or numerical-domain changes
are introduced. Existing fenv/binary32 contracts stay in force. Windows native
CPU code and SDL-created desktop OpenGL GPU rendering remain the execution path.

The scheduler checks for exited compilers before each new launch and polls at
50 ms while all slots are occupied. On the first observed compiler failure or
missing object, it stops launching sources, terminates and waits for its still
running compiler processes, and refuses to link a partial set. Ownership is
registered before starting each process and retained through its process handle;
cleanup never searches by process name/PID or kills unrelated processes. Only
direct MSVC `cl.exe` is supported, not compiler wrappers spawning child tools.
Exception and normal Ctrl+C unwinding use the same cleanup. Forced termination
of the PowerShell host, OS shutdown, or a denied termination cannot be guaranteed
by script-level cleanup; cleanup errors are retained in the build receipt.

Each source writes stdout to its numbered `.log` and stderr to the matching
`.stderr.log`; streams copy directly to disk without interleaved console output.
Live progress is ordinary stdout: each `compile-progress ` prefix is followed by
one compact JSON record containing the job bound and started, completed, failed,
cancelled, running and not-started source counts. Records occur at initialization,
source transitions, compilation completion and cleanup, never on idle polls:
at most `2 * sourceCount + 3` records (719 for the current source gate).
Completed means exit zero with an object present; failed includes launch failure,
a nonzero compiler exit or a missing object; cancelled means the script terminated
an active compiler. Counts do not certify build acceptance.

No `compile-progress.json` or progress temporary files are created or updated.
Observers may ignore stdout or read the supervising wrapper's existing console
log. Parse only complete newline-terminated records with the exact prefix, and
ignore other console lines and an incomplete trailing line until more arrives.
For a live log reader, use `FileAccess.Read` with `FileShare.ReadWrite`, and dispose
promptly; do not replace or modify the log. A wrapper should redirect stdout to a
regular file or continuously drain its pipe while waiting for the build.
Progress requires no file replacement permission, retries or security changes;
output and other I/O errors are not silently suppressed.

The final build receipt includes the current in-memory compilation summary; it
does not reconstruct counters from stdout. A later link or input-hash failure
still fails the build even when compilation is completed.

The output includes response files, compile/link logs, hashed source/header and
compiler inputs, a receipt, the PE32+ AMD64 executable, and the supplied SDL DLL
when requested. The script checks inputs did not change during compilation.
It does not attest toolchain DLL/header/SDK closure, OpenGL capability, CRT/runtime
installation, actual audio device readiness, assets, or gameplay correctness.
The `/MD` Visual C++ runtime and SDL2 dependencies must be verified separately;
there is no installer, download, PATH-wide runtime lookup or asset copier.

## Source reconciliation and limits

`interactive50du.sources.json` freezes 352 C++ owners from the actual layered
`prepare_interactive_build50dv.py` / `prepare_offscreen_build50do.py` /
`check_terrain_registered_drive50bv.py` declarative lists. Six additional
vendored zlib C files bring the build total to 358. The matching PowerShell
C++ count gate is 352. The three generator provenance hashes still match the
source reviewed on October 16; generators were read, not executed or imported.

The passing Linux Ryloth build uses 354 C++ units. It contains all 352 Windows
units plus the Linux-only offscreen EGL context and the additional unused
`static_draw_assets50dw.cpp` library unit. The dedicated RA renderer and session
owners are included by existing translation units. This is source-list coverage,
not evidence of a Windows build or smoke at the Ryloth revision. See the
[October 16 source review](../../../docs/recovery/windows-ryloth-source-review-20261016.md).
Hash gates require renewed review if their generator files change.

The Windows closure excludes `offscreen_gl_context50dc.cpp`,
`vehicle_offscreen_gl50dd.cpp`, and `vehicle_offscreen_present50de.cpp`.
GameHost must reject headless operation explicitly; no offscreen fallback or
success stub is allowed. `joystick_transport50dk.cpp` is not in the direct
integrated closure. `native_camera_view50dx.cpp` now has a concrete original-camera caller and is
included in both central and Windows source closures.
Audio, material-audio, speech, original UI, Options/Controls/Game Options, Rider, rendering,
physics and original-AI owners in the direct generator are retained.

Many file names say `fixture`; they are retained development-client owners, not
permission to run historical test scripts. Source-only clones omit some local
assets/fixtures. Missing compile inputs fail normally; nothing is generated or
substituted. Compilation does not enable or prove menu-to-race, all-track, long-run
or full-game correctness. No host-driver, keyboard-driving or synthetic continuation
is authorized by this entry. Later gameplay validation must independently enforce
eight original AI owners, zero humans including camera slot 0, and no host driving.
Frontend smoke is a separate root-approved check; the build cannot consume it.

## Runtime launch boundary

A successful build is not a successful Windows launch. The October13 00:04UTC
launch of source `0d1c758` exited1 before menu rendering because the invocation
combined `RR_START_MENU=1` with `RR_ORIGINAL_MAIN_MENU=1`. These select different
menus. Original Main requires the host startup menu to stay off:

- `RR_ORIGINAL_MAIN_MENU=1`
- `RR_START_MENU=0` (now also the default when Original Main is selected)
- `RR_ORIGINAL_UI_ROOT` points to the actual original UI archive root
- `RR_ORIGINAL_MAIN_AUDIO=muted` or its fully prepared enabled-audio path

Original Main now has a dedicated frontend-only lifetime: its UI archives,
context, shaders, textures and audio are prepared without opening the race
corpus. The existing CORPUS_JSON argument remains required syntactically, but
that file is only needed for actual Course race entry. `RR_RACE=0` may be used
for a menu-only smoke; it explicitly disables Course race Confirm. Main/Options
can render without a roster; optional Rider/Course screens still require their
real UI caches. A present malformed roster remains an error at Course admission.

Every accepted Course launch closes the frontend and loads the selected race
from its own authentic assets. Rider19 still needs its prepared supplemental
roster/rig/models and original archive. `RR_RACE_MODE=2` is the current Course
domain. Keep these source guards intact when preparing a playable runtime.
For a driving smoke, `RR_ORIGINAL_AI_ALL_RACERS=1` is required for every racer.
A menu smoke must report actual `original_main_ready` after successful native
render and window swap, followed by clean exit; context creation alone is not
menu readiness. No runtime success is established by these source changes.

## Separate release staging

`Package-Interactive50DU.ps1` consumes an explicitly approved inventory and an
existing build receipt; it never invokes this build script. Its new-directory
output places the native executable and SDL2.dll in `original/`, beside original
SLUS data, and keeps `generated/` and optional `rider/` separate. Build receipts
now identify staged runtime hashes and explicitly retain `distributionReady=false`.
A build receipt remains build evidence only, even when SDL was supplied.

See [the packaging contract](../../../docs/recovery/windows-release-packaging-20261014.md)
for the schema, provenance requirements, receipt boundaries and manual invocation.
The manifest count gate above remains unchanged. Older status prose with different
counts requires parent reconciliation against the reviewed manifest/generators;
packaging does not regenerate lists or infer source identity from a count.
