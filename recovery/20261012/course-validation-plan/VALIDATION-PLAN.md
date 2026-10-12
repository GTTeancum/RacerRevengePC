# Source-only centralized Course validation plan

Prepared 2026-10-12. No command in this document has been executed. Both new
candidate sources are uncompiled, unimported, untested and not syntax-checked.
Main is owned by root; these files are outside the checkout.

## Result and blocker

The intended route is actual integrated Main 4/page1 → Rider 5/page1, initially
Rider19 → Confirm → Course 6/page1 → Back → a fresh Rider 5/page1. “Back” here is
Course Back to Rider, not an extra Rider Back to Main. No race launch is admitted.

The current runtime owns this bounded path only with explicit muted UI audio,
fresh mode0 one-human shared profile, Course slot1/subtype1/laps3/entrants8,
Planet0, no active Card popup, and authenticated Rider IPUM cache. Course uses
the `native_host_rgb` presentation policy. This is real native menu integration,
not an isolated diagnostic renderer; passing does not establish original GS
composition/order or retail-pixel equivalence.

The five decoded Rider19 RRTEX files and their manifest are currently missing.
No durable cache was found in Library after exact/fuzzy cache searches and
complete recent inventory. Authentic original archives and full ELF are local.
The compiled cache catalog already contains all five real hashes and
`retainedCatalogAvailable=true`; old prerequisites saying otherwise are stale.
Do not change or replace those catalog pins to make a new conversion pass.

## Centralized scheduling and source freeze

- Last completed centralized session: 2026-10-12 03:35:12.454092 UTC.
- Earliest next session: 2026-10-12 04:35:12.454092 UTC. No slot is assigned.
- Root exclusively owns `/workspace/shared/rr-central-test.lock` and
  `/workspace/shared/rr-last-central-test.json`. Eligibility alone is not approval.
- All extraction-tool compilation, cold conversion, native compilation and the
  single menu launch below count inside the same assigned session. Do no probe,
  compiler-version call, import, syntax check, asset conversion or test before it.
- Freeze root's final integrated commit and record its full SHA. At source review,
  the main ref still named bfe1552d0fddbae5a1a0da7900e73782926992e4 while Course
  integration was in progress. That SHA must not be claimed as the final Course
  binary's source. No working-tree edits while compilation/observation runs.
- One launch only. A preparation, compile, receipt or runtime failure ends this
  focused attempt. No fallback rasterizer, replacement art, alternate menu
  driver, manual key retry, second executable or second take.

## Local inputs and exact staging

Repository: `/workspace/shared/rr-recovery-git`

Prepared native assets and process working directory:
`/workspace/shared/recovery-20261010/native-assets-prepared-01`

Its real native corpus:
`evidence/staticType2Producer49X/original-word/native-fixtures.json`

