# Racer Revenge PC

A native PC reimplementation of Star Wars: Racer Revenge, with shared code designed for a future original-Xbox port.

## Status

This branch preserves an **unexecuted direct-GC original-AI capture harness**. It requires the separately guarded AI runtime and existing central reservation workflow; it permits one process and a 12-second observation window after ready. See the [harness README](tools/capture/native_ai/README.md) for prerequisites, explicit AI/brake acceptance and source-stop limits.

The inherited runtime source is an earlier tested Linux prototype with rendering, physics, audio and eight-racer racing. It does not contain the new original-AI integration. See [current validation](docs/current-validation.md) for that baseline's scope. No runtime, parser, syntax check or encoding was performed for this helper backup.

## Building and game data

Supply your own original game data and generate the required local caches. Game assets, executables, and memory dumps are not included. Some source-comparison tests also need locally generated fixtures.

Start with the [interactive build guide](docs/interactive-build50dv.md). The Linux client needs a C++20 toolchain, Python 3, zlib, SDL2, and desktop OpenGL. See [input requirements](docs/source-only-exclusions.md) for omitted data and [technical notes](docs/) for implementation details.

## Controls

- W / S: left-engine throttle / brake
- Up / Down: right-engine throttle / brake; unequal engine input steers
- Space: boost; Left Shift: powerslide; R: repair
- Backspace: return to checkpoint; F5: restart; Escape: quit

Gameplay, roster integration, progression, performance, and cross-platform support remain unfinished. This is a development prototype. Vendored components retain their existing license notices.
