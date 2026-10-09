# GC original trigger source: recovered callbacks, blocked native hit leaf

This source-only stage is uncompiled and untested. It contains no live trigger
activation and makes no full GC lap claim. It uses original resources; no guest
code is executed and no memory dump is needed.

The authenticated GC archive defines nine spherical triggers. The eight LostAI
branches target nodes22,172,181,134,186,137,193,49 in source order. Each excludes
the player main collider, repeats after exit/reentry, and omits Reset, whose
source default is0. On entry1B27C0 matches the entering collider against each
racer's main690 identity and writes only its C80 navigation target. Pose, speed,
C84/C88/C8C and unrelated navigation history must remain unchanged.

Gate is trigger9, zone11, one-time-only. Omitted StartEnabled and
RandomTriggerPercent mean true and100, respectively. Percent100 skips the RNG
path entirely. Callback order is the authored Animation action, then PodSound.
Animation1 resolves to opening owner0;101170 sets active90 and integer sentinels
A0/A8=-1. It does not directly move the door. The existing world animation owner
then advances the actual MOT through its ordinary service.

PodSound uses original sfx/race/bigdoor.vag. It selects the nearest registered
listener using XZ distance and starts only when that distance is strictly below
MaxDistance2500. No registered listener means no sound. MinDistance0 affects a
computed gain that is discarded by this action's null sound pointer at104140.
The sound starts once with gain1, pan0, base pitch and no loop in source World4D8
group. This is distinct from a generic spatial emitter or a fabricated door cue.

The reusable entry state records both sides' collision-contact lists, fills null
holes before extending the high-water count, preserves list order, suppresses
repeat entries while still overlapping, and executes actions before reciprocal
adds. The world trigger service rolls interval bits, prunes using coarse overlap,
removes reciprocal entries before exit actions, and leaves high-water counts and
null holes intact. Enable/disable does not erase one-time-fired/contact history.

## Why authentic hit production remains unjoined

Original main690 is exactly main spring358's collider38. The actual native entry
route therefore belongs in its two-pass collector: method2 then method1, in
source order, maximum64 merged owners. Unparented GC trigger colliders register
under method1. Per-hit source E4=294BB0 returns early for category TRIGGER, then
E8=2A00A0 dispatches the actions. Triggers must never generate spring impulses.

The original type2-versus-sphere5 branch121884 calls108E90. At a leaf it interprets
20-byte triangle records, including signed normal fields and vertex indices, and
reads an external vertex descriptor at3DFE60. The type2 COL loader instead builds
32-byte endpoint pairs. The direct caller does not establish that descriptor.
Bounded inspection of all eight actual original POD archives confirms podboxm.col
and chariotm.col tag2; their f.col counterparts are tag0. This is not a filename
role ambiguity. No flags-only root-overlap bypass was found.

The source-backed helper intentionally implements only shape5 collection bounds
and122BA0's lifecycle coarse exit overlap. Using that coarse root overlap for
entry is a host policy and cannot establish108E90 narrow-query parity. Recovering
the native leaf requires explicit provenance for the external vertex context and
byte interpretation, including opaque endpoint words and valid extents. It must
not be filled with a guessed sphere/point or segment/sphere result.

## Independent leaf-result confirmation

The final independent source trace confirms that callback admission requires an
actual nonzero geometric leaf result.122820 skips a zero returned value;
109E0C's coarse admission only recurses, and109C14/109C1C return the final0/1
result. Coarse traversal success is not a hit and cannot authorize an entry.
11BBC0's additional +50/+54/+58 allocations belong to the separate collision+90
result object; they do not convert shape+50's endpoint payload.11FA30..11FB14
updates the prepared matrix/root-sphere cache without replacing that payload.
Therefore no authentic native entry patch is justified by the available typed
ray resource or coarse status. External descriptor provenance and the leaf's
actual byte/opaque-W inputs remain unresolved.
