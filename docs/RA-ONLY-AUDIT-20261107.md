# Track-route audit (RA-only and tiered behaviour), November 7, 2026

> **Governing rule (2026-11-12):** one common path for every course and every
> pod, differing only by loaded data
> (`D:\Programming\GitHub\RacerRevengePC\CLAUDE.md`). Every RA-only, tiered or
> per-track branch listed here is a port stand-in to retire, not a design.

Scope: every branch on `isRaTrack`, `consumedRaStage.race()`,
`sourceConsumedPauseActivation`, `humanResultsCold` and
`canonicalTournamentGrid` in `tests/native/terrain_interactive_*.cpp`
(121 lines in 9 files), plus the per-track consumed-stage and intro
conditions. Purpose: separate legitimate per-track data handling from original
features that the earlier port work connected to only some tracks.

## Track tiers found

| Tier | Tracks | What they get |
|---|---|---|
| A | RA | Rebuilt original race object (`OriginalRaceConstructor50LA`): original Results screen (human tournament), race-end clock owner, native player speech hookup, race intro, and the original Pause for the muted all-AI spectator test mode only. |
| B | TA, MA, SA, GA, GC | A consumed original race stage: original pre-race intro (`retainNativeTaIntroAuthoring50LA` is shared by all five), native intro race wiring. |
| C | MB, SC, TC, RC, TB, GB, MC | Direct original-disc route only: no consumed stage, no original intro. |

All 13 tracks with a direct original human race load the original HUD
(Speed/Boost, TimeStats, Damage, PositionStats; `humanOriginalHud`). That was
deliberately extended from RA to every track and is not a gap.

## Hard-coded race identity

Only one exact race check existed:
`original_pause_observation50la.h` `retainedRaceIdentity` requires track 7,
course order 4, record pod 9, class A. It applies to the RA consumed-race Pause
route only. Build84's host Pause route (`bindHost`) does not use it.

## Gaps (original feature connected to only some tracks)

1. **Results screen.** The original Results104 screen is constructed only for a
   human RA tournament (`humanResultsCold` requires `consumedRaStage.race()`,
   `cold_owners50du.cpp:188-221`). Every other track shows the host results
   panel at the finish. Highest user impact after Pause.
2. **Pause on RA itself.** Before build84 the original Pause existed only for the
   RA all-AI spectator mode with `RR_AUDIO=0` (`session50du.cpp:746`); a human
   on RA got the host menu. Build84 puts the original Pause on every non-RA
   track. RA human races still use the host menu because the RA human
   tournament route hands `sourcePause` to the Results load
   (`original_results_cold_driver50la.cpp:65`). Needs its own join.
3. **Intro on tier C.** MB, SC, TC, RC, TB, GB and MC have no original pre-race
   intro; tiers A and B do (`publication50du.cpp:1924`).
4. **Player speech hookup.** `retainActualSpeechOwners` and `nativeHumanAudio`
   are RA-only (`continuation50du.cpp:2147, 2372-2380`). Other tracks still play
   speech through the older path (an SA race logged 21 speech cues).
5. **Race-end clock owner.** `humanEndRace94` binds the original race's end
   clock on RA only (`continuation50du.cpp:320-330`); other routes keep the
   earlier owner. Tied to item 1.
6. **All-AI spectator Pause.** RA only, test mode. Low priority.

## Not RA-original anywhere

* Pod shadows: RESOLVED (2026-11). The host-invented footprint shadow and its
  `!isRaTrack` gate are removed; every course, RA included, draws the original
  projected shadows (`src/game/original_projected_shadow50ps.h`), with the
  receiver flag read from each track's own TOB
  (`src/game/original_static_model_params50ps.h`).

## Per-track loader branches (stand-ins, retire into one path)

World, sky, ground, collision queries and gates, navigation maps, runtime
triggers, static candidates, material audio track selection, lighting
selection, track materials/normals/textures, live-zone membership and follow
queries. In the port each track loads its data through its own loader, and the
branches select that loader.

