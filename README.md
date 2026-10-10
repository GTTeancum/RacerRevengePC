# Racer Revenge PC

A native PC reimplementation of Star Wars: Racer Revenge, with shared code designed for a future original-Xbox port.

## Status

Native Linux gameplay is running with eight original-AI racers. The latest capture is partial: GC still stops at an unfinished AI recovery path, and its Gate opening remains incomplete.

Original-art UI/menu integration is in progress and is not active yet. See [current validation](recovery/20261010/VALIDATION-1619.md) for the tested build and remaining limits.

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
