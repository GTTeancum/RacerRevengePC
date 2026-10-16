# Gasgano round-four generator review

## Verdict

No blocking source defect found in the incremental round-four patch. By textual inspection, it closes the round-three Gasgano-only catalog omission hazard for the current seven-member baseline, without changing the default input set or inventing decoded outputs.

Precision on “preservation”: this is complete regeneration. It preserves all seven baseline source identities and their ordering, and constructs truthful provenance for each new output. It does not preserve the historical seven output hashes or codec/generator records byte-for-byte. If retaining those exact historical records is required, this is not an append/merge solution. The patch and accompanying documentation explicitly disclose this distinction.

## Reviewed material and limits

Read `local-codex-round4/packet/handoff/local-codex-racer/ROUND4.patch` and its report, plus the current generator and complete additional generated catalog in `/workspace/shared/rr-recovery-git`. Current generator text includes the round-three pins; the current generated catalog still declares seven entries. This review follows the parent's report that round three was integrated as `1630d00`; no separate Git check was run.

Only source/report reads and this review-file write were performed. No patch application, tests, builds, syntax checks, imports, parser/decoder execution, conversion, gameplay, SLUS execution, or other independent execution checks were performed. No production file was edited.

## Source selection and baseline coverage

- `gasgano_additive` becomes true only for an explicit selection containing `07c_2`.
- That mode replaces the selected subset with `SOURCES`, which retains the original eight input tuples and appends Gasgano. Argument order or repeated selections cannot omit baseline members or add duplicate output rows through this flow.
- The archive and every effective input size/hash are authenticated before helper import or FFmpeg invocation. Missing baseline files cannot silently downgrade the operation to a Gasgano-only run.
- During frame processing, every required member except `12c_4` must be kind 2; `12c_4` must be kind 1. The existing skip branch cannot silently discard a required kind-2 member.
- Before any output write, the ordered emitted members must be exactly `04c_1`, `04c_2`, `10c_1`, `10c_2`, `12c_1`, `15c_1`, `15c_2`, then `07c_2`, each under `ui\\textures\\` with `.pst` suffix. This baseline matches the current generated catalog's seven rows and order. Their whole-source sizes and hashes remain the original pinned tuples.
- The skipped-name list must contain only `12c_4`. The eight-row capacity cap remains intact.

Source membership is therefore explicitly protected, rather than inferred merely from the final count. Existing exact five-mip Gasgano input checks remain untouched.

## Defaults and compatibility

Omitting `--source` still chooses exactly `DEFAULT_SOURCES`, without Gasgano. Explicit non-Gasgano selections retain the previous selected-only behavior. No CLI option, native API, C++ storage layout, source pin, decoder command, generated C++ row structure, or manifest schema identifier is changed.

The added manifest fields are conditional on Gasgano mode: policy, requested selections, effective inputs, and required baseline members. The existing output/provenance fields remain. These changes are structurally compatible with the existing generator's local dictionaries and compiled catalog format by inspection. Unknown downstream consumers of additional manifest fields were not exercised.

## Provenance and publication

All rows pass through the existing real decoding path in a later authorized run. RRTEX and RGBA digests are derived from that run's actual bytes; YUV digests come from the unchanged native helper; codec metadata records the installed binary/version and actual current generator digest. The patch does not copy old pixel hashes and falsely label them with a new generator identity. It adds no generated asset or catalog row now.

Consequently, a future regenerated baseline may differ from historical pixel outputs if the installed codec differs. The parent must review the future run's actual output/provenance and publish matching regenerated RRTEX files with its generated catalog. Installing only a new catalog over unmatched old artifacts is not established as safe. Historical provenance remains in the existing artifacts/history; the new generated rows describe a new conversion event.

All parsing, conversion, source-list checks, and converter-stability checks precede writes. The generated catalog is written last in a fresh output root using exclusive creation. A filesystem failure can still leave incomplete staging files, as the documentation correctly notes. No atomic install or complete staging rollback is provided; publication remains parent-owned.

## Remaining boundaries

- The baseline is fixed to these seven members and does not discover future catalog additions. Reconcile the installed catalog before later execution/publication if membership changes.
- Non-Gasgano selected-only runs can still emit partial catalogs by design; safe deployment of those remains the parent's responsibility.
- Selecting Gasgano now requires all nine pinned source files and regenerates eight kind-2 sources. Any later execution authorization should account for that expanded work.
- This patch grants no execution authority and proves neither conversion success nor runtime/cache/rendering/full-racer closure. Gasgano remains unadmitted.

Conclusion: acceptable source-only hardening of the current seven-plus-one regeneration path, with the historical-record preservation distinction above retained in any summary.
