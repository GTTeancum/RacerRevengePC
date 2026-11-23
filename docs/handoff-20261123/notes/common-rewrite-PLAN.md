# Common rewrite: one race path for every course and every pod

Written 2026-11-20 against main **b607b29**. This is a plan only. Nothing in src, tests, platform
or tools was edited, built or run, and the game was not launched. Governing rule:
`D:\Programming\GitHub\RacerRevengePC\CLAUDE.md`.

This plan replaces the step-by-step sequences in:

* `D:\Programming\GitHub\RacerRevengePC\work\scratch\branch-inventory\PLAN.md`: 9 header rounds,
  each followed by a full test.
* `D:\Programming\GitHub\RacerRevengePC\work\scratch\common-race\PLAN.md`: steps CR0..F, about 11
  header rounds.

It replaces them with **one rewrite, one rebuild and one verification round**. The per-role design
choices in common-race PLAN section 3 ("Keep" / "Retire") still hold where this file does not
override them.

Inputs read for this plan:

* `D:\Programming\GitHub\RacerRevengePC\work\scratch\branch-inventory\PLAN.md`
* `D:\Programming\GitHub\RacerRevengePC\work\scratch\common-race\PLAN.md`
* `D:\Programming\GitHub\RacerRevengePC\work\scratch\pod-load-all\PLAN.md` and its `QUEUE.txt`
* `D:\Programming\GitHub\RacerRevengePC\work\scratch\146c\COMMON-ROUTE-PLAN.md`
* `D:\Programming\GitHub\RacerRevengePC\work\scratch\148r\results-tt-ra-design.txt` and
  `D:\Programming\GitHub\RacerRevengePC\work\scratch\148r\item2-pause-plan.txt`
* `D:\Programming\GitHub\RacerRevengePC\work\scratch\149p\item2-notes.txt`
* `D:\Programming\GitHub\RacerRevengePC\docs\HANDOFF-20261106.md`
* git log 2026-11-12..b607b29: 96 commits

Translation-unit (TU) counts come from `D:\Programming\GitHub\RacerRevengePC\work\scratch\ra-route2\fanout.py`
run against the current 636-TU Windows manifest
`D:\Programming\GitHub\RacerRevengePC\platform\windows\native\interactive50du.sources.json`.

---

## 0. Short version

* **Stand-ins remaining: 29 families.**
  * 20 of the 21 course families from 2026-11-12. B-C19 (shadows) is done; B-C10 is mostly done
    but stays listed, because its count tables remain.
  * 4 of the 5 pod families. B-P01 is done apart from two leftovers.
  * 5 race-flow stand-ins added by the Time Trial, Pause and DLC work since then (F01..F05).
  * The course split still runs through **1 165 uses of 12 `isXxTrack` flags** in 23 files.
    `isSaTrack` is gone: SA and every add-on course already share one "common route" (`commonRouteTrack`).
* **Target:** one race (1A65C0) with one load in the original 1A6AA0 order; one race world (1B2870
  at race+74); one track load in the original 1A9EE0 order, whose steps are the shared data
  readers; one camera session; one lighting, sky, fog and audio prepare; one lap tracker; one race
  start used by a fresh race and by every restart (1AC8E0); one end-of-race dispatch
  (1A7DD0 → 1AB520 → 1ABBC0 / 1AB5BC); one roster. Courses and pods supply only files and registry
  rows.
* **Size of the rewrite:**
  * About 120 existing files edited and about 25 new files.
  * About 330 per-course files deleted or folded, out of the 402 course-tagged files (about
    80 000 lines). The rest are shared kernels with course-looking names; they are renamed only
    where they are already touched.
  * The 12 monolith `terrain_interactive_*50du.cpp` files hold 23 800 lines and are rewired.
* **One rebuild:**
  * The header union already measured is **228 of 636 TUs**. It includes the renderer header
    (160 TUs) and the lighting header (162 TUs).
  * With new headers and new .cpp files, expect **about 240-280 TUs, about 70-80 minutes** at the
    measured ~17 s per TU.
  * If the final diff reaches more than about 450 TUs, do one full rebaseline (about 630 TUs,
    about 3 hours) instead. It is still one rebuild.
* **Split:** 16 writers, no two touching the same file.
  * W0 freezes every interface and shared header first.
  * 11 role writers (R1-R11) and 4 integration writers (I1-I4) then work in parallel.
  * Merge, a grep gate, a host data check, then the one build.
* **One verification round: 17 test runs, about 63 minutes. No baseline round.**
  * The main per-course proof is the no-game receipt table and host check. Behaviour is compared
    against the existing recorded logs under `D:\Programming\GitHub\RacerRevengePC\evidence`.
  * 14 one-lap Single Race runs (13 stock courses plus Vengeance), with rotated pods. Each covers
    the start, an in-place Pause RESTART, the finish, Results, and then the Hall or the Course screen.
  * 2 Time Trial runs: GC with the records page, and Vengeance with a Course QUIT.
  * 1 tournament run on RA.
  * The first 3 runs are the stop-or-go check.
  * 14 one-minute picture grabs are taken outside the tests (about 14 minutes).
  * Total: 31 launches, about 77 minutes.
* **Decisions for the user:** two (section 6).

---

## 1. Inventory, verified against main b607b29

Verification method: five read-only sweeps over the tree at b607b29 (`git ls-files` only;
`.claude\worktrees\*` copies ignored).

Class definitions:

* **A**: the original branches here too, on the same data. Keep.
* **B**: a port stand-in. Retire.
* **DONE**: retired since 2026-11-12.

Writer IDs (R1..R11, I1..I4) refer to section 3.1.

### 1.1 Class A (keep)

| # | Site now | What | Original |
|---|---|---|---|
| A1 | `D:\Programming\GitHub\RacerRevengePC\tests\native\native_racer_aero_engine21495450la.inl:4-62`, `D:\Programming\GitHub\RacerRevengePC\tests\native\authored_parameters49n.cpp:32`, RA class-B note `D:\Programming\GitHub\RacerRevengePC\tests\native\ra_cold_roster_operation50la.h:274` | PHY class taken from the track's two-letter code, with the sb/ra/sc exceptions; logs `course_route_phy ... site=214954` | 214954..214AA0 compares the track string |
| A2 | `D:\Programming\GitHub\RacerRevengePC\src\game\ai_driving50iv.h` (racers 4, 8, 18, 23 gain ×1.25) | AI steering boost | 1730D0..1730FC |
| A3 | `D:\Programming\GitHub\RacerRevengePC\tests\native\original_race_hud_scope50la.cpp:603`, `D:\Programming\GitHub\RacerRevengePC\src\game\lap_event6_subscriber50id.h`, `D:\Programming\GitHub\RacerRevengePC\src\game\pod_speech50hg.cpp` | Announcer variant for racer 16 | 1A5650/1A5670, 1A5BAC/1A5BC4, 1A5368/1A5380, 1A5914 |
| A4 | `D:\Programming\GitHub\RacerRevengePC\src\assets\original_track_cut_identity50la.h:26` (13-row size+SHA table), `D:\Programming\GitHub\RacerRevengePC\src\game\original_camera_start_authoring.cpp:76-88` | File selection by course code, plus a port integrity check | `TRACKS\XX.RES` is built from the code. The table stays as receipts only, never as a source of counts |
| A5 | `D:\Programming\GitHub\RacerRevengePC\tests\native\original_player_identity50ik_fixture.h` (find, 23 pods), `D:\Programming\GitHub\RacerRevengePC\src\game\original_racer_selection50ij.h:22-62` | Pod file and name table | 21081C, 210820/24 `Pod%02d` |
| A6 | Garage `26_3` DefaultMT rule (branch-inventory A6) | Already data-driven | 2475C0 |
| A7 | Human and AI splits: human manager tail, limp, pad vibration | The original branches on human or AI | 1AE120, 1AF770 |
| A8 | `D:\Programming\GitHub\RacerRevengePC\src\game\original_racer_entity_cold50la.h` (pod id 0 or ≥100 refused) | Bounds check | 210824 |
| A9 | Water present or absent | The original branches on whether the track's data names water | 274570 (no water) / 2745D4 (water factory); per racer 22C21C/210804 |
| A10 | Launch mode 11258, P+34 (tournament or free play), race1B0, human count 34 | Mode branches in the end dispatch, Pause and cuts | 1A7DD0, 1AB55C, 1ABBC0, 1BDA74, 174EA8 |
| A11 | N64 ice, slow snow, zero gravity, magnet and boost strips | Port extensions keyed on terrain-zone data, inert on stock | Not original. Allowed because they are data-keyed and flagged |
| A12 | DLC add-on course or pod rows numbered from 32 | Port extension; course and pod facts come from registry rows | Not original. The rewrite must make add-ons pure data |

Class C, still open:

* C1: portraits for careers 5, 6 and 13 (probably A, because the files are absent on disc).

### 1.2 DONE since 2026-11-12 (no action)

* **B-C19 pod shadows.** One projected-shadow path for every course:
  `D:\Programming\GitHub\RacerRevengePC\tests\native\terrain_interactive_publication50du.cpp:1395-1408`
  (TOB 271A00, portal walk 2DEBC0/2DE7C0).
* **B-C03, mostly.** The 11 `RR_NATIVE_xx_SELECTED_LIGHTS` switches became one
  `RR_NATIVE_SELECTED_LIGHTS` (`D:\Programming\GitHub\RacerRevengePC\tests\native\terrain_interactive_session50du.cpp:256-263`).
  `RR_GA_ORIGINAL_AI_CANDIDATE` and `RR_RA_OWNED_PREPARATION` are gone.
* **B-C08, loop part.** No `isGaTrack` is left in `terrain_interactive_loop50du.cpp`, and the
  GA-only window title is gone.
* **B-C07, the continuation term at :1295.**
* **B-C10, most of it.**
  * The four 13-course allowlists now call one `originalRaceTrackIdAdmitted50LA` lookup
    (`D:\Programming\GitHub\RacerRevengePC\src\assets\original_track_cut_identity50la.h:174`), in
    `original_ta_cut_authoring_candidate.h:143-150`, `original_ta_intro_preload50la.h:21`,
    `race_audio_groups_ta_intro_cold_candidate.inl:55` and `native_race_intro_transport50la.inl:101`.
  * A non-stock TOB's cut count is decoded from `[CSCameras].NumberOfCameras`
    (`D:\Programming\GitHub\RacerRevengePC\src\game\original_camera_start_authoring.cpp:101-123`, 174DF0).
* **B-C06, partly.** These now apply to every counted race:
  * the RA-only end clock;
  * the F8 native bars and F8 renderer;
  * the Results update schedule and Results human root;
  * the Camera34 bind;
  * the RA-only per-frame F8 (`terrain_interactive_loop50du.cpp:3597-3622`);
  * the RA-only native racer manager (now built for Time Trial on every course,
    `terrain_interactive_session50du.cpp:427-436`);
  * the original Pause owner, via `attachCountedRace` on every counted race
    (`terrain_interactive_publication50du.cpp:2372-2395`, commits 247b864 and b607b29).
* **B-C13, partly.** One track-animation player on all 13 courses (101860/101390/101170/1011A0/101DA0).
  One texture-scroll owner, `track_uv_animation50uv_fixture.h` (29B1D0/29B050), replaced every
  `original_*_shield_uv.h` and the SC lava UV.
* **B-C15, partly.** One static-world texture decoder (`course_world_texture50st.h`) and one
  texture budget rule (382B38/298900/2993A8/29E340/29C490).
* **B-C17, partly.** The per-track material-audio catalog readers are gone; one catalog with
  receipts (`material_audio_provider50hi_fixture.h:124-160`).
* **B-P01 per-pod rig admissions.** POD19 an2, POD02 bottom engines, the POD09/12/19/20/22/25 VUT
  skips, the POD18 prefix, the POD20 hands and the POD02 visual are all retired onto the original
  rules (1E7C74 table, 211470, 153238..15325C, 211A54..211B3C, `componentCount==5`). Two leftovers
  are listed under B-P01 below.
* **B-P02, partly.** The Watto `==19` term is gone (`live_audio50hf_fixture.h:472-474`). The speech
  counts are now receipts; pods are admitted from `sounds.ini [Indices]`.
* **B-P03 gate.** `continueSourcePod1AE0A4` now mounts any pod
  (`D:\Programming\GitHub\RacerRevengePC\tests\native\original_racer_manager_entity50la.h:130-149`),
  but it is still not called (see B-P03).
* **B-P04 main switch.** `playerUsesRosterRig=true` (`terrain_interactive_session50du.cpp:363`).
* **Time Trial counted race on every course** (ccdcca9, 0c0541c, a62ebcb) and **Free Play Single
  Race to the original Results** (b0f776a..2b40b66). No new course keys were added.

### 1.3 Class B: course families still open

Writer IDs refer to section 3.1. "Original" gives the function the common path follows.

#### B-C01 Track flags and their plumbing (writers I1-I4; W0 for the context headers)

* **Where:** `D:\Programming\GitHub\RacerRevengePC\tests\native\terrain_interactive_session50du.cpp:151-174`
  defines 12 `isXxTrack = codeEquals(...)` flags plus `commonRouteTrack` (SA or registry, :163)
  and `maOrCommonRouteTrack` (:175). There is a "PORT STAND-IN" comment at :158.
