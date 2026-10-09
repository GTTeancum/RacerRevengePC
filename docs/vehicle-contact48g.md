# Original terrain-following response — 48G

48G recovers the complete numerical response of the vehicle's repulsor/terrain-following component, original `0x1B1A30..0x1B2478`, for an associated root scene. This is a substantive part of the original driving model: it decides whether the pod stays attached to the surface, balances force along the surface normal, corrects height, redirects velocity, restricts reverse motion and aligns the scene to the surface. Production code is ordinary C++17 with typed state and no guest addresses or runtime interpreter.

This component is **not a complete vehicle step or a replacement collision model**. It needs an original-equivalent surface query and the earlier collision response. The current tests supply established typed query results; they do not infer a ground ray, substitute generic suspension, demonstrate a race, or measure playable FPS.

## Deliverables and current validation

- `src/game/vehicle_contact48g.h/.cpp`: `applyTerrainFollowing48G` and explicit state, tuning, surface, body and scene structures.
- `tests/native/physics48g.cpp`: executes original instruction words through all numerical helper calls and the root-scene setters, with a bounded test-only oracle.
- `tools/gameplay/recover48g.py`: hash guards original ELF/RAM, extracts nine code blocks, 44 word-verified generated source excerpts and exact stopped-state input words; generates `contact_words48g.h` and `contact_oracle48g.h`.
- `tools/gameplay/dependencies48g.py`: checks 17 specific loader/setter words, 11 further source excerpts, tuning-key chains and query/material dependencies.
- `evidence/gameplay48G`: inputs, source anchors, failure logs, passing logs and exact-hash manifests. The conventional owned-file manifest is `checkpoint-manifest.json`.

Both **x64 and x86** integrated targets pass **241,921 checks each**: **5,900 complete original-word calls**, comparison of 40 output words per successful call, 12 consequential branch coverage gates, and four host floating-point rounding modes. All 13 project CTests pass on both architectures in `evidence/nativePc48G-build-20261003-154328-472/{x64,x86}-tests.log`; exact log/executable hashes are recorded in `validation.json`. The initial standalone pass is preserved in `evidence/gameplay48G/iteration-03.log`. Tests use `/fp:strict`; no full reference runtime is linked.

The compared output contains every exposed flag, prior contact point/normal, recorded velocity change, body and cached velocity, accumulated force/torque, scene basis and position. Testing includes the saved 47W input, randomized finite cases, zero normals/velocities, normal impacts, tangent redirection, reverse restriction, detached/no-hit/pending states, 64 chained response calls with fixed query input, and transactional rejection of nonfinite inputs/overflow. Fixed-query chains are component-state tests, not a simulated route.

## Proven identity, order and source anchors

The immutable 47W snapshot identifies player `0xC5B0B0`, terrain component at vehicle `+0x348 = 0xD113F0`, linked body at component `+0x128 = 0xD110A0`, and body scene `0xCFD8F0`. Scene parent `+0x180` is zero. Its child pointer is nonzero; test fixtures suppress that notification graph while executing the actual root numerical matrix/cache routines. Original children, rendering notifications and scene ownership are outside this API.

All source names below are relative to `work/native/project/generated/`. Preserved excerpt files include original source line numbers and words checked against the ELF. ELF file offset is `address - 0x100000 + 0x80`.

| Original anchor | Established behavior |
|---|---|
| `0x219CD4`, `0x219CE8`, `0x219CF4` | `RunSimulation`: aero → SpringCollisionModel → this terrain response; body wrapper follows later at `0x219E80` |
| `FUN_001b1a30_0x1b1a30.cpp:29` | Response entry and actual persistent state decisions |
| Same file, line 145 / `0x1B1AB0` | Detached-state handling; contact can restore attachment only through the original height comparison |
| Same file, line 826 / `0x1B1D7C` | Distance and orientation detachment conditions |
| `entry_001b1e28_0x1b1e28.cpp:207` / `0x1B1ED8` | Cancel the normal component of accumulated force |
| Same file, line 419 / `0x1B1FC0` | Height correction uses literal `0x3ECCCCCD` (0.4) |
| Same file, line 587 / `0x1B2094` | Choose velocity projection or preservation of scalar speed |
| Same file, line 1001 / `0x1B2280` | Restrict force/velocity only when cached local forward velocity is negative |
| Same file, line 1247 / `0x1B238C` | Reorient scene using original `0x28A090`, with `t2=1` |
| Same file, line 1311 / `0x1B23C0` | Inactive response adds original airborne orientation torque |
| `0x13C550..0x13C678` | Original polynomial angle approximation, not library `asin` |
| `0x28AF90..0x28B17C`, `0x28A090..0x28A528` | Root scene translation and basis assignment, including original arithmetic order |

