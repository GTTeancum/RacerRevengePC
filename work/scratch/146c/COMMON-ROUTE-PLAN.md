# One common race route for any course code (add-on courses): sizing and plan (146c, 2026-11-16)

Not built. Inventory from the tree at 3cd4443 (146c branch). Goal: an add-on course (code from the
registry row, archive `dlc:/<pkg>/TRACKS/<CODE>.RES`) races through ONE route whose every course
decision comes from data; stock codes keep their loaders for now (flagged stand-ins). No add-on code
may borrow a stock course's loader under another name.

## Size

About 30 files for "the MA/SA/GA data chain takes the course code as data"; 40-50 for a route with no
`isSaTrack` at all. Header round: `original_race_constructor50la.h` (52 includers), camera start
course enum `original_camera_start_effects.h` (30), `original_camera_session50kx.h` (25),
lighting scene enum `original_gc_gameplay_lighting_candidate.h` (24, AVOID list ~167 TUs),
`authored_runtime_ground50cp_fixture.h` (24, AVOID list ~89 TUs), `track_materials50fa_fixture.h` (28),
`course_trigger_sources50ct.h` (14), `ai_cold_owners50iu.h` (13), `original_track_cut_identity50la.h`
(13). One batched header round reaches roughly the whole terrain_interactive / platform set
(~200 TUs, a near-full rebuild).

## Work items (each = make the decision data, keyed by the row)

1. Menu -> race handoff (blocks everything): `original_course_load_selection50kg.h` (course<13),
   `original_ui_rider_profile50kg.cpp` courseRaceSnapshot (4-bit course), `original_course_race_targets50kx.h`
   (13 targets, Catalog/availability/ColdLoadRequest), `original_ui_frontend50kg.cpp`,
   `host_track_selection50im.h` (HostTrack50IM gains one "registry course" value + the port-owned number
   from `dlc_course_entries50dl` selection), `terrain_interactive_drive50du.cpp` 208-350.
2. Session row: `track_session_assets50ig_fixture.h` select() builds a row from the registry for a
   non-stock code (archive dlc:/ path, `tracks\m<code>.spl` / `a<code>.spl`, `.tun` from the archive or
   the inherited stock archive, counts from the SCB, archiveSha256 = the listed SHA so the ground's hard
   SHA gate (`authored_runtime_ground50cp_fixture.h` 77-82) is satisfied by the actual bytes).
3. Session flags: `terrain_interactive_session50du.cpp` 133-400: one `commonCourse` flag for a
   registry code; admission = the MA/SA rule (Player0 + 7 AI, mode2, fresh race); no isXx alias.
4. World owner: `ma_sa_original_world50lm.h` + cold .cpp + `original_single_pass_world50lm.inl`:
   member and mount names built from the code (`TRACKS/<CODE>.SCB`, `TRACKS/<CODE>.RES`); the stock
   SHA/count/texture pins stay receipts; `track_source_fixture::Track` gets no add-on value: the
   course-keyed switches (`track_visual_profile_fixture.h` 44-65, `ma_sa_atm_animation_fixture.h`
   134-150, `track_materials50fa_fixture.h` 258-279) take "no stock profile" for a registry code.
5. Triggers: `course_trigger_sources50ct.h` stockSa member `tracks\sa.tob` -> `tracks\<code>.tob`;
   the SA trigger chain refuses Animation/PodSound (fine: the add-on TOBs author none / LostAI only).
6. ATM: `ma_sa_atm_animation_fixture.h` 233/258/275 hard SHA gates -> absent in data = no owner; a
   non-stock archive that authors ATM needs the data ATM reader (REFUSALS round 2).
7. Lighting: `prepareSaNative` decodes the SCB given; scene enum gets the common scene (capacity 25
   zones/portals already a capacity; .cpp only except the enum).
