# Coherent framebuffer-fetch resource/failure review

Review time: 2026-10-11, approximately 09:39 UTC.

## Verdict and scope

**APPROVE within the source-only resource, failure, fallback, and diagnostic scope below. No blocking finding.** This is not approval of shader language support, framebuffer-fetch ordering/precision, runtime image equivalence, or measured performance; those require the independent shader-contract review and the root-owned authorized runtime check.

Reviewed the complete four-file diff and the surrounding creation, submission, cleanup, and caller receipt paths. The four preimages are byte-identical to the corresponding active files at HEAD `1b413008a52de584aff48d19f06b7e8de82dd6bc` when checked. The active tree was not edited. Only source reads, diff/hash commands, and this staged review write were performed. No tests, builds, syntax checks, project parsers, decoders/importers, scripts, probes, or native/guest launches were run.

## Findings

### Cold shader construction and failure ownership

- The fragment prefix/body split uses string literals and a two-pointer automatic array; it introduces no throwing string construction or allocation into `createOriginalUi() noexcept`. The existing shader callback signature accepts `const char* const*` and is documented synchronous/nonthrowing.
- Vertex compilation supplies one valid string. Fragment compilation supplies two valid strings, with the version directive first and newline-delimited extension/define directives before the shared body. The temporary pointer array is used only during the synchronous `shaderSource` call.
- Shader and program names are assigned into the retained storage before fallible checks. Existing `hasGlResources()`, `cleanup()`, and `deleteShaderName()` cover both partially compiled shaders and partially linked programs. No new GL object kind or unowned name was introduced.
- All ordinary deletion entry points are loaded before any name is allocated. A creation failure runs cleanup; acknowledged deletions clear names, unresolved deletions retain storage for explicit `close()` retry. A fresh create cannot overwrite retained unresolved ownership. The first error remains preserved by `fail()`.
- There is intentionally no automatic fallback after an advertised coherent shader fails compilation, linking, or required-uniform validation. Such a failure rejects cold creation and cleans up. This is fail-closed behavior, not a claim that an advertised driver will compile the candidate.

### Capability and entry-point order

- `initialize()` loads baseline functions, checks existing GL error state, and calls `checkContext()` before any UI shader is created. `checkContext()` verifies desktop GL >= 3.2 and texture support before source capability selection.
- Coherent selection is made solely by exact `strcmp` against `GL_EXT_shader_framebuffer_fetch` in the actual `glGetStringi(GL_EXTENSIONS, i)` list. A non-null resolver, a version number, a substring, or `GL_EXT_shader_framebuffer_fetch_non_coherent` alone cannot select it.
- Extension count is bounded, every queried name and GL error is checked, and the retained coherent flag is set only after required capabilities and optional clip-control loading succeed.
- Without coherent advertisement, GL >= 4.5 or advertised `GL_ARB_texture_barrier` remains required, at least two fragment texture units remain required, and `glTextureBarrier` is resolved before availability can succeed. Coherent selection does not require that pointer and needs only the sampled source-image unit.
- Clip-control selection remains independent: core 4.5 or its exact ARB extension enables/resolves it; the submission normalizes lower-left/-1..1 clip convention when present. No new early resource allocation or unchecked capability inference was introduced.

### Uniforms, texture state, and ordinary RGB

- The coherent preprocessor branch excludes `uiSourceTarget`; its location is neither queried/required nor uploaded. The fallback branch retains all three operations. All other existing required uniforms remain checked.
- A coherent source submission does not bind its attached destination texture to unit 1 and performs no barrier calls. Unit 0 remains the actual source-image sampling unit, including explicitly untextured spans.
- Fallback source submission preserves destination binding for feedback spans, unbinds it for opaque spans, performs one barrier and one retained triangle draw per feedback triangle, then unbinds unit 1 and returns the active unit to zero even following a draw error.
- Common restoration still restores the UI VAO's immutable RGB index buffer while that VAO is current, then unbinds the VAO and array buffer and checks the restoration. The renderer contract explicitly owns a baseline rather than saving/restoring arbitrary host GL state.
- Final `cleanup()` still unbinds texture/sampler unit 1 for every source-enabled renderer, including coherent instances. This unchanged defensive cleanup is not part of the coherent submission path and does not sample that unit. It does not call `textureBarrier`. No new requirement to preserve foreign texture-unit state was introduced.
- Ordinary RGB calls still select `uiSourceComposition = 0`, use the existing immutable quad index buffer, fixed-function blend path, presentation-alpha mask, and one draw per span. Their new coherent diagnostic is false, including when the cold renderer selected a coherent-capable shared program. The shader review separately owns whether the shared inout declaration preserves ordinary RGB semantics.

### Submission failures, history, and counters

