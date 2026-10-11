# Rider source-RGBA composition and authentic single-map specular admission

Source-only candidate, 2026-10-11. Seven native source replacements with exact preimages. Preimages captured from ddd4a3e and independently match the affected files at root baseline 9b211d4. Active checkout remains untouched. No build, compiler, syntax check, tests, project parser, gameplay, guest execution or emulation was run. Static original-data reads, byte-to-text listings, source editing and hashes only.

## Delivered scope

- Authentic Rider19 body pass2 uses SpecularMapCompressed and ALPHA=(0,2,1,1), so its coefficient is the current destination-alpha byte /128. The pod_wndw pass2 uses (0,2,2,1), FIX255, giving 255/128. Neither can safely use a clamped native fixed-function coefficient.
- `prepareOriginalUiRider19SpecularMaterial50KM` admits only canonical Rider 19_1,19_2,pod_wndw PSM pass2 and the exact published specmap1 original image. It preserves authored identities and checks the actual combined-bank source/index/image identity. `convertOriginalUiRider19SpecularMaterialSpan50KM` carries the distinct program and blend policy. Ordinary material/geometry admission remains separate; the old RGB renderer rejects these policies.
- `enableOriginalUiSourceRgba` is an explicit cold renderer opt-in. `submitOriginalUiSourceRgba` accepts only `OriginalUiSourceFrame50KM`, privately sealed by the concrete `rr::platform::OriginalUiRiderDrawContribution50KG`. No generic public Boolean grants complete-frame ownership.
- The source renderer owns a fixed 640x448 RGBA8 texture plus verified single-sample D24 storage. It clears raw source alpha128, uses reverse D24 depth, and preserves source alpha through the final source consumer. Independent presentation storage receives scaled/letterboxed RGB and opaque alpha. Readback/window presentation never expose or overwrite the private source alpha.
- The complete composer must establish real source registry/redirect/FIFO order, every enabled source pass and group action, actual context0/PSMCT32/FRAME mask0, fresh clear ownership, current combined bank, and closed target lifetime. A surface-local pass sort, partial model contribution, image-bank identity, or a source-only stat contribution cannot seal a complete frame. This package does not implement or certify that composer and does not activate Rider UI.

## Native arithmetic and ordered feedback

The source shader represents raw source bytes as byte/255 in RGBA8. Textured MODULATE computes/clamps/floors texture-byte times interpolated vertex-byte /128 independently for RGBA. Untextured spans use raw interpolated vertex bytes. The admitted alpha test discards values below byte1; fog changes RGB only with source F byte weights /256. Each surviving fragment replaces source alpha regardless of RGB blending.

RGB blending evaluates the admitted source equation in byte units: opaque replacement; `(Cs-Cd)*As/128+Cd`; `Cs*As/128+Cd`; `Cs*Ad/128+Cd`; or `Cs*255/128+Cd`, with arithmetic flooring and final color clamp. This preserves coefficients above one. It is a new native implementation, not a GS device model or historical pixel-parity claim. Native texture filtering, edge coverage, interpolation and float precision remain native GL behavior. Original hardware sampling/rounding/coverage equivalence has not been tested or claimed, including the existing host kind2 codec's acknowledged pixel-parity limitation.

Every actual triangle gets its own indexed draw, with a texture barrier before it. The shader reads only its own destination pixel with `texelFetch(..., ivec2(gl_FragCoord.xy),0)`. No multisampling, geometry shader, per-pixel logic operation, fixed-function blend or self-overlapping multidraw is used. Span, quad and triangle order are preserved without sorting or batching. A triangle with duplicate XY corners is skipped as zero-area; this handles the existing `[a,b,c,c]` mesh-triangle transport without an extra second draw.

