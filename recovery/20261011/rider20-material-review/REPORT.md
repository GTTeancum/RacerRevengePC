# Source-only Rider 20 material admission

2026-10-11. Requested source baseline: `03df3c864e572e3ef174ec16a920f583f4d9521d`.

## Result and boundary

Staged a three-file native host patch for the two genuine pod-body specular passes and the genuine Rider 20 shadow material/image pair. The active repository was not edited. This patch does not activate Rider movement or establish native runtime readiness.

The preimages match both the requested baseline and the active source at packaging. Active HEAD advanced to `93083c51a0e5cce4c8e98351b9e73e5ae187182a`; that intervening commit added only the retained Rider opening/Back observation document.

Deliverables:

- `files/`: three complete candidate source files.
- `preimages/`: corresponding exact source bytes before changes.
- `rider20-material-admission.patch`: unified source diff, not applied to the active repository.
- `manifest.json`, `PREIMAGE_SHA256SUMS`, `CANDIDATE_SHA256SUMS`: exact source identities.
- `evidence/original-byte-identities.json`: retained original member hashes and raw-byte comparisons.
- `STATIC-REVIEW.md`: completed independent source-only review, finding no blocking defect.
- `DELIVERABLE-MANIFEST.json`, `FROZEN_SHA256SUMS`: final package identities.

Root owns integration. No builds, tests, syntax checks, project parsers/decoders/importers, sanitizer, native or guest launch, GL probe, or PS2/VU execution occurred. Reading C++ source and original bytes, hashing, exact byte comparison, producing a textual diff and writing staged files are the extent of the work. Runtime, image-import, shader-compilation, geometry and rendered-pixel success are not claimed.

## Changes

### Exact specular identities

`src/assets/original_ui_material50kg.cpp` extends the existing typed specular admission with only the canonical authenticated UIRIDER members `ui\\textures\\20_1.psm` and `ui\\textures\\20_2.psm`.

Each requires pass 2, `SpecularMapCompressed`, exact `specmap1`, and destination-alpha coefficient C=1. New 20 body admission additionally requires the actual inherited fog enabled, alpha test enabled, GREATER 0, failure KEEP, and depth GEQUAL. The unchanged common policy requires ALPHA A=0/B=2/D=1, blending enabled, and depth writes disabled. Thus these bodies retain ALPHA `(Cs-0)*Ad/128+Cd` in the existing sealed source-RGBA route.

Both typed wrappers now explicitly reject null `pass.identity` before any catalog equality. Every newly referenced canonical member is present in the authenticated Rider catalog; absence can never admit a null material identity.

The existing exact specmap1 bank proof remains unchanged: canonical PST identity, complete member extent, no CEL or IPUM provenance, indexed kind 1 with one mip, 256×256, and native published image bytes equal to the decoded original mip. The shared original image is reused through its actual owner. No specular texture is generated or substituted.

The additional fog/ATE/GEQUAL checks apply only to the two new 20 identities. Existing 19 body admission and the distinct `pod_wndw` FIX255 admission preserve their prior behavior. The legacy `OriginalUiRider19SpecularMaterial50KM` type/function names are retained to avoid an unrelated API rewrite; the header now documents the exact five admitted PSM identities.

### Exact shadow pair

The existing typed shadow wrapper now admits exactly `19_shdw.psm` or `20_shdw.psm`. Canonical PSM identity chooses both the required texture basename and the corresponding canonical PST identity. A 20 material cannot select the 19 image, nor can the 19 material select the 20 image. Matching a suffix or image layout alone is insufficient.

All existing shadow state and publication guards remain:

- Pass 1, authored DefaultMT DiffuseCompressed, enabled one-map UV0.
- ALPHA A=2/B=1/C=0/D=1, alpha blending enabled.
- Fog enabled, alpha test GREATER 0 with failure KEEP.
- Reverse-Z GEQUAL, depth writes enabled.
- Exact indexed one-mip 128×128 original PST at offset zero with complete member extent; no CEL/IPUM provenance.
- Published image dimensions and complete RGBA bytes equal the selected decoded mip.
- Existing common GS width, sampler, texture function, clamp, PABE/FBA/DAT and profile restrictions.

