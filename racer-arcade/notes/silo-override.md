# Silo test: racing converted Etti IV under an existing course code

Nov 12 2026. Read-only investigation. No repo file changed, nothing built, nothing run.
Inputs: `D:\Programming\GitHub\RacerRevengePC\CLAUDE.md`, `D:\Programming\GitHub\RacerRevengePC\docs\HANDOFF-20261106.md`,
`D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\notes\conversion-spec.md` section 5, `a2r-track.md`,
`a2r-geometry.md`, `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\export\a2r\eb\`.

## Plain summary

- **No single switch admits it.** Every course's race path checks the exact original bytes, and then
  checks dozens of exact counts. Examples: TA must have 383 archive members, 318 placed objects, 9 zones
  and 29 known textures; SC must have 30 zones, 179 map nodes, 252 collision files, 9 named triggers
  and 451 static-model rows. A "trust files listed in the converter's manifest" switch only fixes the
  byte checks. The count checks and the "this course must have its triggers, lava animation, door..."
  checks need real code changes. Those changes are the same work as WP11 + parts of WP5/WP7 in
  conversion-spec.md section 5.
- **Best target: SC** (Baroonda "Abyss"). Etti IV is physics class B, and SC is class B (exact code
  rule) with AI class B (tournament order 8). SC is also already set up for "no sky cube" (Etti IV has
  none), has 3 cut cameras (Etti IV has 3), and has no positional sound, door or water. Overriding SC
  leaves TA.RES alone. That matters because every course's audio loader reads TA.RES by its fixed hash.
  **TA is the worst target for a silo:** about 25 refusal points, physics class A (wrong class), and
  replacing TA.RES breaks race audio on every course.
- **The smallest silo change for SC:** about 18-20 existing files plus one new general helper, with
  the converter repacking Etti IV as `SC.RES`. Details in section 5.
- **A separate worktree build is a full build: 616 compile units.** It needs two fixes to the build
  inputs first: a count gate and 14 source files missing from the source list. The incremental relink
  tools only work in the main tree.
- **Testing:** the existing logs-only runner works from the worktree. SC is reached through the
  tournament route with the `order8.bin` seed save. Exact commands are in sections 3, 4 and 6.

## 1. Admission: everything that rejects substituted files

Outcome legend: **refused** = `require50DU` / `need` throws `std::runtime_error` and the race load
aborts with a message (`D:\Programming\GitHub\RacerRevengePC\tests\native\terrain_interactive_frame_calls50du.h:19-21`);
**exit** = `std::exit(1)`; **wrong** = loads but behaves as the original course.

### 1.1 Archive and loose files: shared for every course

| Check | file:line | What Etti IV hits | Smallest silo change |
|---|---|---|---|
| Course row: archive SHA + bytes, terrain owner count, zones/portals/placements | `D:\Programming\GitHub\RacerRevengePC\tests\native\track_session_assets50ig_fixture.h:20-34` (world rows), `:75-242` (13 `retainedXX` rows) | different SHA/size; SC terrainOwners 252 vs 36 COL; TA 9/10/318 vs 10/~10/? | The row fields become the expected values only for original files. The general helper (below) supplies the converted file's SHA/size and counts from the manifest. |
| Ground loader archive identity | `D:\Programming\GitHub\RacerRevengePC\tests\native\authored_runtime_ground50cp_fixture.h:76-78` (TA/GA/MA/SA/GC path, reached from `terrain_interactive_world50du.cpp:733`) | SHA differs | **exit**. Replace the inline compare with the helper. |
| Terrain owners == row count | `D:\Programming\GitHub\RacerRevengePC\tests\native\terrain_interactive_world50du.cpp:1114` | 36 vs 37 (TA) | refused. Count comes from data. |
| Particle archive re-read | `D:\Programming\GitHub\RacerRevengePC\tests\native\terrain_interactive_publication50du.cpp:798-805` (decode cap 64 MiB / 4096 entries, fine) | SHA differs | refused. Helper. |
| Canonical mount | `terrain_interactive_publication50du.cpp:809-813` -> `D:\Programming\GitHub\RacerRevengePC\src\assets\original_standard_archive_provider50la.h:77-101` (digest is only stored here, not recomputed; caps `maxEntries`/`maxBytes`) | passes if given the converted digest | none beyond the row |
| TA-only encoded mount | `original_standard_archive_provider50la.h:215-228` (3,334,562 bytes, 533 blocks, 383 entries, 13,099,008 decoded, fixed SHA) | everything | TA only |
| Camera assets archive identity | `D:\Programming\GitHub\RacerRevengePC\tests\native\original_camera_assets50kx.h:139-141` | SHA differs | refused. Follows the row, so it is fixed with it. |
| Cut-camera identity (tournament races only) | `D:\Programming\GitHub\RacerRevengePC\src\assets\original_track_cut_identity50la.h:25-91` (archive SHA, member count, .tob/.vub size+ordinal+SHA, frames, zones, offsets, introAud/scrptLns SHA, event count); cut count per code `D:\Programming\GitHub\RacerRevengePC\src\assets\original_ta_cut_authoring_candidate.h:143-145`; `D:\Programming\GitHub\RacerRevengePC\src\game\original_camera_start_authoring.cpp:98-101` | all hashes; cut count 3 matches TA/SC/TC | Helper accepts manifest rows. Frames/zones/offsets are read from the converted .tob/.vub (the original reads them). |
| Intro identity + events (tournament only) | `D:\Programming\GitHub\RacerRevengePC\src\assets\original_ta_intro_authoring_candidate.h:201-249`; 0 events refused at `D:\Programming\GitHub\RacerRevengePC\platform\linux\native\native_race_intro_transport50la.inl:89` and `D:\Programming\GitHub\RacerRevengePC\platform\linux\native\original_ta_intro_preload50la.h:21` | converted introAud.ini has 0 events | **Converter side:** ship the target course's original `audio\introAud.ini` and `audio\scrptLns.ini` byte-identical. They then pass their pins, but the archive SHA in the same row still needs the helper. |
| Shared race audio reads TA.RES | `D:\Programming\GitHub\RacerRevengePC\src\assets\audio_assets50hc.cpp:266-285` (TA.RES 3,334,562 + SHA, introAud.ini 3 cues) | only if TA is replaced | Do not override TA. |
| Material audio row | `D:\Programming\GitHub\RacerRevengePC\tests\native\material_audio_provider50hi_fixture.h:61-85` (archive SHA, `.aub` size+SHA, row/clip counts; SC `:75-76` aub 80,114 bytes, counts 32/12/2/25) | converted eb.aub (4,087 bytes, TA donor) | **Converter side:** ship the target's original `.aub` byte-identical. Only the archive SHA in the row then needs the helper. Surface-to-sound matching may be wrong (wrong sounds only). |
| Lighting / visibility owner | `D:\Programming\GitHub\RacerRevengePC\src\game\original_gc_gameplay_lighting_candidate.cpp:1095-1160` (`prepareScene`: expected SCB bytes, SHA, zones, portals per course; SC at `:1089`; called from `D:\Programming\GitHub\RacerRevengePC\tests\native\terrain_interactive_setup50du.cpp:2519`) | SCB bytes/SHA/zones differ | refused. Expected values come from the helper; the light list is read from the SCB. |
| Fog values typed per course | `terrain_interactive_publication50du.cpp:1456-1483` | ignores Etti IV's white fog | wrong (looks like the target's fog). Accept for a first silo run. |
| Texture budget per course | `D:\Programming\GitHub\RacerRevengePC\tests\native\terrain_interactive_continuation50du.cpp:2805, 2813-2815` (SC: 64 textures, 8 MiB; cold source 32 MiB) | 32 textures, about 1.3 MB of indexed PST, estimated about 4 MB RGBA | fits |
| Collision owner cap | `authored_runtime_ground50cp_fixture.h:42-43,74` | 36 COL | fits |
| Decode limits | per-course exact values, for example `D:\Programming\GitHub\RacerRevengePC\tests\native\sc_cold_sources50la.h:15-16,93-95` (2,974,353 bytes / 26,714,112 decoded / 984 entries, **checked with ==**) | 1,485,060 / about 4.1 MB / 470 | Change == to <= of the same caps for manifest files. All three fit. |
| Loading screen `UI\TEXTURES\LD_<XX>.PST` | `D:\Programming\GitHub\RacerRevengePC\tests\native\authored_loading_presenter50du.h:129-136` (287,772 bytes + hash from `D:\Programming\GitHub\RacerRevengePC\tests\native\original_loading_background_hashes50du.inc:2-18`) | LD_EB.PST is the right size, but its hash is unknown | **Keep the target's original LD_SC.PST** in the silo. It is a planet screen shared by SA/SB/SC anyway (`.inc:12-14`). Otherwise the presenter refuses it and the runner's `routeCompleted` needs `LD_<track>.PST` loaded. |
| Physics class from code | `D:\Programming\GitHub\RacerRevengePC\src\game\authored_parameters49n.cpp:106-118` | sc -> B (correct for Etti IV); ta -> A | none for SC |
| Global install manifests | `package-manifest.json`, `SHA256SUMS.txt` in the install | not read by the game (grep: no hits in src/tests/platform) | none |

### 1.2 SC-specific (recommended target): every check found

| file:line | Pin | Etti IV | Fix kind |
|---|---|---|---|
| `D:\Programming\GitHub\RacerRevengePC\tests\native\sc_cold_sources50la.h:91` | archive bytes + SHA | differs | helper |
| `sc_cold_sources50la.h:93-95` | decode limits, entries == 984, decoded == 26,714,112 | 470 / about 4.1 MB | cap instead of equality |
| `sc_cold_sources50la.h:19-25, 99-104` | 5 member pins (offset, size, SHA): sc.scb, msc.spl, asc.spl, sc.tob, sc.tun | scb/spl/tob differ; tun passes if shipped as the SC original | helper (by name, not offset) |
| `sc_cold_sources50la.h:107` | zones 30, portals 34, instances 4107 | 10 / about 10 / different | from data |
| `sc_cold_sources50la.h:109-114` | NumberOfSkyCubes == 0, no CubeFile, no .cub member | **passes** | none |
| `sc_cold_sources50la.h:16, 131` | map 179 / AI 180 nodes | 149 / AI count | from data (storage sized by the reader, cap 256) |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\sc_runtime_ground50la.h:37-38` | zones 30, instances 4107, map 179, AI 180 | differs | from data |
| `sc_runtime_ground50la.h:44-47` | every zone List/List collision method | data-dependent; converter must write List | converter |
| `sc_runtime_ground50la.h:51` | exactly 252 placed COL | 36 | from data (cap stays) |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\terrain_interactive_setup50du.cpp:1448` | map lap nodes == 179 | 149 | from data |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\sc_original_world50lm_cold.cpp:9` and `:172-176`; constants `D:\Programming\GitHub\RacerRevengePC\tests\native\sc_original_world50lm.h:26-48` (+ about 245 per-image hashes below) | archive SHA, SCB SHA, 578 scenes, 3855 placements, 252 COL-only, 4091 surfaces, 81 materials, 239,106 vertices, 411,768 indices, 2,760,960 RGBA bytes, 81 pinned images, 82 selections, 4095 pass spans | everything | **largest single change**: closure totals from data; images accepted by manifest hash |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\sc_world50lm.inl` (3 SHA sites) | member identities inside the visual loader | differ | helper |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\sc_trigger_authoring50la.h:74-86` | 9 triggers with fixed names, zones and targets | 0 triggers | trigger count from the TOB; 0 allowed (the original reads NumberOfTriggers) |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\sc_static_model_params_fixture.h:44-48`, applied at `terrain_interactive_continuation50du.cpp:2854-2868` | TOB SHA, 451 StaticModelParams | 0 | count from the TOB; 0 allowed |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\sc_lava_uv_fixture.h:73-98`, required at `terrain_interactive_publication50du.cpp:1052-1058` and readiness `:1356-1357` | lava UV (TOB SHA, 4095 spans, 82 selections, 8 rows) and SC ATM are mandatory | none | "absent -> no owner" (the original only animates what the data lists) |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\sc_atm_animation_fixture.h` | SC animated-texture source pins | no .atm | same as lava |
| `terrain_interactive_continuation50du.cpp:3008-3010` | `sc_zero_sky50KX` prepare | passes once the cold source passes | none |
| `original_track_cut_identity50la.h:61-65`, `original_ta_intro_authoring_candidate.h:229` | SC cut/intro rows (tournament) | tob/vub/archive differ; intro passes if the SC original ini files are shipped | helper |
| `material_audio_provider50hi_fixture.h:75-76` | SC aub row | passes if the SC original sc.aub is shipped (archive SHA via helper) | helper + converter |
| `original_gc_gameplay_lighting_candidate.cpp:1089-1160` | SC SCB pin | differs | helper + from data |

About 12 refusal stages and roughly 20 edit sites.

## 2. Which course to override

The physics class rule (`authored_parameters49n.cpp:106-118`): 'a' anywhere gives A, else 'b' gives
B, else C; then the exact codes `sb` -> C, `ra`/`sc` -> B. Class-B codes are TB, GB, MB, SC and RA. AI
class comes from tournament order (`D:\Programming\GitHub\RacerRevengePC\src\game\ai_cold_owners50iu.h:92-93`:
order < 5 A, < 9 B). That gives TB 5, MB 6, GB 7, SC 8 = AI class B; RA 4 = AI class A.

| Rank | Code | Refusal points | Why |
|---|---|---|---|
| 1 | **SC** | about 12 | class B / AI B; zero-sky route already expects what Etti IV has; 3 cuts match; no door, water or positional audio; TA.RES untouched. Blockers are count/hash pins plus 3 mandatory content owners (triggers, static params, lava UV/ATM). |
| 2 | MB | about 13 | class B / AI B; no door or positional audio. Needs `skies\mb.cub` (`D:\Programming\GitHub\RacerRevengePC\tests\native\sky50gw_fixture.h:98-102,160-165`), which Etti IV lacks; 3 LostAI triggers (`D:\Programming\GitHub\RacerRevengePC\tests\native\mb_trigger_authoring_fixture.h:15-71`), 112 static params, shield UV/ATM, navigation pins (`D:\Programming\GitHub\RacerRevengePC\tests\native\mb_navigation50mb.h:88-136`), 5 cuts vs 3. |
| 3 | TB | about 17 | 10 zones match, but navigation is a fixed `Navigation<270>/<269>` template (`D:\Programming\GitHub\RacerRevengePC\src\game\track_navigation_view50tb.h:54-68,93`), the door motion is mandatory (`D:\Programming\GitHub\RacerRevengePC\tests\native\tb_door_motion50tb.h:16-18`, called `terrain_interactive_world50du.cpp:545-555`), and it needs 16 triggers, 690 static params, 97 positional sounds (`D:\Programming\GitHub\RacerRevengePC\tests\native\tb_positional_sound50tb.h:16,84,105`), `tb.cub` and 5 cuts. |
| 4 | GB | about 18 | like TB, plus 18 triggers, mandatory particles (`D:\Programming\GitHub\RacerRevengePC\tests\native\gb_particle_authoring50gb.h:17,239-242`), and a sky loader pinned to literal fog bytes and `gb.cub` (`D:\Programming\GitHub\RacerRevengePC\tests\native\gb_sky_loader50gb.inl:25-32`). |
| 5 | RA | 25+ | 583 per-PSG hashes (`D:\Programming\GitHub\RacerRevengePC\tools\assets\original_roster\ra_compressed_scene_admission.h:788-798`), per-zone COL histogram, fog text at fixed byte offsets (`D:\Programming\GitHub\RacerRevengePC\tests\native\ra_track_fog50la.h:20-30`), 7 cuts, `ra.cub`, a UI catalog row (`D:\Programming\GitHub\RacerRevengePC\src\assets\original_ui_archive_catalog50kg.inc:44`). Also the recent RA regression runs end in exit 0xC0000409 (evidence `pl-129h-RA-o4-route2`). |
| - | TA (most verified) | about 25 | class **A** (wrong for Etti IV); TA.RES is read by every course's audio loader (`audio_assets50hc.cpp:268,275`). See 2.1. |

### 2.1 TA: hard-coded assumptions (why not TA)

| file:line | Assumption | Outcome |
|---|---|---|
| `track_session_assets50ig_fixture.h:75-88` -> `authored_runtime_ground50cp_fixture.h:76-78` | archive 3,334,562 bytes + SHA | **exit(1)**, the first check reached |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\ta_original_world50le_cold.cpp:9, 11-14` (constants `D:\Programming\GitHub\RacerRevengePC\tests\native\ta_original_world50le.h:34-35`) | size/SHA; 383 entries, 13,099,008 decoded (decode limit rejects anything above 383) | refused |
| `ta_original_world50le_cold.cpp:18-21` | `tracks\ta.scb` 130,511 bytes, SHA c3431a55... | refused |
| `ta_original_world50le.h:255-258` | 318 instances, 9 zones, 10 portals | refused |
| `ta_original_world50le_cold.cpp:35-47, 55, 172-177`; `ta_original_world50le.h:52-83, 179-183` | 1 pass/1 texture per PSM, only PreLit/Diffuse Compressed; 29 pinned images; closure 228 scenes / 284 placements / 34 COL-only / 353 surfaces / 110,196 vertices / 209,514 indices / 4,038,656 RGBA | refused |
| `original_standard_archive_provider50la.h:215-228` | encoded TA mount 533 blocks etc. | refused |
| `terrain_interactive_world50du.cpp:1114` | 37 terrain owners | refused |
| `original_gc_gameplay_lighting_candidate.cpp:578-605, 1129-1154` | TA SCB pin; NumberOfLights 26, Light1 directional in zone 1 | refused |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\material_alpha50gx_fixture.h:41-70` (used `terrain_interactive_publication50du.cpp:1562`) | TA blend table by PSM hash | refused |
| `sky50gw_fixture.h:122-126, 359-503` | `skies\ta.cub` 164,187 bytes + 48 tile hashes, fog "175,182,207" at SCB offset 9928 | refused (`terrain_interactive_continuation50du.cpp:3049`): Etti IV has no cube |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\original_ta_camera_issuer_candidate.h:36-50, 89-90` | ta.tob 13,887 bytes, ordinal 0, 383 members | refused (`publication50du.cpp:2098`) |
| `original_camera_start_authoring.cpp:74, 85, 98` | ta.tob SHA, 3 cuts | refused |
| `original_track_cut_identity50la.h:26-30`, `original_ta_cut_authoring_candidate.h:145-191`, `original_ta_intro_authoring_candidate.h:84-137, 201-204` | cuts and intro (tournament) | refused |
| `material_audio_provider50hi_fixture.h:61-62` | ta.aub 25,431 bytes | refused |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\native_ta_particle_cold50la_fixture.h:33-37, 76, 88-89, 112-119` | archive SHA; demoprt.prt/.psm SHA | refused (demoprt passes, since Etti IV's is a byte copy of TA's) |
| `audio_assets50hc.cpp:266-285` | TA.RES identity for **all** courses' audio | audio fails on every course |
| `terrain_interactive_session50du.cpp:184-187` | TA forces `tracks\ta.tun` in-archive | ok |
| `terrain_interactive_publication50du.cpp:1469-1483`; `D:\Programming\GitHub\RacerRevengePC\src\game\lap_config_ta50id.h:33-34` | TA fog values; TA lap records | wrong (cosmetic) |

## 3. Building

**How builds are made today.** No full build has been run since build70. Every exe since then
(129h is the newest, 2026-11-12 18:33) is an incremental relink:
`D:\Programming\GitHub\RacerRevengePC\build\relink-next.cmd <base> <out> [new.cpp=template.cpp] [res=...]`
calls VsDevCmd, `cd /d D:\Programming\GitHub\RacerRevengePC` (hard-coded, `:7`), then runs
`D:\Programming\GitHub\RacerRevengePC\build\relink-next.py`. That script:
- rebuilds every compile unit whose include closure changed against the base receipt;
- uses 4 workers (`:83`);
- reuses UTF-16 `.rsp` files from `build\takeover-*` directories, which hold absolute
  `D:\Programming\GitHub\RacerRevengePC\...` include and source paths (example
  `D:\Programming\GitHub\RacerRevengePC\build\takeover-d-baseline-70\*.rsp`).

**It does not work from a worktree.** The receipt's input paths and the `.rsp` files point at the main
tree, so a worktree relink would compile main's sources.

**Full build script:** `D:\Programming\GitHub\RacerRevengePC\platform\windows\native\Build-Interactive50DU.ps1`.
- Path-agnostic: it takes `-SourceRoot`.
- Parameters `:4-14`.
- Requires an already-open x64 developer shell (`:20`) and no CL/LINK variables (`:21`).
- Flags: `/std:c++20 /O2 /MD /fp:strict /bigobj`.
- Output: `<BuildDirectory>\rr_interactive_vehicle50du.exe` (`:287`) plus `build-receipt.json`. The
  directory must not exist yet (`:93`).

**Toolchain:**
- MSVC 14.44.35207 (`C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\cl.exe`
  / `link.exe`).
- VsDevCmd at `C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat -arch=x64 -host_arch=x64`.
- SDL2 headers at `D:\Programming\GitHub\RacerRevengePC\toolchains\sdl2-msvc\extracted\SDL2-2.32.10\include`.
- SDL2.dll at `...\lib\x64\SDL2.dll`.
- `toolchains\` is gitignored: point the worktree build at main's copy, read-only.
- FFmpeg headers, zlib and json are tracked in `third_party`.

**Size: a full build of 616 compile units in the working tree.** That is the 610 sources plus 6 zlib
in `D:\Programming\GitHub\RacerRevengePC\platform\windows\native\interactive50du.sources.json`; HEAD
has 609 + 6. Build70's full build was 614 units (`build\takeover-d-baseline-70\build-receipt.json`
`sourceCount`). For scale: the 156-unit renderer-header rebuild takes about 45 min (project memory),
so a full build is about 4x that work. The CMake files do not build the Windows exe
(`D:\Programming\GitHub\RacerRevengePC\CMakeLists.txt` = check projects; `platform\linux\native\CMakeLists.txt` = Linux).

**Two fixes are needed before a full build works (silo branch only):**
1. Gate `Build-Interactive50DU.ps1:75` throws unless the manifest has exactly 608 sources. HEAD has 609;
   the working tree has 610. Change the number.
2. **The manifest is missing 14 sources that the current exe links.** They were added through
   relinks (`new.cpp=template.cpp`), not through the manifest. In the 129h receipt but not in the
   manifest:
   - `platform\linux\native\original_ui_garage_kernel_shadow50kg.cpp`
   - `original_ui_unlock_draw_contribution50pd.cpp`, `original_ui_unlock_draw_resources50pd.cpp`,
     `original_ui_unlock_session50pd.cpp`
   - `original_ui_win_draw_contribution50pd.cpp`, `original_ui_win_draw_resources50pd.cpp`,
     `original_ui_win_session50pd.cpp`
   - `src\assets\original_ui_unlock_background50pd.cpp`
   - `src\game\original_ui_unlock_camera50pd.cpp`, `original_ui_unlock_models50pd.cpp`,
     `original_ui_unlock_page50pd.cpp`, `original_ui_unlock_widgets50pd.cpp`
   - `src\game\original_ui_win_page50pd.cpp`, `original_ui_win_scene50pd.cpp`

   Without them the link has unresolved symbols. Add them, which gives 623 + 6 = **629 units** for a
   worktree from HEAD. Also unchecked: `:77-80` verifies the hashes of the manifest's generator sources
   (`sourceListProvenance`) and may need reconciling.

**Base revision.** Make the worktree from commit `0407f64`, which is build129e and committed. The main
tree carries 24 modified and 4 untracked files that are not committed. Build129h includes some of them,
and the tree has changed again since then (for example `src\game\indexed_type2_contact50cc.cpp` differs
from 129h's input hash). Another session is working there: one builder per tree. The worktree is its
own tree, so its build does not touch main's. But do not take FPS numbers while either build is
running.

**Exact commands (silo; run once the user approves building):**
```powershell
git -C D:\Programming\GitHub\RacerRevengePC worktree add -b silo/etti-iv-sc D:\Programming\GitHub\RacerRevengePC\work\scratch\silo\tree 0407f64
# (silo-branch edits: gate number, 14 manifest rows, admission helper - section 5)
cmd /c 'call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 && set TEMP=D:\Programming\GitHub\RacerRevengePC\work\scratch\silo\temp&& set TMP=D:\Programming\GitHub\RacerRevengePC\work\scratch\silo\temp&& powershell -NoProfile -ExecutionPolicy Bypass -File D:\Programming\GitHub\RacerRevengePC\work\scratch\silo\tree\platform\windows\native\Build-Interactive50DU.ps1 -SourceRoot D:\Programming\GitHub\RacerRevengePC\work\scratch\silo\tree -BuildDirectory D:\Programming\GitHub\RacerRevengePC\work\scratch\silo\tree\build\silo-full-01 -Compiler "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\cl.exe" -Linker "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\link.exe" -SDL2IncludeDirectory D:\Programming\GitHub\RacerRevengePC\toolchains\sdl2-msvc\extracted\SDL2-2.32.10\include -SDL2RuntimeDll D:\Programming\GitHub\RacerRevengePC\toolchains\sdl2-msvc\extracted\SDL2-2.32.10\lib\x64\SDL2.dll -SourceRevision silo-0407f64 -MaxCompileJobs 4 > D:\Programming\GitHub\RacerRevengePC\work\scratch\silo\silo-full-01.log 2>&1'
```
The worktree sits under `work\scratch` because that path is gitignored in main (main's `git status`
stays clean) and the repo rule keeps all project files inside the repository. Later silo rebuilds can
use a copy of `relink-next.py` run from the worktree root against `silo-full-01`: its `.rsp` files
will then hold worktree paths. The `.cmd` wrapper's hard-coded `cd` must be changed in that copy.

## 4. Running a test (muted, unattended, logs only)

**Runner:** `<tree>\tools\capture\run_windows_takeover.py`. Run the worktree's copy: `ROOT` = its own
repo, and `--build`/`--output` must be inside it (`inside()`, `:89-93`). `--assets` and `--sdl` may be
anywhere (`:174`).

**What it always does:**
- Strips inherited `RR_*`/`SDL_*`/`MESA_*`/`LIBGL_*` variables (`:207-208`).
- Sets `RR_TEST_MUTE_OUTPUT=1`, `RR_EVIDENCE_LOGS_ONLY=1`, `RR_VIDEO_MAXIMIZED=0`,
  `RR_NATIVE_FIXED_WINDOW=1` and `RR_TRACK_FPS_BASELINE=1` (`:209-219`).
- Copies the exe to `<output>\RacerRevengePC.exe` and starts it as `--assets <assets>`, with
  cwd = repo root (`:203, 277, 295`).
- The exe sets `RR_ORIGINAL_ASSET_ROOT` from `--assets`
  (`D:\Programming\GitHub\RacerRevengePC\tests\native\terrain_interactive_drive50du.cpp:84-104`).
- `work/native/project/disc/<x>` resolves to `<assets>\<x>`
  (`D:\Programming\GitHub\RacerRevengePC\tests\native\runtime_asset_paths50lb.h:45-46`).

**Driving:** `--hold-throttle --steer-invert --camera-cycle`. Optional: `--race-full` (race until the
watchdog), `--repair`, `--unstick`, `--seconds` (10..1800).

**Course and pod:**
- The course comes from `--route` or, for the tournament route, from the seed save's career progress.
  `TOURNAMENT_ORDER` = TA SA MA GA RA TB MB GB SC MC GC RC TC (`:29`).
- SC = order 8: `--route main-tournament-course-ta --seed-save D:\Programming\GitHub\RacerRevengePC\work\scratch\saves\order8.bin`.
- The pod: `--player-racer N` (1-26 except 5, 6, 13; tournament route only, `:157-172`). Rotate pods
  across runs.
- `--route none --track SC` is a direct start. The session refuses it: SC requires a real
  Main/Course request (`terrain_interactive_session50du.cpp:346-347`).
- Tournament races load the intro and the cut cameras (section 1.1 rows).

**The installed launcher** `C:\Games\Star Wars Racer Revenge\Launch-RacerRevenge.ps1` also strips
`RR_*` variables. So a silo switch must not be an environment variable: neither the runner nor the
launcher would pass it through. Use a file in the asset root instead (section 5).

**How to read the result** (`<output>\RacerRevenge.log`; checked against `evidence\pl-129h-SC-3`):

| What | Log row |
|---|---|
| Race load started / loading screen | `Starting a fresh authored-grid session.` then `original_loading_assets background=LD_SC.PST layout=LD_ICON.LSI background_sha256=...` |
| Pods | `ORIGINAL RACER 0: <name> [PODnn]` |
| Countdown / green | `green_trace phase=1 race_s=... countdown=...` |
| Racing | `race phase=... seconds= player_place= ... racerN[name]=place,lap,...` and `finish_continuation ... sealed_laps=` |
| Finish / results | `finish_continuation player_finished=1`, `race_ack route=garage ... order=8`, `tournament_route course`, `results_device0` |
| FPS | `window_submit_fps <fps> interval_s 5.0 ...` every 5 s (SC 129h: 62.4-62.6, one 48.6 window at load); `track_fps_baseline {json}`, `frame_hitch total_ms=`, `swap_gap_ms=` |
| Admission failure | `require_failed`, `NATIVE_UNHANDLED`, `native_terminate last`, `frontend failure`; the message text names the check (for example "SC original world counts") |

`execution.json`: `routeCompleted` also needs a finished game summary. The recent chained regression
runs end with exit 3 and `windows_evidence_incomplete` even when races ran (the 129h SC-3 run raced
several sessions at 62.5 FPS). Judge from the log rows above, as
`D:\Programming\GitHub\RacerRevengePC\work\scratch\results-test.ps1` and
`D:\Programming\GitHub\RacerRevengePC\work\scratch\pod-load-all\pl-regress.ps1` do.

## 5. Smallest silo change that admits Etti IV as SC

### Converter side (scratch tools, no repo code)
1. Repack `export\a2r\eb` as code `sc`:
   - `TRACKS\SC.RES` with members `tracks\sc.scb`, `msc.spl`, `asc.spl`, `sc.tob`, `sc01..03.vub`,
     `scTVCam.pos`;
   - geometry/texture names unchanged.

   Physics class is then B through the exact `sc` rule.
2. Ship these as **byte copies of SC's originals**, so their pins pass without code changes:
   `tracks\sc.tun`, `tracks\sc.aub`, `audio\introAud.ini`, `audio\scrptLns.ini`, `particle\demoprt.*`.
   The intro then plays SC's announcer lines. Flag this as donor content in the manifest.
3. Write every zone with List/List collision search (`sc_runtime_ground50la.h:44-47`).
4. Manifest: add the archive SHA-256 and per-member SHA-256, plus a `facts` block: entries, decoded
   bytes, zones, portals, instances, map/AI nodes, placed COL, scenes, placements, COL-only, surfaces,
   materials, vertices, indices, RGBA bytes, images, triggers 0, static params 0, cuts 3, intro events.
   Today the manifest has only `bytes` and provenance per file. All values are computed from the
   files, which is what the port would read.
5. Keep the original `UI\TEXTURES\LD_SC.PST`.

### Port side (silo branch, one general mechanism, flagged as WP11 prototype)
- **New header** (for example `tests\native\converted_content_admission.h`). It reads
  `<RR_ORIGINAL_ASSET_ROOT>\converted-content.json` if present. File presence is the switch: no
  environment variable, so the runner and the launcher need no change. It answers two questions:
  - `accept(path, bytes, originalSha, originalSize)`: the original match **or** a manifest row with
    an equal SHA and size;
  - `fact(key, originalValue)`.

  Hooks never write game memory here: it only answers questions. Bounds stay the original caps.
- **Replace the inline identity compares** at:
  - `track_session_assets50ig_fixture.h` (row values via the helper);
  - `authored_runtime_ground50cp_fixture.h:76-78`;
  - `terrain_interactive_publication50du.cpp:802-805`;
  - `original_camera_assets50kx.h:139-141`;
  - `sc_cold_sources50la.h:91, 99-104`;
  - `sc_original_world50lm_cold.cpp:9` and `sc_world50lm.inl` (3);
  - `sc_static_model_params_fixture.h:44-45`;
  - `sc_lava_uv_fixture.h:73+`;
  - `material_audio_provider50hi_fixture.h:75`;
  - `original_track_cut_identity50la.h` consumer;
  - `original_ta_intro_authoring_candidate.h` consumer;
  - `original_gc_gameplay_lighting_candidate.cpp:1095+`.
- **Equality on counts becomes a data-derived value or a cap** at:
  - `sc_cold_sources50la.h:93-95, 107, 131`;
  - `sc_runtime_ground50la.h:37-38, 51`;
  - `terrain_interactive_setup50du.cpp:1448`;
  - `terrain_interactive_world50du.cpp:1114`;
  - `sc_original_world50lm_cold.cpp:172-176`.
- **Absent content becomes no owner** at:
  - `sc_trigger_authoring50la.h:77-86`;
  - `terrain_interactive_continuation50du.cpp:2854-2868`;
  - `terrain_interactive_publication50du.cpp:1052-1058, 1356-1357`.

  This is the original's behaviour: it creates what the TOB lists.

Total: about 20 existing files plus 1 new header. Fog stays SC's typed values (wrong look; accept for
the first run).

**Governing-rule note:** conversion-spec.md section 5 says loading Etti IV under an existing code is a
per-course substitution, which the rule forbids. In the silo, the *code* must still be the general
mechanism: one helper, counts from data, absent content allowed. It is applied first at SC's existing
stand-in sites, which are flagged. Done that way, the silo work is the start of WP11 and is not thrown
away. The SC-only files touched (`sc_*`, the `isScTrack` branches) remain stand-ins to retire.

## 6. Recipe

1. **Assets silo:** copy `C:\Games\Star Wars Racer Revenge` (2,327 files, 501 MB) to
   `D:\Programming\GitHub\RacerRevengePC\work\scratch\silo\assets`. Leave out `RacerRevengePC.exe` and
   `RacerRevenge.log`. Replace `TRACKS\SC.RES` with the repacked Etti IV and add `converted-content.json`.
   The real install and main stay untouched.
2. **Converter:** section 5, converter side.
3. **Worktree + edits:** worktree from `0407f64` (section 3 command), then the silo-branch edits:
   gate number, 14 manifest rows, helper + about 20 sites.
4. **Build:** the full build in section 3 (629 units). Repeats use a worktree-local relink.
5. **Smoke test on the original SC first** (no converted-content.json, original SC.RES). This proves
   the silo build matches main's behaviour:
   ```powershell
   cd D:\Programming\GitHub\RacerRevengePC\work\scratch\silo\tree
   & 'C:\Users\smmel\AppData\Local\Programs\Python\Python312\python.exe' -B tools/capture/run_windows_takeover.py --build build/silo-full-01 --output evidence/silo-sc-orig-01 --assets D:\Programming\GitHub\RacerRevengePC\work\scratch\silo\assets-orig --sdl D:\Programming\GitHub\RacerRevengePC\toolchains\sdl2-msvc\extracted\SDL2-2.32.10\lib\x64\SDL2.dll --logs-only --route main-tournament-course-ta --seed-save D:\Programming\GitHub\RacerRevengePC\work\scratch\saves\order8.bin --player-racer 3 --seconds 600 --hold-throttle --steer-invert --camera-cycle --race-full
   ```
   Either use a second asset copy with the original SC.RES (`assets-orig`), or point `--assets` at
   `C:\Games\Star Wars Racer Revenge`, which is read-only for the game.
6. **Etti IV run:** the same command with `--assets D:\Programming\GitHub\RacerRevengePC\work\scratch\silo\assets`
   and `--output evidence/silo-eb-sc-01`. Rotate `--player-racer` over later runs. Pass criteria:
   - `LD_SC.PST` loading row;
   - `green_trace` and `race phase` rows with lap counts rising;
   - `window_submit_fps` near 62.5;
   - no `require_failed` or `NATIVE_UNHANDLED`;
   - `finish_continuation player_finished=1` with `--race-full`.
7. **Expected first failures, in load order:**
   1. `SC original archive identity` (helper wiring);
   2. world counts;
   3. ground COL count;
   4. visual closure;
   5. triggers;
   6. lava UV / ATM;
   7. static params;
   8. lighting SCB;
   9. cut identity.

   Each failure names its check, so the site list in 1.2 doubles as the debugging order.
