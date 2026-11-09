# Original Results on every track (Nov 8, 2026)

User request: race results must use the original menu, faithfully, as the
original's one common menu. Do not trust earlier port wiring as faithful.

## Original (SLUS_202.68, verified Nov 8)

See docs/RA-ONLY-AUDIT-20261107.md "Correction from the original executable".
One race class (1A65C0, vtable 3CEE80) for every course; its load 1A8A40
(vtable+74) builds Pause100 (1BA130) then Results104 (1BE940/1BEEB0) then the
HUD children; no track id is read. Results uses only common UI assets. End
dispatch 1A7DD0/1A7E18 depends on application 11258 and race1B0 only.

## Port today

Only the RA human tournament runs the race load chain:

1. Cold (`terrain_interactive_cold_owners50du.cpp` ~188-221):
   `prewarmPauseCold50LA(sourcePause)`, then
   `raSession->constructHumanTournamentCold` (`ra_native_session50la.h`), which
   after the RA world/roster/grid calls
   `Application::loadHumanTournamentPauseResultsBeforeCollector50LA`
   (`original_results_cold_driver50la.cpp`): race.pause100_, race.results104_
   (load1BEEB0 with an `original_results_roster50LA::Read`), F8 owner.
2. Publication (`terrain_interactive_publication50du.cpp` ~2043-2150): Pause
   attach, then `publishNativeHumanResultsRoot50LA`
   (`original_results_human_root50la.cpp`) after the HUD bind - requires
   race display owner (`loadDisplay_`), camera adapter34, endHud_, Pause100,
   Results104, F8 and 6 race groups, then splices Pause/Results into the
   application root.
3. Runtime (`terrain_interactive_loop50du.cpp`): `updateNativeResultsAfterRace50LA`
   (held root), `pressConsumedResults1C8C30` (input),
   `prepareNativeResultsFrame50LA` (draw, only `sourceConsumedIntroActivation&&isRaTrack`),
   `resultsDonePending50LA`/`advanceResultsDone50LA`, Hall departure.
4. Everything else: `humanResultsReady` host panel (`HostRaceResults50IL`).

RA-specific gates (Nov 8 count, ~45): `race.world74_` / `actual.raWorld`
identity, RA aggregate `completedPrefixes`, camera issuance
`course==OriginalCameraStartCourse::ra`, `ra_cold_roster50LA::Operation` pin
in the roster Read. Files: original_results_cold_driver50la.cpp,
original_results_human_root50la.cpp, original_results_roster_read50la.*,
original_results_update_schedule50la.cpp, original_results104_input50la.cpp,
original_race_results_parent50la.cpp, original_race_hud_scope50la.cpp,
original_results_feedback_scope50la.cpp.

The port split the original's single race+74 world (class 1B2870) into
`world74_` (RA) / `taWorld74_` / `gcWorld74_` / `maWorld74_` / `saWorld74_` /
`gaWorld74_`; tier B consumed stages (TA/MA/SA/GA/GC) construct the race,
1A6AA0 load begin, 1AA5A0 resources, race groups and the world, then stop.
Tier C (MB, SC, TC, RC, TB, GB, MC) has no race object.

## Plan

1. TA first, generically: the race load chain (Pause100, Results104, F8,
   display/end HUD/root) on the TA race, with checks against the race's own
   world and a roster Read pinned on the native racer Bank.
2. Same code on MA/SA/GA/GC (tier B).
3. Tier C race stages, then the same chain.
4. Retire the host results panel where the original opens.

## Progress (Nov 8, builds 102a-102f, not installed, not committed)

* Done and verified on a TA tournament race (logs): the race load now runs
  the original Pause100 + Results104 + F8 loads on the TA race
  (`common_race_results_load loaded=1`, Results load complete at 1C15AC,
  35 Pause primitives), with the host Pause still bound to that storage.
  Window: inside TA's racer population (`ta_original_world50le.h`
  `beforeRacerPopulation`), after racer sources are admitted and before the
  combined population/collector - the only valid window on TA.
* Generalized: roster Read count/racer60 from the race's own list when there
  is no RA Operation; pause `canRetainPause100` and the cold driver accept
  the race's own counted world; root publication accepts the TA race/world/
  camera (it now stops at the end-dispatch roster check, fallback added,
  unbuilt).
* TA creates its race object only for tournaments (mode34 2, subtype 1);
  the original creates one for every race.

