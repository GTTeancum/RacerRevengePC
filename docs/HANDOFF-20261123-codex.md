# Handoff to Codex, 2026-11-23

RacerRevengePC is a native Windows PC port of Star Wars Racer Revenge (PS2, SLUS_202.68), built from the original
game's code (disassembly-guided reconstruction, original PS2 addresses such as `1AC8E0` appear throughout the code
comments). The user is not a programmer; reports go to them in plain language.

Read first:
- `D:\Programming\GitHub\RacerRevengePC\CLAUDE.md`: the governing design rule (one common code path for every course
  and every pod; no per-course or per-pod code; faithful to the original; flag stand-ins). It applies to Codex too.
- This file, then the notes in `D:\Programming\GitHub\RacerRevengePC\docs\handoff-20261123\notes\`.

## Where things are

- **Branch:** `main` at `cc2fc1b`, pushed to origin. The working branch was `common-rewrite`
  (worktree `D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\cr-main`), fast-forwarded into main after every
  round; both point at the same commit.
- **Newest build:** `D:\Programming\GitHub\RacerRevengePC\build\cr-32\rr_interactive_vehicle50du.exe` (built, not run).
  Last build that was run: cr-31.
- **Game assets:** `C:\Games\Star Wars Racer Revenge`. SDL:
  `D:\Programming\GitHub\RacerRevengePC\toolchains\sdl2-msvc\extracted\SDL2-2.32.10\lib\x64\SDL2.dll`.
- **Scratch:** `D:\Programming\GitHub\RacerRevengePC\work\scratch` (all of `work/` is git-ignored). Copies of the
  important notes and the build/test scripts are versioned in `docs\handoff-20261123\` (paths inside them still point
  at `work\scratch`).
- **Never commit:** the ROM `Star Wars Episode I - Racer (USA).z64`, the `Star Wars - Demolition (USA)` folder,
  `01-ai-settings-188F60.patch`, logs, evidence folders.

## How to build

Incremental relink of the newest build into a new numbered build folder (one builder at a time):

```
cmd /c "D:\Programming\GitHub\RacerRevengePC\work\scratch\common-rewrite\tools\vs.cmd D:\Programming\GitHub\RacerRevengePC\work\scratch\common-rewrite\tools\relink-cr.py cr-32 cr-33 > build-cr-33.log 2>&1"
```

- `relink-cr.py <from> <to> [new.cpp=template.cpp ...]` rebuilds only the sources changed since `<from>` (it reads
  `build\<from>\build-receipt.json`) and links `build\<to>\rr_interactive_vehicle50du.exe`. A NEW .cpp file must be
  named with a template of a similar existing file, e.g. `src\game\x.cpp=src\game\dlc_rider_entries50dl.cpp`, and
  added to `platform\windows\native\interactive50du.sources.json` and `platform\linux\native\CMakeLists.txt`.
- It refuses with "an input changed during the build" if a source changes while it runs: do not edit while building.
- `WT_JOBS=5` sets parallelism. A small change takes 1-2 minutes; a header used by ~200 files about 10 minutes.
- Do not name a variable `far` or `near` (Windows header macros).
- No-game data check: `work\scratch\common-rewrite\check\build_track_check.py --build <build> --root <worktree>` then
  `check\track_check.exe --report ...`. Its "result=fail" lines are known receipt-coverage gaps, explained in
  `check\DIFFERS.txt`.

## How to test

All game runs are muted, unattended and logs-only (no screenshots inside tests); one game instance at a time.

- Runner: `D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\cr-main\tools\capture\run_windows_takeover.py`.
  Typical one-lap Single Race:
  `python -B <runner> --build <build> --output <evidence dir> --assets "C:\Games\Star Wars Racer Revenge" --sdl <SDL2.dll>
  --logs-only --seed-save D:\Programming\GitHub\RacerRevengePC\work\scratch\saves\order0.bin --player-racer 1
  --course ta --route main-rider-course --laps 1 --pause-restart 20 --results-exit main --hold-throttle --steer-invert
  --camera-cycle --repair --unstick --race-full --return-hold 20`.
  Add-on content: `--env RR_DLC_ROOT=D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\roots\testers-cr21\DLC`
  and `--course <addon code>` or `--addon-rider <package>/<item>`. Other useful options: `--main-exit fresh|return`,
  `--pause-quit SECONDS`, `--course-mode time_trial`, `--seed-slow-hall CODE`, `--hold-tournament`, `--hold-garage`,
  `--garage-from tournament|race`.
- Round script and checker: `work\scratch\common-rewrite\round.ps1` (17-run matrix, `-Only <n>`, `-Tag`, `-DlcRoot`)
  and `check-round.ps1` (writes `round-<tag>.txt`). Examples of chained batches: `round29.ps1`, `frames31.ps1`.
- A clean run ends with `native_exit_code=0` or `3` (3 = the harness's wall-budget end, the designed end of an
  unattended run) followed by `common_run_summary ... result=pass`.
- Still frames: outside the tests only, from the live window, with `work\scratch\frames\fastgrab.ps1 -Match <run>`
  (the run's `--output` must be the relative `evidence\<name>` for the grabber to match). Frames land in
  `work\scratch\frames\fast\<run>\`. Look at the frames yourself before calling something working.
- Evidence (logs): `D:\Programming\GitHub\RacerRevengePC\evidence\<run>\RacerRevenge.log`.
- FPS is measured by the game (`race_fps_summary`); report FPS to the user, not milliseconds. Another program
  loading the machine slows every part of the frame evenly; re-measure before blaming code.

Course codes: TA TB TC (Tatooine; TA is the Mos Espa Open), SA SC, MA MB MC, GA GB GC (Gamorr), RA RC. The game's
planets are Tatooine, Sullust, Mon Calamari, Gamorr, Ryloth and Coruscant; only TA's name has been read from the
game, so use codes unless a name is read from the game itself. Add-on (DLC) courses: nm Malastare 100, nd Dug Derby,
nv Vengeance, nb, nc, nr, ns, nu (N64 conversions), lc and eb (arcade conversions).

## What was done (2026-11-20 .. 23)

1. **Common-path rewrite** (about 300 per-course files retired into one race path): verified piecemeal across builds
   cr-20..cr-31. Passing on recent builds: TA, SA, MA, GA, TB, SC, GB, RC, the RA tournament race, the GC Time Trial
   (QUIT with a record -> records page -> Hall), Dug Derby. A full 17-run round on one build is out of scope (user).
2. **DLC** (add-on pods and courses from a DLC root):
   - Rider screen slot 19 general rule; plain flame-table refusal message (game log and `tools\dlc\dlc_pack.py check`).
   - N64-canonical track keys, inert on stock: `[Fog] FarDistance`, `[Environment] AirborneGravity` (96 on the
     converted courses). "N64 tracks are guests": tracks stay N64-true, pods stay Revenge-true.
   - Free-play opponent draw 1AE620 ported; add-on pods may be AI only on add-on courses.
   - DLC career (player add-on pods): rewards, side save with AI growth, career name and unlocks; Tournament screen
     package art and pod model; Garage path written. Still refused for players (see open items).
   - N64 hazards connected (asteroids, door, energy column, falling pillar, methane, debris fade, camera shake).
3. **Main menu EXIT GAME** (PC-native addition) with a yes/no box; clean exit also after a race (Back from Course,
   Confirm from the Hall).
4. **Other:** Results-screen Restart rows wired to the in-place restart; live intro pod frames (checked against the
   original); GB lightning flash overlays from the original `.acb` data; water surface on worker threads (bit-identical,
   MC back to ~60 FPS); add-on pod engine cables on Rider/Tournament.

## Open items

Built in cr-32, not yet run:
- Add-on Tournament screen material check fix (should also reach the Garage). Check with
  `docs\handoff-20261123\notes\eyecheck-career-plan.txt` (runs R2-R5, still frames). If the Garage shows the add-on
  pod with Watto, re-enable the add-on Tournament Confirm for players (the latch line sits under the refusal in
  `platform\linux\native\original_ui_rider_session50kg.cpp`; test switch `RR_TEST_ADDON_TOURNAMENT=1`).
- GB lightning: the log shows flashes firing; `sky_flash_draw ... flash_frames=` now counts flash frames actually
  drawn. The flashes are thin bolts low on three sky faces (original data), hard to see from the race camera.

Not yet passing:
- NV and MB: the unattended test driver does not finish the lap (driver improved in cr-27, still failing on NV).
- Not re-run since their latest fixes: RA single race, TC, the NV Time Trial.

DLC:
- Two-player VS: player 2's Rider page is a page flip inside the Rider screen (build-and-look rounds needed), then
  race setup, split screen (`platform\linux\native\vehicle_window_gl50ds.h`), pad 2, HUD, race end.
  Plan: `notes\two-player-plan.txt`.
- N64 hazard gaps (game side): collision-only animated pieces (Spice Mine Run), Bumpy's door type, fan start phase.
  Converter-estimated values to revisit: shake strength, respawn delay without the explode moment, near-camera
  distance, methane as one cloud. See `notes\n64-hazards-STATUS.txt` and `notes\QUEUE.txt` (tail is newest).

Rewrite stand-ins still flagged (full list `notes\STAND-INS.txt`): start fade on the camera's copy of the race flags;
the quiet first intro frame; no pod contacts on the grid; menu pop-up dialogs booted per frontend; two teardown
stand-ins; two Results-restart stand-ins.

Performance: frame preparation costs about 8-9 ms per frame on every course (FPS fine, little headroom).

## Working with the user (their stated preferences)

- Plain-language, brief reports for a non-programmer; FPS numbers; absolute paths in every message.
- Faithful to the original by default; never offer stand-ins as equal options; flag every stand-in.
- One common path for every course and pod (CLAUDE.md).
- Hooks never write memory something else owns; bounds-check every write.
- Muted, logs-only tests; no screenshots inside tests; still frames from the live window are allowed outside tests.
- Scratch files only under `D:\Programming\GitHub\RacerRevengePC\work\scratch`, never AppData.
- Size work by files and rebuild rounds, never by days.
- Push verified work to origin main when the user asks.
- Do not create rules about how you work unless the user asks for one.
- Related session: "Custom content" builds the DLC packages and the N64/arcade course conversions; converter changes
  (track keys, spline fixes, hazard rows) go to it.
