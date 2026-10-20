# Original-disc-only dependency sweep — October 18, 2026

Latest bounded runtime result (October18 04:58 UTC): Aldar Beedo / Soro Suub Facility reached real GO and throttle movement, Pause, and confirmed Restart rebuilt session3 from original disc files with generated-root absent. Same-run selection and in-race proofs retained. Restarted movement, Main return and finish are still pending; MA has no GUI result yet. The audit and component limits below remain applicable.

This is a source-reachability audit, not all-track runtime certification. The current game entry is `tests/native/terrain_interactive_drive50du.cpp`; its directory name does not make its live dependencies test-only. Runtime checks described below used extracted original disc files and an absent generated runtime directory.

## Verified current milestone

Published `8f49aa9328f10b970335823a5004330dbc84cb40`: Shrivel Braittrand / Mos Espa completed original Main → Rider → Course → race intro/countdown/GO → real throttle → Pause → Restart → real throttle → original Main → clean exit. Same-run selection and race screenshots retained. Anakin / Watchtower has earlier bounded-flow evidence. Neither establishes a complete race finish, every racer, every track or hardware performance.

## Remaining generated-input families

1. Collision/query corpus: legacy `native-fixtures.json`, read by the interactive entry's non-direct cold bootstrap.
2. Loose POD bootstrap geometry/motion/physics/collision files: `runtime_startup50fl_fixture.h` retained branch.
3. Loose tuning/effects/physics members: legacy entry branches instead of original archive-member reads.
4. Roster/import JSON and extracted member files: `native_racer_rig50hw_fixture.h` and `original_course_roster50kg_fixture.h` retained adapters.
5. Per-racer visual catalogs, cacheobjects, RRSCENE/RRTEX and name-provenance files: `original_racer_visual50hw_fixture.h` retained branch.
6. Generic POD01 visual/material pack: retained interactive bootstrap and `pod_materials50ed_fixture.h`.
7. Track meshes/materials/normals: visualGA/GC/MA/SA selections, export reports, RRMESH/RRSCENE and texture cacheobjects via `track_materials50fa_fixture.h` / `track_normals50hk_fixture.h`.
8. Retained decoded source-image files: `source_texture50fc_fixture.h` adapters.
9. Sky-image files: `sky50gw_fixture.h` when original image decoding is disabled.
10. RA crowd-image files when no explicit original asset root is supplied.
11. MA/SA animated texture frame-file dependency has been removed in integrated source. The original ATM/PST decoder passed all 120 fixed source-frame output hashes and 1089 malformed-input checks. The full 354-source application rebuild/link passed. MA/SA live source joins also compiled and passed focused material-adapter checks. GUI validation remains pending.
12. Optional Rider IPUM cache: retained frontend path when original-root native decoding is not selected.

In-memory decoding of original RES/PST/IPUM data into texture buffers is not a generated runtime-file dependency. Compiled authenticity tables are source metadata. Ordinary compiler objects, logs, screenshots and differential-test fixtures are not release data packs.

MA/SA static-world and original-sky component checks also passed. Independent static-image output pins cover 49 of 80 MA images and 7 of 104 SA images; all source images are authenticated, but those partial independent output pins must not be described as complete pixel parity. The reviewed MA/SA live source admission is now compiled, with component association checks passing; no GUI pass is claimed yet.

## Track and flow consequences

- Explicit-original-root TA/GA human routes bypass the identified pack readers. Source validation and bounded GUI evidence support these routes; no global file-access prohibition is implied.
- MA/SA now have coordinated rooted direct-human source joins, original static/sky/ATM readers and material adapters; focused checks pass and GUI validation is pending. Their legacy branches and GC still retain pack consumers. Removing a Course check alone is insufficient.
- RA direct input is presently a specific fixture/ALL8 route, not normal human Course support. Without an explicit original root it also retains sky/crowd file reads.
- GB/MB/RC/SC/TB/TC have metadata but no complete runtime session binding. Authenticated original Course UI also includes MC, The Grand Reefs; the historical twelve-world inventory omitted it. The complete supplied game inventory is 23 pilots and 13 courses, with all thirteen archives present. This does not establish native support or unlocks.
- Original Course currently admits ten authenticated racer identities, not the entire roster. Preserve source unlock/profile rules; never substitute a known racer or synthesize unlocked state.
- Pause/Results do not introduce another identified pack family. Restart repeats cold loading. Finish/HUD/camera/reentry capability must be joined for each newly enabled human track.

## Launch and packaging blockers

`tools/windows-tester-b4c303e/Configure-RacerRevenge.cmd`, `tools/stage_native_runtime.py`, `tools/package_playable50ff.py`, and `platform/windows/native/Launch-Interactive50DU.ps1` are historical retained-runtime paths. They require or package generated corpus/visual/cache data; the Windows interactive launcher also forces ALL8, conflicting with current rooted TA/GA human ownership. Preserve them as diagnostic provenance; do not use their output as an original-disc-only release.

The new `tools/launch_original_disc.py` is development-only: explicit `--assets`, mode2, human keyboard, original Main, separate host menu disabled, inherited RR overrides cleared. It does not convert assets or supply pack arguments. It is syntax/path checked, not Windows runtime certified. Current audio configuration is muted. A clean executable directory and read tracing remain required because legacy relative evidence/work paths still exist in source.

## Release acceptance

Package only the executable, approved launch/configuration files, required redistributable libraries and notices. Users supply original assets separately. Exclude original game bytes, evidence/work/cacheobjects, runtime corpus JSON, generated meshes/scenes/textures/worlds and rider caches. Validate each supported racer/track through normal selection, controllable race, Pause/Restart, genuine finish/Results and return using a clean directory and absent/denied generated paths. Preserve failures and fix native readers/ownership rather than installing a fallback pack or weakening profile/resource checks.