## Finding: the original race-end flow was never finished in the port

`Application::continueConsumedRaceEndEntry1A7DD0` is documented "UNCALLED"
(original_ui_main_entry50kg.h): F8 visibility, Results activation 1A7E18,
HUD stop 1AD230 and the 1AB520 pre-acknowledgement exist, but there is no
live trigger, no input acknowledgement writer, no reward or Hall/save
continuation, and 1AB520's non-tournament branch is unimplemented. Every
course, RA included, shows the host panel at the finish today. The remaining
work is therefore: wire and finish the original end flow (1A7DD0 -> 1A7E18 ->
1AD230 -> 1AB520 -> acknowledgement -> rewards/records/save -> continue) on
the TA race, then the other courses (race objects for every mode and course).

## Progress 2 (Nov 8, builds 102g-102o, TA tournament, player wreck at 12 s)

* The results root now publishes on the TA race
  (`common_race_results_root published=1`). Fixes: TA race/world/camera in
  the root check; the roster Operation fallbacks (pages, career, end
  dispatch); the process-input predecessor check accepts Light -> Aural ->
  loading330 -> Race when the frontend has closed its UI dispatcher (it does
  on every route when a race starts).
* The original end entry 1A7DD0 is now driven at the finish
  (`common_race_end_entry`): F8 visibility, Results activation 1A7E18, HUD
  stop 1AD230 and 1AB520 up to the acknowledgement wait all commit on TA.
  Results controller feedback binds after the TA results load (late bind).
* Next stop: 1AB520 needs race78, the SimpleVCR (1A6DF4). Its producer has no
  caller anywhere in the port; the results load now produces it for the common
  race (unbuilt).

## Progress 3 (build 102p)

* The results load now produces race78 (SimpleVCR, 1A6DF4 shape: 0xA0
  channels, 0x40000 bytes; subtype 3 uses 0x28/0x20000 plus backup) and
  registers Race90. The end entry now runs to 1AC8F8 (world restart,
  "Actual same-Race restart World74 unavailable").
* Regression found: the window close now terminates ("TA consumed race/world
  retirement"): the Race's rollback refuses while vcr78Owner_ is set, and the
  native VCR retirement refuses while the end invocation is suspended. Needs a
  native end-invocation cancel plus VCR retirement in closeNativeConsumedRace.

## Progress 4 (builds 102q-102v): race recording and the Results replay

* `src/game/original_simple_vcr50la` now implements the SimpleVCR record and
  playback (287C90 update, 287DD0 keyframe, 288030 delta, 2883B0 read,
  288670 play with 14FD00 slerp), channel registration (2878F0/287A30) and a
  VCR-owned native channel backing.
* `tests/native/native_race_replay50pd.h` drives the real race78 on the
  common race: channels per racer in the 1AD9A0 order (C60 quaternion, C50
  position, C70 velocity, B5C or B68/B60 controls, C00..C06 bytes), recording
  from GO (timing bit1, as 1AFE30), 287C90 every fixed step, Race90 advance.
  Playback writes the pose into main190, the body roots, controls and the
  229770 decode (drive codes, brake, B9C, impact BA0..BA8, damage 7D0..7DC).
  Native deviation: other parentless scene roots (cockpit, engine models)
  replay from a native track of transforms relative to main190, sampled at
  the VCR frame instants; the port cannot re-simulate them.
* The end flow on TA now runs 1AB520's waiting branch to the end: VCR channel
  enable/backup, race1B0 bit4, 1AC8E0 (pause clear, Aural restart; TA's world
  leaves and music level by native owners), Race90 = 0, replay start, and
  returns (`common_race_end_entry status=4 pc=1abe48`).
* Each Results frame: 287C90 playback, all 8 racers posed, camera
  (`Cut::replayPose`, racers Ready) publishes, Results update 1C8AE0 runs.
  Verified (r102s-end, logs): replayed player positions follow the recorded
  ones along the path; the replay loops at the end of the recording (1A8158).
* Recording size: ~140 bytes per 1/15 s frame for 8 racers, ~2.1 KB/s; the
  0x40000 buffer holds about two minutes, as in the original.
* Results/F8 draw and input on the common race: RA-only world/course checks
  replaced by the race's own Camera34 (update schedule, frame, capture,
  final-camera check, renderer lease, input, F8 bars and renderer).
* Window close: closeNativeConsumedRace now cancels a suspended end
  invocation and retires race78; the display counts Results/F8 renderer
  lease pins separately so PositionStats can retire before the Race.

