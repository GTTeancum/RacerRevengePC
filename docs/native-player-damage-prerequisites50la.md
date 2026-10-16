# Native Player damage prerequisites50LA

This packet supplies independently complete native leaves. It does **not**
enable a live destructive transition, a nonzero-state49W frame, retirement,
death-camera activation, or a completed human DNF. Existing whole-frame,
debris-provider and destruction guards must remain until their actual owners
are connected. There is no checkpoint/teleport replacement.

## Implemented leaves

- `original_vehicle_damage50la.h`: original2289BC..228A90 state1/2 timer and
  escalation selection, original228BC4..228BE0 state4 flash timer, and the
  actual228390 state3/4 effect-stop call. The timer helper preserves4C's
  committed decrement at an actual subsequent virtual-call boundary. A
  selected handler is explicitly `destructionRequired`, never completion.
- The stop leaf reads canonical800/804/80C effect controls and only invokes
  the existing native Stop84 implementations for current1D4 bit1. Deferred
  stops retain their real semantics. It does not clear a particle pool, timers,
  the808/81C arrays, vehicle40, CD4, or a frame phase.
- `original_wreck_launch50la.h`: one already-reached22CB9C..22CDE4 piece
  launch tail. It uses the existing shared308030 provider for exactly six
  draws, strict binary32 source order, the actual lazy-global amplitudes,
  canonical rigid-body velocity/angular velocity, and their actual W lanes.
  It does not claim earlier collection/piece activation or query registration.
- `chariot_lifecycle49v`: the physical human state1/2 continuation established
  by22791C..2279BC and227FF0..228090. The genuine physical-local pose prefix
  runs;21BEC4 skips the speed-based physical/virtual selector;21C0F4 still
  writes840 from probe10C. Its admission requires838 nonzero and83C zero.
  AI nonzero states and a whole49W nonzero-state frame remain unadmitted.

No new translation unit or build registration is needed. The new header
leaves deliberately have no live caller while the preceding destructive
owners are missing. Adding a caller must preserve their documented source
position; these are not effect acknowledgments or generic reset entry points.

## Authentic reached states and frame control

The existing225C50 decision selects these actual handlers:

| Source call | Handler | Genuine state write |
|---|---|---|
|225D10, slot88|2261B0|state4 at2261C4;4C=0,268=0.6|
|225D9C/225E28, slot8C|226750|state3 only at2272F0, after collection work|
|225E7C, slot90|227370|human state1 at2273C4;4C=2|
|225EC0, slot94|227A40|human state2 at227A94;4C=2|

For nonhuman58,227370/227A40 dispatch the real226750 handler instead of
creating states1/2. State3 cannot be published before its genuine collection
work: its source store is at the end of that work, unlike state4's early store.

Base228620 dispatches states3/4 at228764/228770 directly to228390, skipping
217DE0 controls,2176E0 boost,21A000 animation,21C120 visual response,21A660
chariot,222B20 impacts, repair/effect/transition work, and22A620 water. It
continues with2291D0 packed bytes, BA0 reset, the real undertrack branch,
state4's268 decrement,2208A0 tail, history and ordinary child traversal.

States1/2 retain the ordinary prefix. Under manager58 bit1 and3C=0, old4C<=0
skips the timer branch. Positive4C is decremented before testing impact. A
strict `abs(BA8)>50` calls2261B0. Otherwise strict `abs(BA8)>50/2` or updated
4C<=0 calls226750. The equality cases continue without escalation except for
timer expiry. This is separate from the state0 damage-threshold decision.

The secondary force wrapper is also different:218D90 branches nonzero40 to
218EFC, which adds gradient-noise-driven torque, advances864, applies a local force and
the separate4346F0 damping. The ordinary49W secondary force body cannot be
reused unchanged. Existing coupled/body guards remain justified.

## Real missing dependency chain

The source racer2129A4..212B48 loads `[Wreckage] NumberOfParts/PartN` in
authored order.22BF00/22BFD0/22C310 construct each collection and its tuning.
The collection owns count30, ordered piece pointers38 and aggregate3C. Each
2CB4B0/2CB5A0 piece owns its actual model14C and165E90 physics graph:
rigid bodyD0, gravityD4, world queryD8, collision forceDC and E0/E4/E8 query
objects.22C6A0 activates that graph;22CA60 also detaches and launches pieces.

2261B0/226750 must stop the real240/808/81C/effectD40 owners, hide the real
models with28C6E0 (model flags03 bit2, **not** sceneFlags190/191), remove the
actual690/694/364 collision memberships, activate the ordered collections,
and execute the actual79C broad query/callback. State1/2 additionally rebind
joint360 via1B64D0/1B6670, reset35C with167A50, and inherit main70/B0. The
225C50 suffix has actual kill-credit, boost-stop and speech operations too.

Existing reusable native kernels are graph49P/body49W, type2/type3 collision
resource preparation, spring contacts48L and live-zone memberships50EN. They
are kernels, not a complete debris owner. Current racer-specific collision
wrappers and dynamic-slot capacity cannot be passed off as per-piece owners.

Prepared source geometry is also bounded. POD01/03/10/11 retain all six engine
fragment caches. Several other riders retain partial fragment caches. POD19
has a/c fragment rrscene files but b fragments retain an unsupported vertex
attribute variant. A one-piece chariot with zero damage emitters is the
smallest already-available geometry subset; it is not a replacement for the
other authored collections.

The death child22D4B0 consumes this same ordered racer370/374 graph and each
piece's actual active29 bit and model14C local position. A generated wreck
point list or vehicle-position substitute cannot satisfy it.

## Evidence and verification limits

Recovered by static source and original scalar-word inspection only. Input
ELF `SLUS_202.68` SHA256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
The source packet retains exact preimages, raw-word evidence and hashes.
No guest instructions, game, asset decoder, compiler, test, lint, syntax check,
sanitizer, or user-computer operation was run. Runtime/visual acceptance is
unclaimed.
