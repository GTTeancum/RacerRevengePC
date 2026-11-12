# Pod destruction: original behaviour and integration plan (Nov 8, 2026)

User decision (Nov 8): build the full original wreck (debris pieces, wreck
camera), staged and verified, before build 99 is installed.

## Original behaviour (SLUS_202.68, Capstone disassembly)

Impact decision 225C50 (`src/game/impact_transitions50c.cpp`) dispatches the
racer vtable:

| Site | Slot | Handler | Meaning |
|---|---|---|---|
| 225D10 | 88 | 2261B0 | explode: state40=4 (damage >= 0.985 with a hard hit, or B9D bit0) |
| 225D9C / 225E28 | 8C | 226750 | wreck in place: state40=3 (written last, 2272F0) |
| 225E7C | 90 | 227370 | human: state1 (one side lost), 4C=2; AI: 226750 |
| 225EC0 | 94 | 227A40 | human: state2, 4C=2; AI: 226750 |

States 1/2 (human only): 4C counts down; `abs(BA8)>50` -> 2261B0,
`>25` or 4C<=0 -> 226750 (see `native-player-damage-prerequisites50la.md`).

**No respawn.** Rank 1B0814 retires any racer whose state40 is nonzero
(retirementCD4 = first row's lap+1, permanent). The only code that returns a
racer to state0 is 228110 (clears 40/44/4C), reached from 216A30 with a1!=0.
Its a1!=0 callers are the race-restart paths 1AEA70 / 1AE810 (virtual78 with
a1=1) and the replay reset 229F80/22A078. Ordinary resets (2255B0 undertrack /
manual) pass a1=0. So a destroyed racer is out for the rest of the race; a
destroyed human ends as DNF.

### 2261B0 (explode), s1 = racer

1. 40=4, 4C=0, 268=0.6f (0x3F19999A; state4 timer, decremented in 228BD4).
2. racer240 virtual18 (if nonnull).
3. Stop84 on 808[0], 808[1]; if 81C, Stop84 on 81C[0], 81C[1].
4. 28B180(body288 local XYZ) -> racer270; racer274 += 5.0 (explosion point).
5. If 814 and 810: each gets virtual20, virtual80, then 15ACB0(owner, body
   position) twice (explosion emitters).
6. 818[0..9]: entries with +370 > 0.4 get virtual20, virtual80,
   19C780(entry, racer270).
7. D40 (engine emitter) virtual9C.
8. Human only (58==0, 3C==0, race option bit): controller vibration via
   application+4 [racer5C]+10 vtable+18 slots 40 (a1=2) and 44 (2.0f, 2, 0).
9. Hide models 28C6E0: 1B4 (or 190 when 1B4 is null), 1B8, 1BC, 1C0, 1C4.
10. Remove collision: 694, 690, and 364 when 1B4.
11. Launch debris collections 22CA60(collection, racer340, zone) for
    374[0] (1B4), [1] (1BC), [2] (1B8), [3] (1C4), [4] (1C0), each only if its
    flags29 bit0 is clear.
12. Refresh the racer query 79C: 28B1A0(racer194) centre, radius 1000.0,
    11C1F0, 120070(.., 690.E0.4, 2), 11FF60, 122350(79C, 0x40, &698, &69C, 3).

### 226750 (wreck in place)

4C=0; hides the same models (28C6E0 x6); for each collection activates it in
place (22C6A0, with 28B1A0/28A700/28ADD0/28A090 pose setup) instead of
launching; state40=3 at 2272F0; same 79C refresh tail.

### 225C50 suffix after a handler

* Kill credit: attacker = 1AF330(race B0 list, victim D30) when D30 != -1 and
  the mode allows; attacker CD0 += 1; career threshold (table +10E8) may set
  race 1B1 bit1; human attacker: 22BA60(attacker, victim60), 1863B0, 22B570;
  clears attacker D00..D1C combo fields, D04=-1.
* Victim virtual A8 (a1=0): boost stop.
* If victim D2C > 0: announcer speech 1A46E0(race10C, 17..19 random).

## Port state

* Frame admission (`src/game/vehicle_frame49w.cpp:68`) rejects state40 != 0:
  a destroyed racer needs its own frame path (original base 228620 sends
  states 3/4 to 228390 and skips controls/physics).
* The crash site is `tests/native/terrain_interactive_frame_children50du.cpp`
  (`kind=damage` boundary).
* Renderer requires every opponent to be submitted each frame
  (`terrain_interactive_loop50du.cpp` ~2281); a hidden-pod flag is needed.
* Test hook plan: set `fields.flagB9D0` on a chosen racer at a chosen time
  (RR_TEST_DESTROY), which takes the canonical 2261B0 path.

## Stages

1. Destroyed racer leaves the race without a crash: handlers' state writes,
   effect stops, hidden models, collision removal, frozen racer frame, AI skip,
   rank retirement, sounds/announcer/kill credit, camera holds, human DNF.
2. Debris collections loaded per racer, launched/activated, simulated, drawn,
   with their effects.
3. Death camera 22DC90 over the debris view; HUD stop 1AD230; human states 1/2.

## Existing code map (Nov 8 survey)

Usable now: `stopOriginalDestroyedEffects50LA` (228390) and the two timer
leaves in `src/game/original_vehicle_damage50la.h` (never included yet),
`OriginalRaceHud50KG::stopAll1AD230`, rank retirement
(`ai_race_rank50iv.h:93-100`), AI speed controls already honour retirement
(`ai_speed_controls50is.h:97`), `removeLiveZoneMembership50EN`
(`live_zone_membership50en.h:110`), attacker marker D30/D2C
(`dynamic_impact50iw.h:184-187`), 1A46E0 speech request producer
(`lap_event6_subscriber50id.h:115`), model packets honour flags03 bit2
(`original_racer_model_packets50la.h:251`).

Missing for stage 1: handler state writes; intact-pod hide (renderer
`VehicleWindowOpponent50DS` has no hide field; loop requires all opponents);
690/694/364 removal caller; state 3/4 racer frame (49W `admit()` and body
advancement reject nonzero state; pod cables reject nonzero state); 225C50
suffix (kill credit CD0, boost stop A8, speech).

Stage 2/3 gaps: wreck constructors exist only on the RA cold roster and only
reach slot 0 on a path that returns false; activation/launch/frame kernels are
one-piece only and uncompiled (`original_wreck_release50la.h`,
`original_wreck_derived_frame50la.h`); multi-piece group 2CA020/2CA480/2CA620
missing; no debris rendering or zone-tag producer; death camera compiled but
`actual_.deathDebris` is never assigned (fails at 22D4B0).

## Stage 1 implementation (Nov 8, in progress)

* `src/game/native_destroyed_racers50pd.h`: per-race destroyed record (4C, 268,
  270, attacker D30), reset in `activateColdRace`; test hook
  `RR_TEST_DESTROY=slot@seconds` (runner `--destroy`) sets B9D bit0, which
  makes 225C50 select 2261B0.
* `terrain_interactive_frame_children50du.cpp`: `commitOriginalDestroyed`
  replaces the `kind=damage` stop: handler state writes, Stop84 on 808/81C,
  D40 virtual9C (AI: `destroyed16DF70` wreck cue + engine/binder stop; human:
  1A0060 engine/event stops + p_debris cue1), log `pod_destroyed`.
  `advanceDestroyedChildren`: 228390 plain tail and state4 268 decrement each
  frame; the physics part only closes the schedule (one substep).
* Renderer: `VehicleWindowOpponent50DS::hiddenDestroyed50PD` skips the pod,
  cables, transparent pass and shadow; the destroyed player's geometry and
  cables are collapsed to one point and its shadow skipped; Binder quads are
  collapsed. Destroyed racers leave the racer contact pairs.
* Camera: states3/4 without a wreck provider hold the current pose (late
  tick/matrix/visibility) instead of the 177AB0/22DC90 death camera.
* Human states1/2 (227370/227A40 limp phase) currently take 226750 directly.
* Later in stage 1: controller vibration (2.0 s explode / 1.0 s wreck), kill
  credit CD0 for both attacker kinds, the 17..19 kill song draw. Explosion
  particles came in stage 2 (Effects below).
* Still open (Nov 8): the human states 1/2 limp phase (needs the 218EFC
  nonzero-state force path and the 1B64D0/1B6670 joint rebind); the kill
  stinger songs 17..19 (mono VAGs; the music decoder is stereo-only, the
  request returns UnsupportedEvent); the 79C query refresh; 1A64B0 race
  event; the per-member break-off timing of debris groups (BreakRate).

Stage 1 verification additions (Nov 8): the HUD standings check rejected an
AI that completed its last lap after the human's race had ended (the source
finish list 1B07B0 stops growing then). `vehicle_window_gl50ds.cpp` now
accepts an unfinished row with all laps only once the selected racer has
finished or retired; the loop lists rows in source place order.

