# Native engineering status — 2026-10-14

This is an implementation list. Broad visual comparison, handling acceptance,
full-race coverage and menu acceptance belong to human testers, and are not
engineering tasks merely because they have not been exercised here.

## Integrated source

- Original Main, Options, Rider and Course screen wiring, with authenticated
  resource ownership and cold race transitions. Main now owns a dedicated UI-only
  context/renderer and does not load race assets before its first menu frame.
  Every Course selection closes the frontend before actual cold race loading.
  The current Course capability
  domain contains TA, SA, MA, GA and GC and racers 2, 4, 10, 11, 12, 15, 19 and 20.
  Capability admission still requires the corresponding prepared original data.
- Original racer rigs, tuning, physical bodies, cables and effects, including
  POD02's five graphics models and the supplemental POD12/15/19/20 owners.
- Per-level authored lighting and fog, default enabled in the admitted gameplay
  path; MA/SA material fog opt-outs, animated assets and sky ownership.
- Original AI and camera ownership, with native state-5 physical recovery now
  connected to the actual visibility, collision, ground and effect owners.
  Recovery returns through the same AI PostPlayer dispatch. GC water-height
  ownership is connected in the admitted default-on GC water path.
- Linux and Windows build registration for the recovery ground-query unit.
  Central source closure is 337 C++ units; Windows excludes the Linux offscreen
  context and uses 336 C++ plus six vendored C sources.
- Original-AI manager initialization, lap-event FIFO, retirement/finish lists,
  native ranking and gap times, plus a one-way Results presentation adapter.
  The explicit native ALL8 session ends only after all eight actual finish
  entries and completion of the final gameplay image. Early finishers keep
  their original AI. This is not retail zero-human end-condition parity.
- Per-racer material-audio arrays and the actual final contact-channel dispatch
  for all eight racers, after particle commands. Native decoded-rate unity-pitch
  policy is explicit; original IOP sample-pitch metadata remains unknown.
  Zero-rate playback preserves logical activity and cursor. Muted host output
  retains real logical audio owners. Two original MA field-hit headers are
  admitted by exact file identity without changing payload validation.

- Eight distinct racer speech owners, authored banks, shared-random cursors and
  cooldowns; contact/damage event routing uses the actual event actor. The
  followed AI no longer runs the human-only damage-speech prefix. Playback
  uses the explicit existing native per-cue transport.

## Remaining engineering work

1. Complete real profile progression and the later Course resource domain.
   GC's nine authored triggers, LostAI target writes, opening animation and
   BIGDOOR audio are integrated, along with static shadow overrides and its
   authored camera count. Water/reset, shared particles, fog, fire, dust, smoke,
   sparks and original-atlas GPU drawing are default on for genuinely admitted
   GC source-camera sessions. The complete native provider graph no longer
   needs blanket incomplete-world opt-in; diagnostic omissions still do.
   Its explicit mode2 and authentic profile locks remain. Adding GC cold Course
   capability does not unlock the authentic four-course fresh profile.
   Native trigger geometry and listener cadence policies remain explicit;
   no runtime or visual result is claimed for this new source.
2. Complete second-controller menu/hotplug and original missing-controller popup
   ownership, plus the separate Gallery, memory-card and Credits paths. The
   genuine player-one Controls page is now integrated with original animated
   controller, all material passes, localized labels and canonical Accept/Cancel
   preset persistence. Selected native keyboard/SDL gamepad transport and cold
   normal/advanced handling are connected to the direct-human route.
   Native human controller feedback now resumes the actual collision helpers
   and suffix, with real absent-device behavior or SDL rumble transport.
   Collision/damage announcer and native timed music fade are integrated;
   IOP interpolation parity is unknown.
