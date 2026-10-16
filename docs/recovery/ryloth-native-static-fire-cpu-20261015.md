# Native FireP0t mesh/material/image cold candidate

## Deliverable

`tests/native/ra_static_fire_visual50la.h` adds a staged native CPU owner. It remains isolated, unactivated and untested. It is deliberately not a retail240110 geometry-cache or SoulTree-class implementation. No original palette count is invented. No GPU completion or emitter stamp is emitted.

## Authenticated source observations

Read-only bounded RES block decompression authenticated the actual RA.RES size4604561 and SHA1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2. Static field reads, not execution of a geometry/image decoder, established:

- Four FireP0t PSGs are816 bytes, at logical1474560,1499136,1523712,1548288. Their hashes match the already-integrated facet pins. Each root matrix is exactly identity, world-space field0, quantization origin(0,0,0).
- Therefore their semantic positions require exactly one actual SCB row-vector placement. No world-space bypass or extra bind-matrix transform is taken for these four exact sources.
- `textures\\gargoylebody.psm`: logical24846336,362 bytes, SHA8540bd531107844a55c9aa4e8973f1860b834c7ae2e861a9265f68d70204cf47. Exact authored fields: TotalShaderPasses1; microProgramName PreLitCompressed; TextureMap_1 gargoylebody; empty TextureCoordinateIndex_1; alphaBlendingOnOff0. All other fields come from the actual DefaultMT/canonical source loader.
- `textures\\Gargoylebody.pst`: logical26591232,22860 bytes, SHAce2b8a9f2b7bcba13a55d099bd9f6c3dcbc0c02de54e80b8db9f997bc141762f. Revision1/flags0, kind1, four ascending16/32/64/128 mips.

The existing native `decodeTexture48E` can decode this ordinary indexed source directly at future cold load. It retains source palette alpha and the proven kind1 CLUT index permutation. No generated pixel hash, offline conversion, generic UI archive identity or fake PSM is needed. The candidate retains the complete decoded Texture48E and builds MaterialTextureAssets50EA from its own base_rrtex. Image identity1 is solely an internal one-image-bank ordinal, never a source model identity.

## Stages and actual owners

`Owner::stage(source, domain, context, actualMaterialPath, diagnostic)`:

1. Requires the actual RA source, prepared Application domain and live minted Video context. Requires the actual provider to be sealed and exact mounted archive/member bytes/hashes.
2. Appends the real PSM/PST to the same domain; prepares the existing four source facets, retaining their actual DefaultMT material slots. This can invalidate previous domain receipts; caller must schedule it during cold preparation.
3. Creates76 native vertices from exact semantic source positions/UVs/authored RGBA. Requires one root, no palette trailer, identity root, world_space0, zero origin, one19-vertex/four-strip source surface, all bone indices0 and component00ff010f. It uses actual parsed triangle indices, never guesses a triangle count. Every span retains actual worldInstance/zone/ordinal/material slot and SCB attachment matrix.
4. Decodes the exact ordinary indexed PST using the existing native CPU decoder and publishes its immutable one-image bank.

All decoded/indexed data are constructed by code authored here only when the root later calls this method. No decoder or owner was invoked by this worker.

`Owner::populateAndFinish(diagnostic)` is an explicit native cold policy, not a hidden renderer action. Call after all intended domain references are staged. It performs the real existing domain14D370/29E340/14D440 phase with the actual context. A partial attempt is terminal for this owner. It verifies every retained slot's exact PSM, one real DefaultMT PreLitCompressed pass, map0/UV0, actual canonical populated PST, identical four mip source ordinals/extents and128² base image provenance. It captures the actual final collector without fabricating registration indices.

`Owner::currentMap(span, map, diagnostic)` re-resolves a prospective canonical map through the retained receipt before a future native renderer uses it. It returns actual canonical raster/sampler/TEXA fields and real final-leaf ordering, not a guessed renderer policy. Collector mutation invalidates it through existing table checks. Map views are immediate borrows; discard before table mutation.

Cold getters expose immutable mesh, source spans, SCB attachment matrices, original decoded texture and pixel bank. They explicitly do not certify GL upload or display submission. The final native renderer must lower the actual returned policy, reject unsupported fields, and bind the same immutable pixels. No ScreenObject relabeling or invented material is used.

## Lifetime and failure

The owner strongly retains the actual Application domain/context and exclusively owns the already-authenticated source facet owner, which retains the single RA source. Existing facets preserve partial material-reference effects. Cold failures do not publish a staged mesh/image; terminal close releases actual material references before domain/context. No duplicate material manager, geometry cache registration, original model identity, query object or collider is created.

Future renderer binding must retain a strong Owner reference through checked GPU retirement. This candidate has no runtime bind or caller-supplied drained flag. The real completedTrackSpanVisibility path must map these exact span IDs back to the four retained worldInstance values before an attachment stamp may advance. GC's native scheduling pattern supplies the existing precedent, but RA's four emitter manager/list and per-emitter runtime joins remain to be implemented separately.

## Integration boundary and remaining work

- Include this header after `ra_fire_model_sources50la.h` in the existing main translation unit for compile-only coverage after root review. No source-list edit is necessary: all methods are inline.
- Do not put these four spans into a diagnostic viewport and call RA gameplay complete. They are a bounded subset intended for the actual RA static-world renderer.
- Renderer policy lowering/GPU image publication and the four real completed-span mappings remain root integration work. Canonical map state is available; no general original raster parity claim is made by CPU construction.
- RA particle manager, actual shared RNG/serial/pause, original per-emitter chronology and strong renderer retirement remain unjoined. No four-collider inference is made.
- Retail240110 failed EOF read remains unrecovered and its strict existing constructor remains unchanged.

No central checkout edits, tests, syntax checks, builds, converters, gameplay or Windows work occurred.
