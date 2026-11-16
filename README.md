# Racer Revenge PC

A native PC reimplementation of Star Wars: Racer Revenge, with shared code designed for a future original-Xbox port.

## Current handoff

See [the performance handoff](docs/PERFORMANCE_HANDOFF.md) for current source status,
last verified results, untested changes and takeover steps. The status narrative
below is historical consolidation context and predates that handoff.

## Historical consolidation status

Native host CPU/GPU reimplementation is in progress. Linux is a temporary validation platform; portable gameplay and Windows releases are the priority. All driving validation requires original in-game AI for every racer, including the camera-followed player.

The last completed focused check was the native Linux Rider19 menu opening, unavailable-direction/Confirm handling, and Back sequence on October 11, 2026 at 10:27 UTC. It passed on source commit `03df3c8`; later UI resource/lifetime/audio changes and this branch consolidation remain unbuilt and untested. Rider switching and the original menu-to-race flow are still closed. Historical GC gameplay remains partial at an unfinished original-AI recovery boundary; the newer opt-in Gate asset join has no fresh gameplay validation.

Main consolidation preserves the optional [native audio timing observations](docs/native-audio-phase-observation.md). Those observations do not fix or validate simulation/audio drift. Build and capture identities must be rebound to the final merged source before any new validation.

Historical [all-original-AI capture tooling](tools/capture/all_original_ai/README.md) is retained with its original runtime identity and incomplete-prefix limits. It has not been rebound or executed against consolidated main. Older capture helpers are retained for provenance and are not approved driving-test entry points.

The [registered-pod query partial](docs/registered-pod-query50iz.md) is retained as an optional native owner path, uncompiled and untested. No original-AI state-5/reset activation is implied.

## Build and game data

Supply your own original game data and generate local caches. Assets, executables, and memory dumps are excluded; some source-comparison fixtures also require local generation.

Start with the [interactive build guide](docs/interactive-build50dv.md). Linux requires C++20, Python 3, zlib, SDL2, and desktop OpenGL. See [input requirements](docs/source-only-exclusions.md). A source-only [Windows x64 integrated build entry](platform/windows/native/README-interactive50du.md) now retains the same native UI/gameplay owners with SDL2/OpenGL window and audio adapters. It has not been compiled or run; it is not a validated release.

## Controls and frontend modes

- W / S: left-engine throttle / brake
- Up / Down: right-engine throttle / brake; unequal input steers
- Space: boost; Left Shift: powerslide; R: repair
- A / D (or right stick left/right, default control layout): lean left / right while in zero gravity
- Backspace: checkpoint return, or race restart; F5: restart
- Escape: pause/resume; Up/Down or W/S: menu navigation; Enter or Space: select

These driving and pause controls belong to the existing native host session. Its menus are not the original game UI. Automated driving checks use the original in-game AI for every racer, including the followed player.

The original frontend is an explicit separate entry: `RR_ORIGINAL_MAIN_MENU=1`, `RR_ORIGINAL_UI_ROOT` pointing to the original game-files directory, and `RR_ORIGINAL_MAIN_AUDIO=muted` or `enabled`. Enabled audio also requires `RR_ORIGINAL_MAIN_AUDIO_ROOT` pointing to original AUDIO. Main/Options use Up/Down and Enter/Escape; Game Options uses Up/Down for groups, Left/Right for values, R for Defaults, Enter for Accept, and Escape for Cancel. Accept requires the genuine enabled frontend audio owner. These newer Options paths have not yet passed a runtime check.

Vendored licenses remain intact.
