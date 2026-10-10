# Linux native build entry point50DJ

Source-only build wiring; configuration/build has not been executed here. The
managed workspace currently uses direct g++ commands. Do not install tooling
or run another smoke merely to try this entry point.

From project root:

    cmake -S platform/linux/native -B build/linux -DCMAKE_BUILD_TYPE=Release
    cmake --build build/linux --target rr_drive_smoke50db

Core is a static library of actual native runtime/asset modules. The executable
is explicitly a diagnostic client with retained cold fixture setup, not a full
game launcher. Run it from the project root, in the permitted hourly slot only:

    build/linux/rr_drive_smoke50db evidence/staticType2Producer49X/original-word/native-fixtures.json

Optional EGL path:

    cmake -S platform/linux/native -B build/linux -DRR_BUILD_OFFSCREEN_SMOKE=ON
    cmake --build build/linux --target rr_offscreen_smoke50dh

It needs Linux thread/dynamic-loader support and suitable runtime EGL/OpenGL.
No EGL development headers or GL loader library is required by the source.
The optional executable takes the same corpus argument plus an output PPM path;
it requires a real offscreen context and never falls back to fake rendering.

The two executables are alternative checks, not a batch to run together. There
is intentionally no CTest or automatic aggregate test registration. Preserve
strict floating-point flags. No test/build claim follows from this build file.

## Interactive client addition 50DV (source only)

RR_BUILD_INTERACTIVE_CLIENT=ON adds rr_interactive_vehicle50du using the
runtime-loaded SDL2 host, keyboard mapper and desktop GL renderer. No SDL SDK
headers or SDL link library are required by the source interface; SDL2 runtime
and a usable desktop display/context are required when launched. It retains the
explicit development scene from50DU; enabling this target is not a whole-game
completeness claim. No automatic execution or CTest registration is added.