Vehicle `+0x34C` remains the recovered engine; `+0x350` the aerodynamic model; `+0x354` the SpringCollisionModel. Terrain following is `+0x348`, not any of those three. Both vehicle `+0x358` and optional `+0x36C` refer to separate spring-detection objects. Their update and contact handling precede the force producers. The terrain `active104` value is copied into engine `+0x3C` and aero `+0xE0` near `0x219F9C/0x219FAC`, after this response/integration, for the subsequent producer call. Do not move those copies ahead of the current force calculation.

## Preserved numerical decisions

There is **no elapsed-time read** in this response. Its correction factors remain per invocation; scaling them by a newly invented delta time would change the model.

The function clears `active104`. If detached, it clears detachment when there is no surface, or when `body.positionY + offsetY <= surfaceY + 3`. Otherwise it stays detached. With a surface and no retained detachment, zero prior point/normal fields are initialized from the current query.

The distance is `dot(previousPoint - currentPoint, currentNormal)`. If `forceAngleTest108` is zero and distance exceeds `detachDistance20c`, the original detaches immediately. Otherwise, it evaluates the direction of the cross product between prior/current normals against the old forward/up side axis, together with its original polynomial angle threshold. Detaching clears prior point/normal. Accepting stores the current point/normal and sets `active104`.

An absent surface sets `pending10c`. A subsequent accepted surface clears that pending flag only through the same height comparison; otherwise the current response is inactive. The flags remain integer words, preserving their original nonzero tests and any unmodified input values.

For an active response, in original order:

1. Add `normal * -dot(accumulatedForce, normal)` to the force. This cancellation occurs regardless of the force's sign. The normal is not normalized by this operation.
2. Transform `localOffset1c0` by the current scene, project the gap to the returned point onto the normal, and translate the scene by `normal * (0.4 * gap)`. The body's stored `position60` is unchanged here; the later original body wrapper reads scene position.
3. Compute `abs(dot(normalize(worldVelocity),normal))`. If greater than literal `0x3F490FD9` (~0.785398), remove the velocity's normal component. Otherwise compute `normalize(normal × (velocity × normal)) * originalScalarSpeed`. The latter branch preserves scalar speed using the original operation order.
4. Record the new-minus-old velocity if its scalar magnitude is greater than the previous recorded change.
5. Only if **cached local** forward velocity is negative, transform the accumulated force into local directions and remove a negative local forward force. Then zero cached forward velocity and transform the cached local vector into world velocity. This can overwrite the velocity from step 3. The recorded change from step 4 intentionally remains earlier than this override.
6. Set forward to `normal × (oldForward × normal)` and side to `normal × newForward`. Assign those columns and the normal as up. The original setter is called with `t2=1`, so it **does not normalize these axes**. Adding orthonormalization here would be a behavior change.

For an inactive response, add `(angleE8 * (0.3 * airborneTorque204)) * (0,0,1)` to the existing torque, preserving the two zero multiplies and addition order. Other body/scene fields stay unchanged.

`1B1800`, the earlier pre-query phase, clears `forceAngleTest108`, both hit flags, query exclusion `+12C`, and `largestVelocityChange1d0` at each original substep boundary. The response API deliberately does not repeat that phase. Prior point/normal, pending and detached state persist; the largest-change record is only accumulated within a substep. Do not carry it indefinitely between fresh query phases.

## Tuning provenance

`OrientationSpring` is proved through literal `0x3B1B80`, parser `0x214904`, stack `+0x3EC`, argument `f15` at `0x214928`, setter call `0x21494C`, and final store `0x1B12CC`. Field `+0x204` receives `abs(float32(float32(OrientationSpring * bodyMass) * bits(0x4200CCCD)))`. The native API receives this converted runtime field, not the raw PHY value. Saved value: **100000**.

Constructor `0x1B0EE0` supplies `+0x208 = 0x3F1C61A9` (~0.610865, 35 degrees) and `+0x20C = 0x40A00000` (5), stored at `0x1B10EC/0x1B10F4`. Those defaults agree with the saved object. They are named for observed use; no unsupported tuning-key association is asserted.

