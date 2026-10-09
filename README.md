# Racer Revenge PC

A native PC reimplementation of Star Wars: Racer Revenge, with shared code designed for a future original-Xbox port.

## Status

The current Linux prototype includes native rendering, vehicle physics, audio, keyboard controls, and an eight-racer development race. Windows source is retained, but the current interactive client targets Linux.

The current mode1 path has passed a full three-lap race with native lap timing, Dax announcements, last-lap/finish music, and completed-lap display. See [current validation](docs/current-validation.md) for tested scope and remaining gaps.

## Building and game data

Supply your own original game data and generate the required local caches. Game assets, executables, and memory dumps are not included. Some source-comparison tests also need locally generated fixtures.

Start with the [interactive build guide](docs/interactive-build50dv.md). The Linux client needs a C++20 toolchain, Python 3, zlib, SDL2, and desktop OpenGL. See [input requirements](docs/source-only-exclusions.md) for omitted data and [technical notes](docs/) for implementation details.

## Controls

- W / S: left-engine throttle / brake
- Up / Down: right-engine throttle / brake; unequal engine input steers
- Space: boost; Left Shift: powerslide; R: repair
- Backspace: return to checkpoint (restart the whole race in race mode); F5: restart
- Escape: pause/resume menu; Up/Down or W/S: navigate; Enter or Space: select
- Start, Controls, Restart, Main Menu, and Exit are available in the host menus

Windowed startup opens the front menu; headless remains automatic unless
`RR_START_MENU=1` is set. See [host session menus](docs/host-session-menus50if.md)
for exact behavior and the unvalidated implementation boundary.

Gameplay, roster integration, progression, performance, and cross-platform support remain unfinished. This is a development prototype. Vendored components retain their existing license notices.
