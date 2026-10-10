# Racer Revenge PC

A native PC reimplementation of Star Wars: Racer Revenge, with shared code designed for a future original-Xbox port.

## Status

This Linux WIP passed a bounded GC run with all eight racers using original AI, zero HOST/controller input, and source-normal racer lighting visible in the actual capture. It retains source route choices and dynamic contacts. See [candidate scope](docs/native-ai-lighting-integration.md).

Performance remains limited: every racer stayed in startup state 4 during the short run, and no lap was completed. The recorded video is an incomplete silent prefix. Full-course GC driving, original scheduler/shader parity, controller usability and cross-platform support remain unfinished. See [current validation](docs/current-validation.md).

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