`MaxLiftHeight`, `Spring`, `Damping`, `SideForceCoefficient`, and `OrientationSpring` belong to the `Repulsor` parser section. Presence in the section does not mean this response consumes all of them: `1B1A30` does not read spring/damping fields `+0x1F4/+0x1F8`. The original attachment behavior here is direct force/velocity/scene response, not an invented generic spring formula.

## Surface-query contract and outstanding dependencies

The saved query point/normal are exactly equal to record `0x9512E0` fields `+0x30/+0x20`. Terrain fields are:

- local query vectors `+0x130 = (0,3,0)` and `+0x140 = (0,-28,0)`;
- surface point `+0x150 = (4979.41552734375, 2120.56689453125, 12752.63671875)`;
- normal `+0x170 = (0.00633923756, 0.9701666832, -0.2423563451)`;
- response local offset `+0x1C0 = (0,-8,0)`;
- `active104=1`, `hit114=1`, `hit110=0`, `forceAngleTest108=0`, no optional query objects `+0x120/+0x124`.

Setup `1B1240` passes the two contiguous query vectors to `11C3F0` with argument/count 1, mode 1, and the associated body scene. It produces generic query mode `+0x88=2`, with separate query shape/result structures. These facts do **not** by themselves prove a ray or sphere-cast contract.

The pre-query phase applies `11FF90` to the query transform and calls `122660` for a limited object list, or `121920` for generic traversal. If the component was previously active it uses the current body matrix; otherwise it builds an identity basis with the body's translation. At `1B19B4`, query hit bit 5 of byte `+0xF8` and zero `+0x12C` permit copying the result point/normal. If optional `hit110` was already set, the higher surface wins. The stopped state supplies only one observed outcome, not a fresh temporal trace or general traversal proof.

Callback `1B0E50..1B0ED0` reads the collision object's `+0xA4` masks. Saved mask 128 (`global4342B0`) sets `forceAngleTest108`; mask 4 (`global3E0718`) copies object `+0x9C` to query exclusion `+0x12C`; mask 8 (`global3E0720`) appends the collision object to a list capped at 16. Therefore points/normals alone are insufficient for a faithful native query. World-object material/category binding, query selection/face-side policy, shape and boundary behavior must be recovered and compared. The terrain response does not manufacture defaults for them.

The preceding SpringCollisionModel has saved impulse and friction flags enabled, while its separate spring-force flag is disabled. Original normal impulse `297610`, friction `297390`, other-body interaction, detection/ordering and remaining positional branches are **not implemented by 48G terrain following**. Subsequent body scene readback and quaternion reconstruction must use the parent's independently recovered scene routines. A whole substep comparison remains a separate acceptance gate.

## Oracle scope and corrections found during recovery

The oracle executes nine newly extracted original blocks plus frozen direction/axis helpers. It includes actual root-scene setters, cache invalidation/refresh and original helper calls; it does not replace those calls with the native implementation. Original branch delay slots, branch-likely annulment, register values and true return sentinel are checked. Assertions require all 12 selected branches, including force clipping, crest/distance detach, no-hit/pending handling and scene alignment.

The inherited test decoder's `SQRT.S` used the arithmetic source field instead of EE's `Ft`. The parent's parallel scene recovery found this; the new contact oracle also corrects it. Original `109EF8` uses `Ft=12` and is exercised here. Frozen 48E artifacts are unchanged. VU normalization follows the supplied PCSX2 `VUops.cpp:970–1015`: rounded float square root followed by float division, with signed maximum finite Q at zero. It is distinct from scalar EE reciprocal-root handling and the generated zero-Q fallback. The comparison domain is ordinary finite normal arithmetic and the explicitly tested zero cases under host rounding modes. Denormal inputs, PS2 overflow/exception/status modes and complete VU hardware precision are not established by this test.

The first contact test failed after 87 checks because the initial native translation incorrectly required the normal-angle test for a distance-based detach. The exact original branch target proved direct detachment, and the native branch was fixed. This is preserved in `iteration-01.log` and `iteration-02.log`. Numerical comparisons then passed; coverage collection initially missed a branch-delay-slot instruction and was corrected to count executed delay slots. `iteration-03.log` records the final standalone pass. No comparison tolerance was loosened: exposed words match exactly.
