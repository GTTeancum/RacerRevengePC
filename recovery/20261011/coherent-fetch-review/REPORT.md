# Coherent framebuffer fetch source-only candidate

Prepared 2026-10-11 for root-owned integration. Active repository was not modified.

## Base and deliverable

- Base HEAD: `1b413008a52de584aff48d19f06b7e8de82dd6bc`.
- Renderer preimage: `c30fa4f3553bb66c187a3ae923936e9b2069d5ae44743e404ad9ec8b86ab6a9e`.
- `preimages/` contains the exact four copied active files; `files/` contains their proposed replacements.
- `coherent-fetch.patch` is a unified diff of `preimages/` against `files/` (one path component precedes the repository-relative path).
- `SHA256SUMS.txt` records all eight source/preimage hashes. Patch hash: `0aaf4586d3f1382019605528f6d08b428adb42985818d4f7cb5ca1f3605d9351`.
- Proposed renderer hash: `d4173271ac67d9fd14b8e8feacf6640d40bbeadb98dbdf51a35d8a96ec79663d`.

## Selection and shader

The existing cold source-UI extension enumeration compares the complete extension name to `GL_EXT_shader_framebuffer_fetch`. The similarly named noncoherent extension does not select the new route. Coherent support selects the fragment-shader prefix with GLSL 150, the required extension directive, and a private macro; the shared body declares `inout vec4 viewerOutput` and omits `uiSourceTarget`. Only the destination acquisition changes to `floor(viewerOutput*255.0+0.5)`. All source sampling, alpha tests, fog, ALPHA arithmetic, RGB clamping, and source-alpha replacement remain shared and unchanged.

The existing shader creation, compilation-status, link-status, output-location-0 binding, bounded logs, and resource ownership remain in place. A selected coherent shader compile/link failure is an ordinary creation failure with the existing cleanup path. There is no silent retry as a different shader. All common uniforms remain mandatory; destination sampler lookup, validation, and uniform write are conditional because that sampler is absent from the coherent variant.

The coherent route requires one fragment texture unit and does not resolve `glTextureBarrier`. Without the exact coherent extension, the prior GL4.5/ARB texture-barrier support and entry point remain required, with two texture units and the compiled texelFetch variant. Clip-control detection/loading is unchanged in substance.

## Draw and failure behavior

The existing source-only index stream, exact original triangle triples, equal-XY omissions, span ranges, buffer upload, and bounds remain unchanged. Each coherent nonempty source span submits its complete retained index range once, in existing span and primitive order. Empty ranges issue no draw. Coherent source submission neither binds the destination texture as a sampler nor calls any texture barrier. Fallback opaque spans remain batched; fallback feedback spans still issue one barrier and one real triangle per draw.

The existing span GL check precedes successful draw/triangle/quad counter commits. Work-start receipts, source-target generation, history poisoning after partial work, final GL checks, immutable RGB EBO restoration, private image publication, and separate opaque presentation remain unchanged. Ordinary RGB submission retains its original draw path and blend state. No additional GPU objects, dynamic frame allocation, geometry representation, or cold resource allocation were added. The new shader prefix uses static strings and a two-pointer local array.

## Observability

`VehicleWindowDiagnostic50DS::originalUiCoherentFramebufferFetch` is set after successful cold creation and reflects the selected route on a source submission. Ordinary RGB submission leaves it false. Existing draw, triangle, and barrier counters retain their meanings.

The optional `native_submit` stage trace adds the actual coherent-route boolean, checked draw/triangle counts, and barrier-call count, including on failure. Accepted Main receipts retain the boolean and barrier count before later calls can reuse the diagnostic. Rider state and capture receipts expose `source_coherent_framebuffer_fetch` and `source_texture_barriers`, gated by their existing accepted-source-frame predicates. No source clocks, exclusion intervals, or one-second gates changed.

## Normative basis and limits

Official [EXT_shader_framebuffer_fetch specification](https://registry.khronos.org/OpenGL/extensions/EXT/EXT_shader_framebuffer_fetch.txt): lines 59–65 provide API primitive-order coherence for the exact coherent name; 318–325 and 523–534 define incoming inout framebuffer values; 608–613 apply the ES3 language changes to desktop GL3+. The target remains the existing single-sample linear RGBA8/D24 target. Destination values do not feed derivatives or source texture coordinates. The independent [shader review](SHADER_REVIEW.md) additionally verifies the desktop normalized-value conversion and precision rules, and inspected Mesa's shared RGBA8 unpack helper. It found no blocker or need for an additional vendor/version gate in this documented scope.

The independent [resource/failure review](RESOURCE_REVIEW.md) approves the frozen four-file candidate within its source-only scope, with no blocking finding. Review hashes are `8a8c8529572a025a5fe0aa5b64f87ea6d8f709022cec5ba39b4f2ea51ba39846` (shader) and `60050b699f7c52e25893215346600ea55954bea9dbc92b5d867c7ea1bfc030ab` (resource/failure).

Verification was limited to source reads, ordinary diffs, and hashes. No build, syntax check, test, project parser, decoder, importer, script, GL capability probe, native launch, or guest launch was run. Actual runtime extension advertisement, shader acceptance, selected route, image correctness, and timing remain unobserved by this work. No performance, universal whole-frame bitwise equivalence, or runtime success is claimed. Rectangle, opaque-finite-guard, and coverage-prepass proposals are not included.