## Progress 5 (builds 102w-103l): Results draw on TA, fade-in, fixes

* Results capture (1C21B0/1C17A0) now succeeds on TA (`results_capture
  captured=1`). Fixes on the way:
  - B4 flags58 for the common race read from the one native Setup manager
    (no counted 1AF7F0 object on TA).
  - race1B0 bit3 (race-start fade-in pending) is cleared at GO on courses
    whose intro fade is native; the Results base producer needs it clear.
  - The host Pause drawing (its material admission pins the collector) is
    retired at the Results start: the original Pause cannot open once the
    race is finished (1A7CDC), and Results' 14D440 rebuild needs the pin free.
  - The Results font `font\denmarkbold16ingame.pst` is resolved from the TA
    RES on TA (not a UI archive). The native image cache now decodes such a
    standalone PST from the resource's own original bytes.
* Window close on the common race retires its Results root, F8 and
  Pause/Results renderer leases before the UI renderer closes.
* Fade-in behind the Results: 1AC8E0 ends by setting race1B0 bit3 (1ACB7C),
  so 1A7BD0 fades the restarted scene in (500 ms, 2AEE90). The dispatch sets
  the bit; each replay frame runs the canonical fade-in prefix
  (`advanceConsumedSessionFadeIn1A7BD0`); the loop now renders while the
  ending fade-out still covers the screen during the replay, and services the
  fade after the presented frame. The Session's fade-in request/async owners
  were one-shot (used by the intro): `rearmFadeInAfterRestart50PD` rearms
  them once every earlier fade finished.
* Bug fixed: `inverseAffine` never wrote its result; the cockpit/engine root
  track replayed at the pod centre (seen as a Binder projection failure).
* Still open: Results draw frame, input/confirm flow, rewards/records/save/
  continue, replay-loop fades (1A82C4/1A82D8 fade out/in at each loop),
  other courses, natural-finish test.

## Progress 6 (builds 103m-103u): Results menu draws and takes input on TA

* Results frame prepared and drawn every replay frame; race HUD displays
  are skipped during the replay (race1B0 bit4 / 1AD230 HUD stop).
* Fade-in after the restart completes (Session fade-in rearm); the replay
  advances, the camera follows, loops at the end. Audio runs every replay
  frame (resumed after the display effect's pause).
* Input: unattended tests tap Enter through a host poll hook
  (`platform/linux/native/test_results_input50pd.h`, RR_TEST_RESULTS_CONFIRM)
  so the Results fixed-UI edges see it. Results UI sounds (raceUI.ini keys
  1..6) are prepared for every human race. Tap 1 accepted; tap 2 runs the
  tournament branch: Results native audio stop 26D070, music fade (native
  race music; the common race has no counted race10C), blocking fade out
  (260850, ~0.5 s) with the event retained, then Results deactivate.
* Data quirk handled as the original: medasmbrz.psm says initial frame 16
  for a 12-frame ATM; 29A020 wraps it on the first advance (frame 4).
* Results textures first registered without a UI source (Application font
  owner) are matched to the overlays member by byte-identical contents.
* Next: 1AB520's nonzero-20C branch (1AB5BC..1AB910), unimplemented in the
  port on every course. Tournament (mode34 2): s0 = profile+100C0,
  187B10 course order, 1875C0 + 187D80 (already committed natively at the
  finish), profile+100BC bit0 = 1, 26D070; record bit 7E4 -> 1AC440 and
  17DE10(App,6,0xB,1); else boundary 102CC == 0 -> 187E30 ? page 0xA : 0xD;
  else next index 4 or 8 -> 17DE10(6,7,1), 13 -> 17DE10(0x24,3) or (6,7) by
  the career medal table. Other modes: record -> 1AC440 + page 0xB; else
  187E30 ? page 0xA : page 0. The port's Results Hall transfer (App2E8 /
  2C6C20 request) is the existing 17DE10 adaptation to reuse.

## Original replay, mapped from SLUS_202.68 (Nov 8)

SimpleVCR (vtable 3D2F70, entity kind 141300; attached as Race child by
1AB1A8 with order 2, so its update 287C90 runs every frame):

* Channels: 2878F0(vcr, src, dst|0, interp94, scale f12, enabled t0) =
  float channel (8C=0, 98=scale, 9C=1/scale); 287A30(vcr, src, dst|0,
  enabled) = byte channel (8C=1, 94=4, 98=9C=1). dst defaults to src.