- The coherent change consumes the existing exact packed per-span index ranges; it does not alter preflight validation, index construction/capacity, target generation, source owner borrowing, or source frame generation checks. Existing cold capacity is 17,408 quads and 2,048 spans, so the per-frame triangle, draw, and barrier counters remain within their uint32 bounds.
- `sourceWorkStarted` remains false through CPU/read-only admission and checked preparation. It becomes true immediately before the nonempty material-span loop. A later error leaves `originalUiAccepted` false and causes the unchanged owner receipt to poison pending history. Preparation failures without material-span work preserve the existing same-generation retry policy. Target clear/generation changes continue to retain the attempted owner as before.
- A source span adds successful draw/triangle/quad counters only after its GL error check. Coherent spans issue one draw only when their retained index count is nonzero. Fallback barrier diagnostics still count attempted barrier calls; consequently a rejected diagnostic can contain attempted barriers and successful earlier-span counters. They are not a success receipt by themselves.
- Presentation copy and baseline restoration are checked before private publication is accepted. A failed later default-window copy can retain an already accepted private publication, as documented; owner history acknowledges that private acceptance. The front end still rejects the overall submission and never treats it as a successful new host receipt.

### Diagnostic publication

- `VehicleWindowDiagnostic50DS::originalUiCoherentFramebufferFetch` defaults to false, is set on successful cold create, and on source submission reflects the selected source route. It is not itself an acceptance flag.
- Main-entry `render()` snapshots the route boolean and barrier count only after `coordinator.submit()` returns success. The unchanged coordinator requires both a successful renderer call and `originalUiAccepted` before that status.
- Rider state and capture output retain the existing accepted-receipt gate: Rider screen, matching GPU publication, and a nonzero source frame generation. If the gate is false, the new boolean is false and barrier count zero.
- Stage trace intentionally reports the attempted submission diagnostic together with `okay`. The new boolean is emitted as the unquoted JSON literal `true`/`false`; uint32 barrier/draw/triangle values use numeric insertion. The additions introduce no newline inside the record, and the record still has one closing newline. Capture uses the existing JSON object's boolean/numeric assignments and one `dump()` line.
- No stage sample is added to source clocks, accepted delta accounting, or the unchanged one-second stall gate. Additional trace output remains real overhead, rather than being subtracted or hidden.

## Limits for use

The source inspection does not establish actual extension advertisement on the target context, successful compilation/linking, driver behavior, rendered bytes, or latency. On a context without exact coherent advertisement, this candidate correctly retains the old per-triangle feedback cost and should not be described as an acceleration. An authorized run must distinguish coherent selection from acceptance and inspect both the route boolean and draw/barrier counters. The independent shader-contract review owns inout legality, output-location binding, primitive-order guarantees, conversion/rounding, and ordinary RGB shader semantics.

## Reviewed hashes (SHA-256)

### Candidate and preimages

- `files/platform/linux/native/vehicle_window_gl50ds.cpp`: `d4173271ac67d9fd14b8e8feacf6640d40bbeadb98dbdf51a35d8a96ec79663d`
- `files/platform/linux/native/vehicle_window_gl50ds.h`: `a7207d67995f6f2f4c473fcd5fc751266b54608890ce3b43b04ca12a8ac196e0`
- `files/tests/native/original_ui_main_entry50kg.h`: `e1577badad3b327b32692e1d0817db93cc314d20a4dd3d8abbb1e3f4ef3d62c2`
- `files/tests/native/terrain_interactive_drive50du.cpp`: `66429d6ed9953658d224d83d92bd73d304184f510683011118a50ea89336a532`
- `preimages/platform/linux/native/vehicle_window_gl50ds.cpp`: `c30fa4f3553bb66c187a3ae923936e9b2069d5ae44743e404ad9ec8b86ab6a9e`
- `preimages/platform/linux/native/vehicle_window_gl50ds.h`: `54127d4473cee04165348172da717c52d3cddea7d48baf0aac80fa499a4a5814`
- `preimages/tests/native/original_ui_main_entry50kg.h`: `9d7df94e541bd1f4f96e7d8fff2a368248ccc5ccf5096492e266e1a08c1b0000`
- `preimages/tests/native/terrain_interactive_drive50du.cpp`: `f109287df5d23b2cc53af428abf307a715414eb44a03babade0cf552377d5586`
- `coherent-fetch.patch`: `0aaf4586d3f1382019605528f6d08b428adb42985818d4f7cb5ca1f3605d9351`
- `SHA256SUMS.txt`: `87e98b8990e970dca9566b282629aa5ed6d3c0c80c6f9f5b7ee301f2259b829d`

### Unchanged supporting source at active HEAD

- `platform/linux/native/original_ui_rider_draw_contribution50kg.cpp`: `e2293e491fd891ff1e5f4f599f17325c8a6823b23f4555a5cf82a9e92181e549`
- `platform/linux/native/original_ui_frontend50kg.cpp`: `68fff5372851ab1d1f44943a17cadaa4fdf2595cdb17856a712f654115bba1f8`
- `platform/linux/native/vehicle_shader_gl50cz.h`: `0adab7eb2649fb146e23b8176974d35fb00b542881cfeb0ce758a104d1e0c41d`
- `platform/linux/native/original_ui_draw50kg.h`: `d4d05eb74e5724b7c1b9ee5c771a1a29a888d56c3ebe3877e6787a25ee7e6779`

Approval applies only to these candidate bytes. A change to them requires a relevant re-review; this report does not authorize any run.
