# Original death camera, source-only recovery

Status: typed native source candidate, not built or tested. No runtime readiness is claimed. This packet does not widen a gameplay-state guard or construct missing wreckage.

## Source identity and recovered slice

Authenticated `SLUS_202.68` SHA256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

Recovered functions:

- `22CF20`/`286C30`, configured by the actual `174F00..174F38` arguments through `22D1C0`/`286D00`.
- `22D810` setup split at the real query side effects: binding prefix, single-ray query construction/attachment, state writes, final `120230`, `286F90`.
- `22DC90` update, including ordered debris aggregation `22D4B0`, target `2301E0`, position `22E260`, ray-slide `22F560..2301D8`, FOV `22E090`, and the source output masks.
- Death submodes 2 and 4 selected by racer state 3, with real `racer348+118` deciding which; submode 3 selected by racer state 4.

Submodes 0/1 and non-body camera binding remain explicit unsupported source paths. They are not needed for the genuine controller entry from racer states 3/4.

The vtable at `3D18C0` maps update+14 to `22DC90`, terrain validation+18 to `22DFD0`, and target+20 to `2301E0`. `286F90` clears only field50. It does not consume the prior camera packet or the transition's t1 argument; `22D810` copies that packet to its stack but the callee ignores it.

## Native API and owner order

`original_death_camera50kg.h/.cpp` supplies the camera owner and source leaves. It requires the separate `original_death_camera_query.h/.cpp` packet and its nullable-E4 extension to `original_camera_world_query`.

The actual controller transition must retain this order:

1. Remove the old camera scene membership (`177C48 -> 1148D0`).
2. Call `beginOriginalDeathCamera22D810` for the `22D87C/286D20` prefix.
3. On first use, publish the successful real death query cold projection to its distinct object, state, category-list and cache owners.
4. Call `attachOriginalDeathCameraQuery120` with the actual racer690 primary collision zone and body288 world position. This performs the endpoint setter, old query removal and Second-method attachment.
5. Call `selectOriginalDeathCamera22D810`. It writes the camera branch/filter fields, calls the final `120230` refresh, then clears started50 through the original `286F90` behavior.
6. Select the already registered child68 in the controller's one actual camera collection. Attach the camera scene at body288 local F0 in the original primary visibility zone; preserve the actual source saved20 overwrite and mode publication.
7. At the selected-child update cut, call `advanceOriginalDeathCamera22DC90`, then the ordinary scene/matrix/visibility update exactly once.

The camera identity includes the actual session, racer, body and child68 keys. The graph node is the real racer340 body288; it is never substituted with racer194, host view position or an intact wreck proxy.

The single death ray has null E4. It must not invoke Follow's `1792D0` callback, borrow either Follow query object, or replace real query hits with success receipts. Its category filter is `D4=0` with one `D8/DC` entry holding the original static mask; its B8 accept mask remains the constructor's `0xffffffff`.

The caller must synchronize genuine query F8/copy aliases from the shared mutable collection world at the same join used by existing Follow queries. Death queries and Follow queries can mark each other's object records. Admission rejects stale aliases.

## Debris is a required live provider

Every `22DC90` call begins with `22D4B0`, even for submode3. The typed borrowed view must be supplied from actual `racer370/374` ownership:

- All collections in source order, with their real collection30 count and collection38 piece order.
- Every piece's actual flags29. Bit0 gates participation.
- Every active piece's actual model14C scene node, from the separately loaded fragment SLT root.
- `28B180` reads LOCAL F0 XYZ. Resolving its world position or reusing the intact pod root changes the source algorithm.

The algorithm accepts active pieces whose distance to the prior debris110 is strictly less than radius7C (cold value100). It weights each offset by `1-distance/radius`, adds the prior target, averages by the number accepted, and updates the three source filters when flags14C bit0 is set. It does not substitute a body position when none qualify.

As of this source packet, the central native wreck collection construction/activation/physics provider was not present in the baseline. That independent provider must load the actual PHY Wreckage/PartN collections, per-fragment SLT roots and physics owners before gameplay can expose mode1. A declared empty collection or cloned intact-model node is not an implementation of this provider.

## Preserved bounds and failure behavior

- The speed branches use the exact `speed>=1000` boundary. Their interior MIN/MAX sequence is the original `max(min(0,ratio),1)`, even though it is not a conventional clamp; this packet does not repair original tuning or arithmetic.
- Strict nearest-even, separate binary32 operations; no fast math or contraction. Reached nonfinite/subnormal arithmetic and zero division reject. This is a bounded native arithmetic domain, not universal PS2/VU arithmetic equivalence or guest execution.
- Source-unwritten scalar priors, particularly field70, remain initialized native backing and are not attributed to the original constructor. The cold projection does not overwrite field70.
- The live body graph is lazily refreshed in source order. Debris positions remain local reads. Cold/source output W lanes are preserved where consumed; output target/look masks follow the source.
- The original collision loop is unbounded. The native API has an explicit query budget (default128). Exhaustion is a capacity failure preserving earlier effects, never a fabricated miss or accepted pose.
- Null terrain60 is the admitted original path. Nonnull reaches the original `2E83D0` terrain stub and remains an explicit boundary.
- Completed query, graph, filter, state and output prefixes survive later rejection. The caller must terminate that frame; replaying it would duplicate query/filter effects.
- Cross-owner admission rejects overlap with query state, lifecycle, mutable world, graph, output or borrowed piece storage.

Add both new translation units to every supported build source list. No build, syntax check, import, lint, test, gameplay run, emulator, or user-computer operation was performed for this packet. Static evidence formatting only read authenticated ELF words; it did not execute original instructions.