* Update 287C90(dt): App11C pause -> return. bit1 (record): time30 += dt; if
  word40 == 0 keyframe 287DD0 else if 44 <= 0 delta 288030; 44 = 4C (1/15 s);
  40 -= dt; 44 -= dt. bit2 (play): 288670(dt). Then 141ED0.
* Keyframe 287DD0: needs 74+68+8 < B4. Header [time30, 0x80000000|word70],
  word70 = 0; float channels raw 4 bytes (A0 = value), byte channels 1 byte.
  Trailer [-1000.0, 0x80000000|word70] written at the cursor, not advanced.
  Full buffer: print once, set bit7.
* Delta 288030: needs 74+6C+8 < B4. Header [time30, word70]. Float: q =
  int(d*scale + 0.5*sign(d)), d = v - A0; if -127 <= q <= 126 write q, A0 +=
  q/scale; else write 0x7F then raw float, A0 = v. Byte: raw byte. Trailer.
* Read 2883B0: trailer -1000.0 -> return 1 (end). 3C = 54; header -> 54/58.
  Keyframe: A4 = A8, A8 = raw (A4 = A8 if size field 0); delta: A4 = A8,
  byte 0x7F -> raw float else A8 += q*9C. Byte channels -> AC.
* Play 288670(dt): bit3: rewind (287BE0) and 34 = max(34 - dt*50, 0); else
  34 += dt*50 (rate 50, set by 287C80). bit5 (ended) -> return. If 34 == 38
  return; 38 = 34. Read frames while !bit5 and 54 <= 34 (bit5 = result).
  t = (34-3C)/(54-3C). Per enabled channel with dst: interp 3 = quaternion of
  4 channels, 14F2F0 each end then slerp 14FD00; 0 = lerp; 1 = lerp if A8 > A4
  else snap; 2 = lerp if A8 < A4 else snap; else snap (byte -> AC, float -> A8).
* 287B60 start record: clear bits 5,1,2,3,7, then reset 287BE0 (cursors,
  times 30/34/40/44/54/74 = 0, 58 = 0x80000000, 68 = n*4+8, 6C = n*5+8,
  70 = 0, 60 = 5C, 64 = B8 if bit0 else 5C, bit5 clear).

Channels per racer (racer manager load 1AD520 -> 1AD9A0, every racer, dst =
same racer; subtype 3 records racer 0 into the ghost racer B0+40):
C60..C6C quaternion (scale 5000, interp 3); C50/C54/C58 position and C70/C74/
C78 velocity (scale 1, lerp); controls B5C (racer58 == 1) or B68+B60 (profile
1040[racer5C] set) or B5C, scale 10, lerp; bytes C00..C04, C06 if racer1C0,
C05 if racer1C4. Race90 (race time, scale 100, lerp) from 1A6E98.

Racer: 2291D0 packs C00..C06 every live frame (racer3C == 0) from B9C bits,
B70/B78/B80, BA0..BA8 (signed 3-level), 0x40, and 7D0..7DC (x255). Racer
update 228620: racer3C != 0 (replay) runs 228F20 (velocity/filters from the
played pose) and 229770 (unpack C00..C06). 229F20 snapshot: C38 = zone
(7F0->4->48), C40 = body center (1202E0). 229F80 reset (racer3C != 0 and
not retired before race1A8): place at zone C38 / center C40 + up*7EC
(216A30), C30/C34 zone rows, 228110, sound D40 +94, vtable+A8, 64 = 68 = 0,
78 = CF8, C60 = quaternion of node, 28B1A0, 7D0..7DC = 0, effects 167A30/
167A40, C00..C06 = 0.

Race update 1A7B40 (vtable+38): race90 += dt while racing. After the end
dispatch, with race78: mode = B4 flags58 bits 3..4. Mode 1 or 3 (and racer
3C[0]+48 bit0 clear): 229F20 every racer, race1A8++, 287B60, set bit1 =
recording starts at the race start. Subtype 3 handles lap ghosts (288630
backup, A0 best lap). VCR bit5 (playback ended): 229F80 every racer, World74
virtual74, 287BE0, set bit2 (loop), camera 2D30B0 / 177AB0(race48, 6, 0, -1)
/ 1750A0, particles 161F60, racer D40 +80/+8C, music 2AED60/2AEE90.

