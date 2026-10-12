# Cold asset-preparation source pin review

## Recommendation
Adopt `REPOSITORY-SOURCES.reviewed.sha256` for the existing cold preparation wrapper after inspecting the comment-only `src/assets/import_texture48e.h` diff. This refresh authorizes a specific source snapshot for cold asset preparation; it does not validate runtime behavior or permit gameplay execution.

## Exact baseline and candidate
- Reviewed current source: accepted commit `57f03494fac79bc21c946b823dd930432b814206`.
- Baseline: the 496 entries in that commit's `tools/assets/recovery/REPOSITORY-SOURCES.sha256`. Its README mentions historical snapshot `791dce663909f04e1c4e45fe580e6bfbcd67cc51`; that commit was unavailable locally, so this review does not claim a verified historical commit checkout.
- Each of the 36 mismatching pinned preimages was instead recovered from retained Git blobs by matching its exact pinned SHA256. Every proposed replacement was byte-compared with the corresponding file in accepted commit `57f03494`.
- Candidate changes exactly 36 entries and retains the other 460 entries unchanged, preserving order and scope.
- Candidate SHA256: `7535e02bafe733c393c5dac6550c5d8ebb49f0fbdf22584e1d4bfc4c06c63e02`.

## Cold execution impact
Static recursive traversal of quoted includes from both compiler recipes in `tools/assets/recovery/prepare_recovered_assets.py` identified 31 repository source/header files, with no unresolved quoted includes. The roots are `import_original_pods.cpp`, `extract_gc_members.cpp`, and the six asset translation units explicitly named by the wrapper.

Only one of the 36 changed files belongs to this closure: `src/assets/import_texture48e.h`. Its diff changes comments only, clarifying indexed raw alpha and the separate authenticated kind2 IPUM cache. It changes no declarations, storage, constants or decoder behavior.

The other 35 changes concern runtime/UI ownership, motion, collision, reset and menu APIs outside both cold-tool compilation closures. The preparation Python scripts and retained generated-index fixtures still match their original pins. Their runtime semantics are not independently approved by this review; the preparation wrapper neither compiles nor invokes them.

## Supporting evidence
- `PIN-REVIEW.json`: per-file scope, old/new SHA256, historical/current Git blob identities and cold-preparation relevance.
- `pin-mismatch-diffs.patch`: full 36-file comparison.
- `pinned-preimages/`: exact historical bytes matched to baseline pins.
- `cold-compile-include-closure.json`: compilation roots and 31-file quoted-include closure.

## Limits
This is a static source and byte-identity review. No compiler, preprocessor, syntax checker, importer, project script, test, sanitizer or gameplay was run. Installed toolchains, system headers/libraries and FFmpeg compatibility remain unverified. The include traversal is a source-level dependency inventory, not a compiler-generated dependency result. Future source changes require another pin review. The repository and its existing manifest were not modified by this task; root owns integration and any separately authorized preparation session.
