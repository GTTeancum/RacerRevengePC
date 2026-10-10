# New original UI archive reconstruction

This is **new source**, authored 2026-10-10. It is not recovered lost source or recovered original game archive bytes. No repository source was modified. The files under `files/` are an isolated integration candidate.

## Evidence and coverage

Source ZIP: `RacerRevenge-Checkpoint48F-NativeWorldCacheAndRendering-Evidence.zip`

Verified ZIP SHA-256: `60c041bd1acd58f7e19cea50b0c2f6d3ae1083f18d82bcbb591b3182d115aa0c`

Manifest ZIP entry: `evidence/importFull48F-20261003/native-cache/generations/3cf500075d2f31e3930d5e74896a3068ca596fba3f47475ba9c4643bcbf60b39.json`

Verified manifest SHA-256: `3cf500075d2f31e3930d5e74896a3068ca596fba3f47475ba9c4643bcbf60b39`

| ID | Ordinal | Container | Members |
|---|---:|---|---:|
| appStart | 0 | APPSTART.RES | 6 |
| localization | 1 | LOC.RES | 10 |
| overlays | 2 | UI/TEXTURES/OVERLAYS.RES | 112 |
| fonts | 3 | UI/UIFONTS.RES | 24 |
| mainMenu | 4 | UI/UIMAIN/UIMAIN.RES | 34 |
| backgrounds | 5 | UI/UIBKG.RES | 23 |
| controllerPopup | 6 | UI/CONTPOP.RES | 10 |
| course | 7 | UI/UICOURSE/UICOURSE.RES | 128 |
| rider | 8 | UI/UIRIDER/UIRIDER.RES | 552 |
| options | 9 | UI/UIOPTION/UIOPTION.RES | 59 |
| Total | | | 958 |

The historical lost catalog reportedly contained 978 members. This manifest supports 958 members in the requested ten archives. The 20-member discrepancy is unresolved; nothing was padded, guessed, or synthesized to match the historical number.

All requested containers exist once in this manifest. Every target container's declared entry count equals the number of selected member records. Each selected member supplies a nonempty ASCII name, logical offset, source byte count, and lowercase SHA-256 digest, including unsupported CEL/DAT records. Exact names and ASCII-case-folded slash-normalized names contain no within-archive duplicates. Manifest member order was retained unchanged. Without raw RES files, correspondence of manifest record order to the original archive table has not been independently proved; the runtime candidate requires exact table order and fails closed if it differs.

`catalog-provenance.json` includes the complete selected metadata, explicit IDs/ordinals, and evidence identities. The C++ catalog copies container `name/bytes/sha256`, and member `member/logical_offset/source_bytes/source_sha256`. Converted texture cache digests are never substituted for original member digests.

## Implementation intent

- Existing public header semantics retained, original seven enum ordinals explicitly preserved, verified course/rider/options appended at 7/8/9.
- Immutable catalog arrays and load-once owner. No mutation of a published owner; repeated load reports alreadyLoaded. Borrowed member views are intended to remain valid through owner lifetime.
- Before native RES decoding, check exact compressed archive size and standard SHA-256.
- Invoke existing `decodeResource48C`, retaining its fixed limits: 64 MiB compressed, 128 MiB decoded, 10,000 entries, 1,024-byte names. This caller further reduces compressed-byte and entry limits to the selected identity.
- Verify exact member table count, case-sensitive authored names, order, offsets, byte sizes, bounds, and SHA-256 for every original member before the sole publication point.
- Empty, retryable owner after failure/cancellation/allocation failure. Existing published owner remains intact on repeated calls.
- Lookup folds ASCII case and treats slash/backslash as equivalent, without path collapsing, basename/prefix fallback, cross-archive fallback, or allocation.
- SHA-256 uses fixed stack workspace and checks cancellation during long input hashing. No archive name is ever used as a filesystem path. No file I/O, rendering, format conversion, cache writer, or substitute art is included.
- SHA-256 constants also appear in retained `audio_assets50hc.cpp`; implementation here is newly written and processes fixed-size blocks without full input copies.

## Verification limits and required integration work

Only ZIP/JSON data reads, metadata consistency examination, file writing, and hash calculations were performed. **No compiler, syntax check, build, unit test, game importer, game execution, PC execution, or Git action was run.** This candidate has not been executed.

No raw target RES archive bytes were recovered here. Therefore no positive real-archive load, original CEL/DAT content interpretation, successful UI composition, or visual-equivalence claim is made. The identity metadata is evidence-backed, not a substitute for archive bytes. Runtime activation must remain gated on actual archive bytes passing authentication.

Root integration/testing still needs: source registration; standard SHA-256 known-answer tests; negative identity/table/member tests; cancellation and retry tests; invalid enum/ordinal/name tests; immutable-view/reload tests; and eventually all ten original archive positive loads when exact source bytes are available. Do not weaken table checks to make unproved input pass.
