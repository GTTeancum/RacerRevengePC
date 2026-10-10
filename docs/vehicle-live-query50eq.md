# Registered vehicle queries (50EQ)

`queryMainLiveIndexedStaticVehicle50EQ` and
`querySecondaryLiveIndexedStaticVehicle50EQ` connect the native current-body
query to its live registration prefix. The existing zero-E0 50CD entrypoints and
their guards are unchanged.

The caller first finishes source219AD4 collection through the main
spring (`vehicle358->38`) and resolves its ordered candidate indices. That list
predates spring295630. The new entrypoints do not collect or rebuild it after a
membership change. Outer122350 also uses the prior query matrix and cached
sphere:1223E4 calls1202E0 directly, whose type2 branch12044C..1204CC copies the
existing shape matrix;126EB0 reads cached center+60..68 and radius+80. There is
no scene getter or11F680 in that outer path. Use the main membership's prior
`lastShapeWorld` and cached sphere from its accepted attachment/update, not a
fresh48O preparation before collection. Source listings are
`evidence/candidateCollection48S/map/candidate_collect.txt`,
`candidate_bounds.txt` and `sphere_xyz.txt` in the same directory.

The live query entrypoints execute null-pair2959EC ordering:

1. Current canonical scene and root sphere via11FF60/11FE50/11F680 and48O.
2. Actual120230/1202E0/280D40 membership update via50EN, using that scene,
   type2 resource and newly prepared cached sphere.
3. Verify the native object, shape, matrix, resource, update revision and cached
   sphere receipt; execute295A08 limited122660 through indexed50CC/49X.

After the main query, call `prepareSecondaryLiveQueryScene50EQ` before the
secondary entrypoint. It retains50J's lazy parentless canonical scene refresh,
observer/activation/substep gates and body-cache mirroring, while requiring the
secondary's attached typed E0 and matching native object. Legacy50J still rejects
registered callers. A successful getter remains committed if a later secondary
prefix or query rejects; it neither updates membership nor runs the query itself.

`VehicleLiveQueryContext50EQ` carries references to lifecycle, world, the typed
nonnull `registrationE0`, reusable live workspace and an independently retained
`expectedObjectRef`. Main and secondary need separate memberships/object refs.
Legacy admission E0 must remain nonzero; it is only a presence marker, never an
address to dereference. Initial attachment, reset ownership and teardown remain
the caller's responsibility. Mutations are serial after collection has closed.

Use `VehicleLiveQueryResult50EQ.status` to distinguish admission/preparation,
prefix, receipt and source-query failures. `prefixCommitted` means successful
live state persists even if the later query rejects; inspect `prefix` or
`queryStatus` for that stage's result. Do not roll back that prefix as though the
whole original call were transactional. 49X destinations and result still retain
their existing failure-preservation contract. A combined span check rejects
unsafe live/query/input aliasing before either stage writes.

The existing PostWorld, current clean body scene, secondary activation/substep,
null-pair, type2, enabled-query and provider gates remain. No dynamic/provider
callbacks, rootless type2 extension, fallback candidate admission or new
allocation is introduced. Cold query flags must follow constructor295424's
11B570(query,2): `(flagsF8 & ~0x0e) | 2`, so both own query objects are enabled
queries without becoming static targets. The collision-object type byte is
the query's collision type tag; it is separate from shape tag2.

Source inspection only for this change. No build, test, runtime, guest execution
or performance claim is made.