Original ELF, read as authenticated data only:
`work/native/project/disc/SLUS_202.68`
(2,972,720 bytes; SHA256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`).

Cold all-AI world audio files:
`work/native/project/disc/AUDIO`. These are available. Cold all-AI setup still
requires `RR_AUDIO=1`; `RR_ORIGINAL_MAIN_AUDIO=muted` independently gives the UI
no output/PCM owner. The original-UI branch prevents world ticks and world output
opening. Do not set RR_AUDIO=0 with RR_ORIGINAL_AI_ALL_RACERS=1: startup rejects it.

Original UI tree:
`/workspace/shared/input-recovery-20261012/extracted/RacerRevenge-original-game-UI-menu-and-videos/UI`

APPSTART.RES and LOC.RES:
`/workspace/shared/input-recovery-20261012/extracted/RacerRevenge-original-game-APPSTART-LOC`

The old recorder's UI_ROOT and AUDIO_ROOT hardcoded recovery paths are absent.
Stage a NEW unified ordinary directory, proposed:
`/workspace/shared/recovery-20261012/course-validation-01/ui-root`
with APPSTART.RES, LOC.RES at its root and the actual original UI tree below UI/.
Required archive list (all native-authenticated by original_ui_archive50kg):

- APPSTART.RES; LOC.RES
- UI/TEXTURES/OVERLAYS.RES; UI/UIFONTS.RES; UI/CONTPOP.RES
- UI/UIMAIN/UIMAIN.RES; UI/UIOPTION/UIOPTION.RES
- UI/UIBKG.RES; UI/UIPODTEX.RES; UI/PODPHYS.RES
- UI/UIRIDER/UIRIDER.RES; UI/UICOURSE/UICOURSE.RES

UIRIDER.RES: 5,552,683 bytes, SHA256
`112cd71ac9cfcc909b0edb56e0e420bb358b7078b5abae5f48c62b95a6bbf856`.
UICOURSE.RES: 1,227,424 bytes, SHA256
`f45585dd7fe7ba2a24cfc5b6ce4d3fb5efa6bc279f636e071aecf5ccf970c855`.
All twelve exact identities live in src/assets/original_ui_archive_catalog50kg.inc.

New cold cache proposal:
`/workspace/shared/recovery-20261012/course-validation-01/ipum-cold/evidence/ui-rider-ipum50kg`
containing five 19c_1-mip0.rrtex … 19c_1-mip4.rrtex and manifest.json.

## Missing cache preparation

`extract_rider19_pst_candidate.cpp` is a new narrow wrapper around the existing
OriginalUiArchive50KG owner and import_decode48c implementation. It authenticates
the entire exact rider archive and member catalog before publishing only the
fixed member `ui\textures\19c_1.pst`. It does not follow member names as output
paths, execute original instructions, create a fixture, or synthesize textures.
It requires logical offset1,228,800, extent23,096 and the retained PST SHA256:
`ad0bcd92f415d65d8bf68ae9e65fe355043e297e834cdffeae4ceac7cfa60cd3`.
Output must be a new directory; fixed output filename is 19c_1.pst.

Then run existing tools/assets/recover_rider_ipum50kg.py with the exact archive,
PST and ELF. It authenticates the unchanged 50FC decoder helper, inspects original
ELF anchors as data, and uses installed FFmpeg's native host IPU codec. No guest,
VU, PS2 or emulator execution. The five decode subprocesses each have a30-second
bound, and the version call15seconds. Root may apply a180-second phase ceiling.

Compare generated RRTEX hashes to the existing compiled catalog:

0 fe3d1143c277222cb007cd8135cf6cddb79558a6280c318378a329ef99b66575
1 9698ecfcc57039c0f2556a2e64eb0e9b7353d9400b3f7624111a912919ac48bd
2 1eeb1faf0a0379d50a02faefd99d47b1d7fce3d523bf51243e2c0a6a1e4ee114
3 c02fc660a81ade4c0016fa5f9739b8ebffd8ca470b0b3a9ab89aeb5dd6730afb
4 57c5076fb52f906996b8d74726e7089bb08344463e377063a480c566c6c56bae

Retained codec provenance is FFmpeg7.1.5-0+deb13u1; executable hash
e8a8d46f5225f3062cec7c07fb145d58ae73c603cb740dcd5bad34bfb54e455a.
Neither its present availability nor regenerated byte parity has been executed
or probed for this plan. Native cache loading authenticates the byte pins. If
conversion differs, keep evidence and stop; do not overwrite main's generated
catalog or silently substitute an image. The generated catalog output is
retained provenance, not a requested main edit.

## Build once, then one observer launch

Use root's integrated tools/prepare_interactive_build50dv.py to write the native
build argv. It includes the sixteen new Course compilation units, Rider Confirm,
runtime/header routing and actual native rendering owners. Do not compile only
the detached Course fixture and call that integrated validation.

Existing dependency/object cache from the successful GC build:
`/workspace/shared/recovery-20261012/gc-light-fog-validation-01/objects`

Existing build_focused_incremental50ci.py validates compiler flags, dependency
hashes and object hashes before reuse. It recompiles changed runtime/header
dependencies and new Course sources and relinks. It is serial. Its last-build.json
is overwritten in the cache; copy that receipt to the new session immediately.
Pin final executable SHA256 and diagnostic-header SHA256 only after build.
Provenance must retain final source commit, source state, full argv, build logs,
compiled/reused objects, native executable identity, recorder and asset hashes.
Old GC executable does not contain current Course integration and is not usable.

The new recorder candidate is independent of the enabled-audio Rider recorder:
`record_course_roundtrip_candidate.py`. Root must statically review and integrate
or explicitly accept it. Source-only review caught and corrected two critical
differences: settled profile generations are1,1,3,4, and muted audio time stays0.

### Ready and input gates

1. A first actual Main submission must emit original_main_ready native_frame1,
   width640,height448 within180seconds. That receipt alone does not open input.
2. Obtain a freshly acknowledged Main image/state with route4/1 switches0,
   selection group1/slot1/saved1, profile mode4/generation1. Require input_shown,
   aperture_input_enabled, aperture_state0, lifecycle0, phase0.
3. Send down Enter + capture in one bounded stdin write; preserve that image and
   immediately matching source-state ACK. Then up Enter + capture, and await its
   separate ordered capture barrier. Do not retry a partial write.
4. Wait for Rider5/1 switches1, Rider19/slot1, attached selection, actual accepted
   source model frame, nonzero draw/triangle/bank/registry evidence, panel_y0,
   input_blocked=false, prompt_mask3, all generic open gates, mode0 one-human
   profile generation1, remembered[19,19,19,19]. Observe at least1real host second
   with advancing source_milliseconds before the second down Enter/capture and
   up Enter/capture pair. The source controls actual _sel animation/panel exit;
   no fixed-duration override or ready flag injection.
5. Course must activate screen6 and produce matching original_course_state and
   capture ACK with switches2, page1, presentation=native_host_rgb,
   accepted_native_frame=true, nonzero native draw/quads, course1/subtype1,
   laps3/entrants8, popup=false and actual open gates. Profile generation3:
   Confirm commits generation2; mandatory Course entry commits generation3.
   Observe at least3real host seconds with advancing source_milliseconds, then
   only Escape down/capture and Escape up/capture.
6. Back must activate a fresh Rider5/1 switches3 with Rider19, accepted original
   model render, same settled panel/input conditions and profile generation4.
   Observe at least1real host second, then explicit quit. No extra Rider Back.

No directional, controller, steering, throttle, mouse driving, restart,
checkpoint, pause, synthetic course-driver, or Course Confirm input is allowed.
Exactly three press edges and three ordered release barriers are retained.
All eight cold racers remain original-AI configured, including camera0; none
advance while the original menu owns the loop. Every state and final close must
have world_ticks0 and mixed_frames0. Cold gameplay settings do not prove any
gameplay here; menu evidence must be described as menu evidence.

### Capture/acceptance and stopping bounds

- One native process only; clean explicit environment; headless-live640x448.
- Capture target default10Hz, upper limit30Hz, at most920frames. This is a target
  and ceiling, not a measured frame-rate claim. Preserve native duplicate images.
- One outstanding capture. Native original_main_capture JSON must precede its
  corresponding original_main/rider/course_state immediately; match route,
  owner token, accepted update sequence, native frame and relevant model/native
  geometry fields before preserving the complete atomically published PPM.
- Fresh native frame and accepted-update counts must advance for every image.
  Source state, real receipt timestamps and PPM SHA256 are recorded per frame.
  GPU/light token stability is checked per screen lifetime, never globally.
- Main-ready to complete sequence: at most30seconds; startup180seconds;
  each ACK5seconds; each navigation stage12seconds. Quit10seconds, then owned
  process-group SIGTERM,2seconds, then SIGKILL,2seconds. No success with forced
  signals, unreaped process, nonzero exit or partial/unacknowledged frame.
- A successful observation requires all four open checkpoints, activation order
  [5,6,5], exactly3presses/3releases, all requests=ACKs=preserved images, zero
  gameplay/worldPCM, terminal quit state and original_main_closed cleanup=true.
- Any unavailable destination, wrong route, original-AI boundary, host menu or
  gameplay receipt, audio output, wrong source identity, malformed capture,
  deadline miss or source stall is an honest failed attempt. Preserve raw log,
  failure, last accepted frame, pending receipt, and terminal evidence.
- Source rejects an actual active update stall over1second. Do not clamp or
  replace it with synthetic ticks to make the menu progress.
- Native child file ceiling8MiB/file; no core dumps. Raw log32MiB; frame journal
  and event journal16MiB each; stdin16KiB; named PPM payload bound920×860175bytes.
  Native HOME/TMP/cache ancillary files have no aggregate quota claim.
- No encoder or audiovisual mux is part of this minimum. Review retained
  acknowledged frames for actual Main/Rider/Course entry, Tatooine warm palette,
  Course background/hologram/layout and return. An automated “observed” receipt
  establishes only these bounded lifecycle/ownership/input facts, not visual
  parity. A later user-facing clip must use real timings and state limitations.

## Source references and pitfalls

- tools/gameplay/record_original_rider_30hz_recovery.py: useful existing atomic
  capture/real key-edge protocol, but expects three rejected Rider directions/
  Confirm, enabled PCM and final Main. It cannot be reused unchanged.
- tests/native/terrain_interactive_drive50du.cpp: original menu maps Enter to
  input4, Escape to input6; original-UI loop ends in continue before any driving
  packet/world step. Capture adds Course native_host_rgb/native draw evidence.
- tests/native/original_ui_main_entry50kg.h: exact ready/report/activation names,
  real clock sampling, deferred owner switch, common-profile and Course reports.
- platform/linux/native/original_ui_frontend50kg.cpp: bounded muted Course gate,
  cold destination preparation before commit, mandatory native owner retirement.
- platform/linux/native/original_ui_course_session50kg.cpp: real page-entry,
  palette/hologram/aperture and only supported Back input. Active Card unsupported.
- platform/linux/native/original_ui_rider_profile50kg.cpp: Confirm, Course entry
  and Course Back each commit actual shared profile state and generation.
- tests/native/original_ui_main_output50kg.h: muted advance returns without PCM
  clock increment. Never use audio_accepted_nanoseconds as muted dwell time.
- Course ordinary widget color application can affect subsequent settled
  frames; review more than the first activation frame. Source aperture translucency
  is not assumed to be an opaque host cover.
- Windows focus issue remains separate; no user's-PC access or test is involved.

See COMMAND-MANIFEST.txt for the proposed ordered commands. Every placeholder
must be resolved by root from the actual final build; none represents an existing
runnable artifact or a completed validation.
