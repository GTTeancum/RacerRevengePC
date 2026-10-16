# Dormant four-sample coverage implementation handoff

UNTESTED. Source edits and manual source review only. No builds, tests, syntax checks, shader compilation checks, sanitizers, profiling, converters, gameplay, source execution or disc access. Root alone integrates and validates. No speed or PS2 visual/raster parity claim.

Base: supplied commit abbf73d740ef23b8529448755d7494c234033e8b, tree 12c4ffb473a0804bf09ba0e3d269b27cdd1c0ef8. Archive verified as 11,547,167 bytes, SHA256 a9b83d548bb8f1609d1605ad90abab2ef204e30dcbe5c889dbb84ec16eaeca45.

## Implemented scope

- Appended NativeTrackCoverage50DS selector to VehicleWindowTrackSpan50DS, default AA0. Invalid selectors reject; dynamic-world requests for the new policy reject explicitly. No source owner or RA adapter sets the new selector in this patch.
- Static material creation retains demand from every cold track span, including culled spans. Actual source AA remains the static owner's responsibility; this selector is native rendering policy, not proof of authenticity or complete RA admission.
- Demand-only GL4.0/GLSL400 floor and procedures/limits; no context request/version/default-target change. All-AA0 instances skip every new GL call/allocation/program and stay single-sample.
- Private native_coverage_storage.inl owns exact actual-viewport four-sample fixed-location RGBA8/depth24 textures for both scene banks, equal-sample destination snapshot, single-sample RGBA16F byte payload/R32F center-depth scratch, and GLSL400 per-sample composition.
- Existing published targets[2], default window, UI targets, menu targets and blur/history resources retain their existing roles and formats.
- AA1 static spans enable native MSAA; ordinary AA0 rendering disables it while sharing the same scene depth samples. Alpha-to-coverage/one, sample mask, sample coverage and sample shading state are explicitly disabled at coverage transitions.
- Complete source base groups and original map-2 triangle order/distance gating remain. Each AA0 map-2 triangle rasterizes its original center coverage/payload, snapshots current four-sample destination color, then composes against gl_SampleID with center depth and LEQUAL/no depth writes. Map-2 alpha is independently replaced.
- Final color-only, equal-extent NEAREST resolve occurs after binders/GC particles and presentation-alpha restoration, before host HUD, race HUD, time stats, speed boost and blur. Existing publication/window/history/finish paths remain in control.

## Narrow shader plumbing

The racer program gets an optional compile-time RR_COVERAGE_PAYLOAD definition only for AA1-demanded instances with source pairs. New pass3 runs the existing source texture, noperspective color, MODULATE/floor/clamp, fog and alpha-test kernel, then writes source byte payload plus gl_FragCoord.z. Base and scratch therefore use the same linked vertex shader and transform. No separate raster vertex program or duplicated source shading kernel was introduced.

The private composition shader repeats only the existing destination-byte round, destination-alpha multiply/floor/add/clamp and independent source-alpha store. It uses a detached immutable snapshot, never in-place feedback or averaged destination alpha. The existing single-sample barrier path remains for all-AA0 instances.

No broader shader/material semantic rewrite was needed. The ordinary source pair cases are still 0/1/2; >=2 admits private pass3 solely under the coverage macro. Extra output location1 is attached only to raster scratch; normal scene draw-buffer selection remains color0.

## File ownership

Modified:
- platform/linux/native/vehicle_window_gl50ds.h: appended coverage contract.
- platform/linux/native/vehicle_window_gl50ds.cpp: validation/demand, GL proc fields, include, allocation/program hooks, static coverage binding, scene selection, source-pair dispatch, resolve, resource/abort hooks.
- platform/linux/native/vehicle_shader_gl50cz.h/.cpp: optional payload program construction and pass3 branch.

Added:
- platform/linux/native/native_coverage_storage.inl: all private resource and pass implementation.
- docs/recovery/native-coverage-implementation-20261015.md: this handoff.

No changes to source AA inventories, static lighting leases/streams, SourceCamera, RA activation, material data, AI, simulation, window context creation, CMake/defaults or tests.

