# Ground-free Follow QueryBridge factoring

Base: dff77a0. Isolated source-only candidate; no build, syntax check, test or runtime invocation. Central checkout is unchanged.

## Change and admission boundary

`ground-free-query.patch` changes only `tests/native/original_follow_query_bridge50kx.h`.

The existing public `QueryBridge(Storage&, const Ground&, ...)` retains its exact argument types, optional defaults and ownership expectations. It delegates with `ground.world` to a private `QueryBridge(Storage&, const rr::assets::World48F&, ...)` overload. The latter has no defaults and is accessible only to QueryBridge itself and the forward-declared, future `ra_source_camera50LA::OwnershipBridge`. That named class is not implemented or instantiated by this patch. No public World/Storage factory or mutable Storage accessor is added.

World48F is read synchronously solely for the original SceneInfo check. It is not retained and supplies no collision geometry. The complete constructor member-initialization list and body remain in source order: fixed arrays and mesh stack allocate first, then SceneInfo validates, then actual Storage/query/filter/normal-mode binding checks, then owner/world/workspace pointers bind, and finally the real death120 table row is reserved with identity and no shape. No query preparation, zone attachment, bank seal, camera publication or RA admission follows merely from construction.

SceneInfo behavior is unchanged: nonempty zones, valid scene_info_block, exact normalized `sceneinfo` identity; omitted UseEnvironmentNode admits the recovered default false; any duplicate field or value other than exact `F` rejects. The unsupported environment443A98 path stays closed.

## Lifetime and partial effects

The future RA bridge must privately obtain the world and Storage from one authenticated RA owner graph. The overload itself does not authenticate an arbitrary pairing, which is why it is private. That future bridge must strongly retain the source/candidates and the matching exclusive live-world lease; no temporary World facade, copied ColdScene or unrelated Storage is sufficient.

Actual main690 and secondary query records, filters, terrain metadata, normal-mode owner and optional recovery/frame/movement owners remain stable borrows. Storage, all its candidate/mesh backing, those records and any real wreck owners must outlive the QueryBridge and its successful retirement. Construction stays before Bank::seal; prepare, query operations and close remain under the same bank0 execution lease as the actual query arena. Close must succeed before backing is destroyed. Incomplete removal must retain the real owners for retry; this patch does not add automatic destructor cleanup or weaken existing failure behavior.

Allocation/validation exceptions retain the existing constructor behavior. No added rollback clears the final death identity reservation, no fake completeness is published, and no source identities are reset. Existing public callers acquire no new lifetime dependency: Ground/world was already needed at constructor time, while actual geometry and all later operations were already Storage-based.

The future friend must construct via direct `new QueryBridge(...)` within its member scope, or equivalent access-checked member code; `std::make_unique` is not a friend and cannot call this private overload.

## Step 2: actual capacity inputs, before any Storage construction

Use the existing `track_runtime_storage50IG::Limits`; do not change or resize a surviving static-only ColdStage. Select a fresh runtime construction route under the same source's exclusive serial world lease. Existing static-only ColdStage currently requests default Limits and remains unchanged.

Required inputs:

- Complete authenticated collision binding sequence: exactly each of the 178 existing RA SCB collision owners once, plus only genuinely constructed additive trigger/animated owners. Authoring records alone do not enlarge the live registry. Final gameplay extent remains pending its actual additive owners; unresolved LostAI history is a separate gameplay issue, not a prerequisite for allocating legitimate camera/body owners.
- `terrainOwners`, `zones`, `portals`, `coldPayloadBytes`: existing ceilings only reduce (1021 terrain owner ceiling, 256 zones, 4096 portals, 32 MiB payload). Actual source world and binding sequence determine counts; do not choose dummy portals or material assignments. Storage's existing zone search validation, exact-once source ownership, MaterialID zone/ordinal binding, payload accounting and additive checks remain authoritative.
- `originalCameraQueries=true` for this actual Follow/death owner route. It reserves three records in addition to main690, secondary spring and terrain348. Capacity is not an attached query.
- `originalRecoveryQueries=true` only with the actual banked694/existing7A4 recovery route and genuine `undertrack_owners50ew_fixture::Owners`, frame, movement and selected collision resources. The constructor requires these arguments all together, a nonzero actual maximum recovery mesh stack, and exactly two reserved recovery refs. If absent, all corresponding arguments/counts remain zero/null; no synthetic recovery objects.
- `wreckPieceQueries`: exact count of real constructed `OriginalWreckQueries50LA` piece owners with authenticated `OriginalWreckAssets50LA`, three distinct actual native identities each, and actual PhysicsBody category. Leave zero for authored-only pending pieces. Each piece adds three records and later must pass `bindWreckQueryWorldCold`; no count is inferred from placement inventory alone.

Let T be the actual binding count, R be 0 or 2 recovery references, and W be the actual wreck piece count. With the camera route, total reserved object extent is T + 6 + R + 3W and must be <= 1024. Main, secondary and terrain refs are T+1, T+2, T+3. Recovery refs (when real) are T+4/T+5. Camera sphere/ray/death are T+4+R, T+5+R, T+6+R. Wreck refs begin immediately after death. For the existing 178-row static prefix alone, the camera-only reservation would be 184, or 186 with actual recovery; these are arithmetic examples, not final RA registry admission.

Two capacities are QueryBridge inputs, not Limits fields: recoveryMeshStackCapacity is the checked maximum nodes+faces+1 across every actually selectable original podboxf mesh; wreckMeshStackCapacity is the corresponding maximum for actual piece E4 meshes. QueryBridge separately scans every actual Storage terrain mesh and compound member and takes the maximum of these extents. Use source mesh counts with overflow checks; no guessed generous capacity replaces source provenance.

Construct and retain the actual query owners before invoking the private overload: main690 and secondary with their actual filters/resources, terrain348 metadata, real collision normal-mode owner and optional recovery owners. Current body/category/namespace construction and Bank Ready prerequisites are still required. `undertrack_owners50ew_fixture::prepareStorage` and `prepare` are the existing recovery storage/source APIs; QueryBridge::prepare consumes actual main membership center/zone and real radius178 later. No new Storage, camera or query readiness is provided here.

## Review status

Manually read the generated unified patch and surrounding current APIs. No source-proven constructor behavior change beyond replacing the constructor-only Ground dependency with private World48F delegation was found. No execution-based validation has been performed. The future typed RA ownership bridge, full runtime capacity route, body/bank/session join and same-light-owner camera binding remain separate work.
