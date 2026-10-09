# Native finish continuation 50IA

The 2026-10-09 11:16 UTC Linux build and focused finish-continuation run passed
on source `b97e661b0b86e81a08e434045f945ecfa272cae9`. This supersedes 50HZ's
host freeze-and-hide presentation policy. Validation scope and remaining limits
are recorded below and in [current validation](current-validation.md).

## Proven original behavior

Original manager `1B02F0` recognizes a completed lap target, writes the finish
bit at record+48 (`1B035C`, alias racer+CD8 bit0), appends the record to the
finished-order array, and sums its lap times. It neither removes nor freezes
a vehicle. Finished records retain their ordered result.

Player `1A15C0..1A1644` reads controls, then, when the normal manager gate has
ended and player CD8 bit0 is set, continues navigation and invokes automatic
finish controls `1A36A0`. Normal vehicle base update `228620` still follows.
AI update `16E968` requires only manager started bit1 and still runs after the
ended bit is set. Native lap code suppresses new lap increments after CD8
bit0 while continuing accepted map navigation.

In a normal lap-finish race, manager completion depends on human racers, not
the entire AI field. This host has one human. The ordinary results countdown
is four seconds. Original event/audio conditions can hold it near 0.5 seconds;
the original results-screen transition and that wait remain unjoined.

The recovery's source word ledger and detailed source analysis are in the
separate proof handoff. The original ELF identity is
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

## Native integration

- `race_session50HM::simulationAllowed` is separate from human
  `controlsAllowed`. Countdown still holds the field. Racing and complete
  host-results phases permit native vehicle updates.
- The host seals results only after a complete native world frame and real
  completed lap observations. Each new sealed edge sets its bank-local native
  CD8 bit0 and mirrors the same byte into lap continuity. No lap, position,
  orientation, velocity, target, or map index is synthesized.
- The full accepted finish-frame record pass is committed before results
  close. Once the one human finishes, the official `observed` ranking snapshot,
  race clock, finish count, places, times and lap counts stop changing. Still
  unfinished AI retain their last rank/lap row and are not falsely marked FIN.
  Separate `live` observations keep validating every native frame afterward.
- Finished racers continue through the same coupled-body, terrain, portal,
  contact, animation, rendering, and engine-audio owners. A failed native slot
  remains stopped and displays only its last complete batch; failure is never
  treated as a finish or a reason to hide it.
- The native CD8 flag suppresses additional completed laps. Race observation
  rejects any later lap mutation of an already sealed result. Live accepted
  positions and map/checkpoint observations keep updating. Existing sealed
  finish place and time remain unchanged. Unfinished racers can later advance
  their native lap counters, but that never creates a closed official result.
- Human control packets are released after finish, and the host course driver
  no longer owns that human. Recovered `driveFinishedPlayer50IA` executes after
  navigation and before wrong-way/base processing. It writes raw B5C..B68,
  actions B7C/B80, the low-three-bit B9C clear, and original aero steering100.
- Input values are genuine live MAP C88 position, main190 world position and
  local forward/up, cached body speed34, cached world angular A4, authored
  engineDifference5C, and captured post-construction steering10A4. The latter
  is captured once before the leaf can change the live coefficient.
- The selected controller configuration and original handling/geometry/assets
  remain unchanged. The current nonzero twin-engine preset consumes B60/B68,
  which the finish leaf zeros. It can therefore coast rather than steer under
  the leaf's B5C/actions. No unproved single-steering override is installed.
- The original warning subscriber consumes CD8 and stops its own warning
  loops. Engine/binder voices and the single shared audio clock continue.
  The old finish-time `resetVoices()`/`mixer.stopAll()` call is removed.
- Every original opponent descriptor remains submitted with its original
  archetype, cables, flame callbacks, materials and shadow casters. The full
  named standings are retained; no finished opponent is hidden.

## Explicit host boundaries

The full original race manager and original AI controller remain unjoined.
The bounded observer now follows the proved human-end boundary: it closes
all official results after the accepted human finish frame. Existing host
ranking/interpolated finish-time calculations remain host logic; this does
not reconstruct every original timer/ranking arithmetic detail. No late AI
finish is added after that boundary.

