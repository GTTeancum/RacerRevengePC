# Racer Revenge PC

A native PC reimplementation of Star Wars: Racer Revenge, with shared code designed for a future original-Xbox port.

## Status

This branch preserves **uncompiled and untested racer-lighting source**, integrated centrally for a future check. Authored source normals now reach animated player/opponent normal streams and opaque racer lighting. Effects remain outside this lighting path. This is native host illumination; original PS2 shader parity is unclaimed. See [lighting scope and pending checks](docs/racer-source-lighting.md).

The inherited source is the guarded original-AI first-slice candidate. Its bounded GC check exercised Clegg as the only original-AI racer while all eight native racers advanced; it does not validate this lighting change or all-eight original AI. See [current validation](docs/current-validation.md) for earlier prototype coverage. Visual correctness, performance and cross-platform support remain unfinished.

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