## Coordination with root-owned drawTrack cache

configureBlend is untouched. bindTrackCoverage is emitted immediately before each actual indexed track draw, after UV callbacks, outside the material-reuse block. It changes only coverage capabilities, never depth/blend. Therefore root's pass-local depth/blend tuple cache remains valid across this binding; do not fold coverage into that tuple or elide the new bind using tuple equality.

Target changes occur outside drawTrack; scratch/copy/composition occurs only during racer map2 after the track pass has returned. resetNativeGcLighting resets AA0 at pass entry/exit. Existing cache invalidation at source callbacks and pass boundaries must remain. Static-light renderer hunks overlap in the same file and root must merge by ownership, not overwrite whole files. Replacement files in this ZIP are reference snapshots, not instructions to replace root's working files.

## Resource and failure ownership

- Names are written directly into owned fields before checks.
- Actual texture internal formats/extents/four samples/fixed locations and every new FBO are checked at allocation.
- Extent preparation runs before backend submission, not within a draw callback.
- coverageSpare holds partial failed allocations or old resources whose checked deletion failed. Retry deletes it before allocating anything else.
- New extent is complete before swapping with coverageLive. Old resources then retire through checked deletion. A failed retirement returns failure before submission; published resolved banks remain independent.
- Allocation failure restores texture/FBO bindings while retaining names. Creation/close integrate all names into existing resource-presence and cleanup handling.
- Each payload raster, equal-sample color copy, composition and final resolve is checked. Composition restoration runs on failure, unbinding sampled scratch/snapshot resources and restoring racer VAO/program/AA0/depth policy.
- Final resolve failure returns before bank publication. No depth is copied or resolved.
- Existing gl.finish calls, actual display acceptance, history staging/acceptance and completed track visibility are not replaced by a blit or CPU generation.
- No automatic retry at lower sample count or lower shader capability.

## Source-review checklist for root

1. Review nativeCoverage admission defaults and ensure authentic static AA propagation remains separately gated; do not activate RA with this patch alone.
2. Confirm all-AA0 calls retain old target/shader path, and no public request flips the context version.
3. Confirm coverage hooks coexist with the independent depth/blend tuple cache and static-light changes.
4. Review exact four-sample allocation checks, format support, MRT limit admission and retained deletion state under partial failure.
5. Trace every world/racer/transparent pass to coverageLive.scene[bank] until resolve; no depth-dependent pass may follow it.
6. Compare pass3 source byte/fog/alpha arithmetic to the existing pair kernel, and composition byte arithmetic to original destination-alpha path.
7. Review gl_FragCoord.z scratch -> R32F -> gl_FragDepth -> depth24 precision and clipping/interpolation consistency on central Windows. Same linked raster program reduces position divergence; this remains untested.
8. Review per-triangle scratch validity sentinel (-1), clear behavior, no-depth payload capture, fullscreen per-sample depth rejection, sample-specific alpha and detached snapshot ordering at mixed-AA edges.
9. Review restored texture units/samplers, VAO, program, FBO and AA/depth state on successful and failed bridges.
10. Confirm HUD scratch/blur/history/display fade retain single-sample images and actual completion receipts.
11. Root owns build and visible smoke, capability/format failure injection, resize/cleanup failures, mixed AA0/AA1 occlusion/alpha cases, source-pair overlap/distance cases and performance validation. None ran here.

## Remaining risks and integration gate

The feature is dormant and untested. Per map-2 triangle it adds a source raster pass, full-viewport equal-sample copy and full-viewport composition draw. Performance/memory impact may be substantial; no claim of improvement or acceptable frame time. Actual GL capability availability, shader compilation, raster-depth precision and error-path behavior require root verification before any activation.

The new renderer selector is a rendering contract, not complete original static material admission. Static authored-light binding and actual AA propagation remain owned by root/another helper. This patch never enables RA and does not drop unsupported source surfaces.

Patch packaging used textual diffs against the preserved baseline files, not git apply/check or compiler validation. The ZIP contains the patch, this checklist and changed/new source snapshots for review.

