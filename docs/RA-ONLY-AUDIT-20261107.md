# Track-route audit (RA-only and tiered behaviour), November 7, 2026

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

* Pod shadows: tracks other than RA use a host-invented footprint shadow
  (`platform/linux/native/native-pod-shadow50hm.md`); RA has none
  (`renderRequest.enableNativePodShadows=!isRaTrack`). Original shadows are not
  recovered for any track.

## Legitimate per-track branches (no action)

World, sky, ground, collision queries and gates, navigation maps, runtime
triggers, static candidates, material audio track selection, lighting
selection, track materials/normals/textures, live-zone membership and follow
queries. Each track loads its own original data through its own loader; the
branches select that loader, not a feature.

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
