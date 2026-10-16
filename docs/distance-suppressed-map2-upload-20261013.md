# Skip unused distance-suppressed UV uploads

Base e10acc716df73178b701fa63321bce9399b1abee. One renderer hunk; canonical main is untouched.

## Change

Only the bufferSubData in uploadSpecularUvs50LA is guarded by frameSpecularSecond50LA[slot][bodySlot]. Existing body-enabled handling, actual-component validation, bindBuffer and its following check remain in their exact positions. No persistent dirty/residency state is added. Every map-2-enabled body always uploads its full prepared stream.

## Production proof

- prepareSpecularFrame50LA resets each submitted slot/body flag, authenticates the retained mesh and actual model owner, calculates canonical current-camera distance flags, and runs complete prepareOriginalRacerSpecular50LA before publishing result.secondEnabled to this flag. None of that CPU preparation, admission or distance behavior changes.
- submit calls prepareSpecularFrame50LA before any GL submission work. A failed partial preparation cannot reach the uploader or draws; a later submit reruns the entire relevant preparation.
- drawSpecularSpan50LA consumes precisely the same slot/body flag. When false, it executes every original complete base group, increments the existing skipped-group diagnostic, and continues before any map-2 binding/barrier/draw. Actual draw decisions are unchanged.
- Player begin uploads slot 0 on the pending bank before player draws. drawOpaqueOpponent uploads slot opponent+1 immediately before that opponent's opaque draws, using its actual archetype and the same bank.
- Buffer allocations/VAOs are specific to each archetype/body and both banks. A true flag performs a complete overwrite on the chosen bank before that body's draw, regardless of earlier false flags, bank alternation, same-archetype sharing or failed submissions.
- Same-archetype opponents may overwrite the same stream in serial order. A false body ignores the prior stream; the next true body unconditionally overwrites it. Deferred effects use pass 0 and do not read specular UVs, including after shared-stream reuse.
- Stopped geometry still receives current-camera CPU preparation and the current flag. Re-enabling map 2 therefore uploads the new view-dependent UVs rather than trusting a stale bank.
- Failure paths still reject before publication and retain the previous target. No retry uses a cached residency claim: true always uploads, false never draws map 2. Same-context command ordering is unchanged.

## First-use/uninitialized allocation and shader proof

createSpecularUvBuffers50LA allocates valid bounded GPU storage with nullptr data and wires each bank's attribute pointer. Skipping the first false-path upload can leave unspecified attribute bytes. This is not an out-of-bounds fetch: buffer extent and vertex indexing are unchanged and authenticated.

Actual vehicleGlVertexSource50CZ assigns rrSpecularUv50LA only to sourceSpecularUv50LA. gl_Position depends only on rrPosition and worldViewProjection. viewerUv, viewerColor, sourceColor, viewerWorldPosition and viewerWorldNormal do not depend on the specular attribute. Thus position, clipping/depth, coverage and base/source-alpha varyings are independent even if the unused attribute has arbitrary bits.

Actual vehicleGlFragmentSource50CZ reads sourceSpecularUv50LA only as the selected UV when sourcePairPass50LA == 2. The false distance path sets pass 1 for every base group and never selects pass 2. Pass 0/unpaired/transparent paths use viewerUv. Their output RGB, alpha, discard/depth behavior and destination feedback cannot depend on the unused varying. The attribute may still be fetched/interpolated by hardware; no optimization claim depends on eliminating that work.

## Unchanged boundaries

Full-group base ordering, per-triangle map-2 ordering and every texture barrier remain untouched. Source/destination alpha, depth/fog/coverage and shader math are unchanged. CPU geometry/specular work, native gameplay/AI, canonical owners, resource allocation/cleanup, original error polls and final publication remain unchanged.

The omitted call removes up to vertexCount times sizeof(array<float,2>) bytes of GPU upload for each distance-suppressed enabled body, but no measured performance gain or bottleneck/regression causality is claimed.

Validation: source reads/manual review only. No tests, builds, syntax checks, scanners, profiling, asset decoding, runtime execution or user-PC work. Independent source review passes; see INDEPENDENT-REVIEW.md.

# Independent source review

Pass. No correctness issue found in the one-hunk upload guard.

- Pre exactly matches e10acc716df73178b701fa63321bce9399b1abee. Post guards only bufferSubData; body validation, bindBuffer and GL-error check remain unconditional.
- Cold allocation uses nullptr for both banks. Actual vehicle_shader_gl50cz.cpp shows this attribute only feeds sourceSpecularUv50LA; it cannot affect position, coverage/depth, base UV, colors/source alpha, world position or normals. Its sole fragment consumer is pass 2; all other passes ignore it.
- drawSpecularSpan50LA uses the identical slot/body flag and false skips every map-2 group. Pass uniforms are established/reset and paired materials must be opaque. Stale or first-allocation unspecified UV values have no output dependency for suppressed draws.
- Player slot 0 uploads before player draws. Each opponent uploads slot opponent+1 immediately before its opaque draws, selecting the actual archetype/body/bank. Shared-archetype reuse is safe because every enabled consumer fully overwrites before consuming. Deferred effects never consume map-2 UV.
- Every true flag uploads mesh.vertexCount UVs regardless of prior skips/bank history/failures. Failed begin/draw/commit aborts without publication; next submission repeats preparation and upload. No residency state is introduced.
- prepareSpecularFrame50LA still runs every submission before GL work and always performs complete current-camera distance and CPU UV preparation for all paired bodies, including suppressed/stopped geometry. Finite arithmetic, normal checks and transactional UV publication remain unchanged.

No modifications, builds, tests, syntax checks, scanners, profiling, runtime execution, asset decoding or user-computer work performed. Source-level correctness conclusion only, not measured performance or runtime validation.
