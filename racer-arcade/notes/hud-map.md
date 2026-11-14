# HUD track map (radar) on converted courses

Nov 13 2026. Read-only on the port and silo trees; nothing built, nothing run. Scripts and images in
`D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\hudmap`.

## Where the race HUD map comes from

- There is no per-course map picture or model anywhere. The race HUD map is the original "radar"
  (original 1924C0 / 192C70 / 1922B0): every frame it walks the course's **map spline**
  (`tracks\m<code>.spl`, inside `TRACKS\<CODE>.RES`) and draws the nodes that fall inside a small
  window around the player, rotated to the player's heading (about 1,600 ft ahead, 600 ft behind,
  1,260 ft each side). Only the frame (`ui\textures\maptop`) and the marker textures come from
  `UI\TEXTURES\OVERLAYS.RES`, and those are the same for every course.
- Port chain (silo tree `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\tree`):
  - SC map spline read from the loaded SC.RES: `tests\native\sc_cold_sources50la.h:223-235`
    (`readTrackNavigation50EU` on member bytes of `tracks\msc.spl`, count from the manifest fact).
  - Copied into the race map: `tests\native\terrain_interactive_setup50du.cpp:1441-1450`.
  - Handed to the HUD as `actualRadarMapB8`: `tests\native\terrain_interactive_publication50du.cpp:2094`.
  - Radar build: `tests\native\original_position_stats_resources50la.h:600-631` and `:401-437`;
    traversal `src\game\original_position_stats_radar_route50la.h:47-107`; projection
    `src\game\original_position_stats_numeric50la.h:100-130`; draw
    `tests\native\original_position_stats_hud50la.h:179-214`.
- Scan of every original UI archive (`hudmap\uiscan.py`): no map/radar/course-layout member other
  than `dmgradar.pst`, `maptop.psm`, `RadarOverlay.psm` (shared) and the course-select
  `holo_<code>.*` (a scenery picture on a generic 7,072-byte plane, identical size for all 13
  courses; `hudmap\holo_sc_orig.png`). `LD_SC.PST` is a generic planet screen with no map
  (`hudmap\LD_SC_orig.png`).

## What the silo actually draws (checked against captured frames)

- Installed spline: `silo\assets\TRACKS\SC.RES` holds the converted `tracks\msc.spl` (151 nodes,
  Etti IV), `silo\assets-lc\TRACKS\SC.RES` the converted Malastare one (129 nodes); original SC has
  179. World plot of all three: `hudmap\splines_world.png` (red SC, blue Etti IV, green Malastare).
- `hudmap\radarsim.py` re-draws the radar from each spline at the player's logged position and
  heading, next to the radar crop of every captured race frame (silo-cv-09 runs):
  - `hudmap\radar_compare_silo-eb-frames-02.png`, `..._silo-lc-frames-02.png`,
    `..._silo-eb-frames-01_a/_b.png`, `..._silo-lc-frames-01_a/_b.png`
  - top row = actual HUD, middle = converted spline, bottom = original SC spline.
- Result: in these runs the HUD line matches the converted spline (Etti IV start straight, the
  crossover fork at 66 s, the end loop at 74 s; Malastare's hairpins) and does not match SC's.
  Some frames show an empty radar or a line off to the side of the player; position there is
  interpolated from 5-second log rows, so those frames are not conclusive either way.

## So why did the user see SC's map

Not found. My frame checks cover the automated silo-cv-09 runs only. The user plays
`silo\play\RacerRevengePC.exe`, which is silo-cv-08 (different exe from the frames runs), and no
log or frame of the user's own session exists. The code path above has no SC-original fallback, no
cache across sessions, and no hash-pinned map table. To see what the user sees: one frame capture
with the play build (cv-08) on both asset copies, plus the exact screen/moment the user means
(race radar top-right, course-select hologram, or something else).

## Nothing to generate; no converter hook-up needed for the radar

The converter already writes the data the radar is drawn from (`msc.spl`). No `trk_hudmap.py` was
written: there is no original per-course HUD map file to produce.

Remaining per-course item outside SC.RES that still shows Baroonda content: the course-select
hologram picture `ui\textures\holo_sc.pst` in `UI\UICOURSE\UICOURSE.RES` (already listed as
pending in a2r-track.md 3.2). It is a scenery picture, not a map.

## Port stand-ins seen (flagged, not edited)

- Radar map is chosen per course: `terrain_interactive_publication50du.cpp:2094-2099`
  (`isMbTrack/isTcTrack/isTbTrack/isGbTrack/isMcTrack` views), and the HUD accepts only one of six
  per-course map types (`original_position_stats_adapter50la.h:195-218`,
  `original_position_stats_resources50la.h:620-627`). General fix: one map view for every course.
- MC radar pinned to exactly 173 map nodes: `original_position_stats_adapter50la.h:201`,
  `original_camera_session50kx.h:1665`, `original_ta_camera_issuer_candidate.h:655`. A converted
  track loaded in MC's slot would lose the radar (adapter refuses). General fix: count from data.
- The radar map is passed only when `directOriginalHuman` (publication50du.cpp:2094-2099);
  otherwise the HUD radar has no map.
