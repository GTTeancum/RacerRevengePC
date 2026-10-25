# Windows x64 experimental tester: v105 compile/link checkpoint

Status: corrected Windows cross-compilation/link PASS. Windows runtime has never been launched or tested on Windows. This is not a Windows gameplay, persistence, visual acceptance, full-beta, or CI pass.

## Exact build

- Build: game-build-105, corrected Windows cross-compile/link recipe.
- Actual inventory: 501 units, comprising 495 C++ and six vendored zlib C units; eight fresh compiles, 493 reused objects, zero failed units, link exit 0. Elapsed time: 287.76809430122375 seconds. The earlier expected inventory of 499 units was incorrect; the terminal object inventory is authoritative.
- Executable SHA-256: `ca4a5eae44c29246eee2a31e3788699ec55db2ac9bd15d1e7d45ff5e0901e9d7`.
- PE inspection: AMD64/PE32+ Windows console executable, 8 MiB reserved stack. The executable import table names only KERNEL32.dll and msvcrt.dll; this does not remove its runtime-loaded SDL/FFmpeg dependencies or certify Windows execution.
- All 4,690 candidate source files match game-build-105 SOURCE-MANIFEST.json. Manifest membership is not evidence that every source file was compiled or executed. Reused objects are not fresh compilations.
- Existing Windows offscreen-source exclusions: platform/linux/native/offscreen_gl_context50dc.cpp, platform/linux/native/vehicle_offscreen_gl50dd.cpp, and platform/linux/native/vehicle_offscreen_present50de.cpp. Of the preceding 496-unit C++ inventory, only offscreen_gl_context50dc.cpp was present; the other two were already absent. The final 501-unit inventory has no offscreen-named source entries. Visible native SDL context files remain included. These exclusions are not newly verified Windows paths.

Runtime dependency SHA-256 pins, verified from the local dependency files:

- avcodec-61.dll: `04859349beefb78e0d6a2c0dfafa6d5099d851d85de7ad53bff0e249bbae1422`.
- avutil-59.dll: `ab6af2930bc607b9fafa5798d874efcdf22814488ccc56e65bb5b5e706a27099`.
- SDL2.dll: `53e8fa7e9ed43c30bd650c39396f0d9d2067d2fff0b926c9433b75202592e4aa`.
- libwinpthread-1.dll: `fc3cd567190f3f24e8201c2e1602f7d1f1e583a572efb7dbd8746d7928538e94`.

## Implemented Windows delta

The source adds a separate native Windows backend for beta settings/profile files, a process-lifetime file lock, and per-user saves under `%LOCALAPPDATA%\RacerRevengePC\Saves`. This is implementation evidence, not Windows runtime validation of save correctness, locking, replacement durability, or recovery behavior.

The separate opt-in original BASLUS-20268/BASLUS-20268 reader is read-only transport, not beta save functionality. It uses bounded reads, explicit Unicode/UTF-8 path handling and Windows handle/identity checks. It does not create, write, format, or delete original saves. Supported explicit roots intentionally exclude UNC/device/drive-relative paths and normalization aliases. Windows path, contention, reparse rejection and failure behavior have not been runtime-certified.

Windows presentation requests a fixed 1280x896 client area while retaining the source 640x448 canvas. Windows-specific drawable capacity, menu-selection diagnostics and unbuffered exit diagnostics are included. Actual DPI scaling, desktop fit, rendering and input remain unverified on Windows. The `far` local identifier is renamed to avoid a Windows header macro collision.

The CMD launcher and optional PowerShell launcher create timestamped diagnostics beside the package, with `%LOCALAPPDATA%\RacerRevenge\logs` fallback. Logs can include local paths, device/driver information and game selections; users are instructed to review them and exclude original assets and save files. No automatic upload or telemetry is enabled. The launchers clear inherited game/graphics overrides and request normal original Main entry, not fixtures or unlock overrides. No execution-policy bypass or security-setting change is requested.

The intended package dependencies are matching x64 SDL2.dll, FFmpeg 7.1.5 avcodec-61.dll and avutil-59.dll, and libwinpthread-1.dll beside RacerRevengePC.exe. Exact dependency hashes are listed above. Final archive membership is a separate packaging check; hashes and ABI-name checks alone do not establish compatible or loadable Windows dependencies. The tools retain the windows-tester-v104 directory name; this checkpoint refers to the corrected v105 source candidate.

## Evidence boundaries

The Windows runtime has never been launched or tested on Windows. Existing Linux GUI136/137 evidence covers earlier baseline binaries only and is not validation of this Windows delta. No Windows gameplay, audio output, physical controller, save persistence, imported-save behavior, full-beta completion, or CI pass is claimed. Prior visual limitations, including temporal-blending ghost trails and unverified motion-blur/camera-transition parity, are not resolved by this work.

This publication payload is source/docs only. It excludes original assets, executables, DLL binaries, user saves, screenshots, and runtime logs. Existing remote-only documents are preserved. Local packet preparation does not perform a build, test, gameplay run, upload, or GitHub write.
