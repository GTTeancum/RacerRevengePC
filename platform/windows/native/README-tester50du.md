# Manual Windows tester entry

`Launch-Interactive50DU.ps1` opens an already built native x64 client once in a
real window at original Main. It is a manual tester entry, never an automatic
test runner. Creating or integrating these files does not launch anything.
Root owns integration and the shared hourly acceptance slot; this script does
not reserve that slot or authorize a test.

Use Windows PowerShell 5.1 or PowerShell 7, an approved native Windows build,
its adjacent approved x64 `SDL2.dll`, and already installed Visual C++/graphics
dependencies. Obtain the original disc assets and matching prepared runtime
from their owner first. The launcher does not build, prepare/decode, download,
install, copy, delete or modify assets, nor change execution policy.

## Invocation

From the repository root, replace these example paths with your own existing
inputs. Run manually only when ready; do not dot-source the script.

```powershell
& .\platform\windows\native\Launch-Interactive50DU.ps1 `
    -NativeExecutable 'C:\RacerTester\original-disc\rr_interactive_vehicle50du.exe' `
    -OriginalAssetRoot 'C:\RacerTester\original-disc' `
    -GeneratedRuntimeRoot 'C:\RacerTester\prepared-runtime' `
    -Corpus 'C:\RacerTester\prepared-runtime\evidence\staticType2Producer49X\original-word\native-fixtures.json' `
    -RiderIpumCache 'C:\RacerTester\prepared-rider-cache'
```

The four required parameters are the native executable, original asset folder,
prepared generated-runtime folder, and corpus file. `-RiderIpumCache` is optional;
omit its line for Main/Options only. No cache location is guessed. Each input is
resolved from the invoking shell before launch; absolute paths are recommended.
In particular, a relative `-Corpus` is relative to that shell, not the generated
root. The launcher passes its resolved absolute path as the sole native argument.

The original asset folder must directly contain the approved native EXE and
`SDL2.dll` beside `SLUS_202.68`, plus `APPSTART.RES`, `LOC.RES` and the original
`UI` tree. Before launch, the EXE's parent directory must match the resolved
original asset directory (case-insensitive, ignoring trailing separators).
Use the same directory path for both parameters; differing path aliases are not
resolved to filesystem identity. A mismatch stops with a staging error; the
launcher moves or copies nothing. The child's working directory and both
original asset/UI roots point there. Only `-NativeExecutable` is executed;
`SLUS_202.68` remains original data and is never executed through a guest or
emulator. The generated root must directly contain the prepared `evidence` and
`work` trees. Original disc requests map to the original asset root, even if the
prepared runtime contains a historical disc copy.

Rider/Course entry additionally needs the matching prepared Rider image cache,
original Rider/Course/background/pod UI archives, authentic `AUDIO`, and prepared
roster, rig, models, cameras and selected-track inputs. Muted ALL8 still uses
original logical sound/sample owners. Directory/file presence is only a basic
preflight; the native client authenticates archive/cache content and admits each
Course destination. Missing or incompatible data must be supplied by the runtime
owner; this launcher creates no substitute.

## Fixed launch behavior

- Original Main: `RR_ORIGINAL_MAIN_MENU=1`, `RR_START_MENU=0`.
- Default `-AudioProfile muted` output: `RR_ORIGINAL_MAIN_AUDIO=muted`, `RR_AUDIO=0`, `RR_AUDIO_DEVICE=0`.
  No device or WAV recording is requested; this is not enabled-audio coverage.
- Smoke-capable Course configuration: `RR_RACE=1`, `RR_RACE_MODE=2`, initial
  `RR_TRACK=TA`, `RR_ORIGINAL_AI_ALL_RACERS=1`, `RR_AUTODRIVE=0`, and
  `RR_DISABLE_RACER_CONTACTS=0`. Every admitted racer, including camera slot 0,
  uses original AI. This entry provides no human driving or temporary course
  driver. Course selection and admission remain owned by the original UI.
- Before setting those values, the child environment drops every inherited
  `RR_`, `SDL_`, `MESA_` and `LIBGL_` key, case-insensitively. Other ordinary
  environment variables are inherited. Parent/machine environment and shell cwd
  stay unchanged. No headless, diagnostic, trace, capture, forced-lighting or
  synthetic-input switches are supplied.

Use actual keyboard input: arrows navigate/change, Enter confirms, Escape backs
out; close the game window to finish. The console remains attached for native
errors. The script waits for that one process and returns its exit code, with no
timer, automated navigation, relaunch loop, screenshot or success classification.
Keep the console open until the game closes; there is no process-tree supervisor.

## Status and limits

The packaging/audio-profile revision was authored against source `79fc223eda2da3d98929c9f3b1253745d17e7c2a`.
It has not been executed, syntax-checked or tested. No build, asset scan/decoding,
game launch or screenshot validation was performed for it. The EXE and SDL2 DLL PE header checks
reject obvious non-Windows/non-x64 inputs; it does not prove binary provenance,
SDL ABI, CRT installation, GL capability, asset completeness or playability.
Use only the independently approved matching native executable.

The retained [Windows launch receipt](../../../recovery/20261013/windows-launch-0d1c758-receipt.json)
records a failed older launch with conflicting startup menus, not a successful
menu render. This entry follows the corrected [launch boundary](README-interactive50du.md#runtime-launch-boundary)
and the current native muted-ALL8 implementation. Historical receipts do not
validate this launcher or a new binary. A zero exit code or created window is
not proof of `original_main_ready`, visible correctness, Course completion or a
full race. The current muted configuration also does not cover Game Options
Accept paths that require enabled frontend audio. Runtime/menu/gameplay
acceptance remains outstanding with root and human testers.

## Optional audio and staged packages

Omitting `-AudioProfile` explicitly selects `muted`. To request audible frontend
and race output, supply `-AudioProfile enabled`: this sets
`RR_ORIGINAL_MAIN_AUDIO=enabled`, `RR_ORIGINAL_MAIN_AUDIO_ROOT=<original>/AUDIO`,
`RR_AUDIO=1`, `RR_AUDIO_DEVICE=1`, and `RR_AUDIO_ROOT=<original>/AUDIO` together.
Authentic original AUDIO must exist; native owners still authenticate/prepare it.
There is no generated-audio fallback, WAV capture, inherited setting override or
automatic retry with mute on failure. Both profiles retain all eight original AI
controllers, including the player slot, and `RR_AUTODRIVE=0`. Enabled is a request,
not a claim of audio/menu acceptance or device readiness. Muted remains outside
coverage of Game Options Accept paths requiring enabled frontend audio.

The separate [packaging entry](../../../docs/recovery/windows-release-packaging-20261014.md)
records a complete declared file inventory, provenance and staging results.
Its `launch-parameters.json` paths are relative to the package root, whereas the
launcher resolves arguments from the invoking shell. Resolve those paths before
invocation as shown in that document. The launcher does not consume or authenticate
package receipts; parent must approve the receipt and matching files first.
Never point this launcher at a historical `bin/` package without proper restaging.
