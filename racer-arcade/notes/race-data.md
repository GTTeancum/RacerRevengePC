# Racer Arcade race-logic data (beyond geometry)

Nov 12 2026. Program addresses are CPU addresses (eprom.bin file = addr - 0x0C00FC00); mask
offsets are mask.bin offsets. Dumps: `notes\race-tables.txt` (all tables below, decoded),
`notes\race-paths.txt` (every centre-line and AI-edge point). Script: `tools\dump_race.py`;
helper `tools\strnear.py` (strings referenced from a code range). Background: `notes\program-map.md`.
Guesses are marked (guess).

Course id: 0 SA_COURSE2, 1 ETTI4, 2 MALASTARE, 3 SA_COURSE4, 4 Kaadu scene. Pod/pilot kind
0..17 = ALD ANA BEN EBE GAS MAR MAW RAT SEB TEE ODY ARK BOL CLE DUD ELA NEV WAN.

## 1. Race setup flow

`Prep_RaceInit` (f_0C14B6A4, string "<<<< Prep_RaceInit") with race state at 0x0CECDE60:
state+16 = selected course, +20 = player pod kind.
1. `f_0C14B3CC` builds the 12 slots from the **start grid** (section 2).
2. `f_0C14ACEC`: course + music (@0C3E8AC8), `f_0C142DDA(course)` loads landscape/collision/STAT.
3. `f_0C13C868(course)`: NPC (AI) system: 12 NPC records of 0x83C bytes at 0x0C418B64, builds the
   AI guide (section 4) and sets 8 speed levels.
4. for each slot: type 1 = NPC -> `f_0C13C808(slot, kind, level, &pos, yaw)` (via 0C14B6A4,
   yaw = s16 * 2pi/65536, constant 0x38C90FDA at 0C14B6F0); type 2 -> f_0C14B66C (remote/other
   player, guess).

## 2. Start grid, laps, time

- **Grid** @0C3E9778[sel] (6 pointers; sel 0..3 = courses; grids 4 and 5 contain no ANA record,
  so the player-placement rule below cannot use them; users unknown (guess: attract/test)). Records 24 B {s32 kind, s32 AI level 0..7, f32 x, y,
  z, s16 yaw, s16 0}, terminated by kind < 0. 12 records = 4 across x 3 rows, rows 50 units apart
  along -z, x = +-30/+-10 (courses 0, 3) or +-20/+-6.68 (1, 2), y = 2 (9 on course 1). All yaw 0:
  the start line is node 0 of the centre line at (0,0), heading -z.
  **Player placement** (f_0C14B3CC): the player always takes the record with kind 1 (ANA,
  third from left, z +0.1); if the player chose another pod, that pod's record is turned into an
  NPC ANA. Example course 0: ALD/WAN/EBE/... see race-tables.txt.
- **Laps** @0C3E8D68[sel] -> state+512: 4, 4, 4, 3 (SA_COURSE4 = 3).
- **Time** @0C3F7128[sel] -> 5 rows (operator difficulty, chosen by f_0C142AC0) x {start seconds,
  then 4 per-lap extensions (guess)}; start x 60 -> frames in state+516. Course 0 normal row:
  90 s, +50 s per lap.

## 3. Course centre line (lap/progress spline)

Landscape record +24 indexes @0C1695E8 {ptr, n}: a **closed loop of (x, z) f32 points**, node 0 =
(0,0) = start line. InitLandscape stores the pointer at landscape state +28 (0x0CEBEFE8);
`f_0C10119C` ("create guide") turns it into 64-byte nodes at 0x0C4010E0 (position, unit direction,
normal) (guess: used for progress, lap counting and rank; GetRank/InsertRanking at f_0C141EBA..).

| Course | nodes | spacing | loop length | x range | z range |
|---|---|---|---|---|---|
| 0 SA_COURSE2 | 400 | 32.5 | 12982 | -2958..0 | -1656..2964 |
| 1 ETTI4 | 500 | 26.1 | 13043 | -2481..0 | -1351..2091 |
| 2 MALASTARE | 500 | 20.2 | 10078 | -2240..371 | -1248..1300 |
| 3 SA_COURSE4 | 800 | 33.6 | 26874 | -7575..57 | -1236..4411 |
| 4 Kaadu | 100 | 7.8 | 783 | -170..0 | -176..130 |

Course 0 matches the collision block A bounding box (x -3045..115, z -1852..2990, notes\mask-map.md).

## 4. AI (NPC)

