# Racer Revenge PC

A native PC reimplementation of Star Wars: Racer Revenge, with shared code designed for a future original-Xbox port.

## Status

This branch preserves an **uncompiled and untested all-racer original-AI source candidate**, pending independent source review and central integration. The opt-in configuration assigns real AI owners to all eight racers, including the camera-followed slot, with zero humans. It builds on the separate dynamic-contact/music source candidate. See [ownership, admission and remaining boundaries](docs/original-ai-all-racers50ix.md).

This source configuration has not been exercised. It rejects HOST autodrive and retains explicit combat, event, lifecycle and incomplete-GC boundaries. A short prefix or source review cannot establish a complete race. The earlier bounded Clegg-only AI result does not validate all-eight AI. Renderer lighting remains separate untested work. See [current validation](docs/current-validation.md) for inherited prototype history; it does not validate this branch.

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