**Corrected 2026-11-12:** this was earlier marked "legitimate, no action". It
is not. The original has one race-load path that reads each track's data (see
the note near the end of this file: no per-track race class or override
exists). Per-track loaders are port stand-ins to retire into one common loader
driven by track data. See `D:\Programming\GitHub\RacerRevengePC\CLAUDE.md`.

## Suggested order

1. Original Results screen on every track (with the race-end owner, items 1 and 5).
2. Original Pause for human RA races (item 2).
3. Original intro for tier C tracks (item 3).
4. Native player speech on every track (item 4).

## Implementation analysis (user asked for all items in one batch)

* `consumedTaStage/MaStage/SaStage/GaStage/GcStage.race()` also construct an
  `OriginalRaceConstructor50LA` (used as `nativeIntroRace`), so tiers A and B
  both have an original race object. The RA-only items are gated on
  `consumedRaStage.race()` specifically and additionally depend on RA world
  data: `race.world74_` (RA aggregate), `original_results_roster50LA::Read`
  over `ra_cold_roster50LA`, `completedWorldPrefixes(race)`, the RA native
  F8 bars (`prepareNativeF8Frame50LA`) and `prepareNativeResultsFrame50LA`
  (driven only under `sourceConsumedIntroActivation&&isRaTrack`).
* Results on tier B therefore needs: widen the gates to any consumed race,
  a roster adapter per tier-B world (or one over the native racer bank), the
  world-prefix checks per world, and the F8/Results frame drive on those
  tracks. The end clock (`humanEndRace94`) and speech hookup follow the same
  widening.
* Tier C (MB, SC, TC, RC, TB, GB, MC) has no consumed stage; intro and results
  there require building those stages on the tier-B pattern.
* RA human Pause: `humanResultsCold` hands `sourcePause` to the Results load;
  the host Pause route needs a second Pause owner or a shared-storage join.

## Progress (Nov 7, same session, not yet built)

* Done: native player speech joins any consumed race (RA/TA/MA/SA/GA/GC);
  off RA a failed identity join logs `native_human_speech_join_unavailable`
  and keeps the existing speech path.
* Done: the host Pause route also binds on human RA races (it reuses the Pause
  storage the RA human tournament already loads for Results; the all-AI
  spectator route is unchanged).
* Results design for tier B: `original_results_roster50LA::Read` rows come from
  `original_position_stats_adapter50LA::Owner` + the racer Bank (available on
  every track); only its identity checks bind `ra_cold_roster50LA::Operation`
  and the RA aggregate (`raceWorldIdentity`, `completedWorldPrefixes`,
  `loadedOperation*`). Next: a host Read variant validated against the
  consumed race's own world/session, Results104 load in the tier-B HUD
  collector window, and the finish -> Results open/frame drive that RA gets
  from `humanEndRace94` / `prepareNativeResultsFrame50LA`.

## Build85 (installed, not yet played)

Contains build84 (every-track original Pause) plus: original Pause on human RA
races and native speech on all consumed races. Results on other tracks and
tier-C race stages were NOT done: the Results subsystem is ~5,800 lines
(display, record pages, career summary, winnings, roster pages, Hall record
saving) driven by the RA native race display pipeline (root scheduler, race
display owner, course-RA camera issuance, captured timing). It needs either
that pipeline per track or a host-driven adaptation that preserves career and
record saving; tier-C race stages are comparable. Treat as the next dedicated
effort.

## Root cause for Results/end-of-race (Nov 7, verified in code)

