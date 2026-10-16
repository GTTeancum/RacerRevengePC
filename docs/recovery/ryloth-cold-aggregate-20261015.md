# Inactive RA cold aggregate

Candidate: `tests/native/ra_cold_aggregate50la.h`, patch `ra-cold-aggregate.patch`. Source inspection only; no tests, build, syntax check, conversions, gameplay or external computer work.

## Bounded ownership and stages

The aggregate allocates one private `ra_cold_sources50LA::Owner` and strongly retains it until every dependent owner and the exclusive static-world lease retires. There is no source injection setter or second source registry. It composes existing parsers/owners only.

Explicit API dependencies:

1. `decodeSources(originalRaArchive, actualLibm, actualMasks, diagnostic)` authenticates one original RA.RES and calls the existing source bundle. SourceLibmState49T is the caller's actual shared serial owner; collision masks come from the actual category owner. Successful map/AI gate mutations survive a later failure, exactly as the underlying owner already reports.
2. `prepareStaticCandidates(actualNamespace, diagnostic)` requires1. Uses the actual externally reserved namespace/type tags, preserving source178 COL owners, radii, contact/zone provenance.
3. `constructClosedStaticRegistry(diagnostic)` requires2. Owns only the existing178-static prefix and its exclusive serial lease on the same source-backed world. Reserved query capacity remains unbound. No registry/world/query getter or Bank publication is exposed.
4. `prepareAuthoring(diagnostic)` requires1, independently of2/3. Calls trigger source decode, sky selection, ATM raw/framing closure, audio raw closure, particle authoring. These are immutable/preflight operations; their order is a native assembly choice with no claim of recovered whole retail scene-loader chronology. It never calls the material audio Provider, loadAudioBank, image/pixel conversion, canonical population, or renderer.
5. `prepareLighting(actualRacerCount, diagnostic)` requires1. Takes the actual count from the caller's existing cold roster owner; no implicit8 or fabricated roster is supplied. Calls existing prepareRaNative against the exact retained SCB bytes. This is the admitted frozen native authored-zone light policy, not full retail light scheduling. Existing lighting privately owns its usual parsed scene/visibility storage; it does not become a second collision registry or replace the RA live-world cache.
6. `constructDetachedParticlePrefixes(actualApplicationEntities, actualSessionGeneration, diagnostic)` requires completed particle authoring from4. Uses the actual shared Application count owner and native session generation. Counter mutation remains a separate explicit stage. Does not manufacture scene parent, manager, SoulTree graph, callback, active wrapper state or emitted particles.

No unproven ordering among3/5/6 is enforced. Each stage preserves the exact order of its existing underlying implementation. Where full original sequencing requires missing live owners, the aggregate stops at its explicit preparation boundary rather than fabricating loader completion.

Each method is one-attempt. Bad ordering, duplicate attempt, failure or exception latches the aggregate terminal. Completed prefixes stay owned and inspectable by value; a failure never clears the actual libm prefix or automatically decrements completed Application constructors. The caller closes the owner, never retries a partially mutated cold load.

## Publication and teardown

`completedPrefixes()` is value-only accounting; it remains historical after failure/close. `reservedStaticCounts()` reports capacity accounting only. There is deliberately no complete/ready/gameplay capability method, no SessionAssets, no Course binding, and no mutable child getter. `MissingLiveJoins` names remaining collision, scenery, lighting, animation, audio and particle ownership requirements as immutable scope descriptions, not writable flags.

Close first checks the actual particle Entity links/refs and Application lock. If blocked, no other owner is released. Otherwise it closes detached particle prefixes, releases authoring and private light state, then the closed static registry/world lease, candidate/radius backing and finally the single source bundle. Members are nonmovable and child owners remain at stable addresses. No light draw/visibility lease is possible through this API, so close does not invent proof of absent external runtime work. This is an inactive cold owner only.

The three reserved native query slots are still explicitly unbound. Trigger geometry/history, full additive extent, genuine pod/query/scene identities, live membership, image verification, source material/animation lifetime, all42 ambience Sounds and actual D44/stream ownership are not completed by this aggregate.

## Proposed compile-only visibility

In `tests/native/terrain_interactive_drive50du.cpp`, add:

    #include "ra_cold_aggregate50la.h"

immediately after the existing include of `native_ta_particle_cold50la_fixture.h`. The supplied patch includes this single line. No construction, invocation, runtime flag, track selector or registration is added. The next root-authorized Windows build can then parse/typecheck the inactive aggregate and its existing header dependencies as part of the actual main translation unit. This is a proposed verification inclusion, not a claim that any stage has run.

No extra source files, compilation-unit additions or link libraries are required: implementations used here already exist in the main target. Any centralized build result must still establish that claim; no local verification was performed.
