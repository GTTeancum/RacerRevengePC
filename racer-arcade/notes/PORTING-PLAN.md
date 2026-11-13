# Racer Arcade -> Racer Revenge: porting plan (Nov 12 2026)

Governing rule (repo CLAUDE.md): one common race-load / race-frame path for every course and pod.
Arcade content enters Revenge as **data in Revenge's own file formats**, never as new per-course or
per-pod code. The converter is one tool for all courses and one for all pods; Etti IV, Malastare and
the six new pilots are only the first data run through it.

Details: conversion-spec.md (field-by-field), revenge-target.md, courses.md, pods.md,
race-data.md, program-map.md, mask-map.md, hikaru-hardware.md.

## Where we are

| Area | State | Evidence |
|---|---|---|
| ROM images | done: eprom.bin (program, load base 0x0C00FC00), mask.bin (256 MB, unencrypted, fully mapped) | tools\interleave.py, mask-map.md |
| Program | SH-4 disassembler + xrefs; all asset tables decoded (5240 models, 2143 textures, 300 motions, 21 music, 14 sfx packs) | program-map.md |
| Courses | all 5 exported (OBJ+PNG+collision+centre line+tables); previews checked by eye; collision sits on the visual track | export\course0..4, courses.md |
| Pods/pilots | all 18 exported (glTF: pod LODs, engines placed by the game's offsets, pilot skeleton + 11/22 motions); renders checked by eye | export\pods, pods.md |
| Race data | grid (12), laps, time limits, centre line, AI edge lines, AI speed table per course; demo replays | race-data.md, race-paths.txt |
| Emulator | Odin runs on this PC; game blocked on MAME sets hikaru + mie (user to supply) | odin.log |

Course map: 0 Tatooine "Bantha Tracks" (easy), 1 Etti IV "Smuggler's Cove" (normal),
2 Malastare "Pixelito Challenge" (hard), 3 Tatooine "The Boonta Classic" (expert),
4 Naboo/Kaadu oval (unreachable cut content).
New planets for Revenge: **Etti IV, Malastare**. New pilots: **Ark "Bumpy" Roose (ARK), Boles Roor (BOL), Ebe Endocott (EBE), Elan Mak (ELA), Neva Kee (NEV), Ratts Tyerell (RAT)**
(names confirmed: Wookieepedia + the game's voice credits).

## Phases

### Requirement: full track conversion (user, Nov 12)
A converted track is the complete Revenge track: every file an original Revenge track carries
(AI data, checkpoints/laps/sectors, wrong-way, grid, cameras, triggers/events, zones, water,
sky/fog/light, particles, material audio and ambient sound, announcer hooks, HUD map, menu
entries), each from arcade data or generated from it and flagged. Checklist: a2r-track.md.

### Phase 1 - Converter, offline (can start now, parallel to Phase 2)
One tool `arcade2revenge` (scratch tools until it earns a place in the repo; push only on an
independent tools branch):
1. Settle scale and handedness: measure pod size and track width against Revenge (provisional
   x3.28, metres -> feet); confirm mirroring against the running cabinet (needs Odin + BIOS).
2. Writers for Revenge formats with byte-identical round-trip on the 13 original courses first:
   PSG v3 geometry, PST/PSM textures (256-colour quantise, regenerated mips, texel-pair unswap),
   COL v1 collision split by zone, RES3 archives, BINARY parameter files (.scb/.tob/.aub), INI files.
3. Course data: geometry + baked vertex lighting (arcade lights at run time; Revenge is prelit -
   generated, flagged), collision + surface mapping, zones/portals (generated), AI spline and lap
   path from the arcade centre line + edge lines, start grid from the arcade grid, sky, fog, water.
4. Pod data: merge pilot into chariot (Revenge has the pilot in the chariot model), the four
   chariot motions, engines, cables, generated wreck pieces (arcade has none) and pod collision,
   PHY values.
5. Offline checks: every converted file parsed by the port's own decoders; rendered previews.

### Phase 2 - Port prerequisite: retire stand-ins into the common path (blocks racing new content)
Work packages WP1-WP13 in conversion-spec.md section 5 (one course entry, one race stage/world,
camera, track world, navigation/AI/laps, triggers/materials, sky/fog/light, particles/water/audio,
renderer track path, content identity/file acceptance for converted files, remaining pod items,
fixed-size tables + save v8). Sized by scope, not days: 1,058 per-course flag tests in 23 files, ~450 per-course files (conversion-spec.md section 5). Overlaps the pod-load plan. Header-heavy
packages are batched into shared rebuild rounds.

### Phase 3 - Unique planets: Etti IV, then Malastare
Codes `eb` (class B) and `lc` (class C), track IDs 13 and 14. Verify like every course: muted
unattended runs with rotated pods, every course, before a build is handed over.

### Phase 4 - Unique racers: RAT, EBE, ELA (free pod numbers 05, 06, 13), then ARK, BOL, NEV
(27-29, needs the save widening from WP13).

### Phase 5 - Remaining tracks: Tatooine "Bantha Tracks" and "The Boonta Classic"
One free track ID left (15); the fourth new course needs the 4-bit course field widened.
Arcade "Boonta Classic" is a different layout from Revenge TC "Boonta Eve Classic".

## Gaps with no arcade source (must be authored, flagged as generated, not original)
- PHY handling for the six new pilots: the arcade has stats only for the four player pods
  (ANA BEN GAS SEB); arcade AI pods share one model.
- Wreck pieces and pod collision (arcade has no wreck pieces).
- Baked vertex lighting, zones/portals, surface types (arcade collision has only a wall flag 0x8000).
- Revenge-only race features on the new courses (water areas, triggers) where the arcade has none.

## Decisions for the user
1. Supply MAME sets `hikaru` and `mie` (to run the game and check mirroring and look).
2. Tournament placement of new courses (AI class follows tournament order): recommended a fourth
   circuit or single race only.
3. Authored PHY values for the six new pods: copy from nearest Revenge pod, or tune.
