# Native original-AI first slice: unfinished recovery source

This branch preserves partial source work based on GC candidate 53d44876 and the frozen native AI speed/control/boost components. It is unfinished, uncompiled and untested. It is not integrated into the active GC candidate, and no original-AI racer is enabled.

The snapshot includes native AI navigation, desired-position geometry, manager-order racer-list classification, startup following/avoidance, steering, separate AI frame read/bind entry points, and bounded chariot/static-impact continuations. The exact-distance/no-iteration planner path preserves incoming output. These are source changes awaiting a connected implementation and central validation.

The new AI admission marker defaults false. Vehicle mode 1 remains rejected until a genuine cold/runtime join binds it; existing player entry points explicitly reject AI mode. The main, grid and racer-owner files are unchanged base copies. Main-loop and cold-owner joins are incomplete, and cold helper code remains under source review.

Original root scheduling, target search, branch selection, reset and additional modes remain explicit unfinished boundaries. No controller, complete original AI, full race, source parity or successful build is claimed. No tests, builds, syntax checks, candidate importers, gameplay or emulation were performed while preserving this snapshot. Original assets and raw original evidence are excluded. See [mode continuations](original-ai-mode-continuations.md) for the bounded source behavior.