- **AI guide = left and right track edges**: @0C2F90A8[course] {left list, right list} -> @0C2B161C
  {ptr, n} (x, z) pairs, one pair per centre-line node (n 400/500/500/801; course 3 has 801, one
  closing point). Built by `f_0C13AD2E` ("NPCGI : CREATE GUIDE") into 40-B segments {left x,0,z,
  right x,0,z, length, unit dir...}. Course 0 edges are about +-23 from the centre (width ~46).
  Course 4 has no guide (no AI).
- **Speed levels** @0C2F93E0[course] 8 f32, passed x60 as int to f_0C13AB18(level, v):
  course 0: 45 .. 60.5, course 3: 93 .. 112.5. **Resolved in section 13: target lap time in
  seconds per AI level.**
- **Per-course (node, value) curves** @0C2F9260 / 92D0 / 9338 / 93B8 = {ptr, n} lists of (node,
  0..1) breakpoints, e.g. course 0 (0,.5)(26,0)(140,0)(160,1)... (meaning unknown; guess: line /
  throttle bias along the track). Course-to-table mapping is in address order (guess); course 3's
  table reaches node 1496 > 801, so the unit may not be nodes.
- **Per-pod-kind AI parameters** @0C2F90E0[kind] 4 f32 -> NPC +20..+32 (f_0C13B114), e.g. ANA
  (1,0,0,0), SEB (0.9,0.9,0,0). Section 13: param 0 = how strongly the NPC follows the course
  racing-line curve, param 1 = block (+) / dodge (-) strength near the player; 2/3 unknown.
- NPC also reads the course collision root (f_0C142D92) and STAT (f_0C13B940).

## 5. Collision, sectors, surfaces

Course collision root (mask, see program-map.md) = {n sectors (0x100), sector array, block base}.
**Sector** (44 B; count n from the collision root: 256 on courses 0 and 3, 128 on courses 1 and 2, 32 on course 4): 4 corner points (x,z) x 2 f32 + first corner repeated (10 f32),
then a pointer to a -1-terminated list of triangle indices. Course 0 sector 0 spans x -400..300,
z 0..-51.2. Sectors chop the track into n slices (zones). Triangles: 8000 x 196 B on course 0
(0x17ED00 bytes, then the lists). Words 44..47 of a triangle record hold small integers; 44, 45
and 47 are -1 in exactly 400 records (guess: edge-neighbour indices, -1 = outer edge; 400 = node
count). **No surface-type / boost / slow / water flag field was found** in the triangle record,
and no material string ("slow", "boost" occur only as HUD/motion names). Water: only the per-pod
"POD WATER REF" objects (@0C300058[kind], 5 object ids: splash models for chariot + engines,
f_0C13FF48); no water-area table found.

## 6. STAT and LENS

Same header for both: {name ptr, n, poly array, f32 minX, minZ, maxX, maxZ (10-unit margin),
s32 grid cells (30 STAT / 15 LENS), grid ptr (cells of pointers into index lists)}.
- **STAT** (courses 0, 1, 3 only; f_0C142D98): n fixed 68-B polys {plane nx, ny, nz, d; 4 vertices
  xyz (v3 = v0 for triangles); u32 flag} - course 0: 2044 polys, all in x -1106..-13, z 1884..2929,
  y -4.5..55 (one built-up area). Flag: course 0 all 0, course 3 1169 x 1 / 410 x 0. Users
  f_0C102378, f_0C102992, f_0C10586C, f_0C13B940 (NPC). (guess: walls / building faces for
  camera and AI avoidance.)
- **LENS** @0C2F7A90[course] up to 4 structs (course 2 and 4: MALASTARE_LENS + _LENS2). Variable-
  length polygons (average 74-90 B) covering the whole course. Read by f_0C135D8C(pos, dir, course,
  which) -> f_0C136754: a segment-vs-polygon test. **Resolved in section 16: line-of-sight
  blockers for light flares/glows; full STAT/LENS layouts there.**

## 7. Landscape callbacks (per-course code, as in the original)

Landscape +40/+44/+48 are called with 1 at InitLandscape: +40 (0C146460, 0C1466F0, 0C14693A,
0C1465CC, 0C146DF4) sets up animated objects (f_0C1432C0, f_0C146006; sun flare (not Kaadu, see section 18) via f_0C146440 ->
f_0C13790C/f_0C137AD6 on courses 0, 1, 3); +44 (0C147078, 0C14738A, 0C147296, 0C147430; none on 4)
loads extra static objects (10-15 LoadStaticObject calls on courses 0 and 3); +48 small
(0C1477C4..0C14788C). Not decoded further.

## 8. Recorded runs (VTR) = attract demo

