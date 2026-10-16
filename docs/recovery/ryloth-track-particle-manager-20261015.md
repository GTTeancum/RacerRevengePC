# Aggregate-owned RA track particle manager

Base: deff266. Isolated source-only patch. No build, syntax check, test, conversion or runtime invocation.

## Callable API

Include `tests/native/ra_track_particle_manager50la.h` in the actual native translation unit. It supplies the inline Application-aware definition of:

```cpp
bool ra_cold_aggregate50LA::Owner::constructRuntimeTrackParticleManager(
    const original_main_entry50KG::Application& actualApplication,
    std::uint64_t actualSessionGeneration,
    ra_cold_aggregate50LA::Diagnostic&) noexcept;
```

The main-TU include is part of the patch, compile visibility only. No call is added. The aggregate header forward-declares Application and does not include it, avoiding a cyclic factory include. No out-of-line source/build-list change is needed.

## Required cold sequence and identity

Decode the actual RA source, prepare authenticated authoring, mount that same RA archive in the actual Application domain/provider, then prepare the aggregate's full static-world cold stage. Call this new manager stage before `prepareStaticWorldImages`. Static-world cold preparation selects/retains references but has not performed population or captured a collector, so this is a valid reference-staging point.

The method obtains the real Application domain/entities and requires identity equality with the retained world domain, live world context and actual material Application. Provider revision must be exactly the retained world's revision. It rejects prior image attempts, existing current domain collector, replay, and non-fresh demoprt material rows (nonzero references or populated object) before corpus/reference mutation. This deliberately admits only a fresh cold domain phase; an already-published frontend/race collector or serial warm particle owner is not adopted. No alternate Application or collector rebuild is attempted.

RA authoring already authenticates unchanged `particle\\demoprt.prt`, `.psm`, `.pst` as originalMember2/3/4. Their exact byte sizes/SHA256 are:

- PRT 9210 / d4604f2cfd8f40b9704fb63527fdb7574323cbebc9ac34b255b166e9f6be0a5f
- PSM 5022 / ed7c0a0799dc749f0c406503f0c4825b481038bb790cae2f50fc12897bf9ebe1
- PST 132124 / e01d9c314d4c8c31e9d1e56cbbf4573e722c3ce1965e1daddfbeb33844b82bb6

Before use, each must resolve in the same sealed mounted provider to the exact original RA archive identity, member name and byte-for-byte contents. The actual retained SCB selector is demoprt.tga. The source material name comes from that selector through the existing constructor, never from PRT TextureMap. The new code parses the exact PSM and appends it plus PST to the same canonical domain/resolver. It neither mounts a replacement archive nor constructs a second texture table. Appending retains the existing provider revision.

## Genuine source effects and ownership

The one private aggregate `OriginalTrackParticleCold50LA` uses actual `Application::withParticleMaterialDirectory160F88`; source36B970 is selected from that Application's36B530 and restored by the existing scope on return/exception. `prepareNativeTrack` remains unchanged:

1. Retain actual Application/table owners.
2. Allocate the existing single numeric particle manager.
3. Construct Entity141300 using actual shared counter/session generation/stable component key.
4. Set capacity5000 and actual shared depth global2500 in source order.
5. Parse authentic PRT with the existing native atlas parser.
6. Retain the real DefaultMT material150 through2465B0/299BE0, with original slot-store-before-retain.

Partial corpus changes, counter/depth writes and material effects are retained on terminal failure; no retry replaces a pool or rolls source globals back. The manager and its actual Application/domain pins remain owned until checked cleanup. The actual Application address (identity-only, never dereferenced after the call), strong entity/domain owners and generation are retained for the future camera identity check. Generation and Application identities are cross-checked against detached emitter prefixes, whichever of these two explicit stages was constructed first. No emitter is attached to the pool here.

The subsequent existing world image stage performs its single whole-domain population and collector capture, now including the genuine positive particle material reference. Its 44 static families/49 world images remain unchanged; demoprt is a separate particle family, not a fabricated world surface. Canonical demoprt construction may still reject unsupported material/texture state, preserving existing terminal diagnostics. The new stage never claims populated or GPU-ready particle resources.

## Close and future camera borrowing

Aggregate close first checks actual world publication and runtime/Bank obligations. A retained private strong manager borrower also blocks close. Detached emitter prefixes retire before the manager. The existing manager close validates actual Entity refs/links/Application lock, destroys numeric pool, releases material150, and decrements the real shared counter in source order. The source depth global is not reset. Failure keeps owners for retry. World/source/Application domain remain retained until all these steps complete.

Future camera binding must take a strong reference to this exact private manager and release its real model/Entity borrowers before dropping that pin. There is no public manager getter or caller-supplied substitute. The camera implementation must additionally enforce actual SourceCamera visibility/Follow cleanup before Bank destruction, as recorded in the camera plan. This patch alone adds no camera, scene ancestry, renderer bank, GPU publication, emission, full runtime extent or gameplay admission.

### Fresh-domain limitation

A reused frontend/session domain may have a current collector even when this RA aggregate is new; this stage then rejects before appending sources or retaining material150. Creating a different Application/domain to evade that rejection is not an admitted integration strategy. Successful static-world source/reference staging alone must still leave no current collector at this point. The later single whole-domain population captures the combined world/particle graph; this stage never invalidates an existing receipt or rebuilds beneath a reader. Serial-session domain continuation requires a separate genuine retirement/revalidation design.
