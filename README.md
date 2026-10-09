# Racer Revenge PC

A native PC reimplementation of Star Wars: Racer Revenge, with shared code designed for a future original-Xbox port.

## Status

The GC HOST mixer passed a focused build and 60-second capture. All eight racers cleared the opening and node 40, then stopped OffCourse at nodes 43/44. **No GC lap passed.** The verified video is silent because PCM timing drifted. See [GC status](docs/gc-native-integration50ig.md).

The Linux prototype includes native rendering, physics, audio, keyboard controls, and an eight-racer development race. Clegg selection through Main, pause/resume, and a 20-second gameplay capture have passed. An earlier mode1 baseline completed a three-lap race with native timing, Dax, music, and completed-lap display. See [current validation](docs/current-validation.md) for scope.

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
