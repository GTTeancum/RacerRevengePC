# RacerRevengePC: items 1 and 2 source handoff

## Status and scope

This batch prioritizes normal selected-racer/track startup and essential menu/input routes for beta handoff. It is **source-only, uncompiled and untested**. Earlier passing checkpoints do not establish that this batch compiles or works at runtime. No new GUI runs, racer/track sweeps, lap-completion tests or finish/Results tests were performed.

**Items 1 and 2 source integration complete:** the distinct racer-manager/record actor attachment, canonical subtype-3 profile transition to one live human, original rank-zero pose, RA/non-RA camera and radar wiring are integrated. The initial Course receipt is preserved; fresh native Restart repeats the actual profile clear without replaying that receipt. This is code completion for the defined startup/menu/control scope, not evidence of compilation or runtime correctness. Work stops here for user sign-off before any build or testing.

## Normal entry and input

Once an approved build is available, the normal entry is:

```sh
./terrain_interactive_drive50du --assets /absolute/path/to/original/disc
```

Use the original disc directory, including its original UI, track, pod and AUDIO files. Normal entry does not require a generated runtime data pack or a diagnostic corpus argument. The command is documented here, not executed for this batch.

`--assets` selects original Main and sets asset/UI roots. When not explicitly supplied, it defaults `RR_RACE_MODE=2`, `RR_START_MENU=0`, `RR_ORIGINAL_MAIN_AUDIO=enabled`, and the Main audio root to the disc's `AUDIO` directory. Native runtime mode 2 is distinct from the original menu's game-mode field. Explicit policies are preserved. `RR_AUDIO_DEVICE=0` disables physical audio output without removing logical audio ownership; explicitly selecting the diagnostic muted Main policy leaves Game Options Accept unavailable because that policy has no mixer owner.

An accepted Course Confirm retains its actual input device: controller A selects controller driving; keyboard Enter/Space selects keyboard driving, winning simultaneous confirmations. An explicit `RR_HUMAN_INPUT_DEVICE=keyboard` or `controller` overrides this choice. Direct diagnostic startup defaults to keyboard.

Normal keyboard rows use W/S for throttle/brake, Left/Right to steer, and Up/Down for original left Y. Advanced rows use W/S for the left engine and Up/Down for the right engine. Space boosts, R holds repair, Left Shift powerslides, and Escape opens/resumes Pause. F5 or the Restart Race menu requests a new grid session. Controller Start opens Pause or resumes its main page; D-pad selects, A confirms, and Y backs out/cancels. Restart and Main return retain their confirmation menus. These are implemented mappings, not new runtime verification claims.

## Delivered source changes

### Normal selection and original-disc loading

- Ordinary Main/Rider/Course entry no longer depends on diagnostic launch defaults. Accepted Course selections use the actual selected identity and cold-reconstruct their session, including unchanged rider/track selections, so retained control choices are applied.
- Tournament game mode 2 now has its own launch proposal and exact four-write transaction, preserving its fixed three-lap, eight-entrant, one-human/seven-AI semantics rather than pretending to be game mode 0. The original mode-0 transaction remains separate.
- Fresh Career selection and selected Garage entry no longer hardcode racer 19. Authored rider locks, selected identity and clean-career constraints remain in force; this is not a universal unlock or resumed-career implementation.
- Selected Garage resources use the actual rider's original E.RES and PSG, with exact archive/member provenance through scene, model-material, texture-cache and downstream resource owners. Catalog entries cover the original supported career identities; original bank and scene constraints remain enforced.
- RA now has a genuine selected-human loading path, rather than an AI-only race presented as human driving.
- Course event types 1/2/3 have authored navigation, widget visibility and actual-versus-remembered entrant semantics. Lap selection retains its 1..25 range and entrants their 1..8 range.
- Live roster ownership uses actual admitted counts rather than fake padding to eight. RA/non-RA triggers, GB/MC query retirement, HUD adapters, audio and two-phase scheduling carry the live prefix. Singleton native bank/contact/peer ownership is supported. MC particle vehicle construction tracks expected/initialized live racers while preserving fixed per-vehicle effect topology.

Principal paths:

