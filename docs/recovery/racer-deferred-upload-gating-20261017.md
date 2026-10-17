# Deferred racer upload gating

UNTESTED implementation, independently source-reviewed. Base vehicle source blob: 83df1db87f602f85ac9e06dad15655d850b70bd2. Integrated vehicle source blob hash: eb9aaa7d8e651acd209c3432383c0e399e89533c. No builds, syntax checks, tests, benchmarks, or gameplay were run. The reviewed diff was subsequently integrated centrally without execution validation.

## Bounded scope

The private upload helpers gain effectsOnly=false. Only the two commit-time deferred reupload sites pass true. The 50CV player upload and every opaque opponent upload remain unchanged. Both cold material tables compute hasDeferredEffects by OR-ing blend != opaque over all material entries. Including unused entries is conservative.

The vertex-transfer predicate is !effectsOnly || !selected.materials || selected.materialSummary[c].hasDeferredEffects. The normal-transfer predicate is !selected.materials || (!effectsOnly && selected.materialSummary[c].normalDependent). Every helper invocation, buffer bind, error gate, material admission, source-specular upload, draw, and restoration remains.

## Consumer proof

The deferred draw loop skips every opaque material, and binds a component's VAO/EBO only after reaching a nonopaque span. An all-opaque component therefore has no deferred geometry consumer. Every possibly consumed component still receives the complete current 24-byte Vertex48B stream, including submitted UVs, current world positions, generated colors and alpha. No new restriction on submitted vertex bytes is introduced. UV overrides remain uniforms.

Normal attributes are still fetched/interpolated and the fragment shader can calculate a normal. They are output-independent for deferred draws: podMaterialUsesNormals requires opaque blending; applyPodMaterial therefore sets useLighting and useNativeGcSelectedLights to zero for every deferred material. Only the useLighting branch uses normal to alter final color. Cold zero initialization and ordinary opaque normal uploads remain. The null-material-table path remains conservative.

Source map-2 UV generation and uploads are unchanged. Deferred applyPodMaterial explicitly resets sourcePairPass50LA to zero. No map-2 pairing is deferred or reordered.

## Ownership, failures, and subsequent consumers

Material policies are copied into private tables during creation. Frame material UV updates and texture names do not alter their blend classification. Both player and alternate archetype tables receive the summary. No user callback occurs inside the synchronous upload/deferred-draw interval.

Same-bank and same-archetype reuse remains correct: every component with a possible deferred consumer is freshly uploaded for that racer. Skipped components have no deferred consumer. Later binders and particles bind their own geometry; no later racer draw consumes skipped arrays. Every subsequent frame's player and opaque opponent uploads retain their complete behavior, including after a failed unpublished frame. No residency cache survives the call.

All original checks remain at their original relative positions. The proposal removes only provably unnecessary bufferSubData calls, not buffer binds or error polls. All prior validation is unchanged, including CPU normal checks and independent live source arrays.

## Source-level cost only

Each effects-only invocation avoids one vertex transfer and 24 times vertexCount bytes for each all-opaque component with a material table. It also avoids normal transfers of 12 times vertexCount bytes for components whose normalDependent flag otherwise authorized a transfer. At most one player restoration and seven opponent reuploads per frame can use this scope. Mixed components conservatively retain their entire vertex upload.

Actual material/component incidence, byte total, driver behavior, and FPS effects were not measured. Coverage bridge count is unchanged. This is not a demonstrated performance gain.
