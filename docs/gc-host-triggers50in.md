# Opt-in GC HOST trigger backend (uncompiled / untested)

This is a separately staged playability option, disabled by default. It is not
an implementation of original native entry-hit production. Enabling it requires
`RR_GC_HOST_TRIGGERS=1` in addition to the existing explicit GC admission
(`RR_GC_INCOMPLETE_WORLD=1`, `RR_RACE_MODE=2`). A TA session ignores the GC option;
a later cold GC selection can use it. Invalid option values fail cold admission.
No native collector, collision result, spring record, physical-body category,
impact event, material or response callback is modified.

## Geometry and scheduling policy

The backend reads all nine original GC.TOB spheres through the authenticated
source authoring owner. For each accepted racer frame, it compares those spheres
with that racer's real main and enabled secondary type2 COL root boxes. It uses
the retained matrix from the successful actual native spring query, not an
invented pod-center distance threshold, fixed synthetic volume, movement ray or
forced Gate activation. It is a global authored-order scan without original
zone/category/collector filtering, and root boxes are a coarse entry proxy.
The recovered source lifecycle coarse-overlap helper is reused for this explicit
HOST entry policy; that does not prove original 108E90 leaf parity.

Original 11FED8 obtains scene100 and 11FA40 overwrites the type2 shape matrix.
The loaded COL localMatrix is therefore NOT multiplied into this prepared matrix
again. vehicle_live_query50eq commits membership.lastShapeWorld, and main's
acceptLiveQuery copies it back into query.worldMatrix. Both query objects and
navigation progress are banked per racer. Observations run within the same bank
visit after the native frame is accepted, using its query-time matrix. They do
not claim a new end-of-step geometry sample or original callback timing.

Slot0 is explicitly PLAYER, independently of selected racer60. Main and
secondary have separate opaque HOST identities. IgnoreTrigger=PLAYER excludes
only player main; secondary entries can own bookkeeping but LostAI never
matches them. The main query observation precedes secondary, and each walks
triggers in TOB order. This is the existing HOST racer participant order.

The source entry/action owner preserves active bits, one-time firing, interval
bits, reciprocal CC lists, null holes and D0 high-water order. Existing contacts
suppress repeated narrow admissions, including touched writes. Actions run in
source order before reciprocal insertion. World service follows racer and
animation service once per accepted shared world step, pruning contacts in
source order with last accepted geometry. A stopped racer retains that last
sample. Menus/focus pause produce no samples or services. Countdown services do
not fabricate entries. Fresh sessions restart the owner; pause keeps its state.

All nine source percentages are100: this backend consumes zero trigger RNG
values, with no new cooldown. The existing animation service keeps its own
original shared RNG draw and timing policy. Gate's one-time flag remains set
on exit; LostAI can run again only after exit/prune/reentry.

## Actions and navigation-domain ownership

Gate action1 activates original opening owner0 through its existing sequence
API. The actual MOT owner then advances normally; no door pose is directly set.
Action2 submits original BIGDOOR only after the recovered strict XZ listener
admission. The selected listener here is the existing committed HOST chase
camera, explicitly not original listener-object parity. Before a real listener
sample exists, there is no listener and no substitute sound admission.

Source sound starts with gain1/pitch1/pan0, no loop, in World4D8. Voice126 is
cold-reserved exclusively, checked unbound/inactive, and uses existing group
volume/duck/pause/output ownership. It sits outside occupied0..109,117,118..125.
The original VAG identity, mono22050Hz,45612frames and no-loop framing are checked
cold. Missing PCM is logged and counted as unavailable, never another cue.
`RR_AUDIO=0` records the action as explicitly disabled. PCM/voice ownership ends
before the shared mixer closes.

The eight LostAI callbacks write ONLY canonical C80 for the matching racer main,
using targets22,172,181,134,186,137,193,49. Reset is omitted/default0; no pose,
speed,C84,C88,C8C or lap state is changed. Cold binding requires AGC195 for
C80/C84 and separately MGC188 for C88/C8C, so target193 is legal in the correct
AI domain. All runtime target admissions use the actual AGC count.

The existing HOST input driver requires previous→current adjacency. Exact
LostAI can change C80 while leaving C84 on an unrelated edge. A separate,
explicit HOST input-only adapter is armed ONLY by an accepted LostAI callback:
- While C80 remains that exact target, retain C84 as the driver predecessor if
  it already has an authored incoming edge.
- Otherwise select the actual incoming AGC segment closest in XZ to the actual
  current pose, with clamped segment projection. Exact ties choose the lower
  authored predecessor index, with no epsilon and no RNG.
- Use that index ONLY in the temporary CourseDriverInput.previousNode; never
  overwrite canonical C84 or relax the generic driver's route checks.
- Retire the pending HOST projection when source C80 changes.

Targets22/172/181/134/193/49 have unique predecessors21/171/180/133/192/48.
Target186 has185 and190; target137 has136 and184. This is why `target-1` is not
used. The nearest-segment selection is HOST steering policy, not recovered AI.
Original AGC source SHA256 is
52f9a4aa91d71a11b1cabdce7e4b7faac3311001d439579d985410d6bd87f306.

## Failure and evidence

All authoring, route, geometry, provider and owner failures are explicit. A
non-Complete runtime result latches a terminal owner status. Earlier accepted
actions are not rolled back and a half-fired one-shot is never retried. Main
ends the session even if an opponent's existing exception boundary initially
catches the failure. Audio's pure preflight checks every current mixer.play
rejection condition; between preflight and apply the single-threaded Gate
animation action cannot change mixer ownership. Unexpected post-preflight
failure is terminal, not a silent sound drop or callback replay.

No hot I/O, allocation or decoding is introduced. Existing successful-frame and
world-service allocation guards include these hooks. No tests, builds, syntax
checks, importer execution or gameplay were run while preparing this stage.

Telemetry identifies `gc_host_trigger_backend` in UI state and emits
`GC HOST TRIGGERS RUNTIME` with `native_entry_parity=0`, accepted samples,
root overlaps, entry/exit callbacks, per-branch C80 writes, Gate activations,
sound requests/culls/disabled/actual starts/unavailable, service count, route
input projections and zero trigger RNG draws. These are diagnostics, not test
results or a completed-lap claim.

Centralized future validation should check default-off behavior, correct cold
GC admission with original assets, natural Gate entry/one-shot/audio, branch
C80-only writes and input projections, exit/reentry, pause/restart, source RNG
accounting and zero successful-path allocations. Native leaf parity remains a
separate blocker documented in gc-trigger-source50in.md.