## Stage 2 plan (debris)

1. Cold: per racer, load PHY [Wreckage] collections/pieces with the existing
   inventory (`original_wreck_asset_inventory50la.h`, proven on RA) on every
   track, decode each piece PSG to a mesh with the pod's materials. Known gap:
   POD19 "b" fragments use an unsupported vertex attribute variant.
2. Launch: 2261B0 -> 22CA60 per collection (pieces start at their authored
   place on the pod, then the 22CB9C launch tail with six 308030 draws,
   `applyOriginalWreckLaunchTail50LA`); 226750 activates in place (22C6A0).
3. Per frame: piece rigid bodies with gravity from [Wreckage], ground contact
   against the track, settling (`original_wreck_physical/flight/settling`
   kernels are one-piece typed and uncompiled; adapt or write native).
4. Draw: new renderer path, cold-uploaded piece meshes with per-piece world
   transforms and the racer archetype's textures.
5. Each piece keeps a one-node scene record (localC0 = pose) so the death
   camera's debris view (22D4B0) reads real piece positions (stage 3).
6. Effects: group/piece smoke and fire (2CC060 and per-piece owners).

## Stage 2 implementation (Nov 8, in progress)

* Cold (`tests/native/native_wreck_debris50pd.h`): every racer's PHY
  [Wreckage] inventory through the existing authenticated loader, each piece's
  LOD0 mesh (bind matrices applied), spans/materials, authored placement and
  Weight/Inertia; the collections' Drag/RateDamping/Restitution/Friction; the
  pieces' base textures in one bank per racer. All 8 racers load on TA
  (7 pieces each, 9 for POD08).
