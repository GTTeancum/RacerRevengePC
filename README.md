# Racer Revenge PC

A native PC reimplementation of Star Wars: Racer Revenge, with shared code designed for a future original-Xbox port.

## Status

This branch preserves **unexecuted 12-or-60-second all-eight original-AI capture and matching prefix-export tools**. They are bound to runtime `bf42968a`, exact existing source/ledger inputs and one centrally reserved process. The next requested window is 60 seconds. Fresh images use paired native audio counters; artifact success never upgrades runtime acceptance. See the [tool README](tools/capture/all_original_ai/README.md).

The inherited runtime is an earlier prototype, not the required all-AI candidate. Old capture tools remain available. No imports, syntax checks, gameplay or encoding were run for this backup. Source boundaries and incomplete-prefix disclosures remain explicit.

## Building and game data

Supply your own original game data and generate the required local caches. Game assets, executables, and memory dumps are not included. Some source-comparison tests also need locally generated fixtures.

Start with the [interactive build guide](docs/interactive-build50dv.md). The Linux client needs a C++20 toolchain, Python 3, zlib, SDL2, and desktop OpenGL. See [input requirements](docs/source-only-exclusions.md) for omitted data and [technical notes](docs/) for implementation details.

## Controls

- W / S: left-engine throttle / brake
- Up / Down: right-engine throttle / brake; unequal engine input steers
- Space: boost; Left Shift: powerslide; R: repair
- Backspace: return to checkpoint; F5: restart; Escape: quit

Gameplay, roster integration, progression, performance, and cross-platform support remain unfinished. This is a development prototype. Vendored components retain their existing license notices.