1AB520 waiting branch: extra viewports race34[i>0] vtable+18; enable every
channel (90 = 1), 288630, race1B0 bit4, 1AC8E0 (world restart), every racer
3C = 1 and 229F80, VCR bit1 clear / bit2 set, announcer 1A4AB0(0) and
1A46E0(4, 0), 1A9750(race, 1) (single full-screen camera34, fog 4000/1333/64,
race48[0] +20, race48[1..3] +18), race48+84 = 0, 1750A0(race48, racer).

## Finding: the original Results run over a race replay

1AB520's waiting-results branch (after the acknowledgement read) enables the
SimpleVCR end channels/backup (1ABA68), sets race1B0 bit4, then restarts the
race world: World3D0 trigger list reset 2DCC70, App11C pause clear, Aural
restart 103210, music fade restart 1AC960, particle reset 161F60, world
physics restart 2702E0, racer restarts. That is the original replay of the
race behind the Results screen. The port has these leaves only against the
RA world (`endWorldCurrentNative`, `endReset*` on `world74_`), and nothing
records the race into the VCR during play. Faithful Results therefore also
needs race recording and replay playback, on every course.

## After Results: the original acknowledgement and its screens (Nov 8, verified in SLUS_202.68)

1AB520 (race vtable+78) runs every race update after the finish. It stores
P+7E4 bit0 (1ABFC0, new Hall record) and P+7E8 (1ABE70, rival beaten), then
reads Results+20C. Nonzero (set by the Results confirm at 1CA794) takes
1AB5BC, which needs App2E0 (1 except the predefined-track debug boot).

Tournament (P+34 == 2): s0 = P+100C0, s1 = 187B10 order, 1875C0, 187D80,
P+100BC bit0 = 1 (1AB648), 26D070, then the first matching row:

| Condition | Call | Screen |
|---|---|---|
| 7E4 bit0 | 1AC440(App28C), 17DE10(6,11,1) | Hall of Fame |
| 102CC, s1+1 is 4 or 8 | 17DE10(6,7,1) | Win |
| 102CC, s1+1 == 13, M[C][C][12] == 1 | 17DE10(0x24,3,0) | end scene, then credits |
| 102CC, s1+1 == 13 | 17DE10(6,7,1) | Win |
| 102CC, otherwise | assert (PodRaceEvent.cpp 2244) | none |
| 187E30 | 17DE10(6,10,1) | Unlock |
| otherwise | 17DE10(6,13,1) | Garage |

Other modes: 7E4 -> 1AC440, 100BC, (6,11,1); else 187E30 -> 100BC,
(6,10,1); else (6,6,1) Course (a2 copied from a1 at 1AB90C: page 1).

The port previously committed 1875C0/187D80 at the finish. On the common
race it now commits at the acknowledgement, as the original does (the
host-panel fallback still commits at the finish).

17DE10 stores flags/screen/page at App318/31C/320 and attaches the launcher;
its tick 2C6CD0 runs 0x40 (tier scene), 4 (destroy race), 8 (destroy UI),
1 (load race), 2 (17DB80 change screen; with no UI manager, ld_ui builds
one first), 0x20 (ld_end, PodTierWinScene).

### 187E30: eight unlock checks (OR, first true wins)

U[r] = P+10FAC+4r. M[c][p][o] = P+1938+c*0x894+p*0x50+o. Defaults: the
Hall table copied to P+7EC by the memcpy at 1858EC at the end of 1801D0.

| Check / grant | Grants | Condition |
|---|---|---|
| 187EE0 / 187F40 | U[R], R = signed byte P+11280+12*(1125F&15) | U[R]==0 and 7E8 != 0 |
| 187F90 / 187FF0 | U[1] Young Anakin | every default lap (7EC) > live (102F0) |
| 188010 / 188050 | U[14] Sebulba Ep.1 | M[18][18][12] in 1..3 |
| 188070 / 1880D0 | U[3] Clegg | M[C][C][3] == 1 |
| 1880F0 / 188150 | U[25] Watto | every default race time (838) > live (1033C) |
| 188170 / 1881D0 | U[21] Maul | every default knockout count (884) < live (10388) |
| 1881F0 / 1882B0 | U[26] Vader | U[1], U[25], U[21] and any of M[1][1][12], M[25][25][12], M[21][21][12] in 1..3 |
| 1882D0 / 188440 | gallery tier N (11018 bits 5-6) | course is a circuit finale, N = wins at orders 3/7/12 > stored tier |

