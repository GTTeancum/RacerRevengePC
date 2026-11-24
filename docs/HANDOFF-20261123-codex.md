# Handoff to Codex, 2026-11-23

See the [current release handoff](HANDOFF-20261124-release.md) for the published installer and maintainer instructions.

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

GB lightning verified from the normal race camera in cr-162. A bolt appears beside the minimap at 8.05 s and clears by 8.19 s. Native frames are in `evidence/cr162-gb-lightning/capture/race-session-2/track-20.png` and `track-21.png`.

## Open items

Built in cr-32, not yet run:
- Add-on Tournament screen material check fix (should also reach the Garage). Check with
  `docs\handoff-20261123\notes\eyecheck-career-plan.txt` (runs R2-R5, still frames). If the Garage shows the add-on
  pod with Watto, re-enable the add-on Tournament Confirm for players (the latch line sits under the refusal in
  `platform\linux\native\original_ui_rider_session50kg.cpp`; test switch `RR_TEST_ADDON_TOURNAMENT=1`).

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

Rewrite cleanup (cr-165): the counted race now owns the start-fade flag. Fresh starts and restarts clear it
when the video manager accepts the fade. The restart also resets triggers, particles, and scenery at their
individual PS2 call sites. The Results replay loop resets the scene before rewinding playback and particles
after restoring the camera.

Checked on TA with eight racers: Pause Restart reached a second GO (61.9 average FPS after the restart).
A separate one-lap race reached the eight-place Results screen and completed a full replay loop.
Evidence: `evidence/cr165-ta-rewrite-restart` and `evidence/cr165-ta-rewrite-results` in the main checkout.

Counted races now build the first pod image on the scheduled intro frame, including after Pause Restart.
Grid contact checks run after all held racers update. Tatooine reached GO with eight entrants on both
starts. The precise original pre-GO contact caller is still unidentified. The PS2 race-return code at
17C1C4 constructs a new frontend dispatcher, and 1B7BD0 creates both dialogs in that dispatcher.
The port already follows this lifetime. Pause Quit now fades the race over 500 ms before returning to
Course. Evidence: `evidence/cr167-pause-course`. The historical inventory is in `notes/STAND-INS.txt`.

Time Trial now fades the authored load screen before the first race image. On Tatooine, Pause Quit
returned to Course for a nonqualifying lap. With slow Hall times seeded in a test save, a 1:07.16 lap
opened the record page. Confirming it released the paused race and opened the Hall with that lap in
first place. Native captures: `evidence/cr171-tt-record` and `evidence/cr173-tt-hall`.

Results Restart (cr-174) accepts R or controller X. A replay now draws all racers from their recorded
poses before fading in the Results panel. The restart runs inside its original input dispatch, so the
same event reaches its shared tail and later events see the disabled Results screen. On Tatooine the
AI pilot finished a one-lap Single Event, restarted from the eight-place Results screen, finished the
second race, and confirmed back to Course. Average race FPS was 60.9 and 61.8.
Evidence: `evidence/cr174-results-restart`. Pause Restart also reached a second eight-racer GO
(`evidence/cr174-pause-restart`). Time Trial Pause Restart now submits the music fade queued during
Pause before starting the next song. Its fresh and restarted countdowns each ran 327 held frames and
released the racer at GO (`evidence/cr175-tt-restart`).

Tournament Results Restart (cr-176) also cancels the outgoing race’s pending tournament result in
the profile. Two full three-lap Mos Espa tournament races ran consecutively via the Restart row
at 1C9774. Accepting the second result committed it and returned to frontend screen 13. Average
race FPS was 61.4 and 61.7. Evidence: `evidence/cr176-tournament-restart`.

In cr-177 the Time Trial record page uses the Results renderer while the race is paused. Native
framebuffer captures show the name-entry page displaying 1:07.61 and the Hall listing the same player
and time after confirmation. See `evidence/cr177-tt-hall/record-hall-review.jpg`.
The bounded capture runs exit with code 3 when the requested frontend hold expires.

Performance: frame preparation costs about 8-9 ms per frame on every course (FPS fine, little headroom).


## Personal Windows launcher (cr-179)

`build/personal-launcher/RacerRevengeLauncher.exe` in the main checkout is the
single-file, GUI-only personal installer. It accepts a USA Racer Revenge PS2
ISO or extracted disc folder. The supported Racer Arcade ZIP and USA Episode I
Racer N64 ROM independently select the embedded personal DLC packages. Pugwis
is an editable example under `Templates/demolition-pugwis`. The installed game
runs directly. The installed launcher saves video, input device, and both player
schemes in the game's INI. Builder and WinForms source are in
`platform/windows/launcher` on this worktree. The embedded personal content
keeps the executable local. Setup and settings renders, a direct-start log and
native game captures are under `evidence/personal-launcher` in the main checkout.