* GPU (`native_wreck_debris_gpu50pd.h`, renderer `createDebris/drawDebris`):
  one combined piece-local mesh, per-piece spans with textures, drawn per live
  instance after the opaque opponents.
* Launch (`native_wreck_debris_sim50pd.h spawn`): 22C6A0 placement (pod
  translation + pod rotation x authored offset, pod orientation) and, for
  2261B0, the 22CB9C tail: six shared-308030 draws, angular +-20.0, linear
  x/z +-293.333 and y 0..293.333, plus the pod's velocity/angular velocity.
* Motion follows the original force/settling laws (167250, 166400, 167640):
  gravity = [Wreckage] Gravity x 32.2 on -Y; quadratic drag -Drag x speed^2;
  rate damping -RateDamping x angular velocity; mass = Weight / 32.2, the
  166400 box inertia; contact time ramps drag to 50 and rate damping to 200
  over the second 2 s window; a piece stops below speed 10, spin 0.5, linear
  acceleration 10 and angular acceleration 1 while in contact.
* Contact is native, not the original 166E40/2CA840 spring solver: a
  box-support segment against the track's static mesh candidates (front faces,
  as 1082B0), pushed out only along the contact normal, Restitution for real
  impacts and none below two steps of gravity (resting contact), Coulomb
  Friction. A piece below every track collision bound is retired (not drawn).
* Contact refinements (101e-101j): push-out only along the normal by the
  remaining penetration after the post-contact velocity move (no downhill
  creep); a contact skin and slop so a box on a slope keeps its contact; ground
  friction halves spin per contact step; a contact within the last 0.25 s
  counts as contact for the 167640 damping ramp and stop test.
