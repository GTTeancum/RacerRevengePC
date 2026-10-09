# Racer Revenge PC

A native PC reimplementation of Star Wars: Racer Revenge, with shared code designed for a future original-Xbox port.

## Status

The current Linux prototype includes native rendering, vehicle physics, audio, keyboard controls, and an eight-racer development race. Windows source is retained, but the current interactive client targets Linux.

This branch contains newer original-roster integration that is unbuilt and untested.

## Building and game data

Supply your own original game data and generate the required local caches. Game assets, executables, and memory dumps are not included. Some source-comparison tests also need locally generated fixtures.

Start with the [interactive build guide](docs/interactive-build50dv.md). The Linux client needs a C++17 toolchain, Python 3, zlib, SDL2, and desktop OpenGL. See [input requirements](docs/source-only-exclusions.md) for omitted data and [technical notes](docs/) for implementation details.

## Controls

- W / S: left-engine throttle / brake
- Up / Down: right-engine throttle / brake; unequal engine input steers
- Space: boost; Left Shift: powerslide; R: repair
- Backspace: return to checkpoint; F5: restart; Escape: quit

Gameplay, roster integration, progression, performance, and cross-platform support remain unfinished. This is a development prototype. Vendored components retain their existing license notices.
