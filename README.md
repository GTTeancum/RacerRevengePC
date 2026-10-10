# Racer Revenge PC

A native PC reimplementation of Star Wars: Racer Revenge, with shared code designed for a future original-Xbox port.

## Status

This branch is the **uncompiled and untested combined Linux candidate** for all-eight original AI, dynamic contacts and source-normal racer lighting. Its source has been centrally integrated after review. Explicit source boundaries still stop incomplete behavior. See [candidate scope](docs/native-ai-lighting-integration.md).

The previous bounded GC run used original AI only for Clegg, with all eight native racers advancing. It observed startup state 4, later state 2 and brake/steer bindings, but no lap. That result does not validate this new candidate. See [current validation](docs/current-validation.md) for limits. Full GC driving, original scheduler/shader parity, controller usability and cross-platform support remain unfinished.

## Build and game data

Supply your own original game data and generate local caches. Assets, executables, and memory dumps are excluded; some source-comparison fixtures also require local generation.

Start with the [interactive build guide](docs/interactive-build50dv.md). Linux requires C++20, Python 3, zlib, SDL2, and desktop OpenGL. See [input requirements](docs/source-only-exclusions.md). Windows source is retained.

## Controls

- W / S: left-engine throttle / brake
- Up / Down: right-engine throttle / brake; unequal input steers
- Space: boost; Left Shift: powerslide; R: repair
- Backspace: checkpoint return, or race restart; F5: restart
- Escape: pause/resume; Up/Down or W/S: menu navigation; Enter or Space: select

Windowed startup opens Main; headless requires `RR_START_MENU=1` for menus. Other human selections, Audio Options, and Results presentation await focused coverage. Progression, other tracks, performance, controller usability, and cross-platform support remain unfinished. Vendored licenses remain intact.
