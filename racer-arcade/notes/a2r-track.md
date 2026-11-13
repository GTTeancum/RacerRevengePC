# a2r track converter: complete converted tracks

Nov 12 2026. One tool chain for every course, no per-course code. Tools in
`D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r\`: `trk_*.py` (track
data, this note), `geo_*.py` (geometry/textures, `notes\a2r-geometry.md`), `a2r_space.py` (shared).
One complete track per course in
`D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\export\a2r\<code>\`
(eb, lc, ua, td, ka). The geometry helper's intermediate output stays in `export\a2r\course<N>\`.

Run, per course N:
`python -B -I geo_course.py N` (visuals), `python -I trk_convert.py N` (track data, merge, archive), then
`python -I trk_verify.py ua eb lc td ka`.

Legend: **A** arcade data, **A-derived** computed from arcade data, **G** generated (flagged in
each `manifest.json`), **D** copied from a Revenge original (donor), **pending** = not produced
yet, with the reason.

## 1. One space transform: `tools\a2r\a2r_space.py`

`revenge = (3x + ox, 3y + oy, -3z + oz)`. Normals `(nx, ny, -nz)`, and every triangle's winding is
reversed (the mirror flips orientation).
- Scale 3.0: from the pod chariot, 2.92-3.01 (a2r-geometry.md 1.1).
- Z negated: the arcade is right-handed, Revenge left-handed (1.2).
- `ox`/`oz` put the course's visual landscape minimum corner at (2000, 2000) ft.
- `oy` = 1000 ft (G).
- The course codes (`CODES`: ua eb lc td ka) are in the same file.

`course_transform(N)` takes the landscape points exactly as `geo_course.arcade_triangles` reads
them, and caches the result in `export\a2r\space\course<N>.json`.

Who uses it:
- `geo_course.py` (its `transform_fn`/`apply` now call `a2r_space`; constants and codes imported).
- `geo_verify.py` (its preview transform).
- `trk_convert.py`: collision, splines, grid, sectors, sun vector.

Those three import-only edits are the only changes to `geo_*.py`. No other file defines a scale,
flip, offset or code (checked with grep).

Check: the transform stored in each geometry manifest equals `a2r_space.course_transform(N)` for all
five courses (`export\a2r\geo_verify_rerun.log`). Changing a constant in `a2r_space.py` and rerunning
the three steps moves everything together.

The track files made before this step used x3.28, unmirrored, with the origin taken from the
collision. All of them were regenerated.

## 2. Readers and writers: byte-identical round trip on the 13 original tracks

Log: `export\a2r\roundtrip.log`.

| Format | Module | Result |
|---|---|---|
| RES3 archive | `trk_res3.py` + `trk_deflate11.py` | 13/13 byte-identical, rebuilt from the members alone |
| COL v1 (type 0 float, type 0 quantized, type 1 compound) | `trk_col.py` | 2,097/2,097 |
| BINARY parameter files (.scb .tob .aub) | `trk_binparam.py` | 39/39; gc.scn -> gc.scb and gc.toy -> gc.tob compile byte-identical |
| INI-style text (.spl .pos .tun .wtt .ini .prt .emt .scn .toy .acb .mcf .psm .vut) | `trk_ini.py` | 2,260/2,260 lossless; .spl files match the plain canonical writer (26/26) |
| Cut-camera keys (.vub) | `trk_vub.py` | 57/57, and 57/57 rebuilt from one template record plus keys |
| PSG / PST / PSM | `geo_psg/pst/psm.py` (geometry helper) | 5,877 / 1,115 / 2,438 byte-identical (a2r-geometry.md) |

How the hard cases were solved:
- **RES3 compression.** The originals used zlib 1.1.x. It picks the same matches as today's zlib
  but also ended a deflate block at every 4,096th symbol when the block looked poorly compressible.
  `trk_deflate11.py` reproduces that block split and zlib's Huffman tree builder. Edge case: when
  the split fires on a final match, the old encoder adds an empty final block (GB block 358).
- **BINARY files.** Keys and sections can repeat (GC/SA .aub, MC .scb, TB/TC .tob); labels use a
  stable sort. The .scn/.toy compiler drops spaces inside values; the .aub files keep them.

## 3. Checklist: everything an original track carries

Full listing with per-track counts: `export\a2r\inventory.txt` (`trk_inventory.py`).

### 3.1 Members of TRACKS\XX.RES (written as `install\TRACKS\<CODE>.RES`)

| Member (xx = code) | In originals | Produced | Source / how |
|---|---|---|---|
| `geometry\*.psg` visual models | 13/13 | yes | geometry helper: arcade landscape, 2,048-ft grid pieces, baked lighting (G). Placed as world pieces at `0.0,0.0,0.0`, as TA `Zone01GroundStrips` and GC `gc_z1_track_a` are. Zone = the zone of the nearest centre-line point. |
| sky | 12/13 as `skies\xx.cub` | **yes, as a real sky cube** | `skies\<code>.cub` in the originals' exact layout (section 9), rendered from the arcade dome. Environment `CubeFile=<code>.cub`, as TA/TB/MA/GA/GB/GC. The dome is no longer a scene model. |
| `textures\*.pst/.psm` | 13/13 | yes | geometry helper: one per arcade texture/material. PSM `TextureMap_1` names the PST. |
| `textures\*.atm` animated textures | 13/13 | pending | the arcade scrolls one texture on Etti IV and Malastare: GPU panning slot 1, 3/16 texel per frame in V (race-data.md 18). Which materials use that slot is not known yet (24), and Revenge animates with .atm frame sets, which are not written yet. |
| hook +44 extra static objects | courses 0, 3 | yes | **A** objects and placements from `notes\race-hooks.txt` (course 0: 11 placements; course 3: 16), converted with the geometry helper's pipeline (`trk_hookobjects.py`; geo_course helpers, geo_encode, geo_pst, geo_psm used unchanged) and placed as world pieces. The 4th placement value is a **uniform scale**, not a yaw: f_0C146F3A multiplies the bounding radius by it (0C146FCC), and the objects that get 20-180 are unit-sized (A-derived). Examples: start arch 1230 at the start line, x90 (course 0); 1243 x55 (course 3). |
| `tracks\axx.spl` AI spline | 13/13 | yes | **A-derived racing line** (race-data.md 13). Each node lies at the arcade AI's own lateral target between the left and right AI edge lists, from the per-course racing-line curve (@0C2F9260 / 92D0 / 9338 / 93B8), on the floor height. Widths = distance to the edge lists, at least 30 ft (G floor; the arcade line often runs on an edge). Node choice G (at most 6 ft off the centre line, 60-400 ft spacing, at most 250 nodes). SpeedLimit G. DistanceToFinish A-derived (the originals' 3D formula). Course 4 has no AI data, so its AI spline is the centre line with 80 % of the wall width (G). |
| `tracks\mxx.spl` map / lap spline | 13/13 | yes | arcade centre line. Gate widths A-derived from the collision cross-section + 30 ft, kept clear of other parts of the course. 5 ProgressNodes (G) at 2/25/50/75/97 % of the lap. The arcade has no checkpoints: a lap there is net forward progress through all collision sectors (race-data.md 12), which these spread markers stand in for. |
| start grid | no file | via map node 0 | Revenge computes 8 slots from map node 0 and its predecessor. Node 0 = the centre-line point at the arcade grid's front row (0C3E9778). All 8 slots land inside the arcade grid area on the collision floor and on the visual road. |
| laps | no file | n/a | Revenge's lap count is a race option. Arcade laps (4, 4, 4, 3; @0C3E8D68) are recorded in the manifests. |
| wrong way / race position | no file | via splines | Revenge derives them from map gates and DistanceToFinish. The arcade uses a 2-second wrong-way rule on its sectors (race-data.md 12): same idea, no extra data. |
| `geometry\*.col` collision | 13/13 | yes | **A** arcade collision triangles, through `a2r_space`, split per zone and per arcade surface code. **Orientation from the arcade winding** (`a2r_space.collision_order`): every arcade collision triangle is wound with cross(v1-v0, v2-v0) pointing down, which the mirror and an extra reversal turn into up-facing Revenge faces; the converter refuses any down-facing result. The first version took the per-record stored normal, which is not consistent, and left 956 of 4,180 floor faces down; pods fell through in the silo race. **G** AABB tree and planes, using the originals' conventions. All COLs are in world coordinates at Position 0, so edges shared across zone splits are bit-identical. Names `xx_zNN_sXX_floor_col` (floor category from "floor" in the name, `track_collision_tags50an.cpp`), `xx_zNN_strip_floor_col` (the arcade 0x8000 edge strips: flat, about 1 unit wide, kept as up-facing floor as in the arcade) and `xx_zNN_wall_col`. One collision-only placed model per COL, as the originals do (`Zone01_COL_floor`). |
| walls (`xx_zNN_wall_col`) | 13/13 | yes, generated | **G, derived from arcade data.** The arcade has no vertical walls: its physics treats entering an 0x8000 strip as a wall hit. The converter puts a vertical wall on every edge between drivable floor and a strip, and on every open outer edge of a strip, facing the track. Height = the visual barrier top above the edge where that is higher than 82 ft, else 82 ft (the Revenge originals' median wall top above the floor). Walls continue across zone splits (shared end points). Etti IV: 1,000 + 1,000 edges, 4,000 vertical faces. |
| zones / portals (`tracks\xx.scb`) | 13/13 | yes | **A** the arcade's own collision sectors, read from each course: 256 / 128 / 128 / 256 / 32 (the units it tracks position and laps with, race-data.md 12). **G** grouped into 8-28 zones of about 4,000 ft of lap. Each portal sits on a zone's entry edge, clipped to the road cross-section + 100 ft. |
| `.scb` Environment, Lights, Fog, GhostOpacity, TerrainZone1..8 | 13/13 | yes | **Fog colour A** (hook +40: Etti IV 255,255,255; Malastare 30,7,64 night; race-data.md 15). **Light2 = arcade light 0** (direction mirrored with the course, colour; race-data.md 20) [A]. **HasLensFlare** T on courses 0, 1, 3, 4, where the arcade draws its sun lens flare (flare objects 1219-1225, hidden by the LENS line-of-sight test); F on Malastare [A]. **Malastare's 32 flickering point lights** -> `ObjectPoint` Light3..34 (positions and colour A). Their attenuation comes from the arcade range 60-150 ft, and the frame-parity flicker becomes a Revenge flasher (pattern on/off, FlashFilter 0.8) [G]. Ambient A (median material ambient). The other arcade directionals (fill lights; Malastare's 3 extra) are listed in the manifest but not written: every Revenge track has exactly one directional, and a second one is unproven in the port. Fog Visibility and terrain keys D (GC). |
| `.tob` CSCameras, PodRacer | 13/13 | yes | 3 cut cameras G; `TrackAllowFloor=1` as every original. No Animations/Triggers/ParticleEmitters/StaticModelParams (SA has none either). |
| `tracks\xxNN.vub` intro cameras | 13/13 (3-7) | yes, 3 | G paths: grid flyover (300 frames), side shot (250), behind the grid (150). Record layout from ta01.vub (D). The arcade has no per-course camera paths, only pod-relative race and intro cameras (race-data.md 14). |
| `tracks\xxTVCam.pos` TV / replay / finish cameras | 13/13 | yes | G: 15-40 cameras along the map spline, outside of bends. |
| `tracks\xx.tun` | 13/13 | yes | D copy of the same-class donor (A ta, B tb, C tc). Arcade AI pace (lap-time targets per level, per-lap ease-off, catch-up) is recorded in the manifest; mapping it to the .tun is pending. |
| `tracks\xx.aub` | 13/13 | yes | D TA material sounds, one default material, no ambient/positional sounds: the arcade has none on any course, only shared sound packs and one music stream (race-data.md 22). |
| `audio\introAud.ini`, `audio\scrptLns.ini` | 13/13 | yes, empty | G NumEvents 0 / NumberOfLines 0 (Revenge has no lines for these planets). Reader acceptance unchecked. |
| `particle\demoprt.prt/.pst/.psm`, `textures\debug*.tga` | 13/13, 12/13 | yes | D byte copies from TA. |
| `anim\*` + `.tob` Animations | 6-8/13 | none needed | no animated scenery on the race courses. The "Kaadu" routine of hook +40 is the sun lens flare (race-data.md 18); Kaadu creatures exist only on the unreachable course 4 (pending there). |
| `particle\*.emt`, `tracks\xx.wtt`, `tracks\xx.esb` | 8/13, 2/13, 4/13 | not produced | the arcade has no emitters, no water physics and no ecosystems (race-data.md 21-22). Etti IV only draws mirrored pod reflections across arcade y = 0 (= Revenge 1,000 ft); Revenge has no file for that, and the manifest records the plane. |

### 3.2 Per-track files outside TRACKS

| File | Produced | Source / how |
|---|---|---|
| `UI\TEXTURES\LD_<CODE>.PST` loading screen (640x448, one level, like LD_TA) | yes (`install\UI\TEXTURES\`) | A-derived: top view rendered from the converted geometry. |
| course names (`LOC.RES` `STR_PLANET_*`, `STR_TRACK_*`) | text table (`menu\loc_strings.txt`) | A names, G ids. LOC.RES (UI.loc + L_UI_*.bin, 5 languages) not rewritten yet. |
| `UI\UICOURSE\UICOURSE.RES` holo_xx.psg/.mcf/.pst | pending | course-select hologram, not made yet. |
| HUD track map | none | shared `maptop` overlay; nothing per track. |
| announcer VAGs (`AUDIO\SPEECH\DAX\TRACKS\...`), race/intro music | pending | no Revenge lines exist; arcade music streams 20/6/19/1 need the Revenge music container. |

## 4. The complete track folder `export\a2r\<code>\`

| Path | Content |
|---|---|
| `install\TRACKS\<CODE>.RES` | the track archive, every member listed above |
| `install\UI\TEXTURES\LD_<CODE>.PST` | loading screen |
| `members\` | the same RES members loose (geometry, textures, tracks, particle, audio) |
| `source\tracks\<code>.scn/.toy` | readable sources of the .scb/.tob |
| `menu\loc_strings.txt` | new LOC strings |
| `manifest.json` | transform, provenance per file and per field, flags, pending list, arcade laps/AI speeds/grid |
| `verify\` | verify.json, top.png, start.png, geo_top.png, geo_chase_start.png, geo_chase_mid.png |

| N | Code | Class | RES members | Landscape PSG | Hook-object PSG (placements) | COL | Lights | Nodes | Zones | Lap (ft) |
|---|---|---|---|---|---|---|---|---|---|---|
| 0 | ua (provisional) | A | 382 | 166 | 61 (11) | 26 | 2 | 113 | 10 | 38,550 |
| 1 | **eb** Etti IV | B | 470 | 351 | - | 36 | 2 | 149 | 10 | 38,750 |
| 2 | **lc** Malastare | C | 285 | 179 | - | 27 | 2 + 32 point | 126 | 8 | 29,940 |
| 3 | td (provisional) | C | 578 | 318 | 71 (16) | 50 | 2 | 220 | 20 | 80,200 (longer than any Revenge lap; largest TC 76,800) |
| 4 | ka (provisional) | A | 75 | 26 | - | 16 | 2 | 17 | 8 | 2,230 (Kaadu oval, not an arcade race) |

## 5. Verification (`trk_verify.py`, log `export\a2r\verify.log`)

All five: no problems.
- **Archives.** `<CODE>.RES` re-read with `trk_res3`. Every member re-read by its own reader (COL,
  BINARY, INI, VUB, PSG, PST) and re-written byte-identical. Loose copies equal the archive. The
  .scb/.tob equal the compiled text. The loading screen re-reads at 640x448.
- **Cross-references.**
  - Every scene model's SLT resolves to its PSG and/or COL.
  - Every PSG material resolves to a PSM, and every PSM texture to a PST.
  - Every .tob cut camera has its .vub, and every fixed name the race loader reads is present.
  - Nothing in the archive is unreferenced except the 4 debug textures, which 12 of the 13
    originals also carry unreferenced.
- **Port reader limits.** Splines are within them (at most 256 nodes, at most 64 KB, number syntax,
  one closed loop).
- **On the visual road.**
  - Every AI node lies on the decoded visual geometry. Node height minus road surface: median
    -0.11 to +0.01 ft; 95 % within 0.2 ft (Etti IV) and 0.04 ft (others).
  - Every AI node lies on the collision floor.
  - The 8 Revenge grid slots are on the collision floor.
- **Looked at.** `verify\geo_top.png` and two chase views for each course draw the textured
  decoded PSG geometry with the collision (walls magenta, floor cyan), AI spline (yellow), AI gates
  (orange), TV cameras (blue) and cut-camera paths (white) on top.
  - Etti IV, Malastare and Boonta: spline, gates and walls follow the visual road in the top views
    and in the chase views.
  - Etti IV's long eastern loop runs over open sea in the arcade's own top view too.
  - Boonta's narrow canyon/tunnel stretches show only a thin road from above, as in the arcade
    preview.
  - A first version of these renders came out blank: textures were passed in the wrong colour
    range. Fixed and re-rendered.
- **Value ranges.** Against the 13 originals: `export\a2r\ranges.txt`. Etti IV, Malastare and
  Bantha Tracks are inside the original ranges for nodes, spacing, AI and map widths, progress
  nodes, zones, portals, COL count, TV and cut cameras. Etti IV AI width median 210 ft (originals
  150-270), map 411 ft (300-550). COL files hold more faces each (arcade collision is a few large
  road pieces). Boonta's lap and the Kaadu oval's tiny lap are outside the Revenge range.

## 6. Arcade data from race-data.md

| race-data.md | Status |
|---|---|
| 12 laps / sectors / wrong way | used: sectors define zones and portals; the lap = sector-progress rule is noted against the G progress nodes |
| 13 AI racing-line curves | **used** (AI spline positions) |
| 13 speed table = lap time per AI level, per-lap ease-off, catch-up | recorded; .tun mapping pending |
| 14, 23 cameras | no per-course paths exist (race cameras are pod-relative); cut/TV cameras stay G |
| 15 fog colour, near/far | **used** (colour); near/far -> Visibility mapping unknown |
| 15 / race-hooks.txt +44 extra static objects | **used** (11 + 16 placements converted to PSG; 4th value = scale) |
| 15 +48 per-block fog factors | pending (no Revenge equivalent found) |
| 18 animated scenery | none on race courses ("Kaadu" = sun lens flare -> HasLensFlare); scrolling texture pending (which materials) |
| 19 race position | the arcade sorts by net sectors; Revenge derives position from its splines, no data needed |
| 20 lights | **used**: light 0 -> Light2 (direction, colour); Malastare 32 flickering point lights -> ObjectPoint with flasher; fill/extra directionals listed, not written |
| 21 water | none (reflection plane only, recorded) |
| 22 sound | none per course |

## 7. Open items

- **Handedness and race direction:** taken from the geometry helper's evidence and the centre-line
  order. Not confirmed on the cabinet (needs Odin + the MAME BIOS sets).
- **Not checked in the port** (that needs a build, which is outside this task):
  - intro audio with 0 events;
  - a sky dome as a placed model with NumberOfSkyCubes=0;
  - the meaning of FlashPeriod for the Malastare flicker (taken as the time for the whole
    32-step pattern, 0.53 s = one step per frame);
  - collision-only zone models (originals do this);
  - the per-course identity tables (WP11).
- **Not written yet:** the shared UI/LOC archives, music and announcer audio.
- **Course codes** ua/td/ka are provisional (a2r_space.CODES).

## 8. Silo test: Etti IV packed as SC (Abyss)

Following `notes\silo-override.md` section 5 (converter side).
- **One override parameter:** `trk_convert.py 1 --code-as sc`. The code comes from
  `a2r_space.course_code(N, override)`. Physics class follows the packed code (sc -> B by the exact
  rule). Geometry and texture names stay `eb_*`, as the note asks. Output:
  `export\a2r\override\sc\` (same layout as section 4).
- **Member names** are SC's exactly: `tracks\sc.scb`, `asc.spl`, `msc.spl`, `sc.tob`,
  `sc01..03.vub`, `scTVCam.pos`.
- **Borrowed, byte-identical from the original SC.RES** (the port pins them; marked `BORROWED` in
  manifest.json and in converted-content.json): `tracks\sc.tun`, `tracks\sc.aub`, `audio\introAud.ini`
  (3 events), `audio\scrptLns.ini`, `particle\demoprt.prt/.pst/.psm`. The original
  `UI\TEXTURES\LD_SC.PST` stays in place (not written).
- **Every zone uses SearchType List.**
- **Facts** (`trk_facts.py`): computed from the files the way the port counts them. Checked against
  the port's pins for original SC and TA:
  - entries, decoded bytes, zones, portals, instances, map/AI nodes, placed COL, scenes,
    placements, COL-only, surfaces, materials, vertices and indices (both per placement), images (one
    per material), triggers, static params and cuts reproduce SC and TA exactly;
  - RGBA bytes match only for materials with a plain PST: the port also counts the .atm frame sets
    of animated materials, which converted tracks don't have.

  Etti IV as SC (after the collision fix): 480 entries, 13,787,136 decoded bytes, 10 zones, 10 portals,
  397 instances (351 placements + 46 COL-only), 149 map/AI nodes, 46 placed COL, 351 scenes, 351 surfaces, 32
  materials/images, 82,526 vertices, 149,880 indices, 3,264,512 RGBA bytes, triggers 0, static params 0,
  cuts 3, intro events 3.
- **Limits** (`trk_silo.py`, every one listed in silo-override.md): all within. Archive
  1,645,259 / 2,974,353 bytes; decoded 13.8 / 26.7 MB; entries 480 / 984; COL 46 / 252; instances
  397 / 4,107; textures 32 / 64; RGBA 3.3 / 8 MiB; texture sources 1.1 / 32 MiB; nodes 149 / 256; no sky
  cube; all zones List; 3 cuts; 3 intro events.
- **Installed:** the original `TRACKS\SC.RES` was copied to `silo\originals\TRACKS\SC.RES` first,
  after checking it against the port's pinned SHA. Then the new `SC.RES` and
  `converted-content.json` (schema `a2r-converted-content/1`) went into `silo\assets\`. Both were
  re-read after writing. `C:\Games` was only read.
- **converted-content.json:** per replaced file:
  - path relative to the asset root ("/" separators), bytes and SHA-256;
  - `replaces` {bytes, SHA-256 of the original};
  - the facts block, and every member {name, offset, bytes, SHA-256, borrowed};
  - plus `kept_original` (LD_SC.PST with its hash), `borrowed_members`, the limit table with port
    file:line, and the source (arcade course 1, transform, converter command).
- **Expected to look wrong in the first run:** SC's fog (typed per course in the port, per
  silo-override.md) and SC's announcer lines during the intro.

### 8.1 Collision fix after the first silo race (pods fell through, no walls)

- Cause 1: faces were oriented from the arcade per-record stored normal, which is not consistent
  (956 of 4,180 floor faces faced down). Revenge collision is one-sided. Fix: orientation from the
  arcade winding, which is consistent (every triangle wound with its normal down), through
  `a2r_space.collision_order`.
- Cause 2: the arcade 0x8000 triangles are flat edge strips, not walls. Fix: generated vertical walls
  on both edges of every strip (section 3.1, "walls").
- Checks on all five courses (`trk_colcheck.py`, plus the coordinator's `scratch\planecheck.py`,
  `downcheck.py` and `wallcheck.py`), with the Etti IV / silo SC values:
  - floor faces up: 6,180, down: 0;
  - wall faces vertical: 4,000 of 4,000;
  - open floor edges without a wall at their base: 0 of 1,000;
  - edges shared across zone splits: 1,461, bit-identical (world-space COLs);
  - AI nodes without a floor face below in their own zone's COL: 0 of 149.
- 3,982 of 4,000 wall faces face the nearest centre-line point. The other 18 are on hairpins, where
  that point lies across the bend; the facing comes from the floor triangle the wall borders.
  wallcheck.py's "facing the floor side" test is ambiguous here because these walls stand exactly on
  floor edges.
- Re-installed: `silo\assets\TRACKS\SC.RES` is 1,645,259 bytes, SHA-256 a7855d25...; converted-content.json
  is updated, and every limit is still within.

## 9. Sky: how Revenge does it, and the converted skies

### 9.1 Revenge skies (13 originals, port)
- **Scene:** `[Environment] CubeFile=<name>.cub` (TA ta.cub, TB tb.cub, TC ta.cub, MA, GA, GB, GC
  sky21.cub, MC mcset4.cub), or `NumberOfSkyCubes` + `CubeFile1` + per-zone `CubeFileForZoneN`
  (RA, RC, MB, SA), or `NumberOfSkyCubes=0` with no cube (SC only). The member is `skies\<name>.cub`;
  GB also has `skies\gb.acb` (a cube-animation INI).
- **CUB file** (all 12, verified by `trk_sky.read_cub`): `u32 1, u32 1, u32 16, u32 99100`; a palette
  block (start/count/compressed, 768 RGB, 32,768 + 65,536 lookup tables; not used by the IPUM tiles);
  then the cube:
  - `u32 32` (format), `u16 0, u16 0`;
  - face masks `00ff 00ff 00ff 00ff ffff 0000`: the four side faces have only their upper 8 tiles,
    the top has all 16, there is no bottom;
  - 2-bit size codes `0xaaaa x4, 0xaaaaaaaa, 0` (all 128 px);
  - origin (0,0,0), look (0,0,1), up (0,1,0);
  - 48 tiles in face order +Z, -X, -Z, +X, +Y (tile = row*4 + column, row 0 at the top), each
    an 11-word descriptor (0,32,0,16,4096,4,0...) and 4 IPUM mips 16/32/64/128.
  - IPUM = `ipum` u32 size u16 w u16 h u32 1, one PS2 IPU intra frame (MPEG-2 intra macroblocks,
    flags byte 0), `00 00 01 B0`, `00 00 01 B1`.

  GB is the exception: 7 cubes, animated through gb.acb.
- **Port drawing** (`tests\native\sky50gw_fixture.h`, `docs\sky-render50gw.md`; WP8 in
  conversion-spec.md 5):
  - The mesh is a camera-centred 4x4 tile grid per face, half extent 25,600 ft, cube centre 1,024 ft
    above the eye.
  - Tile UVs span 0.03125..0.96875: each 128-px tile carries 4-texel margins, so neighbouring
    tiles overlap cleanly.
  - It is drawn first, with an orientation-only view, no depth test or write and no fog, with
    opaque clamped textures.
  - Below the authored tiles (the whole lower half) the viewport clear shows, which is the
    scene's `Fog ColorRGB`.
  - Tiles decode through the IPU path (FFmpeg-equivalent) with the PS2 RGB16 conversion; pure
    black reads as transparent.
  - The sun and lens flare are separate: the Directional light with HasLensFlare.

### 9.2 Converted skies (`tools\a2r\trk_sky.py`, called by trk_convert for every course)
- **Source:** the arcade landscape +20 dome (240 triangles, unlit, one texture), decoded by
  hikaru_mesh with its texture.
- **Rendering:** every texel of the 48 authored tiles is ray-cast from the typical track position
  (centre-line centroid, median track ground + 9 units, the arcade chase-camera height), through a2r_space, so the mirror is respected. Texel directions follow the port's
  exact tile geometry, margins included, so seams match by construction. Rays below the dome rim
  (76 of 786,432 on Etti IV) take the lowest dome colour in their column.
- **Horizon:** the lowest 12 texel rows fade into one horizon colour, and that colour is written
  as `Fog ColorRGB`, which is the clear colour under the cube. There is no visible edge. The
  arcade's own fog colour is recorded in the manifest.
- **Encoding:** `trk_sky.encode_ipu`: MPEG-2 intra blocks with the default intra matrix,
  quantiser code 2, every AC coefficient coded as an escape, flags byte 0. Every tile decodes
  with FFmpeg's IPU decoder under strict error checking. Mean error on a test pattern is 5 of 255,
  mostly the PS2 16-bit colour step.
- **Size:** about 420 KB per cube, against 160-300 KB for the originals, because of the
  escape-only coding. That is within the port's fixed sky capacity (48 x 128 x 128 RGBA).
- **Palette block:** copied byte for byte from ta.cub (D).
- **Parallax, which differs by design:** the arcade dome is a finite object about 6,500 units across
  around the track (Etti IV), so cloud positions shift as you drive. Revenge's cube is
  camera-centred. Compared from the arcade preview cameras, clouds sit a little lower and
  elsewhere in azimuth; colours match.
- **Sun:** the Directional light already comes from arcade light 0, with HasLensFlare matching
  the arcade's sun flare (section 6).
- **Verification:** `tools\a2r\trk_skyview.py N`. It decodes every tile with FFmpeg,
  draws the sky the way the port does, puts the decoded PSG geometry over it, and writes
  side-by-side images against the arcade previews (`verify\sky_chase_0..3.png`), close-up seam
  views (`sky_seams.png`) and a face sheet (`sky_faces.png`).

### 9.3 Sky colour and cloud size check (after review of sky_chase_2)
- **Converted clouds were paler and smaller/lower than the arcade's.** I measured each stage
  separately (Etti IV cloud band):
  - arcade texture: std 70/62/31;
  - rendered tiles: std 52/50/22;
  - after the IPU encode, FFmpeg decode and PS2 RGB16 conversion: 52/50/23.

  So the encoder does not lose contrast.
- **No half-brightness convention:** the originals' decoded sky texels are stored at full
  brightness (TA: mean 163/175/210, peaks 232-248).
- **Cause 1: projection centre.** Seen from the dome centre, the clouds were smaller and lower.
  Ray-casting the arcade dome straight from the arcade chase camera reproduces the arcade preview
  exactly (`verify\_dome_from_camera.png`). The cube is now projected from the typical track
  position (centre-line centroid at track height + 9 units), which keeps cloud size and height
  close to the arcade view along the track.
- **Cause 2: the horizon fade.** It covered 32 texel rows, which washed out the low cloud band and
  the blue mountain strip under it. It now covers 12.
- **What remains:** a thin haze band between the cube's lower edge and the horizon. That is the
  port/original convention: the cube centre is 1,024 ft above the eye, so the authored tiles start
  about 2 degrees above eye level, and the fog/clear colour fills below.
- **Silo installs:** only through `trk_silo.py`. It refuses when the geometry output is newer than
  the packed archive (run `trk_convert.py 1 --code-as sc` first) and holds `silo\.install.lock`,
  so the geometry helper's re-installs and the track side's cannot overwrite each other.

### 8.2 Grid blockage in the silo races (back two rows never left the grid)
- **Cause:** a zone split. Zone 1 began at the arcade sector holding map node 0, so portal 1 ran
  between the front row (at node 0) and the slots 3-7 behind it. Pods there start in the last zone,
  on the other side of the portal from node 0.
- **Checks:** none of the 13 original tracks has a portal between node 0 and any start slot
  (checked with the same segment-against-portal test). The converted collision at the grid is
  clean: continuous up-facing floor, walls only along the road edges (x 12854 / 13046 inner,
  12800 / 13090 outer), no ceiling (`tools\a2r\trk_spotcheck.py`, close top view and side
  section in `verify\spot_0_*.png`).
- **Fix, for all courses:** zone 1 now starts at the sector of the centre-line point 300 ft behind
  map node 0 (`GRID_CLEAR_FT`), so the whole grid (200 ft deep) is in zone 1.
- **The two mid-course stuck spots** (x 8970 z 13285, x 12560 z 7860): the reset positions are 2-8 ft
  from the drivable-floor wall on the track side, at the outside of a bend. The test driver (throttle
  held, steering inverted) pins itself against the wall there; the pod is not behind the wall or on
  a gap. The AI racing line keeps at least 23.8 ft from every wall on the whole lap.
- **Automatic checks** (`trk_colcheck.py`, run for every course):
  - a pod-sized sweep (15 ft clearance) every 10 ft along the AI spline;
  - from each of the 8 start slots, along the map spline for 800 ft at the slot's lateral offset;
  - no portal between map node 0 and any start slot.

  Etti IV: AI lane minimum 23.8 ft, start slots 56-96 ft, no portal through the grid.
- **Follow-up from the sweep check on all courses:**
  - Bantha Tracks and Malastare had AI-lane samples 12.8-13.3 ft from a wall, where the arcade
    racing line runs on its corridor edge past a kink in the floor edge.
  - The racing line is now computed at every arcade centre-line point and kept 20 ft inside the
    drivable floor (`AI_WALL_CLEAR_FT`). Node selection also keeps each chord within 6 ft of that
    line.
  - The zone count drops when needed so zone 1 holds the whole grid (the Kaadu oval's zones were
    shorter than the grid).
  - Results:
    - AI lane minimum: Bantha Tracks 17.2, Etti IV 23.4, Malastare 17.5, Boonta 22.1 ft;
    - all start slots clear;
    - no portal through any grid.
  - The Kaadu oval (not a race in the arcade) still has 7 slots within 15 ft of a wall: its road is
    narrower than Revenge's fixed grid (+-40 ft, 200 ft deep).

### 8.3 Jams at zone boundaries (x 6290 z 11460, x 10970 z 7030)
- **Cause:** both spots are 100-130 ft from a portal (zone 3/4 and zone 6/7). Arcade sectors list
  the triangles that span their boundary in both sectors. The converter put each such triangle only
  in the first sector's zone, so up to 60 ft of floor past a portal existed only in the zone before
  it (12 faces at portal 4, 10 at portal 7). A pod crossing the portal into the next zone found no
  floor there in that zone's collision. Nothing protrudes into the lane at either spot, and there is
  no wall corner or step.
- **Fix, for all courses:** a triangle goes into every zone whose sectors list it, and its walls
  follow it (Etti IV: 145 triangles now in two zones).
- **New sweep checks** (`trk_colcheck.py`):
  - walk the AI lane every 10 ft, switching zone at every portal crossed, and require a floor
    face below each sample in that zone's COL files. Before the fix Etti IV had 42 samples without
    floor, including spot 1; now 0;
  - floor steps: wherever two samples 10 ft apart differ by more than 3 ft, re-sample every 1 ft;
    a real step jumps more than 1 ft between neighbouring 1-ft samples, otherwise it is a slope
    (the steepest slope is reported). Now 0 steps on every course. The steepest lane slopes are
    arcade terrain: Boonta 36.5, Bantha 31.5, Malastare 17.8 degrees. Etti IV's was a kerb ramp at
    x 5761 z 9314 (surface code C, 3 ft over about 6 ft).
- **Boonta:** its last AI node lay exactly on portal 1, so both of that node's segments crossed the
  portal. Any portal within 2 ft of a spline node now moves 5 ft back into the previous zone, which
  avoids a double crossing in the port's node-zone walk too.