3. Complete human damage/destruction, death-camera activation, and remaining
   wreck lifecycle owners. Genuine SELECT/Back manual reset and native T transport
   now share the real Ready-phase physical/camera reset, source five-second
   eligibility timer and actual blur owner. Nonzero-state reset remains guarded.
   Genuine TA human undertrack continuation is
   now integrated through controller/material reset, collision/visibility/body
   reset, ground queries and the existing camera Session. It preserves the
   pre-reset source stack position and resumes the same base frame. DNF presentation now carries actual retirement
   markers, has no invented finish place/time, and handles zero completed laps.
   States1/2 retain the original Follow/Attached camera with their source bits;
   Native death-camera query, pose and retained-query transition code is now
   present with genuine first-use query publication and resource-backed HUD
   text/gleam ownership; live states3/4 still need complete ordered debris.
   One-piece wreck construction, authored contact records, shared collision
   queries, visibility, gravity/water forces, body integration and rebound
   kernels are integrated. Whole destruction activation and live effect/frame
   scheduling are still being joined; these kernels do not establish a working
   wreck lifecycle.
   Lap/record text callbacks and exact HUD animation stops are joined. A bounded original
   lap glyph/gleam GPU path is implemented for the human route; other HUD
   presentation remains native and the original layer is not active for ALL8. Canonical human controller/D44 reset fields are
   consumed by that physical/camera reset integration. Bounded ordinary TA human Course is
   now wired: one real Player, seven original AI, accepted control presets,
   full cold reload, genuine source timing/finish/end camera/audio/fade, and
   native Results with restart/return to Main. Native Results substitutes for
   retail Results/save/progression presentation. Other human tracks and
   second-player lifecycles remain unjoined. Source nonzero-state and missing
   provider guards remain; this is not complete human-race support.
   ALL8 driving checks remain original-AI-only.
4. Continue preparing remaining track/rider/UI resources through the original-data
   converters as required by concrete destinations. SA/POD20 and GA/POD04/POD02
   and MA/POD10 resources now support their verified Windows routes; the seven additional
   Rider kind2 sources are prepared. This does not establish readiness for
   every other track/rider combination.
5. Keep Windows as the primary build target, fixing its compile/link failures
   immediately and staging fixes before the next scheduled smoke. Continue
   Linux support and its assigned builds. The dirty development checkout is
   not a staging or integration target.
6. Implement additional track or lifecycle owners only for concrete missing
   runtime paths. Metadata-only track entries are not playable destinations.

## Latest verified execution boundary

Windows x64 Release build and visible Aldar Beedo/POD10 on Mon Calamari MA
(Orotoru G'am) passed October 14 at 34ea1dd3b2b1b54438e5a98d1321c26938336799.
Build ended 16:46:55.905476 UTC; smoke ended 16:47:54.691990 UTC, exit 0,
zero owned processes. Root inspected Main, selected Rider, selected Course,
and actual starting-grid Track screenshots. Moving-race baseline 30.9838 FPS
at 640x448 over 156 GPU-completed swaps/5.0348842 seconds. All 8 original AI;
zero human/keyboard/controller/host-driver activity. Phase/render instrumentation
was enabled; this is not an uninstrumented comparison to earlier GA runs.
Windows current-thread CPU observations reported valid samples with no failures.
No new Linux test or video. Details: recovery/ma-intake-20261014.md.

This basic smoke establishes the selected route, not a full lap or complete
renderer/visibility parity. Gameplay original portal admission remains unjoined.
Next routine centralized test is eligible 17:47:54.691990 UTC or later. Failure
recovery is closed; any new Windows build/smoke failure immediately reopens it.

## Historical execution results

The earlier Ben/GA recovery at `1791f95` ended 13:19:41.246790 UTC and
produced the delivered silent 60-second window capture with a 13.3803 FPS
pre-recording baseline. Genuine POD02 preparation resolved its initial missing
supplemental-owner failure. That video remains historical evidence for1791f95;
no video was recorded during the later34b352f smoke.

The prior Dud/GA Windows pass at `68c157d` ended 12:02:10.403584 UTC and
measured 13.5682 FPS. Its earlier missing Rider neighbors were genuinely prepared.

The historical Windows Occo/SA build and smoke at `804d274` passed on
October 14 at 00:47:43 UTC, with a 7.4919 FPS sample. These are different
workloads; the Dud/GA sample does not isolate a performance improvement.
The subsequent `0fe381e` Dud/GA attempt built but failed before gameplay;
that failure was resolved by the real Rider cache catalog in `68c157d`.

The October 12 Linux compile-only session at 21:22–21:25 UTC used commit
`a65e4e3`: compilation and linking succeeded, but its post-build dependency
receipt comparison failed on path normalization. The overall session was
recorded as failed. No game was launched, and no fresh Linux result is claimed.