To reproduce the borderless launcher artwork, pass
`--race-frame work/scratch/icon/mos-espa-race-frame.png` to the personal builder
from the main checkout. The builder crops the native race capture and embeds it
in the executable. Updated window captures and profile checks are under
`evidence/launcher-redesign`.

For launcher-to-game resolution checks use an isolated install. Save the
settings through the launcher, then start the same launcher with
`--test-play-capture`. It invokes the normal Play handler after the window
opens. Only this opt-in launch passes the game's explicit asset argument so
its existing process-local capture route can run. Ordinary Play continues to
start the game without arguments. The game clears inherited test overrides on
normal startup. Compare the game process's parent PID, requested size, race
target capacity and captured race image dimensions. cr-179 allocates the race
targets to include the retained window drawable, even when it exceeds the size
of the attached monitor. The supported window size is 640–3840 pixels wide and
448–2160 pixels high. Native captures and logs are in
`evidence/launcher-play/fixed` in the main checkout.

### Keyboard bindings (cr-180)

Open **Controls & video > Keyboard bindings**. Select Standard or Twin engine,
select an action and press its new key. Delete clears the assignment, Escape
cancels it and Restore layout defaults resets the selected family. Duplicate
keys are flagged within each family. Zero-G lean applies to Standard 1.
Use bindings returns changes to the launcher's draft. Save settings or Play
writes the profile. Cancel discards the editor's draft.

The launcher and game share the action definitions in
`platform/linux/native/keyboard_actions.inc`. The game reads physical SDL
scancodes from the local INI sections KeyboardStandard and KeyboardTwin on
startup. Existing installations without those sections use the defaults.
The overrides apply to racing on the Player 1 keyboard. Mouse bindings remain open.

The cr-180 launcher Play tests exercised remapped Standard throttle and both
Twin engine axes through process-local input. All eight original presets
passed packet-level checks for rebinding, clearing, release and focus loss.
Native form pictures and game logs are under `evidence/keyboard-bindings`
in the main checkout.

### Controller bindings (cr-181)

Open **Controls & video > Controller bindings**, choose the player and preset,
then choose a physical button, trigger or stick axis for a race action. Each
player has eight independent profiles. Reusing an assigned input swaps the two
assignments. An input can also be unbound. Restore preset defaults affects the
displayed player and preset. Changes become persistent through Save settings or
Play. Direct game startup reads the same per-install INI. Original PS2 prompts
and menu navigation retain their original labels and inputs. Start opens Pause.
The second player's available actions reflect the Versus driving input path.

The editor uses the game's original preset rows for action labels. The native
packet composer applies overrides after device validation and before per-action
pause masking. Analog triggers retain pressure and stick axes retain magnitude.
Launcher-to-game captures and logs are in `evidence/controller-bindings` in the
main checkout. The packet-level test is `tests/native/controller_rebinding_check.cpp`.

## Requirement: regular file cleanup

The user requires regular cleanup. The folder reached 90 GB; on 2026-11-23 it was cut to about 40 GB without losing
any findings. Keep it from growing again: clean up after every test round and every build round, not at the end.

What was removed on 2026-11-23, and the rules to keep following:
- **Old build folders.** Builds are incremental: `build\<newest>\link.rsp` names every .obj it links, and those
  objects live in older build folders. Before deleting a build folder, check it is not named in the newest build's
  `link.rsp` (and keep `build\takeover-raload-141m`, where each link writes the map file). Delete every other old
  build folder. After deleting, confirm every .obj in the newest `link.rsp` still exists.
- **Raw dumps in evidence folders.** `.ppm` frame dumps, `.wav` recordings and the per-run copies of the game
  `.exe`/`.dll` are regenerable and are not findings. Delete them once a run has been read; keep `RacerRevenge.log`,
  `stderr.log`, `stdout.log`, `execution.json` and other small logs/json. Skip runs written in the last day.
- **Helper worktrees** under `.claude\worktrees`: remove them with `git worktree remove` (no force) once their
  branch is merged or no longer needed; the branch and its commits stay. Then `git worktree prune`.
- **Still frames** (`work\scratch\frames\fast\`): keep only the frames you cite.
- **Never delete** without checking: the newest build and anything its `link.rsp` uses, the DLC tester root
  (`work\scratch\dlc\roots\testers-cr21`), the Custom content session's working files (`work\scratch\racer-arcade`,
  `work\scratch\racer-n64`; ask that session instead), the ROM and the `Star Wars - Demolition (USA)` folder (user's
  files, never committed), and anything modified in the last day.
- Remaining large item at handoff: 14 GB of copied asset folders in `work\scratch\racer-arcade\silo\assets*`
  (Custom content's; it was asked to prune them).


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