* **Uses:** 1 165 in 23 files.

  | File (`D:\Programming\GitHub\RacerRevengePC\tests\native\`) | Uses |
  |---|---|
  | `terrain_interactive_continuation50du.cpp` | 278 |
  | `terrain_interactive_setup50du.cpp` | 223 |
  | `terrain_interactive_world50du.cpp` | 193 |
  | `terrain_interactive_publication50du.cpp` | 165 |
  | `terrain_interactive_session50du.cpp` | 101 |
  | `terrain_interactive_frame_children50du.cpp` | 42 |
  | `terrain_interactive_loop50du.cpp` | 32 |
  | `terrain_interactive_cold_owners50du.cpp` | 25 |
  | `terrain_interactive_frame_physics50du.cpp` | 14 |
  | 8 context headers `terrain_interactive_{world,setup,publication,continuation,loop,cold_owners,frame,cold_contacts}50du.h` | 5-12 each |
  | `terrain_interactive_cold_contacts50du.cpp`, `native_racer_owners50hn.inc`, `gb_/mc_application_cold*.inl`, `*_prefix_retirement*.inl` | the rest |

* **Original:** none. The course code is read only to build file names and in A1.
* **Replace by:** nothing of its own. The flags go when every reader below reads track data. The
  grep gate (3.5) must find zero.

#### B-C02 Per-course admission and route gates (I1)

* **Sites:**
  * `terrain_interactive_session50du.cpp`:
    * :180-206: 13 `xxOriginalDisc` feeding `directOriginalHuman`.
    * :209-213: `taOriginalDisc` forces `tracks\ta.tun` and `sourceStaticCandidates`.
    * :218-219: `courseRaceCold`, a 12-course OR.
    * :312: `humanCourseEnabled` needs `isTaTrack||directOriginalHuman`.
    * :327-352: per-course `require50DU` admissions.
    * :386-397: the GC incomplete-world block.
    * :640 and :670: TB and GB fleet requirements.
  * `D:\Programming\GitHub\RacerRevengePC\tests\native\terrain_interactive_drive50du.cpp`:
    * :217-234: 11 `initialXx`.
    * :235-241: 7 `xxNativeWorldRequested`.
    * :242 and :253-259: the admission list and the course→HostTrack chain.
    * :348-360: the per-track OR.
  * `D:\Programming\GitHub\RacerRevengePC\platform\linux\native\host_track_selection50im.h`: the
    MA/SA/GA mode2 and GC expressions.
  * `D:\Programming\GitHub\RacerRevengePC\tests\native\original_course_race_targets50kx.h`:
    13 targets plus `commonRouteTarget()` at :72.
* **Original:** a race launches from the Course screen for any course (17C16C / 17D678 → 1A65C0).
  There is no admission list.
* **Replace by:** one rule for every course: the course archive resolves (stock disc or `dlc:/`
  overlay), its integrity receipt is logged, and the launch is a rooted Course entry. HostTrack
  becomes "a course code" plus "stock or registry".

#### B-C03 Course-named switches still present (I1, R8)

* **Sites:**
  * `RR_GC_HOST_TRIGGERS` (`terrain_interactive_session50du.cpp:228`).
  * `RR_GC_NATIVE_GATE` (session :231, drive :285).
  * `RR_GC_NATIVE_WATER` (session :373, drive :285).
  * `RR_GC_INCOMPLETE_WORLD` (drive :273).
  * `RR_RA_NATIVE_WORLD` (drive :244).
  * `RR_RA_PIXEL_PROVENANCE` (`D:\Programming\GitHub\RacerRevengePC\platform\linux\native\vehicle_window_gl50ds_world.cpp:1614`).
  * The 13 `nativeXxSelection` lighting aliases (session :266-281).
  * `RR_AUTHORED_WORLD_FOG` forced on for GB/MC only (`terrain_interactive_publication50du.cpp:1645`).
* **Original:** none.
* **Replace by:** delete. Keep at most one course-independent diagnostic per feature
  (`RR_NATIVE_WATER`, `RR_NATIVE_TRIGGERS`, `RR_PIXEL_PROVENANCE`), default off.

#### B-C04 Thirteen consumed race stages (R1)

* **Sites:**
  * Stage headers: `D:\Programming\GitHub\RacerRevengePC\tests\native\original_consumed_{ra,ta,ga,gc,sa,ma,mb,sc,gb,mc,tb,rc,tc}_stage50du.h`
    (54-58 lines each; 7 TUs each, RA 9). The SA copy is already the "common route" stage
    (namespace `original_consumed_common_route_stage50DU`, accepts `HostTrack50IM::SA` or `::Course`).
  * Owners: `terrain_interactive_world50du.cpp:278-290`.
  * Begins: world :844 (TA), 868 (GC), 890 (MA), 909 (MB), 932 (SC), 951 (GB), 980 (RC), 1002 (TC),
    1021 (MC), 1044 (TB), 1146 (common route), 1169 (GA); RA in
    `D:\Programming\GitHub\RacerRevengePC\tests\native\ra_application_cold50la.cpp:81`.
  * Closes: `terrain_interactive_continuation50du.cpp:3876-3894`.
  * "Which stage has a race" chains:
    * `D:\Programming\GitHub\RacerRevengePC\tests\native\native_common_race50pd.h:19-31` (`race()`)
      and :35-49 (`course()`);
    * continuation :345, :354, :2496-2498, :3373-3384;
    * `original_race_hud_scope50la.cpp:121-122`;
    * `terrain_interactive_setup50du.cpp:1069` (10 of the 13);
    * publication :902, :952, :2249.
* **Original:** one PodRaceEvent 1A65C0 (vtable 3CEE80), then 1A6AA0 / 1AA5A0 / 1A6B5C / 1A6BC0 for
  every course.
* **Replace by:** one `original_consumed_stage50DU::Owner` holding one race, fed by the launch's
  course code.

#### B-C05 Race world at race+74 (R1)

* **Members:** `D:\Programming\GitHub\RacerRevengePC\tests\native\original_race_constructor50la.h`
  (44 TUs):
  * `world74_` (RA) :323;
  * `taWorld74_` :354, gc :367, ma :379, mb :389, sc :399, gb :409, mc :419, tb :429, rc :439, tc :449;
  * `commonRouteWorld74_` :461, ga :473;
  * `anyWorld()` :615.
* **Per-course world-prefix headers:** 12 of them,
  `D:\Programming\GitHub\RacerRevengePC\tests\native\original_{ga,gb,gc,ma,mb,mc,rc,sa,sc,ta,tb,tc}_world_cold_prefix50la.h`
  (15 TUs each). Each one has `constructXxWorld1A6BC0`, `retireNativeXxWorldPrefix` and
  `constructConsumedXxRaceWorld1A6BC0`. They differ only in member names, type names and the order
  of the exclusion list.
* **RA's version:** `Application::constructConsumedRaceWorld1A6BC0` in
  `D:\Programming\GitHub\RacerRevengePC\tests\native\original_race_world_join50la.h:284`.
* **Readers:**
  * `D:\Programming\GitHub\RacerRevengePC\tests\native\original_race_camera_owner50la.cpp:34-45, 54-67`
    (`raceCameraCourse34`);
  * `original_race_world_join50la.h:23-66` (per-course exclusion lists);
  * `original_results_human_root50la.cpp:45`;
  * `original_results_feedback_scope50la.cpp:46`;
  * `original_race_hud_scope50la.cpp:53, 193`;
  * continuation :354, :4768;
  * `original_race_world_tob_prefix50la.cpp` (35 hits), `original_race_world_load_prefix50la.cpp` (19),
    `original_race_native_retirement50la.h` (17).
  * About 50 files in all.
* **Original:** race+74 is one World 1B2870 (0x550 bytes). Its prefix runs 1A6BC0, 141300, 26FCF8 and
  1B2918; retirement runs 1B2870, 235150 and 1B5F64.
* **Replace by:** one member `world74_` of one type (the common race world, 2.2) and one prefix,
  `D:\Programming\GitHub\RacerRevengePC\tests\native\original_world_cold_prefix50la.h` (it already
  exists and is shared).

#### B-C06 RA-only race features (R1, R2, R9)

Still RA-only:

| Site | Gate | Original | Writer |
|---|---|---|---|
| `D:\Programming\GitHub\RacerRevengePC\tests\native\original_race_f8_layout50la.cpp:113, 129, 142` | 1994B0 F8 layout needs `world74_` and RA (no caller) | 1994B0 for every race | R1 |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\original_session_f8_intro50la.cpp:19, 41-44` | Real F8 reveal 199770 only on RA | 199770 for every race with an intro | R2 |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\original_race_world_load_prefix50la.cpp:296` | 1A9EE0 native prefix only for RA | 1A9EE0 for every course (2.2) | R3 |
| `D:\Programming\GitHub\RacerRevengePC\src\game\original_race_music_manager50la.cpp:978, 985` | 1A4440 listener and 10C completion only on RA | 1A4440 → 1030B0 every race | R1 |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\ra_source_camera_ownership50la.h:1729` | `currentIntroController174A30` needs RA issuance | 1A95B0 → 174A30 every race | R2 |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\terrain_interactive_session50du.cpp:747-752` | Pause activation = RA + all-AI; intro activation = RA + human + game34==2 + 11258==1 | 1A8A40 builds Pause100/Results104/F8 every race; cuts by 174EA8 (subtype, P+34, App+350) | I1 |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\terrain_interactive_continuation50du.cpp:2222` | `nativeHumanAudio` only with the RA stage | Race audio for every human race | I3 |
| continuation :2556-2569 (definitions :4678-4760) | B0/B4 native racer init needs RA and `world74_` | 1AA548 B0, 1A7710 B4 for every race | I3 + R10 |

RA's own route. These are by design inside the RA route, and retire when RA joins the common path:

| Site | What | Writer |
|---|---|---|
| `D:\Programming\GitHub\RacerRevengePC\tests\native\original_camera_session50kx.h:842`, :2695-2744 | `raceMode8C_` only bound on RA; direct RA checks in the end route (the live caller already uses `canonicalCourseEndRoute()`) | R2 |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\original_camera_unused_children50la.inl:56, 99, 122, 166` | Start7C, Cinematographer70, Tv78 and TvBody built only for RA issuance | R2 |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\terrain_interactive_cold_owners50du.cpp:168-226` | RA cold block (`humanResultsCold` :190) | I2 |
| `terrain_interactive_publication50du.cpp:2331-2367`, :2471-2483 | ALL8 spectator music/Pause draw/`bindRace`; RA Results root only with intro activation | I3 |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\original_pause_observation50la.h:467-491` (track 7, order 4, pod 9, class A at :489-490) | ALL8 spectator identity | R9 |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\original_pause_input50la.inl:38, 124, 140` | ALL8 Pause return (`consumedRaAll8*`, `world74_`) | R9 |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\ra_application_cold50la.cpp:70-88` | `beginConsumedRaPrefix` only with Pause or intro activation | R1 |
| `D:\Programming\GitHub\RacerRevengePC\tests\native\ra_source_camera_factory50la.h:122-222` | RA builds its own Session; `constructCurrentRaIntro174FA4` needs 7 cuts, configuration34==2 and launch 1; no zero-cut 174A30 | R2 |

**Effect today:**

* RA Free Play and RA Time Trial get no counted race and end on the host text panel
  (`terrain_interactive_loop50du.cpp:1966-1988`).
* RA's tournament uses its own route.
* The ALL8 spectator race exists only on RA.

**Original:** nothing in the race or end flow reads the course. RA Free Play is the same counted
race with zero cuts (174E90), with Results104 at the finish (1A7DD0) and the 1AB828 rows
(148r design §3).

#### B-C07 TA/RA/Course specials (I1, I3)

* **Sites:**
  * `terrain_interactive_continuation50du.cpp:360`: `humanEndRace94Strict` requires RA or TA.
  * `terrain_interactive_drive50du.cpp:316-320, 365`: on Restart, the consumed launch is renewed
    only for TA or a registry Course. The DLC commit widened this, but it is still course-keyed.
  * `terrain_interactive_publication50du.cpp:709`: `isTaTrack||commonRouteTrack||isRaTrack||directOriginalHuman`.
  * Session :209-213: TA tuning in the archive.
* **Original:** the same end path and restart path on every course (1AC8E0; 1AEA70 → 228110).
* **Replace by:** drop the course term. The restart renewal disappears entirely, because the
  restart becomes in place (2.6).

#### B-C08 GA-only gates (I3)

* **Sites:** `terrain_interactive_publication50du.cpp`:
  * :1295: GA ATM owner `isGaTrack&&worldAuthorsAnimatedTextures50DU`.
  * :1541: readiness.
  * :2062: log.
  * :2674 and :2679: receipt fields `loaded_track_incomplete` and `results_visible`.
* **Original:** none.
* **Replace by:** the data term only.

#### B-C09 Twelve camera-session factories (R2)

* **Factories:** `D:\Programming\GitHub\RacerRevengePC\tests\native\original_ta_camera_issuer_candidate.h`:
  Ta :67, Gc :123, Ga :187, CommonRoute :251, Ma :321, Mb :385, Sc :450, Gb :514, Tb :581, Mc :646,
  Rc :713, Tc :778.
* **Declared:** `D:\Programming\GitHub\RacerRevengePC\tests\native\original_ui_main_entry50kg.h:1085-1126`
  (108 TUs).
* **Chosen:** `terrain_interactive_publication50du.cpp:2250-2263`.
* **Other sites:**
  * Per-course assets: `D:\Programming\GitHub\RacerRevengePC\tests\native\original_camera_assets50kx.h:36, 66, 96, 130-147`
    (`loadRc/Tb/Gb`, course pick); calls at publication :2129-2149 and continuation :3320, :3335.
  * MC: the 173-node MC map at `original_camera_session50kx.h:1903-1909`.
  * `NativeTaIssuance` per-course `xxRace/xxWorld/xxMap` triples at :467-523.
  * `D:\Programming\GitHub\RacerRevengePC\tests\native\original_ta_camera_lifecycle.inl`: 21
    `OriginalCameraStartCourse::` uses.
* **Zero-cut controller:** `original_ta_camera_lifecycle.inl:42` (174A30, 174E48..174EC4).
  It is issued only through publication :2283; RA cannot get it.
* **Original:** 1A6DC0 → 1A95B0 → 174DF0/174E58 (cut count from World+3E4 =
  `[CSCameras].NumberOfCameras`) → 174A30, plus 175510 cut resources, for every course.
* **Replace by:** one `createNativeCourseCameraSession50LA(race, world74, tob, map view)` built from
  the GC factory (the one with full checks; common-race 3.3). RA uses it too.

#### B-C10 Remaining camera and cut hard-codes (R2)

* **Sites:**
  * `D:\Programming\GitHub\RacerRevengePC\src\game\original_camera_start_authoring.cpp:97-100`:
    the stock cut-count literal (TA/SC/TC 3; GC/MA/GB/MC 4; GA/SA/RC/TB/MB 5; RA 7).
  * The 13-row runtime table plus a registry slot in `original_track_cut_identity50la.h:118-129`.
  * The 13 `specifications_` in `D:\Programming\GitHub\RacerRevengePC\src\assets\original_ta_intro_authoring_candidate.h:309-341`.
* **Original:** 174DF0 / 1B28EC read the count from the TOB.
* **Replace by:** the TOB decode at :101-123 for every course, stock included. The literal becomes
  a receipt. The intro specs come from the archive INIs (registry path already does this, :324).

#### B-C11 World, collision, ground, static candidates, storage (R3)

* **Sites in `terrain_interactive_world50du.cpp`:**
  * Cold sources per course: RA :429/:480, TB :532, RC :603, TC :655, SC :697, MB :717; MC 48 MiB
    :474/:499.
  * Legacy-ground exclusion lists: :770, :1221, :1275, :1636, :1672; ground chooser :784-788.
  * RA's own height and pose: :1180, :1189, :1202-1203.
  * MB candidates: :1244, :1269.
  * Course chains for candidates, bindings and storage: :1637, :1653, :1675.
  * Payload ledgers: :1537-1633.
  * Member bytes and AI tuning: :1748-1787, :1803-1808.
* **Elsewhere:**
  * `terrain_interactive_setup50du.cpp:660-790`: per-course collision closers.
  * `terrain_interactive_cold_owners50du.cpp:115-146`: trigger memberships; :522-528: seven lift binds.
  * `terrain_interactive_frame_physics50du.cpp:169, 183, 239, 255, 301, 339`: per-course query wrappers.
* **Per-course types:**
  * `D:\Programming\GitHub\RacerRevengePC\tests\native\{gb,mb,mc,rc,tb,tc}_collision_world50xx.h` and
    their `_lifetime.cpp` (12 files);
  * `{gb,mc,rc,sc,tb,tc}_runtime_ground50la.h`;
  * `{gb,mc,ra,rc,sc,tb,tc}_cold_sources50la.h`;
  * 33 `*_original_world50l{e,h,m}*` files;
  * `{gb,mc}_application_cold50xx.*` and `*_prefix_retirement*.inl`.
* **Original:** World 1B2870; physics index 270CDC/27C110; collision zones 271230/27BB40; one zone
  world from the SCB.
* **Replace by:** the common track load (2.2). Construction follows the generalised RA chain;
  runtime follows the MC/GB aggregate shape (one `Storage` / `LiveZoneWorld50EN` with a lease);
  every count comes from the track's files. One lift bind, one query bracket and one closer.

#### B-C12 Navigation, AI map, lap progress, race session, recovery (R4)

* **Navigation views:**
  * `D:\Programming\GitHub\RacerRevengePC\src\game\track_navigation_view50{gb,mc,tb,tc}.h` (94-96 TUs
    each);
  * `track_navigation_view50mb.h` (102 TUs; shared store of 293 nodes and 256 portals);
  * the legacy `track_navigation50eu.*`;
  * `track_navigation50{gb,mc,tb,tc}.cpp`;
  * `D:\Programming\GitHub\RacerRevengePC\src\game\rc_navigation_lease50rc.h`.
* **AI inputs:** `AiRuntimeInput50{IV,MB,TC,TB,GB,MC}`
  (`D:\Programming\GitHub\RacerRevengePC\src\game\ai_runtime50iv.h:59-191`).
* **Selection sites:**
  * `terrain_interactive_setup50du.cpp:791-898`: source choice and selectors.
  * `terrain_interactive_frame_children50du.cpp`: AI dispatch :147-202, recovery :87-103 and
    :873-891, lap effects :269-275, wrong-way :339-349.
  * `terrain_interactive_loop50du.cpp:387-392` (session chooser) and :1517.
  * Publication :2237-2241 and cold_owners :385-405.
* **Original:** one `.spl` reader for every course, 2A1160/2A1260 (node limit from 2A1260), with
  ProgressNode markers 2A1980..19C0 and the 1ACF70 zone index.
* **Replace by:** one `Navigation` sized from the file's node counts, one AI input and one recovery
  owner. Lap progress moves to the one lap tracker (2.5).

#### B-C13 Triggers, ATM, crystal (R5)

* **Trigger chains:** only GA and TA use the common trigger reader
  (`D:\Programming\GitHub\RacerRevengePC\tests\native\course_trigger_sources50ct.h`, set up at
  world :1311-1312). The others have their own chains:
  * GC `load(archive,195u,12u)` at world :1289-1297;
  * MA :1368, SC :1387;
  * TC/MC/GB/TB/RC at :1418-1507.
* **Per-course services:** continuation :3186-3213.
* **Authoring headers with fixed counts (14):** `ga_trigger_authoring_fixture.h` 10,
  `gc_trigger_authoring50in_fixture.h` 9, `ma_` 13, `mb_` 3, `sa_` 4, `rc_trigger_authoring50la.h`
  11, `ra_` 2, `gb_` 18, `mc_` 21, `tb_` 16, `tc_` 22, `sc_` 9, and others.
* **Other per-course types:**
  * 5 trigger groups `D:\Programming\GitHub\RacerRevengePC\src\game\native_trigger_group50{ga,gb,mc,tb,tc}.h`;
  * 11 `*_trigger_actions*` fixtures;
  * 21 `*_atm_{animation,window}_fixture.h` plus the `*_atm_*_index.inc` files and
    `ra_atm_authoring50la.h` (used at publication :1166-1314 and continuation :3058-3069);
  * the RC crystal (publication :1239-1243 and :1635, loop :3212,
    `D:\Programming\GitHub\RacerRevengePC\tests\native\rc_crystal50rc.h`).
* **Original:** one 274E90 trigger list (`numberoftriggers`, `triggerN`, `triggerNactionM`); 2A00A0
  entry and 2A0E20 actions; 2A0B30 service. ATM comes from the track's animated-texture data.
* **Replace by:** one trigger owner. Base it on `NativeTriggerGroup50MC` (the only one in 2A00A0
  order; common-race 3.7), size it from `numberoftriggers`, and feed it from `course_trigger_sources50ct.h`.
  Use one ATM reader for the authored data. The RC crystal becomes the material/animation data it
  is.

#### B-C14 Sky, fog, lighting (R6)

* **Lighting:** 13 prepares in `D:\Programming\GitHub\RacerRevengePC\src\game\original_gc_gameplay_lighting_candidate.h:498-570`
  (`prepare` for GC plus 12 `prepareXxNative`, SA now `prepareCommonRouteNative`; 162 TUs).
  * Light owners: setup :2494-2501, :2599-2609 (`gb/mc/rc/tb/tcLightOwner`).
  * Lighting aliases: session :266-282.
* **Sky:** MC (`mc_sky50mc.h`) and RA keep their own sky owners (continuation :2931, :2961-2989).
  SC has a separate zero-sky owner (:2952). The clear-colour chain is at loop :3130 and :3374-3377.
* **Fog:**
  * `D:\Programming\GitHub\RacerRevengePC\src\game\original_track_fog50cr.h` exists and is used.
  * GB and MC fog owners remain (continuation :3312-3316, publication :1184-1231), with
    `gb_track_fog50gb.h`, `mc_track_fog50mc.h` and `ra_track_fog50la.h`.
* **Original:**
  * 273C30 fog: end = 4000/3 + visibility × (4000 − 4000/3); start = (end − 64) × (1 − haziness);
    1A98DC restore.
  * 274AE0 sky.
  * SCB `Lights`/`LightN`.
* **Replace by:** one `prepare(track data)` built on `prepareMaSa` (loops `NumberOfLights`, all
  light types and flashers), one animation lease API, one sky loader and one fog reader.

#### B-C15 Materials, budgets, CROWD AA1 (R7)

* **Sites:**
  * Continuation :2825: `Track` enum; :2837: budget `maOrCommonRouteTrack ? 64 : 32 MiB`;
    :2839-2842: per-course `decodeSourceTexture`.
  * Publication :1765-1771: per-course alpha; :1779: AA1 applied only on TB/RC (the comment at
    :1773-1778 says TA, TC, MA, MB, MC, SA and SC author it too).
  * `D:\Programming\GitHub\RacerRevengePC\tests\native\material_alpha50gx_fixture.cpp` (per-track
    selectors), `track_materials50fa_fixture.h`, `track_normals50hk_fixture.h`,
    `track_visual_profile_fixture.h`.
* **Original:** one material path (274160, 14D370/29E340, 382B38/3E1A70); AA1
  (`OnePassAntiAliasingOnOff`) is authored data on each pass.
* **Replace by:** `selectMaSa` reading the authored AA1 everywhere, `original_single_pass_world50lm.inl`
  as the one decoder, and one budget sized from the loaded sizes with one ceiling.
* **Visible change on 7 stock courses:** AA1 starts applying where it is authored. That is the
  faithful result, and the frames must show it (5.6).

#### B-C16 Particles and water (R7)

* **Particles:** the readers are common (27DE00/27C600/27C7B0; 279910/2793A0/2787B0/278B80/278D70).
  The owners are not:
  * `nativeTaParticles` (continuation :3228, closed :3619);
  * TB :3232-3243 and RC :3281;
  * GB/MC particle renders (publication :1184-1231);
  * loop drawing :3707-3750;
  * `{gb,mc,rc,ra}_particle_{authoring,source,attachments}*.h`.
* **Water:**
  * `nativeGcWater` (session :378, world :1198, continuation :2463-2478 and :3102-3109);
  * `mcWaterOwner` (world :1549, continuation :2886-2894 and :4004, frame_children :78 and :870).
* **Original:** one 1B4DC0 particle manager with 274370 services; water is 274570 / 2745D4 from the
  SCB Environment water name. Branching on whether water exists is A9.
* **Replace by:** one particle owner, and one water owner created when the track's data names water.

#### B-C17 Audio per course (R7)

* **Sites:**
  * `material_audio_provider50hi_fixture.h`: source-type overloads `prepareRc` :167, `prepareTb`
    :186, `prepareGb` :205, `prepareMc` :224.
  * Setup :975-1037 (teardown RC/TB/GB/MC), :1083-1085 (live audio only TC/TB/GB), :1186-1209
    (prepare chain and `Track` enum), :1241-1293 and :1325 (providers).
  * `{gb,mc,rc,tb}_positional_sound50xx.h`.
  * The GB-only listener commit at `terrain_interactive_loop50du.cpp:2961-2962` and :2983-2984.
* **Original:** one AUB reader (271600/271674..271804); listener 1A4440 → 1030B0 from the camera,
  every course.
* **Replace by:** one AUB reader on the track archive, one positional-sound owner, and the listener
  on every race.

#### B-C18 Roster and Follow bridge (R10, I2)

* **Sites:**
  * World :1202-1203: RA pose from `main190`, the others from a ground probe.
  * Continuation :864, :2267, :2276.
  * `terrain_interactive_cold_owners50du.cpp:272-300` (legacy identities; pod694 exclusion stand-in
    :284-296).
  * Setup :1764 and :1910-1915 (`legacyFollowQueries` against `raWorld->nativeFollowQueries()`), :2019.
  * `D:\Programming\GitHub\RacerRevengePC\tests\native\native_racer_owners50hn.inc:163, 170`.
* **Original:** 1AA510 → 1AD520/1AE280 places and loads every racer (humans 1ADF80, AI 1AE3CC →
  2129C0).
* **Replace by:** one roster owner, one placement (2A2880 via the race start, 2.6) and one Follow
  bridge. This also closes pod-load-all sections 0, 6 C and 9 Q1.

#### B-C20 Renderer course paths (R8)

* **Sites:**
  * `D:\Programming\GitHub\RacerRevengePC\platform\linux\native\vehicle_window_gl50ds_world.cpp`:
    `raWorld` 136 uses; `mc/gb/rcParticleWindow` 13 each; `rcCrystal` 16.
  * `vehicle_window_gl50ds_storage.cpp`: `mcParticleWindow` 24.
  * `vehicle_window_gl50ds.cpp`: `raStaticFire` 14.
  * `vehicle_window_gl50ds.h:375-389, 1031-1034` (160 TUs).
  * `ra_static_world_gl_storage.inl`, `rc_crystal_gl_storage.inl`.
  * The request is filled per course at publication :290-496 and :1197-1240.
* **Replace by:** one track draw path and one particle window, fed by the one track load's members.

#### B-C21 Course-keyed test harness (R11)

* **Default pod per course:** `terrain_interactive_drive50du.cpp:261-272`: TA/SA/MA/GA 19, RA 9,
  MB 16, SC 7, TC 24, RC 17, TB 18, GB 23, MC 1, GC 1.
* **Test profiles:** `D:\Programming\GitHub\RacerRevengePC\tests\native\original_ui_main_entry50kg.cpp`:
  * `RR_ORIGINAL_TEST_{WAN_MB :312, GASGANO_SC :325, TZIDIK_TC :343, SCORCH_RC :361, SEBULBA_TB :379, KRAID_GB :397, YOUNG_ANAKIN_MC :415}`;
  * `RR_ORIGINAL_TEST_PROFILE` gasgano / mawhonic-ryloth :434-440;
  * `RR_ORIGINAL_TEST_VISIBLE_RIDER` :300.
* **Other sites:**
  * `D:\Programming\GitHub\RacerRevengePC\src\game\original_ui_course_widget_controller50kg.h:12-19`
    (8 test bools, 117 TUs).
  * `D:\Programming\GitHub\RacerRevengePC\tests\native\original_frontend_session50la.h:584, 592, 699, 702`.
  * `D:\Programming\GitHub\RacerRevengePC\tests\native\original_ui_capture_navigation50la.h:39, 60, 99, 196, 227, 377`.
  * `D:\Programming\GitHub\RacerRevengePC\tests\native\windows_framebuffer_capture50ld.h:88-95`.
  * Drive :247-250 (mawhonic RA request).
  * Course-named routes in `D:\Programming\GitHub\RacerRevengePC\tools\capture\run_windows_takeover.py`:
    `main-rider-course-sa`, `main-gasgano07-course-sa`, `main-mawhonic09-course-ra`, and others.
* **Replace by:** one test profile taking `(course code, pilot, mode)`, with the course picked on
  the Course screen like a player would.
* **Also broken:** `D:\Programming\GitHub\RacerRevengePC\work\scratch\batch145\results-test149.ps1:7-8`
  runs the runner from a deleted worktree path, then a stray `un_windows_takeover.py` line. Replace
  it with the round script in 5.3.

### 1.4 Class B: race-flow stand-ins added since 2026-11-12 (R9)

| ID | Site | What | Original |
|---|---|---|---|
| F01 | `D:\Programming\GitHub\RacerRevengePC\tests\native\original_pause_callback_scheduler50la.inl:318-327` (row 1 logs `restart=port_rebuild`); loop :1369, :1484; continuation :4672; `terrain_interactive_drive50du.cpp:291-374` | Pause RESTART tears the whole session down and reloads it (`runSession50DU` again). Best lap CB4 is lost; renewal is course-keyed (B-C07) | 1BD934 → 1AC8E0(race,1): an in-place restart with no reload |
| F02 | `D:\Programming\GitHub\RacerRevengePC\src\game\original_race_end_entry50la.cpp:200-213` stops at 0x1abc0c; loop :1962-1965 `timeTrialRestartPending148` → rebuild | Time Trial finish restart by rebuild | 1ABBE8: 199770(F8,0,1,1,1,1), then 1ABC0C 1AC8E0(race,0); 216690 keeps CB4 when 11258==3 (`D:\Programming\GitHub\RacerRevengePC\src\game\lap_timing50id.h:96`) |
| F03 | `terrain_interactive_loop50du.cpp:1287-1317` (`queuePauseCourse1BDA74` logs `record_check=pending_1BDAB4`), scheduler :329-344 | Time Trial Pause QUIT always takes the no-record Course route | 1BDAB4 1ABFC0(race,1) → close Pause, 1A4B90(2), 199770, Results104 activate, 1CA8E0(Results,3,0) → page 3 → 1CA6A4 1AC440 → 17DE10(6,11,1) Hall. Downstream already exists: `D:\Programming\GitHub\RacerRevengePC\src\game\original_race_record_rule50kg.h:239-256`, `D:\Programming\GitHub\RacerRevengePC\src\game\original_results104_input50la.cpp:151-176, 423-494`, `D:\Programming\GitHub\RacerRevengePC\src\game\original_results104_mode50la.cpp:6` |
| F04 | `D:\Programming\GitHub\RacerRevengePC\platform\linux\native\host_race_results50il.h:8`; instance publication :2544; polled loop :1199-1231, shown :1966-1988 (its comment at :1973-1978 is out of date) | Host text panel for RA Free Play, RA Time Trial, Practice (11258==2), a rejected Results root, and any end-entry stop other than 0x1abc0c | 1A7DD0 → 1AB520 → 1ABBC0 (TT/Practice) or Results104 + 1AB5BC rows |
| F05 | 149p stand-ins in `D:\Programming\GitHub\RacerRevengePC\tests\native\original_pause_callback_scheduler50la.inl` and the loop | Rows 9/10 run 26D070 / 1A4AB0 / 267BE0 and the 500 ms 2AED60 fade from the port's teardown and frontend; row 9 uses the port's ReturnOriginalMain; host Pause fallback when the owner refuses; Start after the finish is left to host menus; 1AF770 handled by the loop | 1BD9B8 / 1BDB5C sequence as written; 1A87A0 refuses Start when race B4+58 bit0 is set |

Lap counting is **not** written separately per course. There is one algorithm:
`D:\Programming\GitHub\RacerRevengePC\src\game\lap_progress50gf.cpp:20-45, 114-160` (FF8 marker,
CC8++ at 1A0F18, 1A0F24 → 1A2B80, scan 1A1108). What is per course is the wrapper layer:

* five typed views `LapProgressTrackView50{MB,TC,TB,GB,MC}` (`lap_progress50gf.h:37-170`);
* five slots plus a ternary in `D:\Programming\GitHub\RacerRevengePC\tests\native\lap_drive50gj_fixture.h:36-89, 261-265`;
* per-course binding at setup :1422-1476;
* five `race_session50HM` rows (`D:\Programming\GitHub\RacerRevengePC\tests\native\race_session50hm_fixture.h:55-125`);
* the five-way wrong-way split.

That layer is what the shared lap tracker (2.5) retires. It is part of B-C12.

### 1.5 Class B: pod families still open (R10)

**B-P01 leftovers** (the family itself is DONE):

* `D:\Programming\GitHub\RacerRevengePC\tests\native\native_racer_rig50hw_cold.cpp:84` refuses a
  pod whose UIACCEPT.INI SHA-256 differs. That breaks "fingerprints become receipts" (b429596);
  make it a receipt.
* `D:\Programming\GitHub\RacerRevengePC\tests\native\native_racer_rig50hw_fixture.h:313` refuses a
  pod with exactly one wrist. This is a general-rule gap (per-record 28EB90 construction). Leave the
  refusal in, logged; no pod key.
* Stale POD02 comments at `original_racer_visual50hw_fixture.h:221, 235`.

**B-P02 speech holes:**

* `D:\Programming\GitHub\RacerRevengePC\tests\native\native_player_speech_authoring50la.h:184-188`
  (POD26 sparse curses).
* `D:\Programming\GitHub\RacerRevengePC\tests\native\selected_player_audio50ij_fixture.h:68-79`
  (POD26 Event2 hole).
* `D:\Programming\GitHub\RacerRevengePC\tests\native\source_announcer_authoring50ie_fixture.h:60-73, 136`
  (`scorch17DeclaredEvents`, pinned by hash).
* Original: 2C1170 reads each pod's `sounds.ini` the same way; 292650 selects Event1..NumEvents; an
  absent EventN is a hole.

**B-P03 human source pod:** `continueSourcePod1AE0A4` has no callers. Join the human 1ADF80 →
2129C0 path in the common roster (2.9).

**B-P04 POD01 leftovers:**

* `terrain_interactive_world50du.cpp:369-370` (`Pod01.phy` on the development input).
* World :1189 ("same actual Pod01 lift", `!isRaTrack`).
* `terrain_interactive_setup50du.cpp:2382-2389` (flame owners from POD01.RES).
* C2: `terrain_interactive_session50du.cpp:511-516` hard-codes `Pods\Pod01\` for the query
  bootstrap collision.
* Dead `playerUsesRosterRig` branches: cold_owners :300 and :551, continuation :2267 and
  :2694-2700, world :1859, roster_cold :161, session :397.
* `D:\Programming\GitHub\RacerRevengePC\tests\native\original_course_roster50kg_fixture.h:4, 37`:
  an add-on pod is laid over slot 0.
* Replace by: every slot loads its own pod's files, and the bootstrap uses the slot's pod.

**B-P05 per-pilot UI code:**

* `D:\Programming\GitHub\RacerRevengePC\src\game\original_ui_rider_dynamic_owner50kg.h:40-46`.
* `original_ui_rider_dynamic_owner50kg.cpp:77, 86` (fixed 23-rider order).
* `D:\Programming\GitHub\RacerRevengePC\src\game\original_ui_rider_backend50kg.cpp:245, 255, 378`.
* `D:\Programming\GitHub\RacerRevengePC\src\game\original_ui_lit_geometry50km.cpp:340-379`
  (widgets 57/34/35, by byte count and SHA).
* Replace by: population from the unlock data and the pod rows, and one coloured-surface rule.

**DLC-pod seams.** These are not stand-ins, but the rewrite must not cement them:

* `D:\Programming\GitHub\RacerRevengePC\tests\native\terrain_interactive_roster_cold50du.cpp:153-157`
  ("an add-on pod never joins a stock AI field").
* `addonRiderMayBeAi` (`D:\Programming\GitHub\RacerRevengePC\src\game\dlc_rider_entries50dl.cpp:73-77`,
  registry key `aiOnAddonCourses`, `D:\Programming\GitHub\RacerRevengePC\src\assets\dlc_registry50dl.cpp:423-427`)
  has no callers.
* Tables of fixed size 23/27: rival `<27u` (`D:\Programming\GitHub\RacerRevengePC\platform\linux\native\original_ui_rider_profile50kg.cpp:186`),
  `firstUiAcceptEvent[27]`, the announcer `namedBanks[27]`.

---

## 2. Target common design

### 2.1 One race, in the original order

Every race (tournament, Single Race, Time Trial, Practice, ALL8 spectator; stock or add-on course;
any pods) runs this one sequence. A step branches only on the data or mode in the last column.

| # | Original | Common owner (new name) | Reads | May branch on |
|---|---|---|---|---|
| 1 | 1A65C0 race; 1A6AD4..1A6BA8 (2410E0, 24B6E0, 1AA5A0, 2350D0) | `original_consumed_stage50DU::Owner` (one) | launch | 11258, P+34 |
| 2 | 1A6BC0 new 0x550, 1B2870 World at race+74 | `common_race_world50CR::World` (one member `world74_`) | none | none |
| 3 | 1A9240 camera root; 1A39F0/1A3C00 music manager | existing owners, ungated | music config | none |
| 4 | 1CC900 loading window, 187330 name, 12CEA0/132A20/132E40 mount | `common_track50CR::Archive` | `TRACKS\<CODE>.RES` or `dlc:/<pkg>/TRACKS/<CODE>.RES`, `LD_<CODE>.PST` | stock or registry row (file source only) |
| 5 | **1A6D80 → 1A9EE0 track load** (2.2) | `common_track50CR::Load` | SCB, TOB, AUB, PRT/PSM/PST, WTT | data (water present, counts) |
| 6 | 1A6DC0 → 1A95B0 → 174DF0/174E58 → 174A30 (+174FA4 with cuts) | `createNativeCourseCameraSession50LA` (2.3) | World+3E4 (TOB `[CSCameras]`) | cut count (0 → zero-cut), 174EA8 subtype/P+34/App+350 |
| 7 | 287430/287710/2878F0 VCR channels | existing | none | 11258 (subtype-3 shape) |
| 8 | 2A1160/2A1260 splines, 1ACF70 zone index | `common_track50CR::Navigation` + lap tracker (2.5) | `m<code>.spl`, `a<code>.spl` | node counts |
| 9 | 175510 cut resources, 1AAB30 | camera session | cut VUBs named in TOB | cut count |
| 10 | **1AA510 → 1AD520 → 1AE280** (humans 1ADF80, AI 1AE3CC → 2129C0; wreck chain 165E90) | `common_roster50CR` (2.9) + existing PodLoad | pod files, registry pod rows | human/AI (A7), field size (11258) |
| 11 | 1AF330, 2C2B50, 2C4B30 speech banks (1AED30 is the start lock, step 14; W0 round 2) | existing, every pod via its own INI | `sounds.ini`, announcer INI | holes are absent events |
| 12 | 1750A0 camera racer binding, 1752D0 | camera session | none | none |
| 13 | race vtable+74 **1A8A40**: Pause100 1BA130, Results104 1BE940/1BEEB0, F8, VCR78, HUD 1A8CC4 | existing population hook, every race | common UI assets | none |
| 14 | race start tail (2.6) | `common_race_start50CR::begin(reason)` | none | reason only (fresh / restart A1) |
| 15 | frame: racers, triggers 2A0B30, music listener 1A4440 → 1030B0, F8 199770 | existing frame owners, ungated | none | race1B0, humans |
| 16 | end dispatch 1A7DD0/1A7E18 → 1AB520 → 1ABBC0 / 1AB5BC (2.7) | `common_race_end50CR` | none | 11258, P+34, race1B0 |
| 17 | Pause 1A87A0 / 1A7C1C / 1BD7C0 / 1BD8CC / 1BDA74 (2.8) | existing original Pause owner, every race | none | humans34, 11258, P+34, App2E0 |
| 18 | teardown: 26D070, 1A4AB0(0.5), 1A4B90(2), 267BE0, 2AED60, 1BE830, 17DE10 | `common_race_end50CR::depart` | none | destination row |

### 2.2 One race world and one track load

**Decision (faithful default).** The one track load runs in the original **1A9EE0 order**, using
RA's native chain generalised. That chain is in:

* `D:\Programming\GitHub\RacerRevengePC\tests\native\original_race_world_load_prefix50la.cpp`
* `D:\Programming\GitHub\RacerRevengePC\tests\native\original_race_world_tob_prefix50la.cpp`
* `D:\Programming\GitHub\RacerRevengePC\tests\native\original_race_world_native_tail50la.inl`
* `D:\Programming\GitHub\RacerRevengePC\tests\native\original_world_*50la.h`

Each step calls the one shared data reader the tree already has for that role. RA's constants
become data reads (common-race 3.4 lists every one: file names, the 4604561/1010/95558/22511 sizes,
178/8/180/2/113/713/7/41/10/20/1472/1261 counts, the `nativeRa*` light calls, the fog string
compare).

The "common route" world (`common_route_world50LM::Owner`,
`D:\Programming\GitHub\RacerRevengePC\tests\native\ma_sa_original_world50lm.h:186`) runs stock SA
and every add-on course today. It is **folded in**: its single-pass decoder stays as the material
step, and it stops being a separate world owner. That keeps add-on courses (including Vengeance)
on the same path they already prove.

| 1A9EE0 sub-step | Original | Shared reader kept | Retired |
|---|---|---|---|
| SCB bind, SceneInfo | 1AA078, 270900 | RA chain (`original_ra_scene_roles270900.cpp`, renamed `original_scene_roles270900.cpp`) | per-course 50LM/50LE/50LH world decodes |
| TOB | world TOB | RA TOB prefix with counts read | 14 trigger-authoring count headers |
| collision, physics index, zones | 270CDC/27C110, 271230/27BB40 | `original_world_physics_index50la.h`, `original_world_collision_objects50la.h` (count limits from data), MC/GB aggregate runtime shape | 6 collision worlds, 6 runtime grounds, 7 cold sources, legacy ground |
| fog | 273C30 | `D:\Programming\GitHub\RacerRevengePC\src\game\original_track_fog50cr.h` | GB/MC/RA fog owners, `RR_AUTHORED_WORLD_FOG` force |
| particle manager, services | 1B4DC0, 274370 | `native_ta_particle_cold50la_fixture.h::prepare` (without the `demoprt.tga` requirement and atlas pins) over the 27DE00 reader | TB/RC/GB/MC/RA particle owners |
| instances, standard models, materials | 274080/274160, 14D370/29E340 | `original_single_pass_world50lm.inl`, `course_world_texture50st.h`, `selectMaSa` | 8 alpha selectors, per-course texture decoders and budgets |
| triggers | 274E90 | `course_trigger_sources50ct.h` → one trigger owner (50MC shape) | 4 other groups, 11 action fixtures |
| water | 274570 / 2745D4 | `native_water_runtime50jc_fixture.h` + `water_resources50jc_fixture.h`, created only when the SCB names water | `nativeGcWater`, `mcWaterOwner`, `RR_GC_NATIVE_WATER` |
| sky | 274AE0 | `original_track_sky50kx_fixture.h` zone→cube selection (`prepareAuthenticated`) | `mc_sky50mc.h`, RA sky, SC zero-sky owner, 8 `prepareXxSource` |
| AUB | 271600 | `readCatalog50HI` (`D:\Programming\GitHub\RacerRevengePC\src\assets\material_audio50hi.cpp:76`) | `prepareRc/Tb/Gb/Mc` overloads, providers |
| lights | SCB `Lights` | `prepareMaSa` path in `D:\Programming\GitHub\RacerRevengePC\src\game\original_gc_gameplay_lighting_candidate.cpp` | 13 prepares, `xxLightOwner`s |
| animation, ATM, UV | 101860 family, 29B1D0 | existing common animation player, `track_uv_animation50uv_fixture.h`, one ATM reader over authored data | 21 ATM fixtures, RC crystal |
| tail | 1AA400..1AA508 | RA tail generalised | — |

**Storage rule.** Every count that sizes storage is read from the track's own files at cold.
Storage is sized once from that count. Every write is bounds-checked first; a write that would not
fit is refused. Nothing writes guest memory: the 1B2870 World and everything under it are
port-owned host objects. Fixed limits that remain are receipts of the largest stock value plus the
largest add-on value seen, and each one is logged. For example, the trigger limit of 22
(`D:\Programming\GitHub\RacerRevengePC\src\game\native_trigger_capacity50tg.h`) is raised to "sized
from `numberoftriggers`". One header owns the layout of each block.

### 2.3 One camera session

`createNativeCourseCameraSession50LA(race, world74, TOB span, navigation view)` is built from the GC
factory. It is declared once in `D:\Programming\GitHub\RacerRevengePC\tests\native\original_ui_main_entry50kg.h`
(W0), and the 12 declarations at :1085-1126 are deleted.

* The cut count comes from 174E58 (World+3E4) for every course, with the stock literal kept as a
  receipt.
* Zero cuts give 174A30 alone; one or more cuts give 174A30 + 174FA4. So RA's 7 intro cameras and
  TA's 3 take the same path.
* 174EA8 zeroes the cuts for subtypes 2 and 3, and for subtype 1 when P+34 != 2 or App+350 != 0.
  That gives RA Free Play and RA Time Trial the zero-cut controller with no RA code.
* `NativeTaIssuance` keeps one `race/world/map` triple. The unused camera children (Start7C,
  Cinematographer70, Tv78, TvBody) are built for every issued session. The MC 173-node special
  becomes the map's own node count.

### 2.4 One lighting, sky, fog and audio prepare

* **Lighting:** one `prepare(const CommonTrack&)` and one animation lease API in
  `D:\Programming\GitHub\RacerRevengePC\src\game\original_gc_gameplay_lighting_candidate.h`. One
  storage layout sized from the SCB zone and portal counts, which retires the per-course "scene
  storage" aliases.
* **Fog and sky:** as in 2.2, with one fog owner that the camera session and the renderer borrow.
* **Audio:** one AUB catalog and one positional-sound owner over the track's sound rows. The listener
  1A4440 → 1030B0 is committed every frame on every race
  (`OriginalRaceMusicManager50LA::advanceNativeListenerConsumedStep`, `D:\Programming\GitHub\RacerRevengePC\src\game\original_race_music_manager50la.cpp:423-474`).

### 2.5 One lap tracker

`common_lap50CR::Tracker` wraps the existing single algorithm (`lap_progress50gf.cpp`
`prepareLapProgressTrack50GFShared50MB` / `applyPlayerLapCrossing50GFShared50MB`; 2A1980/2A273C/2A26E0,
1A0F18, 1A0F24 → 1A2B80, 1A1108).

* It has one view type over the common `Navigation`, sized from the file.
* It has one race-session row type, replacing `SessionRows` and the Mb/Tc/Tb/Gb/McSession rows.
* It has one wrong-way rule, and event 6 (1A55F0..1A5914) reads `lapCC8` from it.
* Timing stays in `D:\Programming\GitHub\RacerRevengePC\src\game\lap_timing50id.h`. 216690 resets
  CB0/CB8/CC0 and keeps the best lap CB4 when 11258==3. That is how the Time Trial in-place restart
  keeps the run's best lap with no special code.

### 2.6 One race start, used by a fresh race and by every restart

`common_race_start50CR::begin(Race&, Reason)` with `Reason ∈ {Fresh, Restart1AC8E0(a1)}`:

* **Fresh:** after the 1A6AA0 load (2.1 steps 1-13), the tail below.
* **Restart (1AC8E0):** builds on the partial 1AC8E0 that already exists for the Results replay
  (`D:\Programming\GitHub\RacerRevengePC\src\game\original_race_end_dispatch50la.cpp:365-485`).
  The existing steps are:
  * World74+3D0 trigger reset (2DCC70);
  * App11C clear;
  * Aural 103210;
  * music 1AC960;
  * particles 161F60;
  * world 2702E0;
  * race90.

  The missing steps are added:
  * 1AEA70 per racer (216690 timing with the 11258 rule, 217420 damage reset);
  * 2A2880 grid placement;
  * 1AED30 start lock;
  * race94 countdown;
  * VCR channel reset;
  * 1ACB18 record branch (11258==3): 287BE0, raceA0 = 0, record actor B0+40 vtable+18 (288630 is the 1ABA68 end backup, not a restart step; W0 round 2);
  * camera reset;
  * 1ACB7C tail (race1B0 bit3).
* **Shared tail:** both reasons end in the same steps: grid placement, start lock, 197040 start
  overlay, race94.
* **Callers:**

  | Caller | Call |
  |---|---|
  | Results replay | 1AC8E0(A1=0) |
  | Pause RESTART | 1BD934 → 1AC8E0(race,1) |
  | Time Trial finish | 1ABC0C → 1AC8E0(race,0) |
  | Practice | 1ABC1C → 1AC8E0 |

* **Retired:** `SessionExit50DU::Restart`, `timeTrialRestartPending148`, the consumed-launch
  renewal (B-C07), and the `restart=port_rebuild` path. A session reload happens only when the
  player picks a new race from a menu.

### 2.7 One end-of-race dispatch

`common_race_end50CR` runs 1A7DD0 for every race:

* **11258 == 1, ordinary:** 199770, Results104 vtable+20, 1AD230, 1AB520 → 1AB5BC rows. In the
  tournament these are 1AB7F8 Unlock, 1AB818 Garage, and so on; in Free Play, 1AB828 → Hall 1AB884
  / Unlock 1AB8E8 / Course 1AB908.
* **11258 == 3, Time Trial:** skip 199770 and Results at the finish → 1AB520 → 1ABBC0 → 1ABBE8
  199770 → 1ABC0C restart (2.6).
* **11258 == 2, Practice:** 1ABC1C bit5, 199770, VCR, 1AC8E0, replay.
* **ALL8 spectator:** the same dispatch with humans34 == 0, on whichever course the launch names.

RA Free Play and RA Time Trial get this with no RA code (148r design §3). HostRaceResults50IL is
deleted. A rejected Results root logs `common_race_end refused ...` and departs to the Course
screen through 18 (port failure handling, flagged; not standing in for an original screen).

### 2.8 Pause on every race

The original owner (149p) stays. These changes complete it:

* **RESTART row** calls 2.6 in place.
* **QUIT row 10 in a Time Trial** runs 1BDAB4:
  1. 1ABFC0(race,1).
  2. If it qualifies: Pause+200 = 0, close, 1A4B90(2), 199770, activate Results104, then 1CA8E0(Results,3,0)
     for the record/name page.
  3. Confirm runs 1CA574..1CA6F0: 1ABFC0(race,2), then 26D070 / 1A4B90(2) / 1A4AB0(0.5) / 267BE0 /
     2AED60, deactivate, App11C = 0, 1AC440 lap rows, save pending, 17DE10(6,11,1) Hall.
  4. If it does not qualify: 1BDB5C, then 17DE10(6,6,1) Course.
* **Rows 9 and 10** run 18 in the original order.
* **ALL8 spectator binding** takes the launch's own course and profile; the RA identity at
  `original_pause_observation50la.h:489-490` goes.
* **Start after the finish** (race B4+58 bit0) is refused, as 1A87A0 does.
* **Host Pause fallback** is deleted once the owner covers every race. A refusal is logged and
  Start is ignored for that frame.

### 2.9 One roster, any pod

`common_roster50CR` builds the field once from rows:

* Each row is a stock pod (`Pods\PodNN\`, A5) or an add-on pod (`dlc:/<pkg>/...`, registry row ≥32),
  marked human or AI.
* Every slot goes through 1AD520/1AE280: humans 1ADF80 → 1AE0A4 → 2129C0, AI 1AE3CC → 2129C0, and
  the wreck chain 165E90. That retires B-C18, B-P03 and B-P04.
* **Tournament field:** 188470 over the class tables 3A8940/3A8960/3A8980.
* **Free Play field:** the 8 Course-screen entries. Time Trial is 1 (1AD520 solo).
* **Add-on pods:** they join an AI field only through `addonRiderMayBeAi` (registry `aiOnAddonCourses`,
  add-on courses only). The rewrite wires that call in place of the hard `stockPodId` refusal at
  roster_cold :153-157. For stock courses the result is unchanged: the registry rule says
  add-on courses only.
* **Next DLC steps:** the field builder takes a row list, so adding add-on pods to the
  tournament, the Garage, or as AI needs no roster code, only rows and the UI work.
* Fixed 23/27-entry tables on the race side (UIACCEPT, announcer banks, speech) read the pod's own
  files, or the registry row for an add-on pod.

### 2.10 What each course and each pod supplies

| Supplier | Data | Used by |
|---|---|---|
| Course (stock) | `TRACKS\<CODE>.RES` (SCB zones/instances/Fog/Lights/Environment: sky cubes, water name, particle texture map; TOB: `[CSCameras]`, `numberoftriggers`/`triggerN`/actions, AllowShadowReceiving; AUB; PRT/PSM/PST; WTT; ATM data), `tracks\m<code>.spl` / `a<code>.spl`, `.tun`, `LD_<CODE>.PST`, intro INIs, PHY class letter (A1), Hall rows by course order | 2.1 steps 4-9, 2.4, 2.5, A1 |
| Course (add-on, e.g. Vengeance `n64-vengeance/nv`) | The same files under `dlc:/<pkg>/TRACKS/`; registry row: code, number ≥32, display name, ai_class, rival, modes (single/time_trial/vs), records in the DLC side file, optional N64 data (`.nft` floor table, terrain-zone ZeroGravity/Magnet/SurfaceTraction) | same as stock; records via 1ABFC0/1AC440 on the add-on Hall rows (4341225) |
| Pod (stock) | `PODnn.RES` (PSG/VUT rig, Pod Setup keys such as BottomLeftEngine, .phy incl. +50 explosive-wreck speed and classes, wreck pieces 7 or 9, materials), `sounds.ini [Indices]`, announcer INI, racer-keyed UIACCEPT event (1E7C74 table) | 2.9, 2.1 step 11 |
| Pod (add-on) | The same under `dlc:/`; registry pod row (name, Huttese name, colours, `aiOnAddonCourses`, UI archive) | 2.9, Rider/Results UI (already landed) |
| Launch | 11258 subtype, P+34, game34, App+350, humans34, field size | 2.1 step 1, 174EA8, 2.7, 2.8 |

No course and no pod is named in race-load or race-frame code. The only course-name compare left is
A1, which is the original's.

---

## 3. Single-rewrite execution plan

### 3.1 Writers and file ownership (no shared files)

All writers branch from one W0 commit on a branch `common-rewrite` (from b607b29), each in its own
worktree. Each writer may compile-check its own TUs against the W0 tree (single-TU compile, no link,
no game). Only the builder links. Paths are under `D:\Programming\GitHub\RacerRevengePC\`.

**W0, interface freeze.** Serial and first; nobody else starts until it is merged. It owns:

* All new headers (3.3), with declarations, data layouts and the log vocabulary.
* The 8 context headers `tests\native\terrain_interactive_{setup,world,continuation,publication,loop,frame,cold_owners,cold_contacts}50du.h`,
  plus `tests\native\terrain_interactive_dependencies50du.h` and `terrain_interactive_loop_providers50du.h`.
  All `isXxTrack` plumbing is removed and one `const common_track50CR::Track&` is added.
* `tests\native\original_ui_main_entry50kg.h`: every Application declaration change (camera factory,
  1A6BC0, race start, Pause, TT record entry).
* The receipts table (4.2).

After W0, any interface change goes back through W0 (one small serial commit), never through two
writers.

**Role writers R1-R11** (parallel):

| Writer | Scope | Owns (edit or delete) |
|---|---|---|
| R1 race structure | B-C04, B-C05, B-C06 (F8 layout, music) | `tests\native\original_consumed_*_stage50du.h` (13 → 1 new `original_consumed_stage50du.h`), `tests\native\original_*_world_cold_prefix50la.h` (12 deleted; shared `original_world_cold_prefix50la.h` kept), `original_race_constructor50la.h`, `original_race_world_join50la.h`, `native_common_race50pd.h`, `original_race_native_retirement50la.h`, `original_race_hud_scope50la.cpp`, `original_results_human_root50la.cpp`, `original_results_feedback_scope50la.cpp`, `original_results_update_schedule50la.cpp`, `original_results_cold_driver50la.cpp`, `original_race_results_parent50la.cpp`, `original_results_roster_read50la.cpp`, `original_race_end_clock50la.cpp`, `original_race_f8_*50la.cpp`, `ra_application_cold50la.*` (renamed to a common race-application cold file), `src\game\original_race_music_manager50la.cpp`, `src\game\original_race_timing_manager50la.cpp`, `original_native_ta_root_audio50la.h` |
| R2 camera | B-C09, B-C10, RA camera route | `original_ta_camera_issuer_candidate.h` (12 → 1), `original_camera_session50kx.h`, `original_camera_assets50kx.h`, `original_camera_unused_children50la.inl`, `original_ta_camera_lifecycle.inl`, `ra_source_camera_factory50la.h`, `ra_source_camera_ownership50la.h`, `ra_native_session50la*`, `original_race_camera_owner50la.cpp`, `original_session_f8_intro50la.cpp`, `src\game\original_camera_start_authoring.cpp`, `src\game\original_camera_start_effects.h`, `src\assets\original_track_cut_identity50la.h`, `src\assets\original_ta_intro_authoring_candidate.h`, `src\assets\original_ta_cut_authoring_candidate.h`, `platform\linux\native\original_ta_intro_preload50la.h`, `race_audio_groups_ta_intro_cold_candidate.inl`, `native_race_intro_transport50la.inl` |
| R3 track load + world | B-C11, 2.2 chain | `original_race_world_load_prefix50la.cpp`, `original_race_world_tob_prefix50la.cpp`, `original_race_world_native_tail50la.inl`, `original_world_*50la.h/.inl`, `original_source_world_*50la.inl`, `ra_cold_*50la.*`, `ra_pod_scene50la.h`, `ra_static_world50la.h`, `original_ra_scene_roles270900.cpp`, the 12 `*_collision_world*` files, 7 `*_runtime_ground50la.h`, `authored_runtime_ground50cp_fixture.h`, 7 `*_cold_sources50la.h`, `track_runtime_storage50ig_fixture.h`, the 33 `*_original_world50l*` files, `ma_sa_original_world50lm.*`, `original_single_pass_world50lm.inl`, `mb_single_pass_world50lm.inl`, `{gb,mc}_application_*` |
| R4 navigation + lap | B-C12, 2.5 | `src\game\track_navigation*` (12 files), `src\game\ai_runtime50iv.h`, `src\game\lap_progress50gf.*`, `src\game\rc_navigation_lease50rc.h`, `src\game\lap_event6_subscriber50id.h` (readers only, A3 kept), `tests\native\lap_drive50gj_fixture.h`, `race_session50hm_fixture.h`, recovery owners, `undertrack_reposition50et.h` |
| R5 triggers + animation | B-C13 | `tests\native\course_trigger_sources50ct.h`, `src\game\native_trigger_capacity50tg.h`, `src\game\native_trigger_group50*.h`, `src\game\trigger_entry50in.h`, 14 `*_trigger_authoring*`, 11 `*_trigger_actions*`, 21 ATM fixtures + `*_atm_*.inc` + `ra_atm_authoring50la.h`, `track_animation_*50an.h`, `track_uv_animation50uv_fixture.h`, `rc_crystal50rc.h` |
| R6 lights, sky, fog | B-C14 | `src\game\original_gc_gameplay_lighting_candidate.h/.cpp`, `native_gc_selected_lights_candidate.h`, `src\game\original_rc_fog_numeric50rc.h`, `original_gb_light_flash50gb.h`, `src\game\original_track_fog50cr.h`, `gb_track_fog50gb.h`, `mc_track_fog50mc.h`, `ra_track_fog50la.h`, `sky50gw_fixture.h`, `original_track_sky50kx_fixture.h`, `mc_sky50mc.h`, `mc_sky_index.inc`, `gb_sky_loader50gb.inl` |
| R7 materials, particles, water, audio | B-C15, B-C16, B-C17 | `material_alpha50gx_fixture.cpp`, `track_materials50fa_fixture.h`, `track_normals50hk_fixture.h`, `track_visual_profile_fixture.h`, `course_world_texture50st.h`, `native_ta_particle_cold50la_fixture.h`, `{gb,mc,rc,ra}_particle_*`, `native_water_runtime50jc_fixture.h`, `water_resources50jc_fixture.h`, `mc_water50mc.h`, `native_gc_water_session50kg_fixture.h`, `material_audio_provider50hi_fixture.h`, `src\assets\material_audio50hi.*`, `{gb,mc,rc,tb}_positional_sound50*.h`, `positional_sound_rows50sr.h`, `original_positional_sounds50sr.h` |
| R8 renderer | B-C20, B-C03 pixel switch | `platform\linux\native\vehicle_window_gl50ds.h/.cpp`, `vehicle_window_gl50ds_world.cpp`, `vehicle_window_gl50ds_storage.cpp`, `ra_static_world_gl_storage.inl`, `rc_crystal_gl_storage.inl`, `original_gc_particle_gl_storage50kg.inl` |
| R9 race flow | F01-F05, 2.6-2.8, Pause/ALL8 | `src\game\original_race_end_dispatch50la.cpp`, `src\game\original_race_end_entry50la.*`, `src\game\original_results104_input50la.cpp`, `original_results104_mode50la.cpp`, `src\game\original_race_record_rule50kg.h` (only if 1ABFC0(race,1) needs it), `src\game\lap_timing50id.h` (check only), `tests\native\original_pause_observation50la.h`, `original_pause_input50la.inl`, `original_pause_callback_scheduler50la.inl`, `original_pause_main_transfer50la.inl`, `original_results104_input50la.cpp` (tests side, if separate), `native_common_results50pd.h`, `platform\linux\native\host_race_results50il.h` (deleted), new `common_race_start50cr.cpp`, `common_race_end50cr.cpp` |
| R10 pods + roster | B-C18, B-P01 leftovers, B-P02..B-P05, DLC seams | new `common_roster50cr.cpp`, `terrain_interactive_roster_cold50du.cpp`, `native_racer_rig50hw_cold.cpp`, `native_racer_rig50hw_fixture.h`, `original_racer_visual50hw_fixture.h`, `native_player_speech_authoring50la.h`, `selected_player_audio50ij_fixture.h`, `source_announcer_authoring50ie_fixture.h`, `original_player_identity50ik_fixture.h`, `original_racer_manager_entity50la.h`, `original_course_roster50kg_fixture.h`, `src\game\original_opponent_selection50kg.*`, `src\game\dlc_rider_entries50dl.*`, `src\game\original_ui_rider_dynamic_owner50kg.*`, `src\game\original_ui_rider_backend50kg.cpp`, `src\game\original_ui_lit_geometry50km.cpp`, `native_racer_owners50hn.inc` |
| R11 harness + checks | B-C21, 4.4, 5.3 | `tests\native\original_ui_main_entry50kg.cpp` (test-profile section :299-440 only; no Application code), `src\game\original_ui_course_widget_controller50kg.h`, `tests\native\original_frontend_session50la.h`, `original_ui_capture_navigation50la.h`, `windows_framebuffer_capture50ld.h`, `tools\capture\run_windows_takeover.py`, new `work\scratch\common-rewrite\round.ps1`, `check-round.ps1`, `check\track_check.cpp` + build script |

**Integration writers I1-I4** (parallel with the role writers). They code against the W0
interfaces only:

| Writer | Owns | Does |
|---|---|---|
| I1 entry | `tests\native\terrain_interactive_session50du.cpp` (938 lines), `terrain_interactive_drive50du.cpp` (419), `platform\linux\native\host_track_selection50im.h`, `tests\native\original_course_race_targets50kx.h` | B-C01 flag definitions deleted; B-C02 one admission rule; B-C03 switches; B-C06 session :747-752 (activation from launch data only); B-C07 restart renewal deleted; C2 bootstrap pod; B-C21 default pod per course deleted |
| I2 cold load | `terrain_interactive_world50du.cpp` (2291), `terrain_interactive_setup50du.cpp` (3758), `terrain_interactive_cold_owners50du.cpp` (578), `terrain_interactive_cold_contacts50du.cpp` (342) | Calls `common_track50CR::Load`, the one stage, the one roster, one lift bind, one closer, one light and audio prepare; deletes every per-course branch in those files |
| I3 runtime | `terrain_interactive_continuation50du.cpp` (4772), `terrain_interactive_publication50du.cpp` (3296) | The one camera factory call, one particle/water/sky/fog/ATM/material path, one renderer request, the Pause and Results binding for every race, the race start and end calls; deletes B-C08 and the HostRaceResults50IL instance |
| I4 frame | `terrain_interactive_loop50du.cpp` (4335), `terrain_interactive_frame_children50du.cpp` (1361), `terrain_interactive_frame_physics50du.cpp` (646) | One AI/recovery/lap/wrong-way dispatch, one query bracket, listener every race, Pause/restart/end through 2.6-2.8, deletes `restartRequested` rebuild and text-panel polling |

### 3.2 Header list for the one rebuild

Every header edit lands in the same build. Measured TU fan-out per group (fanout.py on the current
manifest):

| Group | Headers | TUs |
|---|---|---|
| Race structure | `original_race_constructor50la.h` (44), 13 stage headers (7-9), 12 world prefixes (15), `original_world_cold_prefix50la.h` (16), `original_race_world_join50la.h` (15), `ma_sa_original_world50lm.h` (17), `original_race_native_retirement50la.h` (9), `native_common_race50pd.h` (5), 8 context headers (2-5) | union **52** |
| Camera | `original_camera_session50kx.h`, `original_camera_unused_children50la.inl`, `original_ta_camera_lifecycle.inl` (62 each), `ra_source_camera_ownership50la.h` (48), `ra_source_camera_factory50la.h` (8), `ra_native_session50la.h` (4), `original_camera_assets50kx.h` (3), `original_ta_camera_issuer_candidate.h` (1) | union **62** |
| Application declarations | `original_ui_main_entry50kg.h` | **108** |
| Lighting, fog | `original_gc_gameplay_lighting_candidate.h` (162), `original_track_fog50cr.h` (15) | **162** |
| Navigation and lap | `track_navigation_view50{mb,gb,mc,tb,tc}.h` (94-102), `lap_progress50gf.h` (93), `ai_runtime50iv.h` (65), `race_session50hm_fixture.h` (65), `lap_drive50gj_fixture.h` (16) | union **102** |
| Cuts and intro | `original_camera_start_effects.h` (165), `original_track_cut_identity50la.h` (150), `original_ta_intro_authoring_candidate.h` (135) | union **187** |
| Race flow | `original_race_record_rule50kg.h` (124, only if touched), `original_race_end_entry50la.h` (44), `original_pause_observation50la.h` (6), `original_pause_callback_scheduler50la.inl` (6), `native_common_results50pd.h` (5), `host_race_results50il.h` (5), `original_pause_input50la.inl` (1) | union **124** (44 without the record-rule header) |
| Triggers and ATM | `course_trigger_sources50ct.h`, `native_trigger_capacity50tg.h`, 14 authoring + 21 ATM headers | union **107** |
| Pods and roster | `original_course_roster50kg_fixture.h` (46), `source_announcer_authoring50ie_fixture.h` (21), `original_racer_manager_entity50la.h` (19), `selected_player_audio50ij_fixture.h` (18), `native_player_speech_authoring50la.h` (17) | union **55** |
| UI and harness | `original_ui_rider_dynamic_owner50kg.h` (120), `original_ui_course_widget_controller50kg.h` (117) | union **125** |
| Admission | `host_track_selection50im.h`, `original_course_race_targets50kx.h` | **46** |
| Audio | `material_audio_provider50hi_fixture.h` | **6** |
| Renderer | `vehicle_window_gl50ds.h` | **160** |
| **All of the above** | | **228 of 636** |

The renderer header (160) and the lighting header (162) sit inside the union. Adding them costs no
separate 45-minute round; they rebuild together with everything else.

Not yet measured, and to be added at merge time: the new W0 headers (3.3), the R3 world headers
(`original_world_*50la.h`, `ra_cold_*`), the collision and ground headers, and the particle and
water fixtures. Most of their includers are already in the union.

**Builder rule.** At merge, run `D:\Programming\GitHub\RacerRevengePC\work\scratch\ra-route2\fanout.py`
over every header in `git diff --name-only b607b29` plus every changed .cpp.

* Expect about 240-280 TUs, about 70-80 minutes.
* If the result is more than about 450, do one full rebaseline (about 630 TUs, about 3 hours)
  instead.
* Either way it is **one build**. New .cpp files are added to the manifest, using the
  `new.cpp=template.cpp` relink rule from QUEUE.txt.

### 3.3 New files (W0 writes the headers)

All under `D:\Programming\GitHub\RacerRevengePC\tests\native\` unless noted. The `50cr` suffix
follows the existing `original_track_fog50cr.h`.

* `common_track50cr.h`: `Track` holds the code, source (stock or registry), archive, decoded
  SCB/TOB/AUB spans and per-role counts. It declares the role reader interfaces: collision and
  zones, storage, fog, lights, sky, triggers, navigation, particles, water, AUB, materials and cuts.
  One header owns every reserved-block layout.
* `common_race_world50cr.h`: the single race+74 World type.
* `original_consumed_stage50du.h`: the one stage.
* `common_race_start50cr.h` and `.cpp` (R9): 2.6.
* `common_race_end50cr.h` and `.cpp` (R9): 2.7, 2.8 rows 9/10, 18.
* `common_lap50cr.h` and `.cpp` (R4): 2.5.
* `common_roster50cr.h` and `.cpp` (R10): 2.9.
* `common_race_log50cr.h`: the log vocabulary (section 4).
* `stock_course_receipts50cr.h`: the receipt table (4.2), integrity only.
* `common_track_load50cr.cpp` (R3): the 1A9EE0 chain, holding the generic part of today's RA chain.
* `D:\Programming\GitHub\RacerRevengePC\work\scratch\common-rewrite\check\track_check.cpp` and
  `build_track_check.py` (R11): the host data check (4.3).
* `D:\Programming\GitHub\RacerRevengePC\work\scratch\common-rewrite\round.ps1` and
  `check-round.ps1` (R11): 5.3 and 4.4.

### 3.4 Merge order

Files are disjoint, so the merges do not conflict. The order is chosen so that each merge's
compile check means something.

1. **W0** (interfaces, context headers, `original_ui_main_entry50kg.h`, receipts, log header).
2. **R3** track load + world. Its reader calls hit the interfaces, so they compile against stubs
   until 3-4 land.
3. **R5, R6, R7**: the readers R3 calls.
4. **R4**: navigation and the lap tracker.
5. **R1**: race structure, using R3's World.
6. **R2**: camera, using R1's world member and R4's navigation view.
7. **R9**: race flow, using R1 and R4.
8. **R10**: roster.
9. **R8**: renderer, using R3/R7 members.
10. **R11**: harness and checks.
11. **I1, I2, I3, I4**, in that order. They delete the last readers of the per-course owners.
12. **Gate commit** (3.5), then **the one build**.

### 3.5 Gates before the build (no game)

1. **Grep gate.** Over `D:\Programming\GitHub\RacerRevengePC\tests\native`, `src` and `platform`,
   these must find nothing in race code:
   * `\bis(Ta|Tb|Tc|Sa|Sc|Ma|Mb|Mc|Ga|Gb|Gc|Rc|Ra)Track\b`, `commonRouteTrack`, `maOrCommonRouteTrack`;
   * `HostTrack50IM::(TA|..|RA)` outside file-name building;
   * `RR_(NATIVE_)?(TA|..|RA)_`;
   * `consumed(Ta|..|Ra)Stage`, `(ta|gc|..)World74_`, `createNative(Ta|..)CameraSession`,
     `prepare(Ta|..)Native`;
   * `racer60==` / `PodNN` literals outside A2/A3/A5/A8.

   The output is saved to `D:\Programming\GitHub\RacerRevengePC\work\scratch\common-rewrite\grep-gate.txt`.
2. **Compile check** of every changed TU on the merged tree (no link).
3. **Host data check** (4.3) over the 13 stock archives and Vengeance: every receipt matches, or the
   difference is listed and explained before the build.
4. **Stand-in list.** Every new or kept stand-in is flagged in the code and listed in
   `D:\Programming\GitHub\RacerRevengePC\work\scratch\common-rewrite\STAND-INS.txt`.

---

## 4. Logging and diagnostics, designed in

The goal: one failing run names the failing step, its original address, its role, and the writer
who owns it, without re-splitting the work.

### 4.1 Log vocabulary (`common_race_log50cr.h`; one line format, grep-friendly)

* **Race begin:**
  `common_race_begin run=<n> course=<code> source=stock|registry mode34=<m> subtype=<11258> p34=<v> humans=<n> field=<n> pods=<id,id,...>`
* **Load steps:** one line per 2.1 step and per 1A9EE0 sub-step, in order:
  `common_load step=<NN> pc=<original> role=<role> owner=<R1..R11|I1..I4> result=ok|refused value=<count/bytes> reason="<text>"`.
  The first `result=refused` is the answer to "where did it stop".
* **Track data:** one line per role per race:
  `common_track_data role=<fog|lights|cuts|triggers|nav_nodes|ai_nodes|lap_markers|zones|instances|collisions|storage|water|particles|sky|aub|atm|materials> course=<code> source=<file> value=<v> receipt=match|differs(<stock>)|none`.
  Add-on courses show `receipt=none`.
* **Storage:** for each sized block,
  `common_storage block=<name> count=<n> bytes=<n> limit=<n>`; and on a refused write,
  `common_storage_refused block=<name> want=<n> have=<n> pc=<pc>`.
* **Pods:**
  `common_roster slot=<s> pod=<id> source=stock|registry human=0|1 path=1ADF80|1AE3CC result=ok|refused reason=`
  and `common_pod_receipt pod=<id> file=<UIACCEPT|sounds.ini|...> receipt=match|differs|none`.
* **Race start:**
  `common_race_start reason=fresh|restart pc=1A6AA0|1AC8E0 a1=<0|1> caller=<1BD934|1ABC0C|1ABC1C|1CA...> best_lap_cb4=<t> racers=<n> grid=2A2880 lock=1AED30 result=ok|refused`
* **Laps:**
  `common_lap racer=<slot> lap=<n> time=<t> best=<cb4> marker=FF8`, one line per crossing.
* **Race end:**
  `common_race_end site=1A7DD0 subtype=<s> branch=1A7E18|1ABBE8|1ABC1C route=results|tt_restart|practice|spectator next=<row pc>`
* **Pause:** the 149p lines (`pause_open`, `pause_row`, `pause_quit`, ...) plus
  `pause_quit_record site=1BDAB4 qualify_1ABFC0=<0|1> page=3|none`.
* **Teardown:**
  `common_teardown role=<role> released=1|0` and `course_archive_release released=1`.
* **Run summary:** one line at exit:
  `common_run_summary course=<code> mode=<..> pods=<..> result=pass|fail first_refusal=<step/role/owner> laps=<n> routes=<ack sequence> fps_avg=<v>`

### 4.2 Receipts, so stock behaviour is checked against facts

Before deleting any per-course header, W0 extracts every stock constant from those headers into
`stock_course_receipts50cr.h` and
`D:\Programming\GitHub\RacerRevengePC\work\scratch\common-rewrite\stock-receipts.tsv`, one row per
(course, role). The constants include:

* trigger counts (GA 10, GC 9, MA 13, MB 3, SA 4, RC 11, RA 2, GB 18, MC 21, TB 16, TC 22, SC 9);
* cut counts;
* navigation and AI node counts (MB 265/292, TC 268/293/26, TB 270/269/11, GB 213/257/17, MC 173/173/28,
  RA 113);
* collision, zone and instance counts (RA 178/8/180/10/1261; closures TC 91, MC 111, GB 637, TB 337,
  RC 123, SC 252/261; MB 131);
* light counts;
* fog values;
* AUB rows;
* archive sizes and SHA.

Readers log `receipt=match` or `differs`. These receipts are never used to size anything. They prove
the common reader produces the same values the per-course code had pinned.

### 4.3 Host data check (no game)

`D:\Programming\GitHub\RacerRevengePC\work\scratch\common-rewrite\check\track_check.cpp` is built
with the one build, using the same pattern as
`D:\Programming\GitHub\RacerRevengePC\work\scratch\146p-race\check\career_check.cpp`.

* It runs every common reader (R3-R7 roles, cuts and pods) on the 13 stock archives in
  `C:\Games\Star Wars Racer Revenge` and on Vengeance in
  `D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\roots\addon-arcade-146\DLC`.
* It prints the same `common_track_data` lines and compares them to `stock-receipts.tsv`.
* It also runs every stock pod's speech, announcer and UIACCEPT reads.
* It takes seconds. It must pass before the first game run.

### 4.4 Round checker

`D:\Programming\GitHub\RacerRevengePC\work\scratch\common-rewrite\check-round.ps1` turns each run log
into one row:

* pass/fail, the first refusal (step, role, owner, pc), data receipts matched over total, laps, the
  route ack sequence, Results/Hall/Course reached, archive released, FPS average;
* the diff against that course's newest recorded evidence log (5.1 item 3).

The failure-signature patterns are those of
`D:\Programming\GitHub\RacerRevengePC\work\scratch\batch145\regress149.ps1`: `require_failed`,
`NATIVE_UNHANDLED`, `terminate called`, `_prefix_rejected`, `_begin_rejected`, `refused`, and so on.

---

## 5. The one verification round (revised 2026-11-20: about 1 hour of game time, no baseline round)

The code is common, so the race-flow logic needs proving **once**. That logic is: the fresh start,
the in-place restart, Results, the Hall or Course return, the Time Trial restart, the records page,
Pause QUIT and the tournament return.

What needs proving **per course** is that the common path loads and races that course's data. The
main per-course proof is the no-game receipt table and the host check (4.2, 4.3). On top of that,
each course gets one short race to confirm it in the game.

How every run is set up:

* Muted, unattended and logs-only (`--logs-only` given explicitly). No screenshots inside tests.
* Driven with `--hold-throttle --steer-invert --camera-cycle --repair --unstick`.
* One game instance at a time. Runs are chained by
  `D:\Programming\GitHub\RacerRevengePC\work\scratch\common-rewrite\round.ps1` (R11).
* Speed is reported in FPS.

### 5.1 What each course is compared against (no baseline round)

1. **Main proof, with no game running: the host check (4.3).**
   * It covers the 13 stock archives and Vengeance.
   * It must show `receipt=match` for every row of
     `D:\Programming\GitHub\RacerRevengePC\work\scratch\common-rewrite\stock-receipts.tsv`.
   * That proves the common readers produce exactly the values that each stock course's own code had
     pinned: counts, cuts, nodes, collision, zones, lights, fog, AUB, triggers, and archive sizes and
     SHA.
   * The same holds for each of the 23 stock pods: speech, announcer and UIACCEPT reads.
   * It runs before any game run. The round does not start until it passes.
2. **The same receipts in the game.** Every `common_track_data` and `common_pod_receipt` line in
   every run must also say `receipt=match`.
3. **Recorded evidence logs, for behaviour only.** These are the newest logs per course under
   `D:\Programming\GitHub\RacerRevengePC\evidence\`. What is compared: the screen and route
   sequence, cut count, lap count, trigger fires, archive release, and FPS within 2 FPS.

   | Course | Logs |
   |---|---|
   | TA | `r149p3-TA-23`, `c149b-ta-restart`, `c148-ta-single5`, `c148-ta-tt5`, `c148-ta-pausequit7`, `eyecheck-pause149b-ta-quit` |
   | SA | `r149p3-SA-18`, `c147-sa-r19`, `d146p21-fp-SA`, `eyecheck-stock-hall-record-sa23` |
   | MA | `r149p3-MA-8`, `d146p23-fp-MA` |
   | GA | `w149p3-GA-24`, `c148-ga-single5`, `d146p23-fp-GA` |
   | TB | `r149p3-TB-11`, `w145c-TB-MB-19` |
   | TC | `r149p3-TC-4`, `w145c-RC-TC-19` |
   | MB | `r149p3-MB-7`, `w145c-TB-MB-19` |
   | SC | `r146a-SC-3`, `r145c-SC-3` |
   | GB | `f145c-GB-16`, `r145g5-GB-9` |
   | MC | `w145c-MC-GC-19`, `r145g5-MC-15` |
   | GC | `r149p3-GC-22`, `r146a-GC-22` |
   | RC | `w145c-RC-TC-19`, `r145c-RC-20` |
   | RA | `r143h-RA-16` (tournament only, and the oldest log in the set) |
   | Vengeance | `c147-nv-single`, `c148-nv-tt5`, `eyecheck-pause149b-nv-quit`, `eyecheck-dlc-course-record-nv4` |

   Most of the older logs are 3-lap tournament races. Against those, only the load, route, trigger
   and FPS lines are compared, not lap times or finish order.

   Nine courses have no recorded Free Play log: TB, TC, MB, SC, GB, MC, GC, RC and RA. For them the
   Free Play part is new evidence, backed by the receipt table.

### 5.2 Runner additions needed (R11, test-only)

* **`--laps N`.** Sets the Course-screen laps row to 1 before Confirm.
  * This is the original Free Play Duration setting: `laps14EC` (1..25), saved as profile `laps704`
    (`D:\Programming\GitHub\RacerRevengePC\platform\linux\native\original_ui_course_session50kg.cpp:281, 538-544`).
  * It goes through the original `moveLaps` widget path, not a memory poke.
  * The tournament stays at 3 laps, because the original fixes it there
    (`D:\Programming\GitHub\RacerRevengePC\platform\linux\native\original_ui_frontend50kg.cpp:874`).
* **Pause RESTART.** `--pause-restart SECONDS` already exists.
* **Hall or Course return.** The record outcome decides which screen comes back.
  * With `--seed-slow-hall CODE`, a 1-lap finish sets a lap record, and 1AB884 opens the Hall.
  * Without it there is no record, and 1AB908 returns to the Course screen.
  * Race and knockout records need 3 laps and 8 racers (the 1AC440/1863B0 rule). So a 1-lap race
    makes lap records only. That matches the original and is enough for the Hall path.

### 5.3 Matrix: 17 runs

Pods and courses:

* Player pods rotate, so 17 different stock pods are the player.
* The other 6 stock pods (21-26) still load as AI in these fields, and the host check covers all 23.
* Vengeance runs as `--addon-course n64-vengeance/nv` with
  `RR_DLC_ROOT=D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\roots\addon-arcade-146\DLC`.

**Single Race runs (14).** Each is `--laps 1 --pause-restart 20 --race-full`, then Results, then the
Hall (seeded-slow rows) or the Course screen (no record). Each run covers:

* the fresh race start (1A6AA0 tail);
* the in-place Pause RESTART (1BD934 → 1AC8E0(race,1));
* the finish;
* Results104;
* the 1AB5BC Free Play row.

**Time Trial and tournament runs (3).** These prove the remaining flow logic once.

**Stop-or-go check.** Runs 1-3 come first. Stop the round if any of them fails.

| # | Course | Mode | Player pod | Ends on | Proves |
|---|---|---|---|---|---|
| 1 | TA | Single Race, 1 lap | 1 | Hall (seeded slow) | fresh start, in-place restart, Results, Hall |
| 2 | RA | Tournament, 3 laps (save `D:\Programming\GitHub\RacerRevengePC\work\scratch\saves\order4.bin`) | 16 | tournament return (race_ack) | RA on the common path: 7-cut intro through the one camera session, tournament end rows |
| 3 | GC | Time Trial, 1 lap, seeded slow | 17 | records page 3 → Hall | in-place TT finish restart 1ABC0C (best lap CB4 kept); Pause QUIT 1BDAB4 → 1ABFC0 → page 3 → 1CA6A4 → Hall; water |
| 4 | SA | Single Race | 2 | Course | |
| 5 | MA | Single Race | 3 | Hall | |
| 6 | GA | Single Race | 4 | Course | |
| 7 | TB | Single Race | 7 | Hall | |
| 8 | TC | Single Race | 8 | Course | |
| 9 | MB | Single Race | 9 | Hall | |
| 10 | SC | Single Race | 10 | Course | |
| 11 | GB | Single Race | 11 | Hall | |
| 12 | MC | Single Race | 12 | Course | |
| 13 | GC | Single Race | 14 | Hall | |
| 14 | RC | Single Race | 15 | Course | |
| 15 | RA | Single Race (RA Free Play, zero cuts) | 18 | Hall | |
| 16 | Vengeance | Single Race | add-on pod (`D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\roots\addon-arcade-pods-146`) | Course | add-on course and add-on pod on the same path |
| 17 | Vengeance | Time Trial, 1 lap, no record | 19 | Pause QUIT → 1BDB5C → Course | TT no-record QUIT branch; add-on course in Time Trial |

**Time per run.** The recorded logs show a 3-lap tournament run at 360 s. A 1-lap race is about 1.5-2
minutes, plus loading, the restart, Results and the return screen.

| Runs | Each | Total |
|---|---|---|
| 14 Single Race | about 3.5 min | about 49 min |
| 2 Time Trial (1 lap, restart, about 20 s of the second run, QUIT, end screen) | about 4 min | about 8 min |
| 1 tournament on RA | about 6 min | about 6 min |
| **17 test runs** | | **about 63 min** |

### 5.4 Picture checks (outside the tests)

The runner would allow frames inside these runs: a `--pause-restart` run keeps frames unless
`--logs-only` is given (`D:\Programming\GitHub\RacerRevengePC\tools\capture\run_windows_takeover.py:443-445`).
The repo rule is "no screenshots in tests", so the picture checks stay separate.

* **Runs:** 14 short runs, one per course plus Vengeance, about 1 minute each, so **about 14
  minutes**.
* **Tool:** the frame grabber in `D:\Programming\GitHub\RacerRevengePC\work\scratch\frames`, on the
  live window outside the tests. The user allows this.
* **What to look at:**
  * fog, sky and lights;
  * water on GC and MC;
  * particles;
  * gates and doors where they are visible early;
  * AA1 crowd seats on TB and RC, and on the seven courses that gain AA1;
  * RA's static fire, through the one draw path.
* **Compared with:** the existing frames in the `D:\Programming\GitHub\RacerRevengePC\evidence\eyecheck-*`
  folders.

**Total game time: 31 launches, about 77 minutes.** That is 17 test runs (about 63 minutes) plus 14
picture grabs (about 14 minutes).

To get nearer 60 minutes, the picture grabs can be folded into runs 1-17. That saves about 14
minutes, but it breaks the "no screenshots in tests" rule, so it needs the user's yes (section 6).

### 5.5 Pass criteria

Every run must show all of the following:

* **Clean exit:**
  * `common_run_summary result=pass`;
  * no `refused` or `require_failed`;
  * `course_archive_release released=1`.
* **Data:**
  * every `common_track_data` and `common_pod_receipt` line is `receipt=match` (stock), the same as
    the host check;
  * the `common_load` steps appear in the 2.1 order, with no refusal;
  * the `common_storage` counts equal the receipts.
* **Restart:** `common_race_start reason=restart pc=1AC8E0` with no session reload, meaning no second
  `Starting a fresh` line and no archive release in between.
* **Race and return:**
  * lap count 1 for every finisher, from `common_lap`;
  * Results reached;
  * then the expected Hall or Course screen (`frontend_race_return`).
* **Against the recorded logs** (5.1 item 3):
  * the same intro cut count where an intro plays;
  * the same trigger fire set within the lap driven;
  * FPS within 2 FPS of that course's newest log.

Extra checks for three runs:

* **Run 3 (GC Time Trial):**
  * `common_race_start reason=restart caller=1ABC0C`, with `best_lap_cb4` unchanged across the
    restart;
  * `pause_quit_record qualify_1ABFC0=1 page=3`, then the Hall, with the record in the stock save.
* **Run 17 (Vengeance Time Trial):** `pause_quit_record qualify_1ABFC0=0`, then the Course screen.
* **Run 2 (RA tournament):**
  * 7 cuts (`common_track_data role=cuts value=7 receipt=match`);
  * the tournament `race_ack` sequence of `r143h-RA-16`.

### 5.6 Expected differences against the recorded logs (not regressions)

* **RA Single Race:** it is now the counted race with zero cuts and ends on Results and the Hall.
  Before, it ended on the host text panel.
* **Restart:** it is in place, with no reload. The Time Trial best lap is now kept.
* **TT Pause QUIT with a record:** it now opens the records page. Before, it went to the Course
  screen.
* **AA1:** it now applies on TA, TC, MA, MB, MC, SA and SC, wherever it is authored.
* **Receipt mismatches:** any `receipt=differs` row is listed, with its reason, before the build
  (3.5 step 3).

### 5.7 When a run fails

* The `common_run_summary first_refusal=` field names the step, the role and the owning writer, and
  that writer fixes it.
* Fixes are batched as .cpp-only changes wherever the frozen interfaces allow.
* The builder relinks once. Only the failed runs plus runs 1-3 are rerun, at most about 25 minutes.
* A header change after the build means a second build. W0 must approve it, and it is treated as an
  exception.

---

## 6. Risks and decisions

### Risks

1. **Big-bang risk.**
   * About 330 files are retired at once.
   * The 1A9EE0 chain has never run on 12 of the 13 courses, so the first round will probably fail
     somewhere.
   * Mitigations: section 4 makes each failure name its step and owner; the host check catches data
     faults before any game run; runs 1-3 stop a broken round within about 15 minutes.
2. **Thinner race coverage, by design.**
   * Each course gets one 1-lap race. So behaviour on laps 2-3 of each course (triggers that fire
     only on later laps, 3-lap race records) is covered only by the receipts and the RA tournament
     run.
   * Nine courses have no recorded Free Play log to compare against.
   * Mitigations: the receipt table holds every pinned constant, extracted before deletion; and
     because the flow is common, proving it once covers every course.
3. **Interface freeze discipline.** All 16 writers depend on W0 being right. Every late change goes
   through W0, one at a time, before the build.
4. **Headers not yet measured.**
   * They may push the build past the full-rebuild threshold.
   * If so, the build takes about 3 hours instead of about 75 minutes, but it is still one build.
5. **Renderer.** RA moves to the one draw path. This is checked by the 5.4 frames.
6. **FPS on GB.** The common trigger, particle and collision owner is sized from data. This is
   checked against GB's newest log.
7. **Tool fault.**
   * `D:\Programming\GitHub\RacerRevengePC\work\scratch\batch145\results-test149.ps1` has a stale
     runner path and a stray line.
   * The round uses R11's `round.ps1`, which also needs the new `--laps` option.

### Decisions for the user

1. **Picture grabs inside the test runs** (5.4).
   * This saves about 14 minutes and brings the total to about 63 minutes.
   * It goes against the repo's "no screenshots in tests" rule.
   * Default: keep them separate, about 77 minutes in total.
2. **Fault policy** (5.7): .cpp-only fixes, one relink, and a rerun of only the failed runs plus runs
   1-3. Default: yes.

Everything else takes the faithful choice:

* the original 1A9EE0 track load;
* the in-place 1AC8E0 restart;
* the 1BDAB4 records page;
* the original 1-lap Duration setting for the short races;
* AA1 wherever it is authored;
* no host Pause fallback;
* no per-course or per-pod code.
