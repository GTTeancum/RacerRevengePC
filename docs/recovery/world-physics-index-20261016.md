# Same-source World3D4 physics index

Inactive, source-only extension of the same `ra_pod_scene50LA::Owner` used by
`ra_cold_aggregate50LA`. No second scene or copy of its model data is created.

Source loader270CC4 allocates the index,270CD4 calls27C010,270CDC publishes3D4,
and270CE4 calls27C110 with the actual world zone count.27C010 clears count and
array lanes;27C110 constructs zone-indexed capacities, used counts and pointers.
27B834 defaults StaticType to Standard;27BB08/0C resolves Standard to type1.
27B89C..27B8AC counts only type2 PhysicsObject.27B8DC skips capacity allocation
27C1D0 for zero counts. Actual insertion27C290 and iteration27C330 therefore
have no entries on the admitted Standard-only route.27C030 frees index backing,
not model references.

Admission inspects all1261 authored instance parameter blocks of the SAME
immutable authenticated RA source owner, checks all10 zones and original
per-zone ordinals/counts, and rejects duplicate StaticType, PhysicsObject and
unknown values. Missing StaticType retains the original Standard default.
This does not use the smaller178 decoded collision subset as a model count.
Existing source authentication pins tracks\\ra.scb to
57a0c9a244714d2f5f7b1ffcfb66fb182808348673b25362482458253b78ae81.

The native storage represents the three source arrays as per-zone records.
Per-zone capacity/used/pointer collections remain genuinely zero/absent because
no authored PhysicsObject reaches27C1D0 or27C290. It retains the source owner
through retirement. No Entity reference is fabricated or released by the index.

`prepareWorldPhysicsIndex270CDC` requires this aggregate's actual completed cold
world prefix before any scene topology/model/particle construction or runtime
borrows. It is a bounded loader leaf, not a claim that intervening loader calls
have completed. Failed validation/allocation publishes no index. Explicit
native construction-failure rollback retires only an index actually acquired;
the complete original world destructor is still unavailable. The index exposes
no mutation API, so its admitted zero-entry retirement invariant is preserved.

Normal world cleanup still needs actual3CC collision-object wrappers, scene
loader ordering, LightManager148110, context103DD0 and their concrete owners.
No driver activation, callback completion or world readiness is introduced.

Proof: retained rr-clock-full-static-fields.txt at270CC4..270CE4,
27B740..27B918,27BB08..27BB20,27C010..27C3C8 and original RA source parameter
blocks. Implementation was reviewed statically only; no builds/tests/decoders
or guest code were executed.
