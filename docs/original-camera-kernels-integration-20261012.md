# Reviewed original camera kernels, activation closed

This is the authoritative reviewed kernel-only package. It supersedes the older top-level owner-only `candidate.patch` and historical implementation copies. Integrating it does not select a camera, start an intro, change any racer, schedule a frame, allocate live scene/query owners, request a nonzero blur, or modify rendering. There is no activation flag or readiness claim.

Root integrated these kernels following the user’s engineering-only scope change; the proposed Course session04 was cancelled before execution. No helper build/test/probe or gameplay execution was performed. A later compile of these units establishes only build coverage; it does not exercise the new camera route.

## Exact deliverables

- `camera-kernels.patch`:32 added source/header/catalog files and the2 reviewed48S bounds-file modifications.
- `native-build-list.patch`: only adds12 new C++ units to existing `rr_native_gameplay50dj`; no new target, runtime call, test, UI change or50DU edit.
- `CPP-SOURCES.cmake`: alternative exact source-list fragment. Use either it or the build-list patch, never both.
- `src/game`: frozen integration files. Header-only control/lifecycle/entry wrappers are not necessarily parsed merely by compiling the12 implementation files; their eventual real application includes remain explicit. Do not claim full wrapper build coverage before those headers are part of the actual client.
- `MANIFEST.json`: every file hash, original ELF identity, baseline changes, correction provenance, reviews and asset identity.
- `quoted-include-inventory.json`: static text/hash inventory of126 package/existing quoted include paths, with no missing paths. This was ordinary source-text inspection, not a compiler/preprocessor/import probe and not evidence of successful compilation/linking.
- `reviews`: focused independent source reviews. The latest scene/frame review supersedes earlier controller-header hashes after the single shared selector correction.
- `authored-cam`: unchanged verified original CAM ZIP and transfer manifest. Runtime loading must use the real selected pod's bytes and identity. These files are not automatically installed into the current asset owner.

The source baseline for modified48S files and the optional build list is recorded individually. Original root-confirmed main was1499e840b29d5a968ae7c58d4f252d8a7e7e2d41. Later Course changes may move main; apply the small diffs by their actual file baselines rather than replacing whole unrelated files.

## Dependency closure

The missing prior-candidate dependency `original_gameplay_blur_leaf.h` is bundled unchanged (SHA25626ddc28c8202d2b24ba5b749b61a8cab4642577f78920d2faf9172a894a7ff4b), with its independent review. No other blur candidate/header/GL pass is required to compile these camera kernels. This leaf itself has no producer activation or framebuffer history implementation.

The12 new implementation units are listed exactly in `CPP-SOURCES.cmake`. `collection_bounds48s.cpp` is already present in the target and must not be added a second time. Its only behavioral change allows a shape5 sphere's independently retained cached center to differ from its current shape center, as the actual179A80 setter→120070 route requires; identity matrix, positive radius and localHalf/radius requirements remain.

Existing native-core dependencies are reused in place:49P scene graph,49U rotation,49F camera composition,49N original decimal reader,50Y history type,50JA normal-mode application owner,50BS/BR/CJ/CL/EN collection/search/registration owners,50EO portal geometry,48O pair staging,48P/48U type3 dispatch, and existing collision/compound source views. Their quoted declarations are inventoried; their existing implementation files remain in the same native target. No new external package, executable, decoder, shader, allocation framework, guest/VU interpreter or renderer backend is introduced.

The target already applies `-fno-fast-math -frounding-math -ffp-contract=off`. Preserve those flags and the declared FE_TONEAREST finite normal/zero host instruction-step domains. The reviews do not claim universal EE/VU exceptional arithmetic or exact original CVT.S.W thresholds for every raw RNG value.

## What the kernels close

- Actual independent camera0/common-racer binding, original constructor mode4 and stable mode0/5 timer order, including persistent nondecaying75 terminal blur behavior.
- Corrected procedural zero-cut intro grid/route/activation/pose/FOV/completion, using the one existing shared RNG/cache owner and actual clock samples.
- Authenticated per-pod CAM loading and complete ordinary Follow pose, concrete17A870 query-pair collision response, actual121920 world traversal, sphere108E90 and bounded type3 reuse.
- Existing50Y history reader and a fresh-only constructor write-set projection. No existing live history is reset or replaced.
- Fresh-default attached mode5 pose from real main340/model1B4 and current PHY CE0, with genuine three-draw shake and real mode5 scene transition.
- Actual canonical4C0 scene point registration, portal updates, cold7F0 versus later690 ordinal mapping, and preserved saved20 chronology.
- One shared camera child collection for intro7C/Follow64/attached6C, actual scene114AE0 histories/counters, and stable mode5 composition with real visibility.
- Direct writes to actual BaseTailState50X and VehicleFrameFields49W for intro entry/exit common fields, without any Player/AI alias.

The manifest explicitly identifies the signed-zero MIN/MAX correction; mandatory ordinary handoff23C clear after the real RNG draw; scalar RSQRT correction; full scene span-guard correction; and shared selector correction. Earlier intro/report copies must not replace these files.

## Exact activation work still outside this package

1. **Racer child ownership:** real PodAIEngineEmitter16D8E0/16DA80 at each AI D40 and its source child/backend path. Existing host-selected Player19EEA0 audio cannot supply those semantics or shared-RNG chronology. Work continues separately.
2. **Race root order:** connect the reviewed two-phase ALL8 seam to actual1AAD40/1B48A0/1B4CA0 child ordering and141ED0 flags, including existing world services, scene34, UI44 and timingB4 before camera48. The isolated scheduler is intentionally outside this package; a no-op remaining-child callback is invalid. Legacy ALL8 stays active until this real route is joined.
3. **Canonical ownership:** extend/expose the existing4C0 lighting visibility owner by one camera ref, bind from actual7F0, and retain the one child selector/tick outside racer snapshots. Follow additionally needs two actual4C4 query refs and real cold projections/registration. Default mode5 itself does not call Follow collision or consume50Y history.
4. **Viewport and renderer:** actual cold1A9750(true) and intro-exit1A9750(false), original viewport/aspect/selection fields, real2458C0 work drain. Existing `VehicleWindowGl50DS::finishSubmittedWork` is a concrete compatible native drain after a real committed image; no-image or pending backend work cannot be approved by a fabricated receipt.
5. **Intro exit:** actual mode7 completion sequence, ordered real racer-field leases,2AED60/260320 video request,1AFD80 race countdown/start state,199770 UI flags and original preference restore. Both actual courses lack321_Anims and zero-cut intro audio80 remains null, simplifying those source calls. The blocking next-video260850 fade and its actual clock/render chronology are separate from gameplay blur and remain work in progress.
6. **Display blur ownership:** consuming genuine source requests through actual viewport/sampleable current/prior-displayed targets and accepted displayed-history publication remains the earlier blur-pass integration. No nonzero producer is invented; ALL8 AI1090/1094 remain AI state.

The existing accepted native physics/AI/host presentation remain unchanged by this package. Constructors are fresh-only and must never be used to silently zero an existing live owner. No gameplay or rendered result is claimed.

Root applied the all-platform build closure instead of the earlier Linux-only patch: central interactive311 C++ units, Windows310 C++ plus6 vendored C dependencies. No build has run at integration time. Existing native gameplay camera selection remains unchanged.
