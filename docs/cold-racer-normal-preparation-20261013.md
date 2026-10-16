# Cold exact racer normal preparation

Base commit bf235046be76768cf06f2014be165d000e7102f1, tree 983ab20eb5cc4247482c462687af2623ade11e18. Main is untouched. Four source preimages/postimages plus candidate.patch are supplied.

## Existing owner and scope

VehicleDrawAssets50CR::Storage::Component already owns the immutable semantic scene, compatibility mesh, flattened rigid vertices and prepared mesh descriptor. Its load uses a temporary unique_ptr and commits only that pointer after all components are complete; components/scenes/vectors are never moved afterward. The public owner is noncopyable/nonmovable with no reset/reload. Const asset/descriptor access and documented lifetime preserve source and quantization. Both per-frame batch descriptors and retained renderer meshes borrow this same owner.

The candidate adds an exact-sized vector of VehicleDrawCompressedNormal50LA beside the rigid vector, bounded by the existing 1..8192 vertex admission. Each entry holds three signed bytes plus a validity marker. It is allocated and filled only during cold load from the authenticated original PSG path with normal positive float q. Generic RRSCENE-only and subnormal-q paths retain live decode. No cache map, lookup registry, new framework, per-frame allocation, or renderer field is added. Cleanup is the existing Storage destructor.

## Exact decode and fallback

A small inline helper contains the original loop unchanged: read expanded double; check finite; divide by authoredQ; check finite, [-128,127], trunc equality and exact raw*authoredQ re-expansion; cast to int8. Both fallback frame paths use this helper, preserving failure at the same vertex. Cold loading calls the same helper.

Before cold decode, the code first requires source.sourcePsgBytes, then arithmeticReady (including FE_TONEAREST), IEEE binary64, finite q at least float::min and at most float::max, and exact float-roundtrip q. The source gate precedes all numeric checks/conversions, and the normal lower bound precedes the float conversion. If this optimization admission fails, the optional view remains absent and frame execution retains the original path. Existing prepareVehicleDrawMesh50CM itself already requires FE_TONEAREST; the cache still checks explicitly after it succeeds.

The optional mesh view records its scene identity, count and authoredQ. Both frame producers retain their original current arithmetic/q admission, then select the view only if all metadata matches. Missing or mismatched view uses live decode. Manually prepared descriptors retain defaults and need no API/caller changes.

## Invalid inputs and ordering

Invalid markers do not reject cold load. On the admitted original PSG path all entries are provably compressible by source construction; the validity bit is still checked defensively. Generic finite but non-compressible RRSCENE normals are not decoded early and retain original live rejection timing. Diffuse only reads/rejects a validity marker within its original selectedSlot branch, after its original source.bone check. Unselected material slots retain all compatibility RGBA bytes. Specular checks every vertex exactly as before, including source.bone first. All topology/span/material/count validations stay in their original order.

Cold preparation runs only after prepareVehicleDrawMesh50CM has validated that the flattened scene surface vertex count equals mesh.vertexCount. No intermediate callback or scene mutation occurs before decode. Per-frame loops also retain their original per-span range checks, so valid cached indexing follows the same bounded flattened sequence.

The numeric diffuse/specular helper equations, exact operations, all intermediate finite/overflow checks, source matrices, body distance preparation, camera updates, stopped-racer behavior, UV/RGBA output publication, renderer GPU commands, groups/topology/barriers/state, AI/gameplay and canonical lifetimes are unchanged. Failed cold allocation uses existing load exception handling and leaves the owner empty and retryable.

## Floating environment resolution

Independent review found an actual observer: render_diagnostics50gq_fixture.h reads full MXCSR and terrain_interactive_drive50du.cpp logs it. Unrestricted eager decoding of generic invalid unselected normals could therefore move exception-status changes earlier. The candidate explicitly does NOT use that approach and adds no fenv save/reset.

Cache admission requires the original PSG branch first. import_scene48d.cpp reads raw normals using read.i8 and writes normal_local = raw.normal * scene.quantization[1], where q comes from read.f32. For normal positive binary32 q, multiplying by signed int8 needs at most 31 significand bits, fits normal binary64 (or exact zero), and is exact. Dividing that product by q yields the exact signed int8; the roundtrip multiply is likewise exact. These operations and trunc on the integer add no exception flags or trap condition; neither double subnormals nor float subnormals are involved. All subsequent live decoder checks are preserved or consume exactly these admitted results.

Cache-only RRSCENE data and subnormal-q source inputs stay on the original live path. The genuine native visual caller supplies sourcePsgBytes for each component, so this conservative gate retains the actual intended optimization.

## Integration

This candidate is based on bf23504, including the prior primary-pair feature and numeric invariant hoist. It does not change component-admission conditions. Integrate its two vehicle_draw50cm.cpp hunks into the root's newer source rather than copying its entire base-derived postimage over the independent lower-engine admission hunk.

## Validation and limits

Source reads, isolated code editing and manual diff review only. No builds, tests, syntax check, scanners, profiling, asset decoding or runtime execution. No measured FPS improvement or proof of original regression causality is claimed. Independent source review passes after provenance/q gating; see INDEPENDENT-REVIEW.md.

# Independent manual review

Pass after the PSG/normal-q gate. No blocking findings.

- Exact divide, finite/range/truncation, roundtrip and int8 conversion are preserved in the shared helper.
- Authenticated PSG normals are signed-int8 times binary32 q. Restricting cache fill to normal positive q makes decoding exact, with no added FP flags or trap opportunities.
- RRSCENE-only inputs, subnormal q and unsupported cold arithmetic retain live decoding. Invalid normals do not introduce new load rejection.
- Diffuse selected-material rejection, source-bone checks and per-frame arithmetic/q checks remain unchanged.
- Both hot loops prove flattened bounds before indexing cached entries.
- Storage owns an exact-sized, at most 8192-entry vector per component. It never moves or reloads; descriptors borrow stable storage.
- Publication remains transactional through the final Storage pointer swap. Failed loads expose no cache.
- Default/manual descriptors and mismatched scene/count/q views retain the original fallback.

The original broad cache could affect the existing MXCSR diagnostic observer; the added provenance/q gate resolves that issue without fenv save/reset. Explicit includes have been reviewed.

Canonical HEAD remained bf235046be76768cf06f2014be165d000e7102f1 with no changes to the four main-tree files. Source review only; no builds, tests, syntax checks, scanners, asset decoders, profiling or runtime validation.
