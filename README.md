# Racer Revenge PC

A native PC reimplementation of Star Wars: Racer Revenge, with shared code designed for a future original-Xbox port.

## Status

Native Linux gameplay is running with eight original-AI racers. The latest capture is partial: GC still stops at an unfinished AI recovery path, and its Gate opening remains incomplete.

The authentic original Main menu opens and navigates on native Linux. The latest focused check on October 11, 2026 built successfully but stopped when Options resource preparation failed. The original DAT loading fix, Game Options editing/Cancel/Accept, and newer Rider rendering/resource work are implemented in source and await runtime validation. Rider selection and the original menu-to-race flow are not yet available. See the [latest observation](recovery/20261011/MAIN-OPTIONS-OBSERVATION-0216.md) and [earlier gameplay validation](recovery/20261010/VALIDATION-1619.md) for their separate evidence and limits.

## Build and game data

Supply your own original game data and generate local caches. Assets, executables, and memory dumps are excluded; some source-comparison fixtures also require local generation.

Start with the [interactive build guide](docs/interactive-build50dv.md). Linux requires C++20, Python 3, zlib, SDL2, and desktop OpenGL. See [input requirements](docs/source-only-exclusions.md). Windows source is retained.

## Controls and frontend modes

- W / S: left-engine throttle / brake
- Up / Down: right-engine throttle / brake; unequal input steers
- Space: boost; Left Shift: powerslide; R: repair
- Backspace: checkpoint return, or race restart; F5: restart
- Escape: pause/resume; Up/Down or W/S: menu navigation; Enter or Space: select

These driving and pause controls belong to the existing native host session. Its menus are not the original game UI. Automated driving checks use the original in-game AI for every racer, including the followed player.

The original frontend is an explicit separate entry: `RR_ORIGINAL_MAIN_MENU=1`, `RR_ORIGINAL_UI_ROOT` pointing to the original game-files directory, and `RR_ORIGINAL_MAIN_AUDIO=muted` or `enabled`. Enabled audio also requires `RR_ORIGINAL_MAIN_AUDIO_ROOT` pointing to original AUDIO. Main/Options use Up/Down and Enter/Escape; Game Options uses Up/Down for groups, Left/Right for values, R for Defaults, Enter for Accept, and Escape for Cancel. Accept requires the genuine enabled frontend audio owner. These newer Options paths have not yet passed a runtime check.

Vendored licenses remain intact.
