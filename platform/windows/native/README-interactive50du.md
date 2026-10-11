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

The build is serial Release C++20 `/O2 /MD /fp:strict`, with unique per-source
object names and explicit response files. The linker reserves 8 MiB of stack for
the existing development-client workspaces, rather than relying on the smaller
Windows default. This is bounded platform headroom, not proof of stack sufficiency.
x86, ARM64 and ARM64EC are excluded.
No `/fp:fast`, GNU link switches, fast-math fallback or numerical-domain changes
are introduced. Existing fenv/binary32 contracts stay in force. Windows native
CPU code and SDL-created desktop OpenGL GPU rendering remain the execution path.

The output includes response files, compile/link logs, hashed source/header and
compiler inputs, a receipt, the PE32+ AMD64 executable, and the supplied SDL DLL
when requested. The script checks inputs did not change during compilation.
It does not attest toolchain DLL/header/SDK closure, OpenGL capability, CRT/runtime
installation, actual audio device readiness, assets, or gameplay correctness.
The `/MD` Visual C++ runtime and SDL2 dependencies must be verified separately;
there is no installer, download, PATH-wide runtime lookup or asset copier.

## Source reconciliation and limits

`interactive50du.sources.json` freezes 277 C++ owners from the actual layered
`prepare_interactive_build50dv.py` / `prepare_offscreen_build50do.py` /
`check_terrain_registered_drive50bv.py` declarative lists at
`f4b3393f6b8b72c49f640c2c56facacabb63f0f7`, plus six vendored zlib C files.
Those generator files were read, not run or imported. Hash gates require renewed
review if they change. Linux CMake was not used as the integrated source truth.

The Windows closure excludes `offscreen_gl_context50dc.cpp`,
`vehicle_offscreen_gl50dd.cpp`, and `vehicle_offscreen_present50de.cpp`.
GameHost must reject headless operation explicitly; no offscreen fallback or
success stub is allowed. `joystick_transport50dk.cpp` is not in the direct
integrated closure. `native_camera_view50dx.cpp` is in the old CMake closure but
has no callers in this client and is intentionally absent, matching the generator.
Audio, material-audio, speech, original UI, Options/Game Options, Rider, rendering,
physics and original-AI owners in the direct generator are retained.

Many file names say `fixture`; they are retained development-client owners, not
permission to run historical test scripts. Source-only clones omit some local
assets/fixtures. Missing compile inputs fail normally; nothing is generated or
substituted. Compilation does not enable or prove menu-to-race, all-track, long-run
or full-game correctness. No host-driver, keyboard-driving or synthetic continuation
is authorized by this entry. Later gameplay validation must independently enforce
eight original AI owners, zero humans including camera slot 0, and no host driving.
Frontend smoke is a separate root-approved check; the build cannot consume it.