- `tests/native/terrain_interactive_drive50du.cpp`
- `platform/linux/native/original_ui_frontend50kg.cpp`
- `platform/linux/native/original_ui_course_session50kg.cpp`
- `platform/linux/native/original_ui_rider_profile50kg.h` and `.cpp`
- `src/game/original_course_load_selection50kg.h`
- `src/game/original_ui_transition_owner50kg.cpp`
- `tests/native/original_course_race_targets50kx.h`
- `src/assets/original_ui_archive50kg.h` and `.cpp`
- `src/assets/original_ui_garage_selected_member_catalog50kg.inc`
- `src/assets/original_ui_garage_scene_assets50kg.h` and `.cpp`
- `src/assets/original_ui_garage_model_materials50kg.cpp`
- `src/assets/original_standard_texture_resource50la.cpp`
- `src/assets/original_ui_ipum_texture_cache50kg.h` and `.cpp`
- `platform/linux/native/original_ui_garage_session50kg.cpp`
- `platform/linux/native/original_ui_garage_page_owner50kg.cpp`
- `platform/linux/native/original_ui_garage_model_resource50kg.h`
- `tests/native/mc_particle_vehicle50mc.h`
- `tests/native/ra_cold_aggregate50la.h`, `ra_cold_roster_operation50la.h`, `ra_cold_roster_resources50la.h` and `ra_native_session50la.h`
- `tests/native/gb_query50gb.h`, `mc_query50mc.h` and `mb_source_trigger50mb.h`
- `tests/native/original_position_stats_adapter50la.h`, `original_position_stats_resources50la.h` and `original_damage_observation50la.h`
- `src/game/original_time_stats50la.h`
- `tests/native/original_ai_audio_owner.h` and `original_all8_two_phase_schedule.h`
- `src/game/original_pod_collision_response_owner.h`
- `tests/native/native_racer_bank50hn_fixture.h`, `native_racer_contact50hw_fixture.h`, `native_racer_contact_bank50hw_fixture.h` and `native_dynamic_contact_peers50iw_fixture.h`

### Essential menus, controls and startup owners

- General menu entry retains existing authored routes and unavailable-state checks. Fresh selected-career Garage Entry, Back, Cancel and unchanged Accept now bind the selected resources instead of racer-19 resources. This batch does not add purchase/progression/save behavior.
- Normal entry retains the accepted keyboard/controller choice across the frontend-to-race cold handoff. Native Pause gains actual controller navigation/Confirm/Back handling, rather than Start-only access.
- Camera banks admit the actual 1..8 count. Route-less, reduced-roster and subtype-2 starts use the source direct-start ordering through Follow, preferences, countdown, viewport, music and UI; the full-eight subtype-3 procedural intro remains distinct.
- Practice startup music requests 44/45 use original source request semantics and original `PRACTC2.VAG`/`PRACTC1.VAG` metadata through the retained music owners. A separate two-clip cold profile preserves the existing PCM budget instead of loading all fourteen clips or substituting a silent success.
- The fresh subtype-3 no-record actor receives its own canonical course rank-zero grid pose. It does not copy the player's rank-four pose. The helper uses the actual selected typed or legacy map, selected lift, shared orientation and existing matrix construction. The actual manager attachment/root wiring is integrated. The no-record actor is a bounded native representation; persisted ghost replay and complete retail Player-constructor parity are not claimed.

Principal paths:

- `platform/linux/native/host_session_menu50if.h`
- `src/game/original_ui_course_widget_controller50kg.h` and `.cpp`
- `src/game/original_race_start_camera.h` and `.cpp`
- `src/game/original_gc_camera_start_lifecycle.h`
- `src/game/original_camera_start_effects.cpp` and `original_camera_start_native.h`
- `tests/native/original_camera_session50kx.h`
- `tests/native/original_ui_main_entry50kg.h`
- `tests/native/original_frontend_session50la.h`
- `tests/native/terrain_interactive_drive50du.cpp`
- `src/game/race_music_event50ie.h`
- `tests/native/race_music50ie_fixture.h`
- `tests/native/original_record_grid_pose50la.h`
- `tests/native/selected_grid_navigation50ig.h`

## Separate work and verification boundaries

- Item 3 persistence, resumed-career progression, new purchase/save behavior and replayed recorded ghosts are not claimed complete by this menu/startup batch.
- Fresh no-record actor construction is distinct from replaying a saved ghost. Pending source integration must not be described as ghost playback support.
- Lap completion, race finish and Results flows remain outside assistant testing. Full racer/track combinations and broader functional coverage belong to beta testers, not a repeated assistant sweep.
- No new build, runtime or publication result is claimed. The user must sign off before testing resumes. Retained source identity checks and explicit unavailable paths must not be weakened to make an unimplemented route appear complete.

## Frozen source checkpoint

- Candidate: `full-app-beta-routes-20261025-v100`.
- Build/test status: not compiled, not tested; no new GUI runs.
- Publication: this batch is not published; GitHub main remains `a2f7dd962750d50f63c778302bcf708a6f9c7681` (build 94).
- Parent source integration receipts and exact changed-file pins are retained beside the candidate.
- Stop point: items 1 and 2 code-complete; awaiting explicit user sign-off before verification.