The human bank gets ended manager gate3 after its genuine finish. Current
opponents are host course controllers running through native player-frame
adapters, so their bank gate remains2. This lets their navigation continue as
the actual AI scheduler would; it is an explicit adapter boundary, not a claim
that original manager state is per-racer. Both impact-manager input and the
PostWorld manager-byte observation use the current bank's actual projected
application byte, so the late world-force step cannot overwrite the ended bit.
BoostInput's separate profile34 remains2; it is not a race-manager field.
All racers still get their actual
native CD8 finish bit only when their result actually seals before closure.
AI that have not finished at closure keep CD8 clear and their official rows
remain unfinished, even if their later live lap counter reaches the target.

An unpaused fixed-step four-second countdown supplies the recovered leaf and
then changes the existing status label from FINISHED to RESULTS. It deliberately
does not claim the original audio/event wait or menu transition. At zero this
host retains the live scene until restart or quit. The leaf's source timer
zero outputs remain in force; human input never regains control. Continuing
physics in that retained scene is host presentation, not an original infinite
post-race mode. Closed official standings remain frozen in that retained view.

Only the explicit source finish leaf changes the live aero steering coefficient
before its first parameter use that frame. Subsequent physics uses that stable
value. Per-racer parameters are already part of the independent bank snapshot.
There is no teleport, path animation, common-pose replacement, artificial
braking force, hidden vehicle, or forced controller/vehicle-type change.

## Observability and focused validation

The existing race status keeps all named official rows and their last accepted
ranking positions; those positions freeze when results close. Separate
`finish_continuation` live coordinates report continuing native motion. It reports
`opponents_submitted`, `opponents_submitted_finished`, `opponents_hidden_finished=0`,
and `standings_rows`. The additional `finish_continuation` status provides:

- player_finished, player_cd8, player_manager_gate, finish_driver
- finish_driver_ticks and finish_manual_input_ticks, both accepted-frame counts
- player_native_ticks, sealed_place, sealed_time, sealed_laps
- results_closed, sealed_finish_count, results_countdown and host_results_ready
- active_native, opponents_submitted, standings_rows
- racer0_native_ticks/racer0_cd8 through racer7_native_ticks/racer7_cd8
- racer0_live_laps/racer0_x/racer0_z through the same racer7 keys

These keys exist before finish, permitting a real pre-hold baseline. Driver
frame counts require the recovered leaf actually ran. Manual-input counts mean
a supported race key was held during an accepted native finish-driver frame;
that input cannot replace the source output. Every racer tick is sampled from
its own completed native state, not a renderer count or simulated marker.

The focused Linux check compiled 21 units, reused 172 and linked successfully.
One runtime completed with exit 0, no stopped racers and no reported errors.
The player completed three laps in first place at 160.928 race seconds. All eight
native update counters continued after that finish; the player's native CD8 bit
and ended manager gate 3 persisted. Official standings, sealed finish count,
place/time and player lap count remained fixed. Unfinished AI rows stayed
unfinished after official results closed.

The native finish driver stayed authoritative through a six-second S + Down
brake hold and release. The four-second normal countdown reached host RESULTS.
All seven opponent models and eight standings rows remained present. The
original mix appended 10.08 seconds of actual PCM after the human finish.
Thirty-four completed-render samples measured 54.3274–62.5233 FPS in this VM.
The tested binary SHA-256 is
`5f84dd50403ca6d5fa9f5c3bf9a057db1ec464466bf2947a498f725a8040f61f`.

Autodrive remained enabled; this input-rejection check does not validate manual
racing. No restart occurred in this run, so reconstruction of flags, timers,
controls, bank state and result rows still needs rechecking with this policy.
The prior restart pass used the earlier freeze-and-hide policy. Exact
post-finish displacement is evidence, not a success requirement: the source
brakes/coasts. Continued host scene observation does not establish the original
results-menu transition or audio/event wait. Broader integration, listening
quality/clipping and other platforms remain outside this focused result.
