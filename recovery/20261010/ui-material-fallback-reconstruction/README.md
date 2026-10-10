# Source-backed Main logo fallback and zero-map material support

## Deliverable

Staged copies only:
- `files/src/assets/original_ui_material50kg.h`
- `files/src/assets/original_ui_material50kg.cpp`

No active repository edits, compiler, syntax check, unit test, native loader, game, emulator, or GPU run occurred. Source was inspected as text and original ELF words only. Root owns integration and the next permitted validation. The code is a new bounded native reconstruction, not recovered original C++.

## Actual missing-PSM fallback

`readOriginalUiMainLogoFallbackMaterial50KG(mainArchive, templates, associatedOwner, runtime, material, diagnostic)`:

- Requires authenticated UIMAIN and explicit ScreenMT association; unknown, DefaultMT, or caller-null owners are rejected.
- Reads actual `Widgets.Dat` and requires Icon35's real Texture=mmlogo reference.
- Requires actual `ui\textures\mmlogo.pst` and verifies the corresponding PSM is absent in the authenticated Main archive.
- Produces exactly one pass. The material/pass PSM identity remains null and the authored section remains empty. It does not create a fake material file or label constructor values as authored.
- Retains archive identity, original widget-member identity, actual texture-member identity, registered material name, attempted original PSM pathname, and creation PC2475C0 in distinct `OriginalUiMaterialProvenance50KG`.
- Uses the authenticated APPSTART ScreenMT state, inherited microprogram, map count1, UV set0, constructor group ID equivalent to `none`, original pass-local TEX1 defaults, context0, and the known runtime435670 overrides.
- Existing textured native preparation admits this distinct bounded provenance, still requiring the real decoded atlas and supported native state. It does not certify that the original mmlogo atlas passed decoding or policy checks.

### Original evidence chain

Original executable: 2,972,720 bytes, SHA-256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. The ELF load segment maps file offset0x80 to VA0x100000. The original executable was never run.

1. Icon creation reads Texture at2AC650, removes the final extension if present at2AC658–684, constructs ScreenTexture and calls2DF0B0 at2AC6B0.
2. ScreenTexture2DF104–120 loads ScreenMT pointer from3833F0 into a3 and calls its wrapper's vtable+0C. Wrapper constructor2461E0 sets vtable3D20C0; word3D20CC=2465B0.
3. 2465B0 forwards to299BE0. 299C00 retains a3;299D2C–3C calls14D590 with the actual material key and that template pointer. 14D5B4–C0 stores it in registry+30[key*4]. 14D67C–694 retrieves that exact association.
4. Registry material loading14D370 invokes29B550. It copies/lowercases the original registered path and appends/replaces suffix with literal `.psm` (string3BDD70) at29B5B4–5F0, then opens TextConfig at29B628.
5. Config state -1 at29B630–638 selects the missing-file branch. 29B640–66C strips the extension. 29B674–6A4 sets TotalShaderPasses=1, constructs246FA0, then invokes its vtable+18. The actual vtable3D2110 has word3D2128=2475C0.
6. 2475C0 resolves the same registered material key, retrieves the associated template14D5E0, and only substitutes ScreenMT if the association is source-null. It takes the inherited microprogram, sets map count1 at247630, loads the original same-name texture at247648, sets UV index0 at247654, and calls2476A0 with a1=NULL at247670–674.
7. 2476A0 gates every config getter on that NULL/non-NULL argument while retaining authenticated template fields. It initializes context0 at247C88 and pass-local TEX1 defaults at247F38–5C. Runtime435670 remains independently honored even in the no-config branch.

## Actual zero-map support

The authored `ui\textures\notexture.psm` is one ScreenObject pass with totalTextureMaps=0, textureMappingOnOff=0, alphaBlendingOnOff=0, alphaTestOnOff=0, foggingOnOff=0, depthTestMethod=1. Its SHA-256 is `f88625f9aaaa564646d77f3a95d47e1d60f94a9bf6c3052beda86bbe00bd5b8a`.

`readOriginalUiMaterial50KG` now admits0 or1 map. For0, it does not read TextureMap_1, invent a name, or perform basename fallback. TextureName stays empty. Authored PSM/section identities and all original template/pass/runtime state remain retained. Two-map ParticleCode is still rejected.

Original247350 checks the stored map count and skips the map loop when0. 247360–440 optionally handles EnableFillColor (absent in actual NoTexture; default0). 247444–4BC selects builder2480D0 only when map count==2; zero and one use2476A0. A zero-map pass thus has real GS-state construction but no texture asset lookup. The current bounded native zero-map policy rejects unsupported EnableFillColor/color fields instead of dropping them.

## Separate untextured native policy

`prepareOriginalUiNativeUntexturedMaterial50KG(pass, OriginalUiNativeUntexturedMaterial50KG&, diagnostic)` is a separate atlas-free API/result type. It requires authored zero-map ScreenObject/TME0 and the supported GS subset. It retains depth method, Z-write, environment context, source-alpha/additive/opaque blend, and explicit depth/alpha-store requirements. It never borrows a white atlas or sampler.

Because TME=0 uses raw vertex channels with no MODULATE, untextured fields are:
- colorScale=1
- storedAlphaScale=1
- blendAlphaScale=255/128
- alphaStoreFromBlendFactor=128/255
- maximumVertexAlpha=128 (bounded admission)

Source framebuffer alpha is independent of RGB blend-factor units. The renderer sibling is staging the separate typed untextured native draw adapter. CPU policy readiness is not GPU readiness.

Actual NoTexture is opaque, so raw vertex alpha storage uses scale1. ScreenMT does not author zWriteDisabled; source constructor defaults it0, so actual normal-mode NoTexture **still writes depth** even with depthMethod1 (always). Do not disable depth writes simply because the rectangle is black or an aperture overlay.

Existing `prepareOriginalUiNativeMaterial50KG` stays the texture-taking policy, returning explicit `untexturedAdapterRequired` for map count0. Existing texture sampler and MODULATE requirements are not reused for untextured draws.

## Integration notes

- Caller must retain a concrete known runtime435670 owner; no default-normal inference is introduced.
- Fallback support is intentionally bounded to actual Main Icon35/mmlogo; it does not permit arbitrary missing/corrupt PSMs to become inferred material assets.
- Generic authored one-map behavior is retained. Descriptor structure adds provenance, and diagnostic enum appends fallbackSource/untexturedAdapterRequired.
- Source-derived Main owner must still own pass-group ordering, actual atlas association, arrow animation, original native depth/alpha-store and GPU submission.
- No tests were written/run by this worker. Root should exercise real authenticated APPSTART+UIMAIN, missing-PSM provenance, zero-map absence of texture lookup, runtime overrides, unsupported fill-color rejection, and real native spans during its permitted validation window.

## Evidence location

Raw word text and table/string metadata are under the sibling research directory `../authored-menu-data-inspection/source-evidence/`. `source-provenance.json` hashes the original intervals and associated evidence. Authored raw asset bytes remain only in the outside-Git research directory; this staged source tree contains no copied game payloads.
