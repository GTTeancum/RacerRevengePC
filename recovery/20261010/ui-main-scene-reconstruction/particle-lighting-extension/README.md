# Authenticated Main particle lighting

## Result and API

The actual original Main scene produces all four corner RGB values `(49,49,49)`, rather than white. `prepareOriginalUiMainParticleLighting50KG(sceneOwner, output)` derives these from the authenticated owner and returns a typed status. It rejects any non-Main ambient or nonzero directional color rather than guessing the general four-normal lighting calculation. Output remains unchanged on failure.

`sourceRgbPacked[4]` contains the post-alpha-clear packed RGB values `0x00313131`. `sourceLightingRgb46To49[4][3]` contains the ParticleCode input RGB rows `(49.f,49.f,49.f)` in source row order. RGB is byte-scale, not normalized. `sourceLightingWBits46To49[4]` retains raw integer bits0x40 written by162158, which is not the floating-point value64. The source ParticleCode subsequently replaces alpha using each particle's separately prepared source brightness.

These are additional native source files, separate from the already frozen scene/fire four-file integration. No project parser, compiler, syntax check, tests, native application, graphics, game, or guest instruction execution was performed. Only static source/ELF byte inspection was used. This helper does not register lights or establish the scene scheduler.

## Exact source derivation

1. SCN ambient ColorRGB is `50,50,50`; directional ColorRGB is `0,0,0`. The existing scene owner retains and authenticates those actual values.
2. Scene reader `277494..2774F0` calls `26FA60`. That reader turns comma-delimited integers into raw float channels without normalization. The absent alpha is255.
3. Scene light creation `277D80`, calling `146F60` at `277E84`, supplies those raw colors. Constructor `146FDC..147014` divides all RGBA components by2 before calling `146C40`.
4. `146C40` stores ambient(type4) RGB into light+5C/+60/+64. Registration `148F90..148FD8` copies it into LightingManager+50/+54/+58. Main's registered ambient is therefore25 per channel. Directional(type2) is added to the live light list with RGBzero.
5. `149310` initially sets directional RGBzero, then reads any active type2 directional light color. Main's only directional light is alsozero, so every camera/corner-normal dot product contributeszero. The positive branch clamp at1 does not bind for ambient25/128. Both sign branches reduce to `trunc(255.f*(25.f/128.f))`, which is49. Before truncation the exactly represented value is49.8046875.
6. `149584` or `1495EC` writes packed `0xFF313131` to each output record+10. `162CD8..162D18` clears alpha on all four records. `1620E8..162168` uploads rows46..49 by extracting R(bits16), G(bits8), B(bits0), converting each byte to float, and storing raw integer bits0x40 into W. The W lane must not be misrepresented as float64.

The normals are not treated as white or invented. Their exact values mathematically cannot change RGB when the authenticated directional contribution iszero. A general nonzero-directional implementation is outside this bounded helper.

## Material template association

The particle material is associated with `AppStart\DefaultMT`, not ScreenMT. Startup `17CAE0` loads global3833E8, then `17CAE8` invokes `24A9A0` with string3A86F0=`AppStart\DefaultMT`. The adjacent path at `17CAF4` uses separate global3833F0 for `AppStart\ScreenMT`.

Particle manager creation `160FFF4` reads3833E8 and invokes the material reference's virtual+0C (`2465B0`). The chain `2465B0 -> 299BE0 -> 14D590` preserves that owner argument and associates it with the actual mmprt material. The four scene/fire source files retain the original PSM; the separate ParticleCode material worker owns its one-pass/two-map native state preparation.

## Evidence

Static text files in the parent `evidence/` directory: `scene-light-reader.txt`, `scene-color-reader.txt`, `scene-light-creation.txt`, `light-create-normalization.txt`, `source-light-create.txt`, `lighting-live-selection.txt`, `particle-lighting.txt`, `particle-lighting-normals.txt`, `particle-render-tail.txt`, `particle-owner-render.txt`, `particle-manager-ctor-update.txt`, `material-reference.txt`, `material-reference-create.txt`, and `default-screen-material-template-bind.txt`.

ELF SHA-256: `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. No original payloads belong in clean-source publication.
