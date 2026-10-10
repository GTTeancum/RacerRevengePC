# Racer Revenge PC

A native PC reimplementation of Star Wars: Racer Revenge, with shared code designed for a future original-Xbox port.

## Status

Recovery branch based on the verified GitHub source snapshot51c6e707. Some newer source is recovered separately; the latest pre-reset game build has not been restored.

The reconstructed original-art UI renderer and material adapter pass a focused Linux C++20 compile. Original UI is still inactive pending assets and screen wiring. No new gameplay or visual validation has run. See [recovery validation](docs/recovery-validation-20261010.md).

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