The [Khronos ARB_texture_barrier specification](https://registry.khronos.org/OpenGL/extensions/ARB/ARB_texture_barrier.txt) permits same-pixel single read/write feedback and requires a barrier before a subsequent draw consumes previous writes. The candidate applies the conservative one-triangle draw restriction and a barrier before every draw, including the first after clear. Capability is established from GL4.5 or the advertised extension, never a nonnull resolver pointer alone. Missing support fails closed.

The [Khronos ARB_clip_control specification](https://registry.khronos.org/OpenGL/extensions/ARB/ARB_clip_control.txt) identifies conventional lower-left origin and negative-one-to-one depth. When that feature exists, source submission explicitly restores those settings before using gl_FragCoord and the recovered source depth mapping; older contexts retain the unextended convention.

## Lifecycle and diagnostics

`retireOriginalUiSourceOwner(const OriginalUiRiderDrawContribution50KG&, diagnostic)` explicitly drains preceding GL work on the exact live context/thread, including failed or unpublished attempts. It requires an idle renderer and rejects a different retained owner. Only successful glFinish/error handling releases the owner pointer and generation; on failure the concrete owner stays alive for retry. Call this at real Rider destination destruction before destroying its borrowed resources. Reconstructed destinations may then restart their own increasing nonzero frame-generation sequence; they need not retain an address for the entire application session. A different live owner cannot submit over a retained owner without retirement.

Each successful private publication records its owner and frame generation even if the subsequent window copy fails. Replayed/older generations are rejected. A separate monotonically increasing renderer target generation records actual fresh clear attempts. Ordinary UI submissions and image-bank replacement do not silently retire the source owner. Source alpha is untouched until the next fresh source clear or resource destruction. Submit does not insert glFinish; explicit retirement is a destination-lifetime operation.

`originalUiDraws` counts checked draw commands; `originalUiSourceTriangles` counts checked actual source triangle draws; `originalUiTextureBarriers` counts issued barriers, including a barrier before a failed draw. `originalUiQuads` counts completely submitted span quads. Source frame/target generations are reported separately. These are command receipts, not visible-fragment counts or GPU timing. The deliberately conservative per-triangle approach needs root-owned performance review before claiming acceptable rendering.

## Exact one-map kind2 TEXA ordering closure

Original executable SHA256: `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. Original loaded-segment mapping: file offset = VA - 0x100000 +0x80. Exact region bounds, bytes and SHA256 are in `evidence/source-regions.json`; static words are retained alongside it.

1. `24E994..24E99C` dispatches PST kind2 to `24EA40`; `24EA40..24EA60` sets texture+F8 bit0.
2. The real material draw calls `249610` at `24B2B0` after resolving the texture and before queuing the material state packet at `24B388..24B3A0` through `244B90`.
3. `2497FC` selects the one-map case. `249818..24983C` reads F8 bit0 and writes packet+0x70 with `(255<<32)|0x80FF`, i.e. TA0=255, AEM=1, TA1=255.
4. Thus the actual one-map Rider19c_1 path agrees with the typed decoded-cache policy: RGB5 high-bit expansion, alpha255 for nonblack and zero for black. Authored DefaultMT values are still preserved in the source material; the original late texture binding overrides the emitted packet before draw.

This closes only the specific ordering question for this one-map kind2 path. It does not prove other map-count paths, source historical resident F4, codec pixel parity, or arbitrary format16 material admission. The existing narrow opaque/ATE-off Rider19c_1 gate remains unchanged, ordinary kind2 admission remains closed, and the explicit native highest-resolution/base-only publication policy remains in force with all original CPU mips retained.

## Related authenticated source owners

- Source specular geometry and root/model normal basis: adjacent `rider-specular-material` freeze, integrated 3f85f56. This candidate consumes that geometry rather than substituting Diffuse or a generic highlight.
- Indexed body images: adjacent `rider-indexed-body-material`, integrated 4efd089. Original UIPODTEX 19_1 and19_2 are five-mip images with varying alpha; no white/constant-alpha replacement is used.
- Original specmap1 PST: 66588 bytes, SHA256 `d928592fbf2b0b7bd47075992bdcef7c9be426a766994148dce98ea2360f3f84`; kind1, one256x256 mip, palette alpha255. Original 19_1/19_2 and pod_wndw authored pass bytes remain in the Rider reconstruction packages.
- Original source target setup/clear: `../../ui-alpha-reconstruction/SOURCE-EVIDENCE.md`, especially mode setup25BAB0..25BAD4, FRAME writes25BEC0..25C014 and alpha128 clears25C44C..25C468 /25C958..25C974. That historical audit correctly did not certify a full frame; this candidate still requires the concrete full composer.
- Actual registry ordering/comparator: `../../ui-main-registry-reconstruction/SOURCE-EVIDENCE.md` and `evidence/shader-equivalence.words.txt`, 249BA0..249D2C. The Rider owner must compose its own actual loaded registry, dependency redirects and FIFO. Main projected ranks are not Rider ranks.

Unsupported DATE, FBA, PABE, nonzero framebuffer masks, unknown alpha-fail behavior, retained race alpha, unhandled render-target/prologue/epilogue consumers, absent GPU mip requirements and unsupported geometry/fog branches remain closed. No pass omission or partial pod publication is authorized.

## Integration

Apply `candidate.patch` only to matching preimages, or copy the seven replacements after confirming exact hashes. `manifest.json` records each preimage/replacement and current matching baseline. `FROZEN_SHA256SUMS` freezes deliverable files, evidence, patch and manifest. Historical `stage_backend.py` and `stage_submit.py` are text-edit scratch recipes, not a complete regeneration script, are not integration inputs and must not be executed as checks.

Root owns all future compilation, validation, activation and capture. This package has never been compiled or executed. Its acceptance is source review only; complete Rider rendering and performance remain unverified.