Tier-B consumed stages stop after race construction and world join
(`original_consumed_ta_stage50du.h`: "Only this reached prefix. No complete
slot30 load or App10 attachment"). The original slot30 race load is what
creates the race children Results depends on: `camera34_` and
`cameraNativeAdapter34_` (`bindConsumedRaCameraNative34`, which also checks
issuance course == ra), `loadDisplay_` (`original_race_load_prefix50la.h`),
`endHud_` (`original_race_hud_scope50la.cpp`), `pause100_`, `results104_`,
F8 bars and the end-entry continuation (`endEntryStep_`). Only the RA human
route (`raSession->constructHumanTournamentCold`) runs that load. Results on
other tracks therefore requires porting slot30 race load per world (TA, MA,
SA, GA, GC first; tier C after its stages exist), then relaxing the
course==ra / world74_ checks in `original_results_update_schedule50la.cpp`,
`original_results_human_root50la.cpp`, `original_results_roster_read50la.*`,
`terrain_interactive_roster_cold50du.cpp` to the track's own world slot.
Milestones, each testable: (1) slot30 load on TA with camera/display/end HUD;
(2) Results on TA; (3) MA/SA/GA/GC; (4) tier-C stages + intro; (5) Results on
tier C.

## Milestone 1 scope check (Nov 7): widening alone breaks TA

A first pass widened the RA checks to TA (camera bind, world-init
continuation, end-HUD scope, `humanEndRace94`). The code review that followed
showed the widening cannot run on TA as it stands, so it was reverted. The draft
is saved in `docs/patches/m1-ta-slot30-widening-draft.patch`. Reasons:

* `worldResetPredecessor1A6BF0Current` (sound reset, 1A6BF0) needs the RA
  aggregate's `worldAudio_` to equal the race's `raceAudio_`, with six attached
  race groups and no TA intro emitter. TA never runs `beginConsumedRaceLoad1A6AA0`,
  so it has no `loadDisplay_`, `loadFifo_` or `raceAudio_`, and its audio
  groups carry TA intro authoring. Continuing TA world init would therefore
  fail and reject TA race construction.
* `ownsNativeCameraAdapter34` / `nativeCameraAdapter34LifetimeCurrent`
  require the camera Session owned by `world74_->runtimeQueries_`. TA's Session
  is issued by `Application::createNativeTaCameraSession50LA`. Widening the
  Session-level course check turns TA's current no-op success into a failed
  bind.
* The end-HUD stop (`Registration::stop1AD230`), end clock, F8 bars, Pause
  parent and Results input all borrow the camera via
  `world74_->borrowRuntimeSourceCamera`. TA needs an equivalent borrow from
  the Application-issued Session.

True milestone 1 work, in order:
1. Run `beginConsumedRaceLoad1A6AA0`, the resources step and the race groups
   on TA, reconciled with TA's intro audio groups.
2. Port the 1A6BF0 sound-reset predecessor to the TA world owner.
3. Add camera ownership for the Application-issued TA Session (own, lifetime,
   borrow) alongside the RA world-owned one.
4. Widen end HUD, end clock, F8 and Pause parent to use that borrow.

### Milestone 1 implemented (build86, not yet run)

Correction to the scope note above: TA already ran `beginConsumedRaceLoad1A6AA0`,
resources and the six race groups (`original_consumed_ta_stage50du.h`), so
TA has `loadDisplay_`/`loadFifo_`/`raceAudio_`. Implemented:

* 1A6BE0 on TA: `constructTaWorld1A6BC0` sets `soundReset`;
  `constructConsumedTaRaceWorld1A6BC0` continues `continueConsumedRaWorldInit1A6BE0`
  (Aural reset, low2, counted Camera34 prefix) like RA.
  `worldResetPredecessor1A6BF0Current` has a TA world half.
* Camera34 custody for the Application-issued TA Session:
  `nativeCameraAdapter34LifetimeCurrent`/`ownsNativeCameraAdapter34` TA branch,
  `Application::issuedCourseCameraSession50LA`, `Race::borrowRaceCamera34` and
  `raceCameraCourse34` (one rule for RA and TA).
* `Application::consumedTaCamera34Engaged`: TA binds Camera34 only for the
  human-course Session whose race94 is the race's own `field94_`; other TA
  Sessions keep the previous no-op. The canonical end root on TA additionally
  needs a bound adapter, a live end HUD registration and the live native Root,
  otherwise the previous end policy runs.
* End HUD Scope, `stop1AD230`, `endStateCurrent` and the end-clock/fade borrows
  use `borrowRaceCamera34`. `humanEndRace94` binds TA's race94.
* Not changed: race-start fade-in (`advanceRaceFadeInPrefix`) stays RA-only;
  the 1A7DD0 end dispatch still hands over to the host results panel
  (Results is milestone 2); `completeCountedRaceWorldNativePrefix1A9EE0` and
  music10C completion stay RA-only.

### Build87 unattended results (Nov 7, logs only)

* Build86 crashed at the TA restart: the widened Session revoke reached
  `revokeConsumedRaCameraNative34` with no Race28C (TA Single Event has no
  consumed race) and refused. Build87 makes TA revoke a no-op without
  Race28C. Diagnostics added: `consumed_ta_gate` (world50du) and
  `camera34_close_state` / `camera_abandon_rejected` (continuation shutdown).
* `main-rider-course-ta` + restart: exit 0, route and restart verified,
  62.5 FPS. Gate log: `mode34=0 subtype=1 admitted=0`, i.e. this route is
  not a tournament, so the consumed TA stage and the new slot30 code do not
  run on it.
* `main-dud04-course-ga` + restart: exit 0, route and restart verified.
  First race 45.8 FPS, second 61.5 FPS (cause of the first-race figure not
  established).
* RA: `main-mawhonic09-course-ra` is admitted only as all-AI with the
  `mawhonic-ryloth` profile and then stops at "RA cold target/runtime readiness
  is unavailable". No RA run was possible.
* Not yet exercised by any run: TA tournament (mode34==2), i.e. the actual
  milestone 1 path (1A6BE0 continuation, Camera34 bind, end HUD, end clock).

### Build88: TA tournament reached (Nov 7, logs only)

New unattended route `main-tournament-course-ta` (`TournamentRoute` in
`original_ui_capture_navigation50la.h`): Main slot3 (mode2) -> Rider19 ->
Tournament Continue -> first tournament course (TA) -> Confirm.

Fixes needed to reach and run the TA tournament race:
* `hallRecordSnapshot` refused mode2, so the Loading Screen text failed before
  every Tournament race (`original_loading_text_rejected step=1`).
* `continueConsumedRaCamera1A9240` and `bindConsumedRaCameraNative34` still
  required the RA world / course ra (lost from the build86 patch).
* `arithmeticAdmitted()` (TA cut loading, 0x282CD0) was Linux-only and always
  false on Windows: TA tournament intro cuts could never load on Windows.
  Added the MSVC x64 MXCSR equivalent.

Results:
* TA tournament: consumed TA stage admitted (mode34=2), race built, Camera34
  bound (`camera34_close_state ... adapter_bound=1 lifetime_current=1`), race
  ran at 62.5 FPS, first-race shutdown passed.
* Restart inside a TA tournament fails: the restarted session has no consumed
  launch (no Race28C) but still takes the tournament cut path, rejected at
  0x174FA4 (`original_camera_cold_rejected status=8`). Pre-existing gap, reachable
  through Pause > Restart Race in a TA tournament.
* End HUD / end clock on TA not exercised: the human pod is not driven
  unattended and runs are capped at 180 s.
* Regression: TA single + restart and GA + restart: exit 0, all races 62.5 FPS.

### Build89: TA tournament restart (Nov 7, logs only)

Restart Race dropped the consumed launch, so the restarted TA tournament race
was built without its consumed race yet took the tournament cut path
(0x174FA4). `ConsumedLaunch::renewForRestart` now mints an unclaimed launch for
the next session when the profile still holds the same selection;
`terrain_interactive_drive50du.cpp` applies it on Restart for TA only
(`consumed_launch_restart track=TA renewed=1`). RA/MA/SA/GA/GC tournament
restarts are unchanged and untested.

* `main-tournament-course-ta` + restart: exit 0, route and restart verified,
  both races consumed (Camera34 bound), 62.5 / 62.5 FPS.
* TA single + restart, GA + restart: exit 0, 62.5 FPS in all races.

### Build99: source findings used (Nov 8, SLUS_202.68 disassembly)

Read with `tools/diagnostics/ps2_disasm.py` / `ps2_xrefs.py` (Capstone).

* Race input. Reader 1A1A20 reads axes, D-pad, throttle (preset 694) and
  brake (690) as held state every frame. Gate 1B0B80 =
  `(flags58&2) && !(flags58&1)` guards only the boost/repair/powerslide flag
  reads. Nothing waits for a neutral pad at the green light, so the port's
  controller latch no longer re-arms at countdown -> race (the 2 s freeze).
* Engine-sound listener. Race music update 1A4440, 1A4558..1A4588, sets each
  view's AuralScape listener through 1030B0 from the CAMERA (+180 position,
  +190 look, +1A0 up). The AI engine loop reads that same listener: 1034F0
  calls 290E10 (listener +10 position) and 103628 calls 290D70 (+20/+30). The
  port fed the AI emitters the player body; it now uses the camera pose.