* Measured (101j, TA): player explosion at 12 s - all 7 pieces at rest within
  4 s, 100-310 units from the wreck; AI explosion - 6 of 7 at rest within 6 s,
  the seventh flew off the course edge and falls (retired below the track).

## Stage 3 (wreck camera)

* `CameraView` in `native_wreck_debris_sim50pd.h`: one root scene node per live
  piece of the camera racer (slot 0), grouped by collection like racer370/374,
  `flags29` bit0 set, `localC0` = the piece pose each world step. Bound at the
  first world boundary after the human's destroy commit with the Session's
  prepared death identity (`Session::deathIdentity()`), and passed to both
  camera constructions as `ActualOwners::deathDebris`.
* The Session holds its pose while the view is unbound (count370 == 0) and
  otherwise takes the original 177AB0 transition, 1AD230 HUD stop and the
  22DC90 per-frame death camera over the debris (22D4B0 weighted centroid,
  radius 100).
* The 22D894/124170 first-use query allocations are reserved with the
  follow-query bridge's cold storage (`OriginalDeathCameraQueryColdOwner::
  reserved120/reservedD8`) and adopted, value-initialized, at 22D8A8: the
  death frame no longer allocates (101e failed the no-allocation frame rule).
* Measured (101j, player explosion): state4 selects mode 3 - the camera starts
  65 units behind the wreck and 20 up, aims at the debris and pulls back. Its
  forward axis stays within 6-11 degrees of the debris centre (cos 0.98-0.995)
  while it settles 456 units away and 42 units above; results follow as before.
* Mode 3 publishes a look direction, not a target (out.mode 0); billboards and
  diagnostics use the presented camera's eye/backward axes.

## Effects (Nov 8)

Original wiring (SLUS_202.68):

* Racer constructor 212B5C..212C40: racer814 = `podExplWreckExplosion.emt`
  (15E800 explosion class), racer810 = `podExplWreckSparks.emt` (15F100
  sparks class); 212C44..212DBC: racer818[0..9] = 19BA50 damage emitters
  over `podEmberFire.emt` / `podEmberSmoke.emt`. 2261B0 starts 814/810 at the
  body position and the 818 entries above 0.4 at the explosion point 270.
* Group frame 2CC824..2CC8E4: when a member's break flag (B0[i]) is set, the
  group's C0 `podWreckSparks.emt` and C4 `podExplosion.emt` restart (88, 80);
  otherwise both stop (84).
* EmitSmoke pieces own MaxNumberOfDamageEmitters `podWreckSmoke.emt` /
  `podFire.emt` damage emitters.
* All ten effect files are in every pod archive (`particle\*.emt`); the sprite
  atlas is the selected track's `particle\demoprt.prt/.pst` (each track has its
  own tinted sheet; MB/MC/RC/GB use other PRT layouts).

Native implementation (`tests/native/native_wreck_effects50pd.h`, renderer
`createEffects/drawEffects`): EMT values parsed as authored; particle laws from
the shared manager (161180: ms ages, Grows adds Growth*dt to size and Y,
HasVelocity, Gravity 32.2 x multiplier, FramesPerSet animation) and render
(62290: brightness 255 Glows / 64, x (1-age/life) when Fades, 128 scale,
Additive = ONE blending). CPU world-space billboards (sparks as velocity
streaks), alpha-blended back to front then additive, track program unlit with
world fog on the blended ones. Native choices: a private random stream (the
race's shared 308030 RNG is not consumed); the native debris separates at
launch, so each group's sparks/explosion fire once then; every explosion
starts the embers (no per-part damage gate); piece fire burns while the piece
moves and piece smoke continues at its minimum rate after it settles.

## Build 128 (Nov 12): threshold, stage E, wreck groups

* Racer +50 explosive wreck speed is read at race activation from each
  racer's own loaded PHY ([Collisions] ExplosiveWreckSpeed, 2159E8..215A28,
  x 0x3FBBBB84 rounded toward zero as the R5900 does) into the host table in
  `src/game/native_destroyed_racers50pd.h`; 225C50 and the 2289BC limp
  countdown read it (`racer_wreck_speed50` log; every pod: 85 mph =
  124.666 ft/s, 0x42F9550B).
* Stage E: the 79C refresh is logged as not represented (its 698/69C outputs
  feed only the wreck collections' world queries, 212A40..212A68 -> 22BFD0,
  which the native debris contact does not use). Manual reset admits states
  1/2 (1A1750 has no state test); in a one-human race the human is retired at
  limp entry, the manager's ended bit (1B0A70) is set at once, and the source
  reset gate (manager58 bit0 clear) and the port's input both refuse it.
  Race end: retirement ends the race immediately (1B0A70 allGone), race94 =
  6 s, so Results follow ~6 s after the limp entry, after the 2 s limp and
  the wreck. A fresh race activation carries the cold-built tether
  (`joint_restore`). Per-frame `pod_limp ... tether= tether_len=`.
* Stage F1-F4 (`tests/native/native_wreck_debris_sim50pd.h`): 226750 and
  the limp build host-side groups (2CA020/2CA190/2CA480/2CA620) for every
  collection with two or more pieces whose loaded pieces match its authored
  list; the primary carries the attached members; the member loop
  (2CADDC..2CB3E4) tests touching members once per frame on the shared
  308030 stream, and a break applies the 2CAEE8..2CB340 kick, detach and
  recompute in source order. The explosion has no groups (22CAD8).
* F3 (`native_wreck_effects50pd.h`): the explode-path group bursts are gone;
  a group's C0/C4 restart at the root on the first latched member, one per
  frame, while members are attached (2CC7F8..2CC8E4), otherwise stop;
  released members' smoke/fire start the step after the break.
* Stand-ins (flagged in code): the group contact is the native probe run
  for the root and each attached member with a lever-arm response, not the
  original DC spring/impulse solver (166E40/2964C0, stage F5, not built); the
  break test's peak impulse counts only impacts faster than the resting
  threshold; group rotational inertia is the mean of the 2CA620 diagonal.
* Stage F5 (build 128d, `tests/native/native_wreck_group_contact50pd.h`):
  a group's body now runs the original 2CA840 nonflight solver. Each frame
  the root's and every attached member's D8 detector runs its own E0 (the
  piece's m.COL type2 rays) at its pose through the 49X/50CC shape2 query
  and the 294BB0/2963E0 callback (50 records per DC); each substep (max
  1/60) applies 167250 forces, the 2964C0 generated-record response of the
  root DC and then every member DC to the root body (296884 depth, 296F10
  contact velocity, 297610 impact with restitution 0 and penetration
  correction 0.005, 297390 friction by `frX` material, [0] = Friction),
  keeps the per-axis largest |1C0| in the primary's C0 and integrates with
  167C40; then 166C60 movement and 167640 settling on the root DC.3C. The
  break test reads the member's own DC.3C and the primary C0, and the kick
  uses the root's local B0, its 167250 speed and current inverse mass.
  2CA620 now sets the full diagonal inertia through the 14E910 inverse.
  Measured (RC, wreck in place): resting contact leaves C0 at about
  0.02-0.8 ft/s, i.e. a 0.01-1.6 % break chance per frame at rest.
  Stand-ins (flagged in code): candidate order is the track storage order
  pre-selected by sphere gap (not the zone SearchList walk), groups/triggers
  are not dispatched, 166C60's E8 ray is the native segment query (with the E8 PHYSICSBODY/WATER exclusion), debris
  water is not modelled, the getters' exact rounding is not reproduced, and
  a group without a usable E0 (none seen) falls back to the old native probe.
  Free and released pieces still use the native box probe.
* Limp input (128d): the human's 217DE0 controls now receive the racer's own
  state40, so in the 227370/227A40 limp steering is halved and biased, the
  B7C/B80 actions are skipped (no drive) and the dual-stick brake is off, as
  218336..2187F0 do. The input reader 1A1A20 has no state gate.
* Shutdown: a close arriving during a display fade (race-start fade-in,
  Results fade-in) cancels the fade before the renderer retirements it
  would otherwise block (the 127d/127t TB and MA terminate at the time limit).
