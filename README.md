# Racer Revenge PC

A native PC reimplementation of Star Wars: Racer Revenge, with shared code designed for a future original-Xbox port.

## Status

This branch preserves an **uncompiled and untested original-AI dynamic-contact candidate**, pending independent review of the main loop, shared event FIFO and feedback prefix. The optional, default-off path connects actual contact lists, native damage/severity and reached music/event requests. See [source scope and remaining boundaries](docs/original-ai-dynamic-contact50iw.md).

This source is staged separately from the active first-slice candidate. Authentic controller feedback, opponent speech, announcer types 1/2 and music fade retain explicit stopping boundaries; no complete collision-effects or full original-AI claim is made. See [current validation](docs/current-validation.md) for the inherited Linux baseline; those results do not validate this candidate. Earlier partial work remains in Git history.

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