`platform/linux/native/original_ui_rider_draw_contribution50kg.cpp` routes only the exact additional `20_shdw` basename into this wrapper. That routing is not itself permission: canonical PSM and matching PST identities are still checked by the wrapper. Other shadow names remain rejected by the ordinary lit route's unchanged A=2 restriction.

The legacy `OriginalUiRider19ShadowMaterial50KM` API name also remains, with its exact 19/20 scope documented. Neither converter nor renderer is modified. The established distinct attenuation mode keeps signed-floor `floor((-Cd)*As/128)+Cd`, source fog/alpha testing, surviving-fragment source-alpha replacement, and source depth writes. Zero-alpha fragments are discarded. Ordinary RGB-only submission remains closed to this special blend.

## Original evidence

Read the existing dependency report, independent static dependency review, shadow-policy review, original PSMs and current source.

- `20_1.psm`: 5,198 bytes; SHA256 `9b95ee0473d9db39d0e4afdaccfd1f6dbffc9a1584b6dfe046654df68f0cf13b`.
- `20_2.psm`: 5,198 bytes; SHA256 `4277ecbecda24ec5a00e54c884d89b4e42f17e5e8a18c4e86b2ba3fe077e3b53`.
- `20_shdw.psm`: 2,622 bytes; SHA256 `b648d543ff05022a310db17cd7700d8a1e2452f9180fe10e741b9fb7276bd7be`.
- `20_shdw.pst`: 17,436 bytes; SHA256 `7bbcaf638b960cb335936a84f5880e5b6da1cfadb3c98e2acd22941f6952d57a`.
- Shared `specmap1.pst`: 66,588 bytes; SHA256 `d928592fbf2b0b7bd47075992bdcef7c9be426a766994148dce98ea2360f3f84`.

These retained-byte hashes occur in the source archive catalog. Both 20 body PSMs exactly equal their 19 counterparts after only texture-basename substitution. The shadow PSM equals its 19 counterpart after basename substitution and removing one final CRLF. The 19 and 20 shadow PST byte sequences differ, confirming that the pairing cannot be replaced by sharing the 19 shadow image. Raw bytes were compared without running the project material or texture code.

The original `DefaultMT.psm` supplies alphaReferenceValue=0. Current constructor source supplies fog=1, ATE=1, GREATER, failure KEEP and GEQUAL; the body specular PSMs explicitly disable depth writes while the shadow inherits depth writes enabled. These agree with the prior independent original-state review.

## Deliberately unchanged

- No broad rider roster or suffix-based allowlist.
- No dormant character pass 2 or unreferenced `20_4` admission.
- No new assets, source palette changes, recoloring, white placeholders or image substitutions.
- No changes to ordinary one-mip diffuse policies; the already-supported path preserves the real blended `20c_5` state.
- No changes to the exact 19 kind-2/IPUM cache or five-mip body admission.
- No material registration, callback order, draw ordering, cold union bank, source lifetime, frame sealing, renderer arithmetic or GPU publication changes.
- No movement/session gates, original roster, profile/unlock behavior, selection/Back semantics, deferred/video/audio consumers or gameplay changes.

The wider Rider 19↔20 path still requires the separately documented immutable cold bank, real material-owner epochs and retirement, source-correct movement transaction, actual deferred/video/render consumers and consistent selected-rider profile handling. This narrow material patch cannot justify enabling a movement direction.

## Source-only review cases

These are control-flow observations, not executed tests:

- Exact 20 body PSMs reach the existing specmap1 proof only at pass 2 with the authentic state; body pass 1 continues through the ordinary diffuse route.
- Every other specular PSM remains outside the finite identity set, including dormant character sections and `20_4`.
- A 20 shadow pass with a 19 basename or 19 PST rejects; the reciprocal mismatch rejects too.
- Shadow state changes, wrong mip shape, wrong publication bytes, missing provenance or different bank identity continue to fail closed through the existing wrapper/converter guards.
- Existing source-RGBA conversion and renderer restrictions remain byte-unchanged.
- Rejected preparation assigns no output policy; existing output assignment occurs only after all checks succeed.

Future execution belongs to root's authorized central schedule. No verification is silently inferred from this source review.
