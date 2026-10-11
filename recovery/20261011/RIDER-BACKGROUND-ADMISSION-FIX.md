# Rider entry rejection: authored background material omitted

Status: source-only diagnosis and one-file candidate. No build, syntax check, project parser/decoder/importer, test, sanitizer, native executable, or guest/chip execution was run. Active source was not changed. Root retains all integration and runtime validation.

## Concrete blocking path

The active Rider draw-resource loader unconditionally treats every background material as a missing-PSM fallback:

- platform/linux/native/original_ui_rider_draw_resources50kg.cpp:174-180 iterates riders 1–26, excluding 5, 6, and 13, and calls readOriginalUiRiderBackgroundFallbackMaterial50KG for the first background 01.
- src/assets/original_ui_material50kg.cpp:519-544 validates that fallback. Lines536-538 explicitly fail if UIRIDER contains ui/textures/01_bkg.psm.
- The authenticated UIRIDER catalog contains that exact member at logical offset 1671168, 329 bytes, SHA-256 61efb8158da84d4b56906f32b9aa5b2063efe719f1aadfc11d0249db851b87ce. All 23 backgrounds have authored PSMs in that archive (catalog lines 425–452).
- UIBKG contains the actual corresponding PST images. The absence of PSMs in UIBKG does not imply absence from the material manager's loaded UIRIDER archive.

Therefore this path necessarily rejects the genuine resources when it reaches background 01. It cannot proceed to static image decoding, texture policy admission, text geometry, stats, binder, Rider session, or GPU publication. Coarse runtime logging does not prove that no earlier gate also fails; it currently omits nested Rider details. This is a deterministic source contradiction, not an observed nested error receipt.

Expected nested diagnostics for this gate before the fix: frontend preparation resource(10), Rider draw-resource material(7), material fallbackSource(15), material sourcePC 0x1E8DC0, field "actual UIBKG image and absent PSM". Rider session/stats/binder/GPU diagnostics remain their defaults. The public unavailable result overwrites frontend status/sourcePC, explaining compatibility with the observed status 2, source 0, Main 20, GPU 0 report.

## Narrow correction

Replace the one background-material call with readOriginalUiMaterial50KG(*in.rider,b.registeredName+".psm",...) using the same explicit ScreenMT owner and known runtime state. Keep addImages(b,*in.backgrounds,name,false,false) unchanged.

This uses the existing authenticated original material reader, preserving actual PSM identity, every authored field, exact texture name, and normal material/geometry/registry checks. It does not remove the fallback's safety guard, invent metadata, substitute artwork, or mark the destination ready. It changes neither profile, navigation, RNG, model nor GPU lifecycle.

The retained original 19_bkg.psm and 20_bkg.psm bytes independently authenticate to their compiled catalog hashes. Both author one ScreenObject pass, their real matching TextureMap_1, and ZWriteDisabled=1. The old missing-file constructor would also have omitted that authored Z-write override if its guard were simply relaxed. The correction reads the real field instead.

## Candidate and evidence

- files/platform/linux/native/original_ui_rider_draw_resources50kg.cpp
- preimages/platform/linux/native/original_ui_rider_draw_resources50kg.cpp
- background-material.patch
- evidence/19_bkg.psm: copied from retained ui-rider-controller-reconstruction/preview/members/ui/textures/19_bkg.psm
- evidence/20_bkg.psm: copied from retained next-rider-admission/members/rider/ui/textures/20_bkg.psm
- evidence/background-catalog.txt: exact catalog rows
- evidence/failing-loader.txt and evidence/fallback-guard.txt: active source excerpts

Preimage SHA256: eb35e84df6627017b215771c3ba9e40eab0f5524c933aed0a1084efc961191fb. The active base behavior was c69b59e; root's subsequent aff4c3f diagnostic changes do not touch this file.

## Other gate findings

Main's mode 4 and Rider profile readable=false are legitimate while Main is current. The cold Rider session uses selectionSnapshotForMode(prospectiveMode) and does not need to mutate the shared profile before admission. The profile initialized/shared-link/generation 1 observations therefore do not identify a broken profile.

The source-RGBA renderer request is set from riderInputsReady before renderer creation. Missing renderer support would fail creation; do not bypass originalUiSourceRgbaAvailable.

This candidate is not evidence that the complete Rider cold chain will succeed. All subsequent resource, sampler, model, session, actual GPU upload, entry/render/Back, and cleanup stages remain for root's next centralized allowed validation.

## Independent static review

The separate widget/resource audit confirmed the candidate matches the current preimage, uses the correct existing reader signature and ScreenMT owner, and preserves the authentic UIBKG images and strict downstream checks. No earlier DAT/widget blocker was demonstrated. Its detailed findings are in widget-audit/REPORT.md and widget-audit/REVIEW.md. This review did not execute any project code or establish runtime success.
