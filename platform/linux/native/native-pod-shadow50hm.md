# Native pod footprint shadows 50HM

**RETIRED (2026-11).** Replaced by the original projected shadows for every
course: `src/game/original_projected_shadow50ps.h` and
`platform/linux/native/vehicle_window_gl50ds_shadow50ps.cpp`
(`work/scratch/pod-shadows/PLAN.md`). The headers and checks named below are
deleted; this file is kept only as history.


This is a host presentation approximation in the actual native renderer. It
uses three soft directional elliptical footprints fitted to the current
world-space opaque chariot / left-engine / right-engine vertices. It does not
recover original shadow authoring, use a shadow map, or change gameplay.

The receiver is the track fragment's real XYZ, so the projected shape follows
sloped and banked mesh surfaces without a floating ground-plane decal. Only
upward-facing opaque, depth-writing, lit track materials receive it. Flames,
other transparent effects, unlit beams, sky and HUD remain unchanged.

Important limits: there is no exact caster silhouette, self-shadow, world-cast
shadow or nearest-receiver occlusion test. Nearby vertically stacked surfaces
can both receive the footprint. Receiver-height fading bounds this artifact;
it does not establish correct multi-level occlusion. The light is the same
native direction normalize(0.35, 0.85, 0.30) used by world illumination.

## Integration contract

The two headers are standalone/header-only, requiring no build-system change.

1. Add `bool enableNativePodShadows = false` to VehicleWindowRequest50DS. Reject
   true unless an actual track uses sourceVertexColors/callerMaterialTextures.
   Keep the default false for all existing clients; the interactive client opts
   in explicitly. This is a renderer presentation setting, not recovered state.
2. Include native_pod_shadow50hm.h and native_pod_shadow_glsl50hm.h from 50DS.
   Add Storage members `NativePodShadow50HM podShadows[3]{};`,
   `I nativeShadowEnabledUniform = -1;`, and two three-element I uniform-location
   arrays for `nativePodShadowCenter[0..2]` and `nativePodShadowShape[0..2]`.
3. Concatenate RR_NATIVE_POD_SHADOW_GLSL50HM in the track fragment literal after
   its declarations and before main(). After source RGB/texture multiplication,
   multiply color by `rrNativePodShadow(trackWorldPosition, normal)` using the
   actual/fallback normalized world normal. The explicit enable uniform also
   ensures no effect on any transparent/unlit draw.
4. Query the seven new track uniforms after link; reject missing locations.
   Names are exactly nativePodShadowEnabled, nativePodShadowCenter[0..2] and
   nativePodShadowShape[0..2]. There are only six vec4s of per-frame shadow data.
5. In renderer backend preflight, AFTER verifying each submitted part is the
   retained mesh and before begin/draw, build a uint64_t allowed-span mask from
   the renderer's copied materials: opaque blend AND depthWrite are required.
   For explicit neutral pod rendering all spans are opaque. Pass the current
   batch.component[c] and mask to buildNativePodShadow50HM. Failure rejects the
   unpublished frame. On disabled requests clear all three footprints. Their
   only lifetime is renderer-owned value storage; no caller pointers survive.
6. At track shader bind, upload every center/shape with existing uniform4f.
   Reset nativePodShadowEnabled for every pass, and every new material binding:
   request.enableNativePodShadows && !transparent && material.depthWrite &&
   !material.unlit. For unspanned source-color rendering, enable only on the
   opaque pass. Sky uses the separate pod shader and is untouched.
7. Interactive client: enable true unless RR_DISABLE_NATIVE_SHADOWS=1. Log the
   approximation accurately. Keep changes separate from physics/source parity.

Preflight fitting is three passes over at most 4096 source-ordered vertices per
part. It allocates nothing and preserves output on failure. No new GPU buffers,
render targets, texture banks or GL entry points are required. The previous
committed target is intact when fitting fails.

## Focused verification

`tests/native/native_pod_shadow50hm_check.cpp` verifies opaque-only selection,
rotation/translation of current rendered geometry, excluded nonfinite effects,
selected-nonfinite failure preserving output, malformed span rejection, and
disable behavior. Compile/run with strict FP and no broad gameplay sweep.

Actual gameplay pixel inspection is still required after integration. A helper
unit result is not evidence of integrated or original-game shadow rendering.

The pure-host check passed with GCC `-O2 -Wall -Wextra -Werror` and strict FP;
its ASan/UBSan build also passed with leak detection disabled because the
managed executor prevents LeakSanitizer from operating under ptrace. No leak
check result is claimed. The separate
`tests/native/native_pod_shadow_gl50hm_check.cpp` compiles and links the GLSL in
real surfaceless desktop OpenGL, then checks six synthetic 1x1 pixels. It passed:
the directionally aligned ground receiver was 133/255; disabled, distant,
above-caster, vertical and out-of-reach cases were 255/255. This shader fixture
is explicitly not gameplay pixel evidence.

## Software-renderer correction 50HP

The first integrated live capture exposed a serious software-GL cost. A single
same-binary 12-second run disabling only native shadows restored simulation / wall
pace from 0.2956 to 1.0033, with normals still enabled. That implicated the shadow
path, rather than establishing a lighting-wide cost.

The GLSL now uses three explicit constant-index caster calls instead of the
per-fragment dynamic uniform-array loop with `continue`. Projection, fade,
softness and opacity math are unchanged; the CPU geometry fitting is unchanged.
A root-built renderer with that helper change ran at 0.9517 simulation / wall
pace over a bounded 12-second live capture, with completed-FPS intervals 41.29
and 55.98. The initial actual gameplay RGBA/PPM image was byte-identical to the
original shadow build. These are useful bounded results, not a full-race or
sustained 60-FPS claim. Sampling different later track positions and variable
host contention preclude treating the interval ratios as an exact microbenchmark.

The six synthetic GPU receiver checks also passed after the change. The old
helper and initial slow-run receipts were retained outside the source tree.
