# Silo build: converted Etti IV raced as SC

Builder notes for the silo worktree. Scoping: `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\notes\silo-override.md`.

- Worktree: `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\tree`, branch `silo/arcade-sc-override`, from `0407f64`.
- Assets with converted SC: `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\assets` (other helper owns it).
- Assets with original SC (baseline): `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\assets-orig`
  (robocopy of silo\assets without exe/log/converted-content.json; `TRACKS\SC.RES` from `silo\originals`, SHA 8305641f...).

## 1. Build inputs (commit 865b3a9)

- `platform\windows\native\interactive50du.sources.json`: added the 14 relinked sources (2 garage-shadow/UI unlock/win
  sets) from the 129e receipt. Receipt vs manifest diff afterwards: none. 623 + 6 = 629 units.
- `platform\windows\native\Build-Interactive50DU.ps1`: count gate 608 -> 623. Generator-source provenance hashes all match.
- Full build: `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\build-full-01.cmd`, log
  `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\silo-full-01.log`, output
  `...\silo\tree\build\silo-full-01`.
- Note: the full-build script re-hashes every header at the end and fails if any changed, so no tree edits while it runs.

## 2. Manifest schema read by the port

`converted-content.json` (the converter's `a2r-converted-content/1`): `files[]` with `path`, `bytes`, `sha256`,
`members[]` (`name`, `bytes`, `sha256`), `facts{}`. The port reads integer/boolean facts only; lists and strings are
provenance. Paths and member names are compared case-insensitively with `/` separators.

## Progress log

- Full build silo-full-01: 629/629 compiled, linked, exe 23,744,000 bytes. The 14 added sources resolved the link.
- Helper standalone check (`silo\helper-check`): manifest parsed (1 file, 470 members, 25 integer facts), converted
  SC.RES admitted by size+SHA, SHA-256 self-test ok, no manifest -> nothing admitted.
- Converted cut-identity builder checked against the ORIGINAL SC.RES (test manifest listing it): reproduces the catalog
  row exactly (cut count offset 10175, ordinals 0/977/978/979, frames 351/201/81, zones 11/3/17, offsets
  10026/10076/10125, intro ordinals 982/981). On the converted file: 3 cuts, frames 300/250/150, zones 10/1/10.
- Baseline run `silo-sc-orig-01` (original SC, silo-full-01, pod 3, --hold-throttle --steer-invert --camera-cycle
  --race-full): loaded, green light, raced at 62.4-62.6 FPS; the player pod was destroyed by wall hits at 43.8 s
  (natural destruction, no finish); the run then sat on the wreck/results until the watchdog. Not a finish.
  Rerun with main's finish recipe (`--repair --unstick --destroy 1@3..7@3 --results-confirm 30`) for a proper finish.

### Port change (commits 082350f, b08ac29)

General helper `src\assets\converted_content50cv.{h,cpp}` + `converted_track_cut50cv.h`; site list in the commit
messages. Builds: `silo-cv-02` (137 units recompiled, clean first time), `-03`, `-04`, `-05` (worktree relink:
`...\silo\tree\build\silo-relink.cmd <base> <next>`; one iteration = `...\silo\cycle.ps1`).

Refusals met with converted Etti IV, in load order:
1. Course screen: "Course cold target unavailable: original SC archive identity differs" -> course-target admission.
2. "converted SC Fog fields missing" (converted SCB has no Haziness) -> only ColorRGB required.
3. "Track particle canonical selected archive descriptor rejected" -> particle stage accepts the converted row.
4. None further: everything else listed in silo-override.md (cold source, ground, world counts, visual closure,
   lighting, triggers absent, static params absent, lava/ATM absent, cut cameras, intro, material audio) passed on
   the first build.

### Converted run silo-eb-sc-04 (build silo-cv-05, pod 3, finish recipe)

- `converted_content admitted path=tracks/sc.res ... sha256=c31d8371...`, `cut_identity track=SC cuts=3 members=470`.
- Loading screen LD_SC.PST, countdown/green, laps 1-2-3, `finish_continuation player_finished=1 sealed_place=1
  sealed_time=372.72 sealed_laps=3`. No require_failed / NATIVE_UNHANDLED.
- FPS: 111 five-second windows, average 62, steady 62.5 while racing; dips to 47 / 41 / 47 only at the two load
  screens and at the finish moment (same places as original courses).
- The log shows 4 `test_unstick` and 3 completed manual resets during the race. That fits the fall-through the user
  saw (converter: ~24% of collision faces pointed down, no generated walls). This SC.RES (c31d8371...) is the old
  one; re-run once the regenerated file lands.
- Caveat: AI were destroyed at 3 s by the test recipe (as in main's finish runs), so this proves load/race/finish,
  not AI racing on the converted course.

### Original SC after the change (silo-sc-orig-02, build silo-cv-05, pod 16, finish recipe)

No manifest in `assets-orig`, so no `converted_content` lines at all. LD_SC.PST, green, 3 laps,
`player_finished=1 sealed_time=479.39`, no failures, FPS average 62.2 over 114 windows (load dips only).

### Data-driven SC sky (commit 3fe2d04, build silo-cv-06)

SC's zero-sky handling now follows [Environment].NumberOfSkyCubes: zero (original SC) keeps the zero-sky owner
and no sky draw; one authored cube in a converted SCB (fact `sky_cubes`=1, one .cub member) goes through the same
sky loader (sky50gw), zone sky selection and sky publication as the other courses. Site list in the commit.
Not yet exercised with a real cube: the converter's sky has not landed (manifest still says `sky_cubes` 0).

### Regenerated SC.RES (collision faces fixed, walls added): silo-eb-sc-05, build silo-cv-06

- Admitted: `bytes=1645259 sha256=a7855d25...` (placed COL 46, was 36). No port change was needed for it.
- Load, green, 3 laps, `player_finished=1 sealed_place=1 sealed_time=336.66` (old file: 372.72).
- Stays on the track: the only 2 unstick resets are both at the start grid (x 12970, z ~11262), at track height
  (y ~1016); the old file also needed one mid-course (x 10299, z 8408). Pod destruction: none.
- Touches walls: the log has no dedicated wall-contact row; the engine-damage counter now records 2 damage
  dips/returns (old file: 0 updates, i.e. the pod never hit anything).
- FPS average 62.0 over 110 windows, 62.5 while racing; dips at loads and the finish moment only.

### How the user launches the silo build

`D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\play\Play-Silo.cmd`: silo build silo-cv-06 as
`RacerRevengePC.exe` plus SDL2/FFmpeg DLLs, started through the standard launcher with
`-OriginalAssetRoot D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\assets`. Log:
`D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\play\RacerRevenge.log`. SC (tournament order 8,
Baroonda slot) is the converted course. Note: `silo\assets\RacerRevengePC.exe` is the installed build101k copy and
does NOT contain the admission change. A regenerated SC.RES needs no rebuild: the port reads the new size/SHA/facts
from converted-content.json.

### Left

- Converted sky: re-run once the converter's cube lands; the first run will show whether its CUB layout matches
  the loader's fixed 48-tile layout (refusals name the check).
- Tests destroyed the AI at 3 s (finish recipe); an AI-racing run on the converted course is still to do, and
  rotated pods.
- Fog look is still SC's typed values in the publication stage (renderer fog); the zero-sky clear colour is read
  from the converted SCB.
- The SC-only files and isScTrack branches touched remain stand-ins to retire into the common path (flagged).
- No full rebuild since silo-full-01; later builds are worktree relinks (silo-cv-02..06). The source manifest now
  lists converted_content50cv.cpp (gate 624) for the next full build.

## Round 3: converted sky, AI racing, fog (commit f188ee6, build silo-cv-08)

New SC.RES 1,747,510 bytes, SHA 57c006e4..., with `skies\sc.cub` and a manifest `sky` record.

- First sky run (silo-cv-07, silo-eb-sc-06) passed only after one fix: the converted SCB names `CubeFile` but no
  `NumberOfSkyCubes`; the original reader defaults that to 1, the port's SC stand-in demanded an explicit "1".
  The manifest's `sky_cubes` fact is null, so the cross-check falls back to `cube_member`.
- Sky through the general path (silo-cv-08 log rows added): `track_sky_published track=SC textures=48 vertices=192
  spans=48`, `track_sky_zone zone=9 enabled=1 matrix=1`. Original SC (silo-sc-orig-03): no `track_sky_published`,
  `track_sky_zone ... enabled=0 matrix=0`, no converted_content lines, fog `start 0 end 4000` as before.
- Fog: colour was already read from the course's own SCB (sky owner / zero-sky clear colour). The fog RANGE used
  per-course constants (SC: Visibility 1, Haziness 1). SC now takes both from its cold owner: original SC stays 1/1
  (unchanged `start 0 end 4000`); converted SC reads Visibility 0.5 and the default Haziness 0.5:
  `Authored native world fog: start 1301.32 end 2666.65`. Colour 180,185,220 from the converted Fog.ColorRGB.
- silo-sc-orig-03 (pod 9 Mawhonic): no failures, 62.3 FPS average, but the unattended driver got stuck 10+ times
  and was on lap 2 at 533 s when the 600 s watchdog ended it. Driving problem with that pod on original SC, not a
  load/sky change; silo-sc-orig-02 (pod 16) finished on the same code paths.

### AI racing on the converted SC (no forced wreck; --repair --unstick --results-confirm 30), build silo-cv-08

| Run | Pod | Finish | Time | Resets mid-course | FPS avg (SC session) |
|---|---|---|---|---|---|
| silo-eb-ai-03 | 3 Clegg Holdfast | 4th, 3 laps | 329.3 s | 2 (x 8978 z 13296; x 12904 z 7691) | 62.3 |
| silo-eb-ai-16 | 16 Wan Sandage | 4th, 3 laps | 325.5 s | 4 (x 10597 z 7164; 12559/7852; 12892/7679; 8978/13279) | 62.3 |
| silo-eb-ai-22 | 22 Knire Dark | 4th, 3 laps | 345.3 s | 2 (x 8962 z 13280; x 12559 z 7867) | 62.3 |

- All three: sky published through the general path, no failures, no pod destroyed, FPS min 46-47 (load screens
  and finish moment only), racer-to-racer contacts present (e.g. 96 responded pairs in the pod 22 run).
- Every run: the same four AI (grid slots 5-8, rows at z 11363 and z 11263) NEVER leave the grid: lap 0 for the
  whole race, position unchanged within 5 units, while their AI reports full speed (~500). The three AI on the
  front row (z 11463) race normally and finish 1st-3rd. The player starts on the back row (x 12970 z 11263) and
  needs the test unstick twice at the grid before it gets away. So something blocks the grid between the front row
  and the rows behind it (around z 11400, x 12900-13000): most likely a generated wall/collision piece across the
  grid. Converter-side data; no port collision change made.
- Repeated mid-course resets at the same spots (x ~8970 z ~13285 and x ~12560 z ~7860) in all runs: places where
  the test driver (throttle held, steering inverted) gets stuck; worth checking against the converted walls too.
- Damage: no pod destroyed in any run. The engine-damage counter recorded no damage dips in the pod 3 run, so this
  log gives no wall-hit evidence for these runs (the earlier regenerated-file run showed 2).

Play-Silo.cmd now points at silo-cv-08.

### AI racing on the grid-fixed SC.RES (1,840,940 bytes, SHA 52f00b9e...), build silo-cv-08, no forced wrecks

Lap counts are at the moment the player finished (a race has 8 pods: player + 7 AI).

| Run | Pod | Player | Time | AI laps at player finish | Mid-course resets | FPS avg / min |
|---|---|---|---|---|---|---|
| silo-eb-ai2-3 | 3 | 7th | 362.9 s | 3,3,3,3,3,3,2 (last AI on its final lap) | x 10992 z 7022 | 62.2 / 47 |
| silo-eb-ai2-16 | 16 | 1st | 341.6 s | 2,1,1,1,1,2,1 | x 6286 z 11421; x 10980 z 7034 | 62.0 / 42 |
| silo-eb-ai2-22 | 22 | 8th | 355.4 s | 3,3,3,3,3,3,3 | x 10945 z 7026 | 62.2 / 47 |

- Grid fix confirmed: every AI leaves the grid in every run; no start-grid resets of the player any more (one late
  reset at x ~12870 z ~11123, just behind the start line, in two runs).
- Sky through the general path in all three; no failures; FPS dips only at load screens / finish moment.
- No fall-through seen: no pod lost below the track; resets are stuck pods.
- The two earlier repeat spots (x ~8970 z ~13285, x ~12560 z ~7860) did NOT recur. A different spot recurs in all
  three runs: x ~10970 z ~7030 (one player reset per run).
- Pod 16 run: six AI jammed together at x ~6290 z ~11460 from at least 161 s to the end; the player had been
  reset there too (x 6286 z 11421). One AI (slot 4, Scorch Zanales) was eventually wrecked there at 191 s. That is
  why the AI were 1-2 laps down in that run. Pods 3 and 22 runs passed the same place without a jam. Worth checking
  that spot in the converted collision (narrow gap or a lip pods wedge on).
- One AI wrecked per run (natural, racing). Engine-damage dips 5 / 0 / 5: pods are hitting walls/each other.

### Floor-fixed SC.RES (1,850,195 bytes, SHA a0756817...), build silo-cv-08, AI racing, no forced wrecks

| Run | Pod | Player | Time | AI laps at player finish | Mid-course resets | FPS avg / min | Damage hits |
|---|---|---|---|---|---|---|---|
| silo-eb-ai3-3 | 3 | 8th | 323.8 s | 3,3,3,3,3,3,3 | 0 | 62.2 / 48 | 3 |
| silo-eb-ai3-16 | 16 | 7th | 315.8 s | 3,3,3,3,3,3,3 | 0 | 62.3 / 46 | 1 |
| silo-eb-ai3-22 | 22 | 8th | 340.6 s | 3,3,3,3,3,3,3 | 0 | 62.2 / 47 | 0 |

- Every AI finished all 3 laps in every run; no test unstick or manual reset at all (start grid included).
- Pile-up check (any pod moving < 60 units for 15 s or more, 5 s samples): none in any run.
- Floor riser at x 5761 z 9314: pods were sampled passing within 150 units of it (2-4 pods per run, Mars Guo
  every run); none stopped or was reset there.
- No pod destroyed; no failures; sky through the general path; FPS dips only at load screens / finish moment.
- The player's low places are the test driver (throttle held, steering inverted), not the course.
