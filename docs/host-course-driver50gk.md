# 50GK: host course-test driver and original-AI boundary

## Result and verification boundary

`src/game/host_course_driver50gk.h/.cpp` provide a bounded **host-authored course-test driver**, not the original AI profile/controller. It emits ordinary preset4 twin-throttle inputs while the existing reader, shaping, engine/aero, physics, contacts, navigation crossings and lap producers remain responsible for the resulting race state.

This worker performed static source/resource inspection only. **No build, unit test, guest CPU/device execution, game launch, steering-polarity test or completed-course proof was performed here.** Client integration, manual takeover and a real-physics polarity/course check must be verified by the integration owner before claiming working autonomous driving. No existing client, renderer, frame or navigation file was edited by this worker.

## What is actually missing from the original AI

Verified original `work/native/project/disc/SLUS_202.68` SHA-256:

`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`

The original AI is a different controller/vehicle path, not a profile option understood by the current human-player reader:

- `16E9C0..16EAF8` runs the AI dispatcher: navigation `16FFE0`, AI state update `16EB00`, `171C60`, `1734C0`, then state-dependent driving/combat functions. Some states explicitly call reset `2255B0`; this host adapter never calls it.
- `16FFE0..17022C` updates AI/map navigation. The often-cited `170000` address is inside this navigation routine; it does not produce throttle or steering.
- `171FE0..17237C` is a desired-position/lookahead planner using current/previous AI nodes, node positions, segment geometry and traversal. The course-test driver does not claim to reproduce this planner.
- `172380..1726E4` selects ordinary desired direction and handles speed/support/stall conditions, calling `172E60` for steering.
- `172E60..173174` produces raw steering `B5C` and clears `B60`. Inputs include canonical orientation `28A530`, body cached speed `34`, body angular lane `A4`, aero field `5C`, pod identity and the supplied desired direction. The original `13C1F0` angle approximation is already available natively as `originalAngle48G`; that helper is not the missing blocker.
- `172C50..172DE8` produces speed/boost decisions using authored node speed and live application/race/AI state; it invokes `173180` and, when that declines, `173770`.
- `173770..1737D8` clears boost flag bit0 and selects throttle `B7C` or brake `B80` by comparing desired speed with `0x3F2E8BC1 * body.cachedSpeed34`. That leaf alone cannot supply steering, AI initialization, target selection or source scheduling.
- `1B282C` is an explicit `C80` store in an external assignment/reset path. It is not an autonomous steering implementation.

The missing work is the complete native AI owner/initializer/dispatcher plus its desired-heading, steering and speed behavior and the correct join before shared vehicle control binding. Simply setting the profile or `vehicle58` on the current player would not establish those dependencies. In particular, the current preset4 branch consumes channels1/3 whereas original AI emits steering0 plus throttle/brake actions through the non-player binding branch.

## Actual resource extraction and profile provenance

The retained RES3 archives were inspected using their name index and zlib blocks. Extracted bytes were hashed directly; no game code was executed.

- `PODS/POD01/POD01.RES`: SHA-256 `08b4d7655cb796da8a29d9b9a5318c0364561c659794ac9de6c9dfed7416eca3`
- Member `Pods\pod01\pod01.ai`: 961 bytes, SHA-256 `fb95da6b56bc3cfe962d99b2ef0a94270d2988bde7e76243106f8ee22d55fd74`
- `TRACKS/TA.RES`: SHA-256 `b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8`
- Member `tracks\ata.spl`: 34,430 bytes, SHA-256 `854bf9f2356485f10fdd4c5ffacf6896fae7fae9a1048241a8337afc6aaeb5bd`

Actual Pod01 AI authoring, excluding comments/blank lines:

```ini
[combat]
Combativeness=0.5
HealthConsciousness=0
WaitToRepairTime=10.0
Defense=0
AttackRecoveryTime=3.0
StartRaceNoCombatTime=9.0
HoverBeforeAttackTime=2.5
WaitBeforeRearEndTime=5.0
FrontHoverPosition=10
SideHoverPosition=3
CombatZoneLength=1500
TargetEvalEnemyAdder=1
TargetEvalHumanAdder=1
CollisionRadius=50
[driving]
Skill=1.0
MyMaxSpeed=600
MyMinSpeed=100
```

**Do not equate every authored key with active original behavior.** The inspected loader `16E630..16E824` selects `[driving]`, reads `DriveAggression` into `1088` (`16E7E0..16E7F0`) and reads `MyMinSpeed` into integer field `108C` (`16E7F4..16E804`). It does not request `Skill` or `MyMaxSpeed` on this path. Constructor stores `16E334..16E33C` establish `1088=0.5` and `108C=0` before loading. The absent-key reader behavior was not newly audited here, so the missing `DriveAggression` key does not justify claiming a fully verified post-load profile state.

