# RA static candidate factoring and live-registry boundary

Isolated source-only candidate against central afd5131. No central edits,
execution, tests, syntax checks, conversion, Windows access or admission changes.

## Delivered patch

`ra-static-candidates.patch` changes only the existing static-candidate header
and adds `tests/native/ra_static_candidates50la.h`.

The former preparation body is factored into `prepareSpan(owners, count, actual
Bindings, candidates, radii, bindings)`. It reads only actual mesh/instance fields,
just as before. Existing ColdScene callers delegate with their original contiguous
owners vector. Original checks, source ordering, radii preparation, numeric masks,
constructor flags, spheres, null material-scene/9C and all per-candidate stores are
unchanged. No ColdScene, pod lift or grid pose is synthesized for RA.

The RA candidate Owner accepts a shared ownership lease to the genuine decoded
RA bundle plus actual caller-reserved namespace/type bindings. It retains the
source before creating pointers, prepares the original 178 candidates and owns
all candidate/radii/binding backing. Every vector is sized before its pointers
are published, with no subsequent resizing. Object and backing are nonmovable.
The source shared_ptr is declared first and therefore dies last. Failure leaves
an unpublished private prefix; one attempt only. Allocation failure reporting
uses a fixed string pointer. Const binding views still contain typed mutable
candidate pointers needed by the existing serial registration interfaces; this
is a contractual cold borrow, not a deep-const security capability.

No arbitrary identity values are selected by this adapter. The caller must
reserve actual firstIdentity/type mappings and preserve their namespace through
retirement. A future registry owner must retain this candidate Owner strongly,
not merely its binding-vector pointer.

## Exact remaining broader join

Current `track_runtime_storage50ig_fixture.h::Owner::prepare` cannot be called
honestly with this RA bundle yet:

- Lines 299–345 require ColdScene, read its owner span and SCB, and unconditionally
  allocate the three main/secondary/terrain query references. Optional camera,
  recovery and wreck references are additional. These are reserved capacities;
  they do not prove real body/query owners have been constructed or attached.
- Lines 378–416 verify source mesh/pose/category/sphere and obtain contact material
  A9 from the same SCB zone/ordinal. A future factoring must preserve these checks,
  including every static owner exactly once and authenticated additive triggers.
- Lines 429–443 initialize registries and publish actual static registrations.
  This is a separate consequential owner stage, absent from the delivered slice.
- Lines 445–455 independently reconstruct a new portal world. RA already owns
  the exact portal world used to bind both navigation tracks. Merely passing a
  different copy would create a second topology/distance-cache owner.

The smallest faithful next central refactor is a source-span/world view plus an
explicit owned-portal versus borrowed-existing-portal construction route. The RA
route must retain its actual source/candidate owners and exclusively borrow its
existing LiveZoneWorld, with the same stable backing used by navigation. Existing
tracks should retain their current owned-world branch. No broad setter or boolean
claim can substitute for this lifetime relationship.

Live membership updates mutate portal-link distance caches. Therefore the RA
source Owner needs a narrow, exclusive serial runtime lease before that route can
be added. Its present sourceDecoded const borrow grants no such mutable lifecycle.
The lease must prevent concurrent source reuse/membership calls and outlive every
registry, query, collection cursor and membership object. The candidate here does
not guess the Application/session key, reserve fake body identities, attach dummy
main/secondary/terrain objects, or claim the real dynamic/trigger set is complete.

RA LostAI triggers/particle and other actual world owners also need their genuine
bindings before the final registry extent is sealed. A static-only intermediate
registry must not masquerade as the completed RA gameplay world.

## Review result

Manual comparison found the static kernel accesses only mesh and instance; no
source name, ColdScene pose/lift, grid, height or unrelated runtime state was
silently dropped by the span factoring. Existing empty-source calls still reject.
Both existing and RA callers use the same original creator numeric kernel.
Parser acceptance, allocation bounds in practice, and runtime query behavior
remain untested. Host/Course/SessionAssets admission remains closed.
