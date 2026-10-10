# Native lighting publication policy: source-only review

Reviewed 2026-10-10. No definite defect found in the requested policy delta or publication-owner implementation. This is a source/API review, not a compilation, syntax check, test, or gameplay result.

## Exact reviewed inputs

- Parent aggregate `files/src/game/ta_object_lighting50js.h`: SHA256 `a24dfc26c14f6575a17967f6f8f55740eaecbf66f66143bf11100a1efaf4c091`.
- Parent aggregate `files/src/game/native_light_publication50js.h`: SHA256 `b7d1f3d10125a1d91834902fcec89f335e40f6480c1cd84b33bcba19dac1b530`.
- Comparison baseline, frozen admission overlay: SHA256 `3fc2c6b9c2e0843d714f0d9282fbcd1f2bd334eafcb3820f9efab1c9706db228`.
- Frozen original selection leaf remains SHA256 `9a4a2ec950fdbcfa67f19928aa5019533010ee783fb5be28c5ac9404a1b44c9a`.

## Findings

The aggregate selection delta adds only the explicitly named `NativePerPublicationRecompute` admission and its nonzero publication-sequence requirement. It does not claim that retail owner A4 is null or that retail lookup 14A860 missed. All pre-existing authenticated registry, actual object, exact ordered query, matching query identity/sequence, sun, actual scene zone, zone rows, portal list, overrides, finite arithmetic, and bounded-count checks remain. Selection ordering and numerical calculations are unchanged.

The noncopyable/nonmovable `Owner` provides one stable model identity and monotonically increasing successful publication sequence. It forces this native policy on its input copy, computes into a temporary, and publishes its retained output, identity, sequence, and caller output only after successful projection. Foreign identities, zero sequences, stale sequences, duplicate successful sequences, and projection failures do not alter its retained state or caller output. A failed projection can be retried; the once-per-sequence guarantee concerns successful publication.

`AlreadyPublished` deliberately does not copy retained output to the caller. Consumers must inspect the primary status and use `current(model, publication)` for an explicit exact-identity/exact-sequence retained read. `current` does not silently return an older sequence. Its borrowed pointer refers to owner storage and may be overwritten by the next successful publication, so consumers must use the normal stable-owner, serialized publication/read lifetime.

This is the authorized native recomputation policy, not recovered retail cache activation or equivalent retail cache-hit behavior. Retail membership-keyed reuse can share an earlier component's matrices; recomputing each actual model's current publication can differ. Genuine scene F0/F4/F8, current vehicle 7F0/scene 19C and zone ownership, exact light query and portals, overrides, stable model identity, publication sequencing, and caller lifetime still require their actual owners. The wrapper supplies none of those facts by itself.

No active checkout files, frozen selection files, or frozen manifests were changed by this review.