* Announcer leader. 2C4DA0 seeds remaining=FLT_MAX, speed=1e-6, walks every
  manager row and keeps the LAST row with place 1 (no break, no uniqueness
  test). With none, FLT_MAX/1e-6 is not < 10, so the tail is false. The rank
  pass 1B0814 can leave no place-1 row (a racer driving the wrong way did, in
  an unattended run). The port stopped the race on both "no leader" and
  "two leaders"; it now follows 2C4DA0.
* Camera views. A fresh profile starts with preference 11030 = 5 (attached
  camera). Triangle (preset row 6A4 = descriptor 0) cycles 5 -> follow 0 ->
  follow presets -> 5 through 176DF0; keyboard C reaches the same action.

## Correction from the original executable (Nov 8, 2026)

The "per track" conclusions above came from the port's own wiring, not from
SLUS_202.68. Verified in the executable:

* One race class for every course: constructor 1A65C0 ("PodRaceEvent",
  called from the race launch code at 17C16C and 17D678) installs vtable
  3CEE80. That table is the ONLY data reference to the race load 1A8A40
  (3CEE80+74), so no per-track race class or override exists.
* 1A8A40 unconditionally constructs, in order: Pause100 (1BA130, 0x8F0),
  Results104 (1BE940 + load 1BEEB0, 0x18C0), then the HUD children (189450,
  18FF80, 193100, 196F20, 18C550/18C6F0, 7x 29C850...). Its only race-data
  branch is race1B0 (mode flags); no course/track id is read.
* 1BEEB0 (Results load) uses only common UI assets: icon_sqr/tri/x/cir,
  denmarkbold16ingame / eurostileboldbold13ingame fonts, medasmgld/slv/brz
  medals, %02d_fsm racer portraits, localized text 1AA/1AC, CHEATS ENABLED!.
  No track member.
* End dispatch 1A7DD0/1A7E18 branches on application 11258 (launch mode) and
  race1B0, not on the track.
* race+74 is one world class (1B2870, 0x550 bytes) constructed by the race
  constructor for every course. The port's separate `world74_` (RA) /
  `taWorld74_` / `gcWorld74_` / `maWorld74_` / `saWorld74_` / `gaWorld74_`
  types are a port-side split; Results/Pause gate on the RA one only.

Consequence: the original Results screen is one common menu. The faithful
route is one common race load (Pause100 + Results104 + HUD children) for every
human race, with Results reading the race's own roster, not a per-track port.