Each check is false once its own grant is made. The Unlock page grants on
Cross (20B480), one per press, and re-shows itself while any flag from that
showing is still set. Rival flag 7E8 is cleared at the next race start.

### UIUnlock (screen 10, PodUIUnlock, page PodUIPageUnlock 209A80)

Same 30 pod/pilot widgets as Rider Select (control factory 20A850 copies
1E9AC0); pods, pilots and textures in UIUNLOCK.RES are also in UIRIDER.RES.
Activate 20AE40 runs all eight checks, shows the first: pod R / 1 / 14 / 3 /
25 / 21 / 26 (group 2 control), or a gallery background artpod01 / artpil01
/ artpla01. Text: loc 0x151 "YOU HAVE UNLOCKED" + pod name (P+3C[r] for the
rival; fixed ids otherwise), colours P+A8/AC+8r. Background tiles NN_bkg.
Doors 0.5 s; opening plays song 7 (ui_peopl.vag, looped); closing fades 1 s.
Input: device 0, event 4 (Cross) only. Exit: mode 2 -> Garage (13), else
Course (6), via 17DE10(App,2,screen,1) at 2A3F70.

### UIWin (screen 7, PodUICircuitWin, page PodUIPageCircuitWin 2076A0)

Shown for 1st-3rd in the finale of circuit 1 or 2, or 2nd-3rd in the
circuit 3 finale. The trophy follows the place (trophy1/2/3.psg + star,
star2, star3; spin 0.05 turns/s about Y), the circuit only changes text and
music (circuit 1: song 10 trphy1.vag, circuit 2: song 8 trphy2_3.vag,
circuit 3: none). Text: loc 160 + place (145/146/147; circuit 3 2nd place
uses 147, an original bug) + circuit (161/162/163). Background %02d_bkg by
career. Input: Cross only -> Unlock (10) if 187E30, else Garage (13).

### Hall of Fame in tournament mode

The Hall confirm (1E3FC0..1E41C8) repeats the tournament rows above (Win /
end scene / Unlock / Garage); its end-scene case closes the doors first and
1E4980 calls 17DE10(App,0x28,3,0).

## Status (Nov 9, build 109e, NOT installed)

Verified in muted unattended logs (TA tournament, player wrecked at 12 s,
two Results confirms): Results replay -> confirm -> 1875C0/187D80 commit at
the acknowledgement -> 1AB5BC chooses Garage -> race retires -> a new
frontend opens the Garage as its first screen (session-less Main boot bank
for shared dialogs/renderer: port stand-in, nothing of Main is shown) ->
Back -> Tournament -> Course -> next race. 600 s run: 7 races, 6 Garage
visits, 0 rejections; races ~62 FPS.

Fixed on the way: Results root check required active Results after confirm;
stale 11-entry Garage IPUM cap (catalog is 33); Garage root order with the
successor dispatcher after loading-manager 330; first Garage frame clock
(continued, not zero); GL context now 4.0 core with 3.2 fallback (AMD gave
exactly 3.2; UI AA1 needs gl_SampleID); fire-emitter observer used a
retirement-only check; Garage sounds released by 268300 at race load as the
original's unconditional reset does; second-Garage-visit ordering, texture,
text-material and bank identity checks.

Built, compiled, NOT yet reached by any run: Unlock (screen 10, pod+pilot),
Win (screen 7, trophy, single-play trophy music), Hall of Fame tournament
mode (+1AC440 at the acknowledgement). End scene (0x24/0x28) and Credits
from it: not implemented (logged, unreceived).

Garage speed: ~17 FPS (fixed-function specular per the Nov 7 approval,
RR_EXACT_PS2_SPECULAR=1 restores exact). User: menu speed does not matter.

## Parked (Nov 9): after-race screen testing, branch parked/after-race-screens

Stopped at the user's request ("mark for later"). Work in progress, not
verified: prepared-save scenarios (work\scratch\gen_save.py, work\scratch\saves)
to reach Unlock (Clegg pending), Hall (slow TA records) and Win (GA finale,
course order 3? check), fixes in Unlock/Win resources, GA camera hold, the
capture route. Logs: evidence\u111*, w111*, t111r-o*, h112*. Track launch
survey t111r: 11 of 12 tracks load into a race from Course (RA stops in its
loading prefix); none was run through Results. No screen (Unlock/Win/Hall)
had been reached yet when stopped.
