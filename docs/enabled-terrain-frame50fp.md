# Explicit enabled terrain-following frame seam 50FP

This extends the working `vehicle_frame49w.h/.cpp` with an explicit, cold-bound
enabled `+348` route. Archived/sealed 49W files are unchanged. Pre-edit working
files are preserved under the external `solo-ground50fp/preimage/src/game/`
directory. This is an integration seam for existing recovered numerical code,
not a replacement collision model or a claimed complete gameplay fix.

## APIs and ownership

1. In the cold `Ready` state, call `enableVehicleTerrain50FP(state, bindings,
   initial, followParameters, workspace, diagnostic)`. `initial` is an authentic
   `TerrainPrepareState48M`; follow parameters must come from the actual pod's
   authoring/setup. The API changes the explicit probe domain from disabled to
   enabled. Auxiliary providers remain unsupported.
2. Bind the actual lower endpoint `+140` into the existing
   `parameters.bodies.main.probeLocal140`, with source W=1. Keep the authentic
   query endpoint pair separately; `+140` and the shape's second endpoint are
   distinct source storage.
3. After the current spring detector queries, in `PostWorld`, call
   `prepareVehicleTerrain50FP(state, bindings, parameters, localSegment,
   prepared, workspace, diagnostic)`. This executes `prepareTerrainSinglePair48M`
   and leaves the phase unchanged. Its returned prepared segment/matrix/sphere
   must feed the actual `348` live-membership prefix and limited terrain query.
4. Execute the real query, including its own filters, candidate order,
   registration and candidate effects. Pass the completed `TerrainQueryResult48M`
   to `commitVehicleTerrainQuery50FP`. A miss is a real completed query; a rejected
   query must not be converted into a miss or a made-up surface.
5. Existing `produceVehicleForces49W` and `respondVehicle354_49W` select this
   explicit mode. They require successful preparation/query stage receipts.
   The response uses `applyTerrainFollowing48G` after the actual main spring
   response and before secondary coupling/integration.

`VehicleTerrainProbe50FP` owns only fields absent from the former disabled
projection: surface normal170, previous point180/normal190, the sixteen collected
identities, current-candidate identity, immutable response parameters, and native
mode/stage receipts. Existing flags104/108/10C/110/114/118/11C/12C/1E8, point150,
and vector1D0 stay solely in `main.components.probe348`. Temporary preparation
and result records are projections, not additional persistent authorities.

At substep entry, gravity still consumes the previous completed `104` before
the current preparation clears per-query fields. Late feedback still uses the
actual completed response. Frame graphics/chariot consumers see the canonical
previous `1D0` until the source `1B1800` boundary resets it.

## Source ordering and graph effects

- `1B1800`: existing 48M preparation clears query flags/count/exclusion and the
  three velocity-change lanes; the fourth lane uses initialized `434260`.
  Prior active/pending/detached and point/normal histories are retained.
- `1B19B4`: existing 48M limited-query output supplies surface point/normal,
  callback flags, exclusions and collected identities. No ground normal is
  inferred by this frame seam.
- `1B1A30`: existing 48G response performs attachment, force cancellation,
  original height correction, velocity response and nonnormalized scene axes.
  No added time factor, clamp, cockpit lift or forced-secondary activation exists.
- The `28AF90` translated local matrix is committed to the real graph and its
  descendants invalidated. Reverse-motion's intervening inverse-direction getter
  refreshes the translated cache. The `28A090` basis setter then invalidates the
  graph again. The body position/orientation/history remain for `167C40` readback.
- An enabled response may therefore leave main190 dirty before `Post368` or
  `MainIntegration`. This is explicitly admitted only after its response receipt.
  Existing secondary getters refresh it when called. Otherwise the actual
  `223030 -> 28BBB0` getter is performed before the already-ready static-impact
  numerical component; when that branch is skipped, body readback owns refresh.

The old mode still requires `probe100=0`, rejects both auxiliary providers, and
executes the exact old disabled preparation/response. Merely setting `probe100`
does not enable this extension. The new mode requires its separate enabled
binding, `probe100=1`, and ordered native stage receipts.

## Authored inputs and boundaries

Relevant recovered references are `docs/vehicle-contact48g.md`,
`docs/terrain-prepare48m.md`, `docs/terrain-query48j.md`,
`docs/terrain-workspace48m.md`, and `docs/undertrack-reset50ew.md`.

Actual `1B1240` setup distinguishes:

- `first130 = bodyCenter1E0 + (0, halfHeight324, 0)`
- `lower140 = (first130 - 2*offset) - (5*MaxLiftHeight)*up434280`
- `localOffset1C0 = (first130 - 2*offset) - MaxLiftHeight*up434280`
- `airborneTorque204 = abs(float32(float32(OrientationSpring*mass)
  * bits(0x4200CCCD)))`
- constructor angle208 `bits(0x3F1C61A9)` and distance20C `bits(0x40A00000)`

The localOffset1C0 distinction was checked directly against the hash-verified
immutable ELF words `1B1480..1B1540`; no saved runtime state or guest execution
was used. Query metadata, application category masks, authoring parsing and live
owner registration remain the caller's authenticated inputs and separate work.

The optional effect route currently rejects grounded-forward active work when
its optional7FC and manager51C owners exist. An explicitly absent development
topology may use the existing original early return; this seam does not bypass
the active branch or claim dust effects. No effect logic is expanded here.

## Validation status

Source-reviewed only by this change's author; no builds or tests were run here.
The parent integration must compile and verify real captured terrain/query data,
enabled/detached/miss transitions, disabled-path behavior, substep receipt
rejections, graph caches, source response ordering and allocation behavior.
Component validations documented by 48G/48M are not a whole-frame proof of this
new join. Live query prefixes can commit external membership before narrowphase
failure; the query adapter must report that status rather than promise whole-step
rollback. Frame API failures preserve persistent frame state, excluding caller
workspace and diagnostics under their ordinary disjoint-storage contract.