Read by `Adv_InitGameScreen` (attract, 0C1338B0): if no game is running ([0x0C422CD4] < 0) it DMAs
the directory (mask 0x0F2166C0, 0xF8 B) to 0x0C422CD4 and the 12 runs (mask 0x0E78A6C0,
0xA8C000 B) to 0x0C422DD0 (f_0C13FDE2). Directory word 0 (= 0) is then used as the **course id**
(course 0) and directory+16 (7199) as the demo length, capped at 3600 frames (60 s); without runs
the fallback is course 0 for 1200 frames. It defines 12 pods with kinds taken from the runs
(f_0C13FD02: frame 10 word 0 & 0x1F) and starts playback at frame 300 (f_0C13FB94, "VTR %d/%d").
Directory per run: {slot, 7200 frames, 7199 last, 0xA48 (unknown), recording-time RAM address
(overwritten at load)}. Playback f_0C13FBE0 -> f_0C140550 unpacks
a frame: w0 & 0x1F = kind, w0/w1 flags, 5 x 24-B part poses {3 rotation rows packed 11:10:11 bits
(x = (w>>21)/1023.5-1, y = ((w>>11)&0x3FF)/511.5-1, z = (w&0x7FF)/1023.5-1), f32 x, y, z}: part 0
chariot, 1-2 engines, 3-4 extra engines (zero for 2-engine pods). A recorder exists too
(f_0C13FCC0 -> f_0C140726, "INIT TAPE"). Run kinds: ANA ALD SEB TEE MAW MAR ARK GAS ELA WAN EBE BEN.
They are human-driven replays of course 0, not AI lines, but they are clean racing lines for
course 0 (usable as reference AI paths).

## 9. Pods: handling, pilot flag, unknown field

- **No per-kind handling table (top speed, acceleration, turn, boost, repair, mass) was found.**
  The only per-kind physical data is the **body layout** @0C2F904C[kind] -> 168-B block (f_0C13A7F2):
  s32 engine count, chariot rec, engine recs, each 8 f32 {x, y, z, yaw deg, a, b, c, d}
  (chariot a..d = 100, 15, 4, 6; engines 300, 30, 4, 6 for every kind; guess mass, radius, ...),
  +164 flag (only MAR = 1). E.g. ANA chariot (0, 0.785, 8.25), engines x +-1.887; BEN 4 engines;
  NEV 0 engines. So in the arcade all pods appear to share one handling model and differ in shape
  only (guess, until the pod update code is traced; physics code is 0C10xxxx-0C12xxxx, unnamed).
  AI difference per kind = the 4 floats @0C2F90E0.
