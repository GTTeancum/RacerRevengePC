# Shared GC particle native draw closure

Date: 2026-10-12. Source-only implementation and independent source review.
No build, test, syntax/import check, decoder/converter execution, gameplay,
GL execution, guest instruction execution, emulation, or user-PC access occurred.
This is an integration candidate, not a rendered-result or bit-parity claim.

## Concrete source/GPU joins

- `OriginalGcParticleRenderOwner50KG` borrows the actual prepared scene manager;
  it never copies into an independent effect pool or advances any emitter/RNG.
  All actual 5,000 active slots and all 129 authenticated demoprt frames can
  enter the same source2500-depth-bucket ordering, including fire, fog and racer
  effects. Descending buckets and same-bucket LIFO follow162870. Runs are source
  state alternations within packet batches of12, never atlas regrouping.
- The original GC PSM/PST owner authenticates source bytes and consumes the
  existing authenticated APPSTART DefaultMT. It retains a dedicated GC identity
  domain and the original128x1024 atlas, source palette/alpha unchanged. Two
  contexts choose alpha-over/additive; they are not two material passes.
- CPU projection reuses the actual numeric ParticleCode helper below the Main
  identity boundary. The public Main52..75/alpha128 wrapper remains unchanged.
  Shared numeric admission accepts the concrete caller's frame count and alpha
  bound; no Main scene/archive/camera identity is fabricated.
- Geometry uses the current owned640x448 gameplay viewport's screen140,
  combinedScreen250 and focalPixels1D8. It verifies the exact expected source
  screen matrix and separately rounded core.combined*screen140 equality. Stale
  core/viewport pairing is rejected. The actual fog phase must already have
  applied near1/far=fog.end38 to this same camera owner before projection.
- GC four-corner lighting uses actual registered global+50 ambient(40,40,45,64)
  and the real selected-light publication from that same GC owner. It verifies
  owner identity and actual directional diffuse(85,85,80), then evaluates the
  camera-facing corner normals from(-1,+1,.25),(-1,-1,.25),(+1,+1,.25),
  (+1,-1,.25). Native binary32 square-root normalization and selected-light
  policy are explicit; original VU/cache/exception parity is not claimed. The
  selected-zone ambient is never substituted for the global source ambient.
- Fog stays authored151/160/177, using row33.z=end and row33.w=255/(start-end).
  Fog proximity brightness comes from the actual persistent manager records.
  Mirror-Y remains nested under mirror-X exactly as1626E0..1627A8.
- Source160BB0/160C5C initializes glow brightness255. Nonglow brightness64,
  persistent fog, linear fade, then nonlinear fade branches retain source order.
  Source alpha conversion retains nearest-even packed-byte behavior. Invalid
  brightness, missing owners, exceptional arithmetic and unsupported native
  blend domains reject rather than clamp or manufacture particles.
- `VehicleWindowGl50DS::commit` invokes the actual indexed GPU draw in the SAME
  unpublished gameplay framebuffer/depth transaction, after Binder and before
  HUD/display blur. It uploads real per-frame vertices/fog streams and reuses
  lower-level native GL buffers/shader machinery, without enabling a UI owner.
  Five thousand particles fit existing bounded GPU storage. Material sampling
  is checked linear/repeat against the immutable authentic atlas on every draw.
- Source reversed packet Z is mapped back to the current core49F native depth:
  nativeDepth=(8388591-packetZ)/8388590. Source GEQUAL becomes native LEQUAL;
  depth writes stay disabled. Draw preflight matches every sourceCombined word
  against the exact world projection50DX packing/Z conversion. Full drawable
  stretching matches the existing native original-camera presentation policy.

## Blend and alpha-store contract

Source MODULATE alpha is byte-quantized and clamped before ALPHA. Normal fog,
dust and spray stay in the native source-over domain: vertexAlpha*atlasMax<=16384.
The actual full atlas maximum is255; persistent fog alpha is approximately26,
and ordinary nonglow fading effects are bounded64. An unsupported source-alpha
factor above1 rejects; it is not silently clamped.

Additive glowing particles legitimately use source A/128 above1. Their RGB is
premultiplied by the byte coefficient in the shader and drawn with ONE/ONE,
which preserves positive normalized-target saturation without a SRC_ALPHA cap.

Each source run then executes a real separate alpha-only indexed draw, with
blending disabled, depth writes disabled, the SAME depth/GT0 test and primitive
order. It replaces destination alpha with sourceA/255 rather than accumulating
source-over alpha or retaining old presentation alpha. Both authenticated states
have DATE=0 and use source alpha for RGB, so deferring the independent A writes
within that run cannot change any intervening RGB result. No other source
material is interleaved inside this native transaction. The subsequent host HUD
uses no blend and the gameplay blur computes fixed-coefficient RGB while
preserving destination alpha. This closes the explicit separate-alpha-store
requirement for this native tail without making a blanket material waiver.

Native GL texture interpolation, quantization and raster coverage remain native
policies, not PS2 rasterizer emulation. Existing world/pod rendering and the
Binder/particle relative tail ordering are an explicit native one-view schedule;
full source material-tree interleaving and hidden source render state are not
claimed. This draw does not change the separately declared emitter/update order.

## Activation contract

Prepare source resources and `renderOwner.prepare(session.particles())` once,
including when the manager is empty before real Course exit. After templates
are available, load the retained original PSM/PST into
`gc_particle_materials50kg_fixture::Owner`. Create its GPU bank with the current
50EB request before `renderer.create`, then supply the three request pointers:
`gcParticleCpuTextures`, `gcParticleTextures`, `gcParticleMaterial`.

At the actual current gameplay pre-render boundary, apply scene fog to the SAME
camera owner and rebuild236DD0, then use that current camera for both world50DX
and particles. After the actual player native selected-light publication,
`build(view, viewport, sourceTrackFog, gcLightOwner, gcDrawLights[0])` must return
`OriginalGcParticleRenderStatus50KG::complete`; submit `frame.gcParticles=draw()`.
Zero quads are a valid current frame. No submitted/visible-pixel receipt is
fabricated: `gcParticleDraws`, `gcParticleAlphaDraws` and `gcParticleQuads` count
checked GL commands, and only the surrounding successful `batchAccepted` and
GPU-completion boundary make them a published/completed submission.

Renderer retirement precedes `materialOwner.closeGpu`; keep source/material/
context owners alive for a failed GPU cleanup retry. Root session/50DU joins and
build-manifest updates are supplied by the integrating owner, not this patch.

## Source references

`docs/original-gc-local-fog-source-review.md` records authenticated TOB/SCB/EMT,
PRT/PSM/PST and source addresses. Relevant executable ranges:
160BB0..160C5C(manager glow255),16105C..16115C(corner normals),162230..162844
(run headers/brightness/UV),162870..162D1C(depth/lighting),149310..149610
(directional/global ambient),2361C0/236DD0(screen continuation),ParticleCode
3213C8..3215E8(projection/alpha/fog). Source members:

- demoprt.psm5022 bytes:ed7c0a0799dc749f0c406503f0c4825b481038bb790cae2f50fc12897bf9ebe1
- demoprt.pst132124 bytes:3c454a0bd95c12cd75168ec3da3c4ced43e60e5a1d1c4536ec65583d0350a165
- GC.RES:69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9
