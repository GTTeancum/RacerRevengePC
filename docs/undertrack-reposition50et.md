# Undertrack reposition50ET: concrete source operations

The live client stopped at `vehicle_frame49w.cpp`'s `228BBC` boundary after
6,821 advancing ticks. The original caller really invokes `2255B0`; removing
the Y guard or rerunning cold grid initialization is not its behavior.

`src/game/undertrack_reposition50et.h/.cpp` now provides these allocation-free,
native operations. None is a callback receipt or claims the entire reset ran.

- Full bounded spline projection: all node positions and ordered `NextN`, not
  only `Next0`. The existing `tracks\\mta.spl` inside TA.RES is available; the
  older49B missing-asset statement no longer describes the retained project.
- `selectUndertrackReposition50ET`: actual2255B0 geometric selection from live
  C80/C88/C8C and canonical scene world/local positions. On the player route,
  C8C is the anchor and C88 is the initial heading candidate. Outgoing neighbors
  replace that candidate only for strictly greater alignment. Ties preserve
  source order. The AI route uses C80 and its first outgoing neighbor.
- The reset point is the anchor position plus `(0,7EC,0)`. The source computes
  a distance-based interpolated point but never reads it again; the native
  implementation retains that computation without substituting it for BC0.
- `bindUndertrackPlacement50ET`: exact BC0/BD0/C30/C34 inputs to virtual78.
  Requires actual post1ACF70 node+78 bytes and application4C0/4C4 table values.
  Missing bindings reject. NodeReverb, nearest zone, and zero-filled node indices
  cannot replace those inputs. The navigation50EU producer is the joining work.
- Concrete1A24F0 prefix clears; complete1A0240 D40 reset;216A50..216A84 D40/D44
  clears; source reset-basis construction and adjacent parentless orientation /
  position scene setters;216C38..216CE4 main-body/lifecycle reset using50AR/50AT;
 217250..21725C prior-point/850 stores; final D28 clear; complete17BCE0 camera
  motion-packet reset.

The full source reset is still an integration requirement. A successful
selection, binding, scene setter, or body reset does **not** authorize advancing
past228BBC. The existing frame guard has not been changed by50ET.

## Required caller order

1. Derive the selection's world position from the real28B2B0/scene refresh and
   local position from28B180. Navigation must maintain the live progression;
   initial grid indices do not remain valid indefinitely. For the AI route,
   commit the source C80 change at225640, before later effects. The selector's
   output is a prepared transaction, not the original partial-failure behavior.
2. Bind and store BC0/BD0/C30/C34, then execute player virtual78 ->1A24F0.
   Clear109C and10CC bits0,1,2,4. If vehicle3C==0, apply complete1A0240 to the
   actual D40 controller, not a disposable copy. Then call216A30 with `(0,1)`.
3. Clear D40 bit2/D44+24C. Remove existing visibility7F0 and collision-query
   memberships690,694,7A4,348 in source216A90..216AE8 order. The existing50EN
   removals implement the native registry mutations, subject to their guards.
4. Build the source basis. Apply actual optional terrain330 / water334 height
   adjustment if those owners exist. Set main190 orientation/position and
   refresh its world; set hidden198 position then orientation. Run the main /
   lifecycle reset block. Place displayed1B4 and physical19C before the actual
   secondary-body reset. The scene helper's explicit order argument retains
   the reversed position-then-orientation calls at the first198 placement.
5.2167A0 re-registers visibility7F0 with C30, mode2 and cached sphere radius.
   It prepares and attaches690,7A4,348,694 with C34 and mode2, in that order.
   The7A4 preparation derives both endpoints from main190 transformed body1E0.
   It is not sufficient to move only the two spring memberships.
6. Execute the actual reset ground query with50AG sequencing and real124CB0
   candidate/zone membership semantics. Supply the completed BC0 to the second
   placement. The20-query miss exhaustion applies the source lift and emits the
   source diagnostic; a miss is not an invented successful ground hit.
7. Set the final main/hidden/display/secondary/engine scenes in216F6C..21723C
   order. Source28BAB0 transforms the authored2F0/2C0/2B0 offsets. Each present
   display model's virtual90 bounds update remains required.50BA only admits
   its documented fresh-model cache domain; do not coerce live flags to fit it.
8. Reset main body again, write prior7B0/shared zero and850=0, stop the actual
   effects with50BK, then clear canonical D28. The action's a1=0 skips217374's
   damage reset, player-end reset, engine-timer reset, progression reset and
   partner reset. In particular, undertrack reposition must preserve damage.
9. On player58==0,17BCE0 resets the actual camera motion fields.178860 then
   removes camera-scene registration1148D0, copies the original anchor to
   camera-scene+180, and reattaches114770 with C30/mode2. Its virtual18 uses C34.
   The known follow-camera table3CE440 maps that call to179A80, which resets /
   reattaches its sphere24 and segment190 queries, then clears camera motion.
   The current generic native chase view is not this original owner state.

The rigid-body reset deliberately preserves body position/orientation and
scene cache projection.167C40 later reloads integration pose from the canonical
scene. Do not seed those body values from the cold setup as an extra reset
effect. The frame owns re-projecting its sole scene matrices/flags and retaining
all current phase, input, clock, damage and unrelated state.

## Evidence and verification

Existing exact source: `evidence/worldIndex49B/reposition-blocker/exact-evidence/
helper2255B0.txt`, `reposition49b-player-virtual78.txt`,
`evidence/resetProbe50AG/reset216a30.txt`, and
`evidence/chariotLifecycle49V/lifecycle/source/orientation_setter.txt`.
The finite VU arithmetic convention is the existing49U/49V convention, including
true-zero saturated reciprocal and explicit positive-subnormal rejection.

Additional static source reads:1A0240..1A02BC,2C2810..2C2844,
17BCE0..17BCFC,178860..1788E0,2167A0..2169B4,114770..1148CC,
1148D0..114920,179A80..179C9C. The retained originalSLUS_202.68 identity is
SHA256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Those reads decoded immutable instruction words; no game/guest execution,
ee.bin,49C48U,PC/device access, test harness or regression run was used here.
Focused compilation and any gameplay verification belong to the root integrator;
no passing test or closed live undertrack reset is claimed by this note.