- **Pilot-table flag** = (slot != 0): slot 0 (the local player) uses @0C2A733C, other slots
  @0C2A6C7C. They differ only for the 4 selectable pods (ANA, BEN, GAS, SEB: texgroup/object
  29/340 vs 28/339 etc.), i.e. the player's own pilot gets a separate model (guess: higher detail).
  The bookkeeping screen lists exactly these 4 ("POD SELECTED: ANAKIN, SEBULBA, QUADINAROS,
  GASGANO").
- **Pod +132 field** = per-pod hook function copied to pod state +196: non-zero only for the same
  4 pods (ANA 0C143F62, BEN 0C1442E2, GAS 0C144434, SEB 0C14452C), all inside the LoadStaticObject
  users f_0C143F10..f_0C1444EA (guess: player-pod extras such as cockpit/dashboard objects).

## 10. Course 4 (Kaadu)

Course 4 has its own landscape (EPROM-resident texset E1, objects E122/E123), collision (mask
0x0E5D4FE0, 0x4B140 B), a 100-node centre loop of 783 units, no STAT, no AI guide, no grid in the
4-course selector, LENS = Malastare's. It is loaded only by f_0C137154 and f_0C138498
(both call f_0C136844 "KAADU MOT / DEF KAADU ID", then f_0C142DDA(4); 0C138498's mode shows
"SPEED ... KMH"). **No code or data reference to either function was found** (no call, no pointer,
no literal), so course 4 is unreachable in this build: a Kaadu riding test or a cut mode (guess).
Correction (section 18): Kaadu do NOT appear on the real courses; the landscape-callback calls are the sun flare.
Kaadu have their own motions (GMOT_K_*, GMOT_Y_kaadu_*), used only by the course-4 code.

## 11. Unresolved

- Pod handling numbers (if any exist apart from shared constants); meaning of body-record a..d.
- Meaning of NPC per-kind floats, per-course (node, value) curves, speed-level units.
- Checkpoints: none found apart from the 256 collision sectors and the centre-line nodes; whether
  laps are counted by node 0 crossing is not confirmed.
- Boost/slow surfaces and water areas: not found (may be absent in the arcade game).
- Camera data: TV/intro cameras not located (sel_course/sel_pod ExecCameraEvent are menu cameras).
- Grids 4 and 5 users; landscape callbacks in detail; LENS polygon record layout.

---------------------------------------------------------------------------------------------------
# Part 2 (follow-up): race logic, AI, cameras, hooks, fog, STAT/LENS layouts

Race manager = 0x0CECDA80 (race setup state 0x0CECDE60 = +0x3E0). Per-frame race order
(Game_ExecRace, 0C14BA18..0C14BAA6): f_0C109810 camera, f_0C144A4A display pods, f_0C147DAC /
f_0C147F2C landscape, f_0C149E40 sector + wrong way, f_0C13C8F4 NPCs, f_0C14A494, f_0C14A226 laps,
f_0C13FDB4 VTR.

## 12. Laps, sectors, wrong way, finish (player)

- **Position on track = collision sector** (section 5: n quads per course, 256/128/128/256/32). f_0C149E40 calls
  f_0C1093BE -> f_0C108A18(pos, sectors, n, current): point-in-quad test (f_0C1089C6, x/z only)
  starting at the current sector, then alternately forward/backward until found; result in race
  mgr **+100**.
- **Lap counting** (f_0C14A226 -> f_0C149FC4): delta = (+100 - +104) wrapped into [-n/2, n/2)
  (f_0C152F88 = modulo), accumulated into **+92**; +104 = last sector; +96 = n sectors (256/128/128/256/32);
  **+108 = next-lap threshold**, initialised to n (f_0C14ACEC 0C14ADB6). When +92 >= +108: the lap
  time (+780) is stored into +784[lap], best lap kept at +824, **+88 lap += 1**, +108 += n, and
  the time limit +80 gets `f_0C1498B2(course, lap) x 60` frames (time table, section 2). When
  +88 >= **+84 laps** (from @0C3E8D68) the race ends (f_0C14990E(4) / (6) state change).
  So there are **no checkpoints**: a lap is n sectors of net forward progress from sector 0 (the
  start-line quad, z 0..-51 on course 0). Driving backwards subtracts. **Shortcuts**: progress is
  the sector index the pod is inside, so a cut that stays inside sector quads only gains the
  sectors it crosses; leaving all quads keeps the old sector (the search fails, value unchanged).
- **Wrong way** (f_0C149E40 0C149EB8..): dot(pod heading, sector centre direction) < 0 ->
  **+980 += 1**, else reset to 0. At >= 120 frames (2 s) the HUD flag bit 0x10 is set
  (f_0C106B74 0C106E56; also f_0C102924 for event 14/15).
- NPCs track progress on the AI guide instead (section 13): NPC +4 segment, +8 t in segment,
  +0 total segments passed (f_0C13BEAC).
- Not found: the race-position (1st..12th) sort. GetRank/InsertRanking (f_0C141EBA..) are the
  best-time tables (4 courses x 10 x 12 B at 0x0CEAF34C), not race position.

## 13. AI behaviour (NPC module 0C13A500..0C13D000, 12 NPCs x 0x83C at 0x0C418B64)

- **Pace cars**: AI block 0x0C418AE0: +0 = guide segment count, +4[8] = segments per frame for 8
  levels, +36[8] = pace position (segments), +68 = player segment, +72 = player lateral.
  f_0C13AB18(level, v) sets **+4[level] = segments / v** with v = table value x 60, so **the
  per-course "speed table" (@0C2F93E0) is the target lap time in seconds per AI level**: course 0
  45..60.5 s (12982 units -> 288..215 units/s), course 3 93..112.5 s (26874 units -> 289..239 u/s).
- **Per-frame pace** (f_0C13AB54): factor = 1 - k x player lap (race mgr +88), k =
  @0C2F9094[landscape] = 0.004, 0.008, 0.008, 0.005, 0.006 (AI eases off each lap).
  **Rubber band** (single player, race state 3): per level, pace position vs player segment:
  pace > 20 segments behind -> x1.1; > 10 behind -> x1.05; > 10 ahead -> x0.95 x (1 - 0.1 x level);
  otherwise x1. Networked: x0.95 for all. Constants @0C13AC94..0C13ACA8.
- **Lateral line**: NPC target point = lerp(left edge, right edge, lateral) on the guide segment
  (f_0C13AFB8); lateral 0 = left list, 1 = right list; f_0C13B06C measures a point's lateral 0..1.
  - **Course racing-line curves** @0C2F9260 (course 0), 92D0 (1), 9338 (2), 93B8 (3) = (segment,
    lateral 0..1) breakpoints, linearly interpolated (f_0C13ACE6). Each NPC frame (after frame 48):
    target += (curve(segment) - target) x **kind param 0** (@0C2F90E0 +0), clamped 0..1
    (f_0C13BFEC). Course 3's table has breakpoints past its 801 segments (1456, 1496) that are
    never reached.
  - **Player interaction** (f_0C13C04A, single player only): if the NPC is within -3..+3
    segments of the player and the lateral gap < 0.1: **kind param 1** >= 0 -> move toward the
    player's line (block) by param1; < 0 -> move away (dodge). Clamped 0..1.
  - Smoothing: NPC +40 += (+44 - +40) x 0.03 per frame (0x3CF5C28F).
  - Per-course NPC routines (0C13C140..0C13C2A0, pointers in NPC +0x560/+0x564, called in race
    state 3) add course-specific lateral overrides by segment range, e.g. course 0 segments
    208..272 / > 272 force 0.3 / 0.5-ish targets (floats @0C13C234..0C13C23C), course 1 160..216.
- **Kind params 2 and 3** (@0C2F90E0 +8/+12 -> NPC +28/+32): no reader found (unresolved).
- **NPC damage** (f_0C13B542): two engine damage accumulators (+1316/+1320) with thresholds give
  states 3/4/5 (one engine out: speed x0.94; 5 = wrecked -> retire via f_0C14A41E).
- **STAT** is read by NPC f_0C13B940 (guess: obstacle avoidance against STAT polygons).
- **Unused grids 4/5**: the only reader is Prep_RaceInit (indexed by selection 0..3). Grid 4
  mirrors course 0's layout, grid 5 course 1's, both without an ANA record (guess: leftovers for
  AI-only demo races).

## 14. Cameras

- **Race camera** f_0C109810, state 0x0C400F94 (init f_0C10968C(player pod kind)): window 1
  projection near 0.01, far 100000, 640x480, **FOV 80 deg** in race (60 deg during setup); view
  mode **+0x84** = 0..3 (initial 0 or 2 from the DEFAULT VIEW operator setting, f_0C142A54).
  Per-player-pod offsets (pod-local x, y, z), tables indexed by kind (only ANA 1, BEN 2, GAS 4,
  SEB 8 are non-zero): cockpit eye @0C25FA64 / look @0C25FAE8 (ANA eye (0,0.4,-5.7), look
  (0,0.3,-6.7)); chase eye @0C25FB6C / look @0C25FBF0 (ANA eye (0,1.2,3.0), look (0,1.1,2.0)).
- **Scripted camera** (race states 5 and 7; guess: start intro / finish): eye and look-at follow
  13-key pod-local splines @0C25F92C (eye (0,0,-5) .. side (4,1,0) .. (4,1,-3)) and @0C25F9C8
  (look (0,0,-6) .. (0,0.9,0)), built by f_0C10A098 into 0x0C401204 / 0x0C401258, eased by
  f_0C10A204.
- **Attract**: VTR replays on course 0 with the same camera code (f_0C10968C also called from
  f_0C100F94).
- **No per-course camera path data was found. LENS is not camera data** (section 16).

## 15. Per-course hooks (landscape +40 / +44 / +48)

- **+40 = fog and lights** (called once with 1): fog block = window 1 (f_0C14E63E(1)) +36..+39 RGBA
  bytes, +40 fog near, +44 fog far; lights via f_0C145A0E (tag-list arguments, not decoded) using
  vectors at @0C3E80FC..; sun lens flare via f_0C137AD6 / f_0C13790C (courses 0, 1, 3, 4; corrected in section 18 - not Kaadu).

  | Course | fog RGB | near | far | light data |
  |---|---|---|---|---|
  | 0 SA_COURSE2 | 192,224,255 | 1000 | 10000 | @0C3E80FC (-1,2,-1) |
  | 1 ETTI4 | 255,255,255 | 1000 | 8000 | @0C3E8114 (0.5,1,0.025) |
  | 2 MALASTARE | 30,7,64 (night) | 700 | 1000 | 8 lights; positions @0C3E8120 (-2286,-95.5,1101), @0C3E82A0, @0C3E84E0 |
  | 3 SA_COURSE4 | 112,120,160 | 1000 | 8000 | @0C3E8108 (-1,2,-1) |
  | 4 Kaadu | 255,255,255 | 1000 | 8000 | @0C3E8500 (0.5,0.4,0.025) |

- **+44 = extra static objects** (r4=1: load; r4=0: draw every frame with hard-coded transforms
  through f_0C146F3A(ctx, obj, x, y, z, a); a = 4th float, guess yaw in degrees). Full dump:
  `notes\race-hooks.txt` (tools\hookplace.py). Course 0 loads objects 1229-1237 and 1240 and places
  e.g. obj 1229 at (-100,0,380), (-70,0,200), (-65,0,-260), obj 1235 at (-150,0,710), obj 1237 at
  (50,5,-408). Course 3 loads 1241-1251, 1254, E3-E5 (15 objects, 16 placements). Courses 1 and 2
  place nothing here; their +44 rotates an object by 3/4096 turn per frame (f_0C015000) and
  overrides the landscape draw count by player sector (below).
- **Landscape draw count** = landscape state +0xA180 (default = landscape record +36: 104, 32, 70,
  45, 150), the count passed to the landscape drawer f_0C147938 (guess: blocks drawn ahead).
  Overrides: course 1 sectors 10..39 -> 23 and 75..110 -> 32; course 2 70 / 60 by a condition;
  course 0 (+48) sector > 215 -> 65.
- **+48 = per-block float table**: landscape state +260 holds 1024 floats set to 1.0 at
  InitLandscape (0C1480EA). Course 0's +48 (0C1477B4) writes 0.4 for entries 49..55, 59..64,
  76..81, 84..88, 91..93, 109..112; course 3's (0C14788C) writes 0.25 / 0.4 ranges. Read by the
  landscape drawer f_0C147CA2 together with the fog window (guess: per-block fog/brightness factor
  for tunnels and canyons).

## 16. STAT and LENS layouts (complete)

Common header (STAT @0C3C2B64 etc., LENS @0C2CA41C etc.): +0 name ptr, +4 n polys, +8 poly array,
+12 f32 minX, +16 minZ, +20 maxX, +24 maxZ (10-unit margin), +28 s32 cells (STAT 30, LENS 15),
+32 grid ptr. **Grid** = cells x cells pointers and ends exactly at the header (STAT 0: 0x0C3C1D54
+ 900 x 4 = 0x0C3C2B64; SA_COURSE2_LENS: 0x0C2CA098 + 225 x 4 = 0x0C2CA41C); each pointer leads to
a -1-terminated list of poly indices; cell size = (max - min) / cells on x and z.
- **STAT poly** = 68 B {f32 nx, ny, nz, d; 4 vertices x, y, z (v3 = v0 for triangles); u32 flag}.
- **LENS poly** = 52 B {f32 nx, ny, nz, d; 3 vertices x, y, z} (f_0C136754 indexes poly x 52).
  Query f_0C135D8C(from, to, course, which) -> f_0C136754: rejects short segments (length^2 <
  @0C3F3220), walks up to 32 grid cells along the segment (f_0C135FA0), tests each listed triangle
  (f_0C1365D0), returns 1 if blocked. Callers: pod light/glow loop f_0C1432EE (a pod light with
  LENS index +0xB34 >= 0 is skipped when hidden from the camera), Kaadu objects f_0C136A68 (course 4) and the sun flare
  f_0C13790C. So **LENS = line-of-sight blockers for lens flares / glows**.

## 17. Still open

- Race-position sort (1st..12th) and the HUD rank source.
- Kind params 2/3; (light tag list and Kaadu: resolved in sections 18 and 20).
- Water plane height and positional sound emitters: not found (no water table; the only water data
  are the per-pod splash objects). Music: one stream per course (section 2).
- Collision triangle record fields (geometry helper's area); surface flags still not found.
- Camera views 1 and 3 in detail; meaning of race states 5 / 7.

---------------------------------------------------------------------------------------------------
# Part 3 (follow-up 2): animated scenery, race position, lights, water, sound, camera views

Correction to Part 2: sector counts per course (collision root word 0) are **256, 128, 128, 256,
32** (courses 0..4), not 256 everywhere; laps are n sectors (race mgr +96 = n). Collision
triangle counts: 8000, 6184, 7352, 16000, 1520 (196-B records before the sector lists).

## 18. Animated scenery

- **The "Kaadu" calls in the +40 hooks are not Kaadu herds.** f_0C137AD6 (init, tail call at
  0C146556) and f_0C13790C (per frame, 0C146580) are the **sun lens flare**: init loads texgroup 94
  into texset 10 slot 27 and 7 flare objects 1219..1225 (ids @0C2F7E00); per frame it projects the
  course's sun direction (light 0 vector, e.g. @0C3E80FC (-1,2,-1)), runs the LENS occlusion test
  (f_0C135D8C) and draws the flare in its own window. Used by courses 0, 1, 3, 4.
- **Kaadu creatures exist only in course 4** (module 0C136844..0C1374AC, "KAADU MOT", "DEF KAADU
  ID"), which is unreachable (section 10). No Kaadu, banner or ship animation is set up on
  courses 0..3.
- **Animated texture (water)**: courses 1 and 2 (+44 hooks f_0C14738A, f_0C147296) advance a
  phase by 3 per frame modulo 4096 and emit GPU command **0D1 "TEX panning" slot 1**
  (f_0C015000(1, phase, 0) writes 0x000106D1 + (phase<<16); notes\hikaru-hardware.md table, 0D1 =
  TEX panning, w1 = Y(31:16) X(15:0) in 1/16 texel). So materials that use texcoord-offset slot 1
  scroll in V by 3/16 texel per frame (256 texels per 1365 frames): the Etti IV sea and the
  Malastare surface (guess on which materials; it is whatever sets offset slot 1).
- Other per-frame animation in the hooks: none found besides the static placements (section 15)
  and Malastare's flickering lights (section 20).

## 19. Race position 1st..12th

f_0C14A494 (Game_ExecRace, every frame):
1. For each slot 0..11 with an active pod (f_0C143054(slot), word 0 >= 0): f_0C14A438 finds the
   pod's collision sector (same search as the player, f_0C1093BE) and adds the wrapped sector
   delta to **standings[slot].progress** (race mgr +836 + 12*slot, word +4; word +0 = last
   sector). n = race mgr +96.
2. **One bubble pass per frame** over the order array 0x0CECE080[12]: adjacent entries are
   swapped when the later one has strictly more progress (equal progress keeps the order).
3. standings[order[i]].rank (+8) = i (0 = first). The player's position (race mgr +832) is taken
   from this (network: per-node, f_0C139974).
So position = net sectors covered since the start, sorted lazily; ties broken by previous order.

## 20. Lights (SetLight f_0C145A0E, decoded; dump tool tools\lightdump.py)

SetLight(id, cmd, args..., 17): all on the stack; light records 76 B at 0x0CEB57B0 + id*76 (ids
< 256). Commands (jump table 0C145AD8): 0 = vector A (3 f32: direction for type 0, position for
type 1); 1 = vector B (3 f32); 2/3 = int +24/+28; **4 = colour, 3 f32 (R,G,B 0..1)**; 5..8 = bytes
+38..+41 (7 = **type**: 0 directional, 1 point); 9 = f32 +44 (**near range**), 10 = f32 +48 (**far
range**); 11/12 = state 1/2 (on/update); 13..16 = flags +68/+72; 17 = end. Light ids are built in
registers; the per-block light-group assignment uses f_0C146440 (landscape state +0x10FC, 1024 x
32 B).

| Course | Light | type | vector | colour |
|---|---|---|---|---|
| 0 | sun | dir | (-1, 2, -1) | 0.95, 0.95, 1.0 |
| 0 | fill | dir | (-1, -2, 1) | 0.5, 0.5, 0.45 |
| 1 | sun | dir | (0.5, 1, 0.025) | 1, 1, 1 |
| 1 | fill | dir | (0, -1, 0) | 0.1, 0.1, 0.1 |
| 2 | key | dir | (0, 1, 2) | 1.0, 0.525, 0.38 |
| 2 | fill | dir | (0, -3, -2) | 0.086, 0.482, 0.169 |
| 2 | dir | dir | (1, 1, 0) | 0.4, 0.4, 0.4 |
| 2 | dir | dir | (-1, 1, 0) | 0.1, 0.1, 0.1 |
| 2 | **32 point lights** | point | positions @0C3E8120 (32 x 12 B) | 0.95, 0.95, 0.75, range 20..50 |
| 3 | sun / fill | dir | (-1, 2, -1) / (-1, -2, 1) | as course 0 |
| 4 | sun / fill | dir | (0.5, 0.4, 0.025) / (0, -1, 0) | 1,1,1 / 0.1 |

Malastare point lights (x, y, z): 0 (-2286.0,-95.5,1101.2) 1 (-2261.3,-84.8,1195.7) 2 (-2202.7,-64.5,
1274.7) 3 (-2114.0,-42.9,1324.2) 4 (-2014.5,-26.6,1336.6) 5 (-1921.8,-17.9,1315.6) 6 (-1847.0,-16.2,
1280.0) 7 (-1772.5,-20.8,1248.8) 8 (-1697.1,-29.8,1220.4) 9 (-1621.6,-39.2,1191.8) 10 (-1542.4,-45.2,
1146.7) 11 (-1489.9,-45.4,1071.9) 12 (-1476.5,-40.3,986.2) 13 (-1458.8,-30.6,912.0) 14 (-1423.6,-17.3,
848.1) 15 (-1369.3,-1.5,798.3) 16 (-1298.8,15.3,760.9) 17 (-573.6,17.5,1319.3) 18 (-476.7,14.2,1347.3)
19 (-370.2,14.0,1323.7) 20 (424.6,18.9,592.5) 21 (395.6,25.6,493.6) 22 (336.0,28.7,416.8) 23 (-1629.7,
-108.5,138.6) 24 (-1937.2,-108.5,399.4) 25 (-2139.9,-110.0,593.2) 26 (-2166.1,-110.0,673.0) 27
(-1457.6,-108.5,92.2) 28 (-1657.4,-108.5,261.7) 29 (-1780.4,-108.5,366.1) 30 (-2006.5,-110.0,567.0)
31 (-2068.1,-110.0,661.9).
Per frame ("set_light_malastare exec", 0C146BFC): all 32 lights get new ranges each frame, the
choice alternating with frame counter parity (0x0C016D48 + light index) between near/far
(20, 50) and (15, 45) (@0C146D64 / @0C146D5C): **flicker**; then directional colours are reset
(0.32 / 0.08 @0C146D68 / @0C146D6C, 0.95, 0.1). Table @0C3E82A0 (36 x 4 ints, -1 = none, e.g.
{0,-1,-1,-1}, {1,0,-1,-1}, {31,0,..}, {33,32,0,-1}) = per-block light lists (guess: which lights
affect which landscape block).

## 21. Water

- **No water height test or water physics was found.** The only water code is **pod reflections
  on Etti IV**: "POD WATER REF" = reflection objects, 5 per kind (@0C300058, chariot + engines),
  loaded by DefineDispPod only when the landscape id == 1 (f_0C147910() == 1 at 0C143BEE) and
  drawn by f_0C1401C2 (from the display-pod loop 0C144A04) with the pod matrix mirrored (sign flips
  0C1402BE..0C14034E; constant 0.3 @0C1403C4, guess: reflection alpha/scale) about the plane y = 0
  (guess: no height constant is added).
- Geometry evidence (flat collision triangles, all three vertices same y): Etti IV 2532 triangles
  at **y = 0** (also 168 at 7, 159 at 15) -> sea level y = 0, consistent with the mirror plane.
  Malastare: 1480 + 432 flat triangles at **y = -120 / -118.5**, 564 at 0, 435 at -8 (guess: the
  low surface is the lake/methane floor; the lights 23..31 sit at y -108.5..-110 just above it).
  Course 0: 4906 at y = 0; course 3: 4698 at y = -0.1.
- Scrolling water texture: section 18.

## 22. Sound

- **No per-course sound pack and no positional / ambient emitters were found.** The race sound
  init f_0C10748C loads the same packs for every course (13, 4, 10, 6, 11, 12 via f_0C14C4A8) and
  sets up channels (f_0C151284). The landscape hooks never call the sound API (f_0C151718 /
  f_0C151284 callers are pod, NPC, race-manager, menu and test code only).
- Event cues: table @0C25F1B0, 16-B records {cue = (sample<<16)|(bank<<8)|0xAA, 0, volume 0x7D,
  length in frames} (e.g. 000800AA len 240, 000501AA len 250), used by the pod/race sound code
  (f_0C107CF0 also watches the standings at race mgr +836, guess: announcer position calls).
- Music: one stream per course (section 2), started at race setup.

## 23. Camera views (complete)

Race camera mode **+0x84** (f_0C109810): **0 = cockpit** (eye +0xB0 / look +0xBC, per-pod
tables @0C25FA64 / @0C25FAE8), **1 = 20-frame blend cockpit -> chase** (counter +0x88; eye/look =
lerp by counter/20, 0C109A70..0C109B30; at 20 the mode becomes 2), **2 = chase** (eye +0xC8 /
look +0xD4, @0C25FB6C / @0C25FBF0), **3 = 20-frame blend chase -> cockpit** (0C109BC4..; at 20
the mode becomes 4 and wraps to 0, 0C109C9A). The VIEW button steps the mode (guess: input code
not traced); DEFAULT VIEW selects 0 or 2 at race start.

## 24. Still open after Part 3

- Exact semantics of light vector B, ints +24/+28 and flags; Malastare flicker alternate values.
- Which materials use texcoord-offset slot 1 (needs the model attribute blocks).
- Water: no physics found; whether pods ever touch it is a geometry question.