ATA has 166 nodes, 166 explicit SpeedLimit fields, authored positions/widths/branch links and Next0 traversal. Limits include both `600.000000` and `606.200012`. The driver obtains them from the real text, never from the Pod01 `MyMaxSpeed` string or a hard-coded replacement.

## API and integration

Namespace: `rr::host`.

1. While the caller already holds the extracted `tracks\ata.spl` text, call `readCourseSpeedLimits50GK(text, bytes, navigationAI.track.count, limits, diagnostic)`. It requires one finite nonnegative SpeedLimit per node, keeps output unchanged on failure and reuses `parseTrackNumber50AD`.
2. Retain one `CourseDriverState50GK`, explicit `CourseDriverTuning50GK` and parsed `CourseSpeedLimits50GK` with the client. The caller must opt in; the adapter does not read environment variables or select itself.
3. At the ordinary per-physics-step input boundary, populate `CourseDriverInput50GK` from the canonical body scene matrix, world velocity, world-Y angular velocity, source `navigationProgress.vehicleC84` / `navigationProgress.reset.vehicleC80`, and per-step `elapsedSeconds=.016f`. A zero dt is allowed for initialization and does not advance either stuck timer. Never pass wall-clock or cumulative elapsed time.
4. `driveCourse50GK(*navigationAI, limits, tuning, input, state, output, diagnostic)` reads those owners only. `true` means Driving. On every false return the emitted output is neutral; inspect diagnostic status. Disabled clears only the host state. Stuck/OffCourse latch until the caller explicitly disables or clears this host state.
5. Compose the usual packet with released buttons, then apply `applyCourseThrottlePacket50GK(output, packet)` while auto input is enabled. It writes only packet7 (left) and packet5 (right). Continue existing sample/append/dispatch/acknowledge and `readVehicleFrame49W` ordering. Do not overwrite frame control state or skip native input events.
6. Manual control disables host ownership immediately. Do not call the packet helper after composing manual axes. On a driver stop, submit neutral axes through the same ingress and report the reason; do not silently restart the race or reposition the player.

Normalized outputs use +1 throttle, 0 neutral, -1 brake. Packet mappings are 0/128/255 respectively. This is the existing declared client packet mapping, not proof of differential steering sign. `steeringPolarity` is explicitly +1 or -1 and must be checked against an actual turn in the live physics path. Positive geometric error points toward positive local X for an upright +Z-forward pose.

## Host-only behavior and limits

- Projects the canonical position onto the source-owned current AI segment, then looks ahead through authored Next0 links. It never stores a new navigation index. It rejects a non-adjacent current/previous pair rather than inventing one.
- Limits lookahead and target speed using prepared authored widths, heading/bend severity, actual SpeedLimit and explicit host tuning. It follows authored AI node positions; it does not synthesize a collision corridor.
- Uses standard host `atan2`/`acos` and a proportional heading/yaw-rate controller, with differential engine/brake demands. It makes no original numerical-equivalence claim.
- Defaults: cruise140, corner minimum50, lookahead45 + 0.35*planar speed capped150, heading gain1.8, yaw damping0.65. These are experimental host choices, not original tuning. SpeedLimit-to-body scale defaults1 as an explicit host interpretation; original AI's `0x3F2E8BC1` speed conversion is not silently applied.
- Stops for excessive corridor/vertical deviation, insufficient real displacement for10 simulation seconds, or no actual source node advancement for40 simulation seconds. Displacement monitoring cannot prove forward progress, so the second source-node timer also bounds spinning/sliding loops.
- No boost, repair, powerslide, reset, scene write, teleport, fabricated contact, portal mutation, branch-choice override, lap update or route advancement is implemented. Existing native boundary failures remain failures for the caller to report.
- Bounded by256 authored nodes, serially called, allocation-free per frame. Adding the new `.cpp` to the client build source list is required. Existing `track_start50ad.cpp` provides its only non-inline parsing dependency.

## Required next verification

Compile the new source and client integration; check full-throttle/neutral/brake packet endpoints, zero-dt initialization, disabled/manual takeover, stuck/off-course neutral output, and actual steering polarity. Then run an unassisted physical course check and report exactly how far source navigation/laps progressed, including any original native boundary that stopped it. A control adapter or successful build alone is not a completed-course proof.
