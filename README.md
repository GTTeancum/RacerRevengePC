# Racer Revenge PC

A native PC reimplementation of Star Wars: Racer Revenge, with shared code designed for a future original-Xbox port.

## Status

The current Linux WIP adds a source-reviewed batching optimization for racer observations. It is uncompiled and untested; speedup is unmeasured. The preceding source passed a bounded GC run with all eight original-AI racers, zero HOST/controller input, and visible source-normal lighting. See [candidate scope](docs/native-ai-lighting-integration.md).

The preceding run remained in startup state 4 and completed no lap. Its video is an incomplete silent prefix. Full-course GC driving, original scheduler/shader parity and real-time performance remain unfinished. See [current validation](docs/current-validation.md).

## Build and game data

Supply your own original game data and generate local caches. Assets, executables, and memory dumps are excluded; some source-comparison fixtures also require local generation.

Start with the [interactive build guide](docs/interactive-build50dv.md). Linux requires C++20, Python 3, zlib, SDL2, and desktop OpenGL. See [input requirements](docs/source-only-exclusions.md). Windows source is retained.

## Controls

- W / S: left-engine throttle / brake
- Up / Down: right-engine throttle / brake; unequal input steers
- Space: boost; Left Shift: powerslide; R: repair
- Backspace: checkpoint return, or race restart; F5: restart
- Escape: pause/resume; Up/Down or W/S: menu navigation; Enter or Space: select

Windowed startup opens Main; headless menus require `RR_START_MENU=1`. Vendored licenses remain intact.
