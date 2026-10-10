# Racer Revenge PC

A native PC reimplementation of Star Wars: Racer Revenge, with shared code designed for a future original-Xbox port.

## Status

This branch preserves an **unintegrated, uncompiled and untested optional timing overlay** on the all-AI/lighting/route candidate. It adds observations around countdown service and existing shared-audio commits. It does not claim to fix simulation/audio drift. See [scope and capture rebinding requirements](docs/native-audio-phase-observation.md).

The active runtime remains on its separate combined branch. Its exact source/phase hashes differ from this overlay, so the current collector must not silently accept this modified source. Earlier Clegg-only validation does not cover this work. No build, syntax check, gameplay or encoding was performed for this backup.

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