8. Camera: `createNativeSaCameraSession50LA` -> a common one; `trackMember` hard checks against
   `originalTrackCutIdentity50LA("SA")` -> receipt; cut rows from data (`course_cut_identity50ci.h`
   derive already builds names from the code; the 13-slot runtime table in
   `original_track_cut_identity50la.h` 114-125 gets one registry slot).
9. Intro: `original_ta_intro_authoring_candidate.h` 13 specs + 4 platform `trackId()` whitelists ->
   intro from the archive INIs (0 events accepted, cameras still play).
10. Material audio: `material_audio_provider50hi_fixture.h` reads `discRoot/TRACKS/SA.RES` and
    `tracks\sa.aub` -> the session archive and `tracks\<code>.aub`.
11. AI: `ai_cold_owners50iu.h` bindFreshAiCreateSelection50IU -> registry row: race class = ai_class,
    record pod = rival; no 1125F track id (an add-on has no tournament order).
12. Consumed SA stage: `original_consumed_sa_stage50du.h`, `original_sa_world_cold_prefix50la.h`
    require HostTrack50IM::SA -> the common stage; race storage `saWorld74_` in
    `original_race_constructor50la.h` (52 includers).
13. Loading screen: `authored_loading_presenter50du.h` 129-135: 13-code hash table -> receipt; the file
    from the row (`dlc:/` via host(), or the named stock LD_<CODE>.PST).
14. Names: LOC lookups on Course screen / loading / Results -> package strings for a registry course.
15. Course screen: appended controls (plan in `dlc_course_entries50dl`), hologram card = template's
    hologram with the preview texture, background from course_background; files: course selection,
    widget controller, widgets, hologram assets, session, draw contribution/resources, rider profile
    commit (keeps 1125E / course700 stock).

## Order

Round A (one batched header round, ~200 TUs): items 1-12 headers together. Round B (.cpp only):
13-15 plus fixes. Verification: 13 stock courses unchanged (receipts stock=1), then each of the five
add-on courses raced to Results with rotated pods, then the Course-screen eyecheck.

## Line anchors (tree at 3d80f07, before the main / 146p merges; re-check after 7aa2051)

- session50du.cpp: 133 select, 137-150 isXx flags, 155-181 xxOriginalDisc / directOriginalHuman, 193-199 course
  admission, 244-256 lighting scene aliases (stand-in), 302-324 per-course require, 327 bindFreshAiCreateSelection50IU,
  448-449 / 560-561 course target == loadedTrack.
- drive50du.cpp: 208 select(RR_TRACK), 239-256 admission list / effectiveTrack / rider 19, 330-350 consume -> runSession50DU.
- world50du.cpp: 1094-1112 SA world load + consumedSaStage, 1286-1301 SA triggers (stockSa), 1807 gate policy OR.
- setup50du.cpp: 1207 material audio Track, 2597 prepareSaNative.
- continuation50du.cpp: 2791 mesh, 2829 trackSource enum, 2839 cold capacity, 2842 decoder, 2880 normals,
  2936 courseWorldArchive, 3047 sky per zone, 3091 binder admission.
- publication50du.cpp: 719, 806, 898-903, 1023, 1143, 1294-1299 ATM, 1444 readiness, 1516 HUD, 1666-1747 selectors,
  2248 createNativeSaCameraSession50LA.
- loop50du.cpp 3065 zone sky; cold_contacts50du.cpp 227; frame_physics50du.cpp 295, 333.
- Hard gates to turn into receipts: ma_sa_atm_animation_fixture.h 233/258/275; camera trackMember
  (original_ta_camera_issuer_candidate.h 31-45); ground SHA (authored_runtime_ground50cp_fixture.h 77-82: supply the
  listed SHA instead).
- Handoff: original_course_load_selection50kg.h 11-13; original_ui_rider_profile50kg.cpp 1369-1374;
  original_ui_frontend50kg.cpp 880-883; original_course_race_targets50kx.h 31, 42, 60-65, 200-251, 275-317.
