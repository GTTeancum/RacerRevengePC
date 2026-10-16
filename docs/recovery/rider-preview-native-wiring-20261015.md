# Native requested-preview wiring: source review only

Base: 5375e5a818d0987a06891c58c33f65071dbd5590.
Deliverable: native-rider-preview-wiring.patch, plus before/after copies of the three changed files. Central repository was read only and its status remained clean. The Application header retains CRLF; frontend sources retain LF.

## Changes

- Application owns a default-zero uint32_t requestedPreviewRiders. At initialization, a provided RR_ORIGINAL_RIDER_PREVIEWS must equal `3,7,9,23,24` exactly and RR_ORIGINAL_MAIN_MENU must already be enabled. Empty, whitespace-modified, reordered, subset and other values are rejected by the exact string comparison. Validation precedes the disabled-Main early return.
- The sole accepted value requests bits 3/7/9/23/24: Clegg, Gasgano, Mawhonic, Kraid and Tzidik. It does not enable the separate Gasgano test profile or write profile selection/unlock state.
- Every native frontend construction copies the retained Application mask into OriginalUiFrontendCold50KG. Storage retains the cold input; its shared Rider-build branch copies the entire mask into actual OriginalUiRiderSessionCold50KG before identifying Main versus Course predecessor. Thus Main-to-Rider and Course Back-to-Rider use the same resource request; manager/frontend rebuilds also retain it.
- Frontend prepare rejects bits outside the integrated original preview mask. Nonzero requests require genuine Rider, Backgrounds and PodTextures archives and a loaded IPUM cache. Bit 3 also invokes the existing exact authenticated Clegg 03c_1/03c_2 texture/provenance checks. The legacy boolean remains functional and additive through the existing SessionCold bind population logic.
- No catalog-count success shortcut is introduced. Existing Session bind validates the integrated mask, unions legacy preview semantics, prepares dynamic descriptors, calls backend.prepareColdAndSeal, then requires preparedRider for every requested bit before the session escapes. Missing/unsupported owners continue to fail through this actual resource path. Native startup still authenticates every compiled retained cache mip through existing loaders.

## Scope and unchanged behavior

With the option absent, both new fields are zero and the new conditional resource gate is skipped. The existing legacy Clegg check retains its old behavior. No source callback, selection algorithm, Confirm gate, profile initialization, AI, input, GPU budget, catalog, converter, or route behavior was edited. Preview preparation does not itself establish race eligibility or a successful Windows smoke.

No builds, tests, syntax checks, sanitizers, gameplay checks, converters, project execution or patch application were performed. Review consisted of reading source and the generated patch. The next authorized Windows Gasgano/Sullust smoke remains the runtime acceptance point.
