# Independent source review: inactive Rider material lifetime ledger

Reviewed 2026-10-11 against baseline `0ee94e6707c5088bd99c01f46cb3e984c50dac0c` at `/workspace/shared/rr-recovery-git`. The checkout reported that exact HEAD and a clean worktree during review.

## Disposition

**No blocking source defect found in this inactive slice.** The two new files and single native CMake registration can proceed as an inactive primitive, subject to the exact identities below. This is not approval to wire callers, enable movement/Confirm, claim original numeric reference-count parity, or claim build/runtime success.

The review used source text, ordinary text searches/diffs/hashes, the retained authenticated original material-table text, and pre-existing original instruction/record listings. No build, test, syntax check, project parser/decoder/importer, probe, native/guest/GL launch, sanitizer, emulation, or new instruction decoder ran. No implementation or active-checkout file was edited.

## Exact final reviewed identities

Paths are relative to this stage root. SHA-256:

- `staged/src/game/original_ui_rider_material_lifetime50kg.h`: `5b205bc3528d7bdef1666bbf5c72b043526c78ed2e83c95a275e137c9af8bf4e`
- `staged/src/game/original_ui_rider_material_lifetime50kg.cpp`: `0d428ddf7a47e9d6e2da7ffa8f2e4d0d0ca10c0504472194cfa4e32e418ff035`
- `staged/platform/linux/native/CMakeLists.txt`: `76caeaf42a67f311f6481f78466d410598cc9de8750bec610430528b4030263d`
- `REPORT.md`: `9540fbcaed708de2bb59f633889b5dfbaca6d63b793a985148e41c0c1086f25e`
- `preimages/platform/linux/native/CMakeLists.txt`: `fd9d2eddb7131ea3998faa99ccd75ef4417966c0bc81392dfc8c9de0231baf0e`, identical to the baseline checkout file.

Any later implementation change needs renewed review. A later packaging-only report change does not change these three reviewed implementation identities. The final header's synchronous, single-threaded, nonreentrant contract and the final stage report's partial-population warning were reviewed before these identities were recorded.

## Authentication and immutable footprints

1. The table gate (`.cpp:28-34`) requires a loaded canonical APPSTART owner, an explicitly nonnull canonical member identity, nonnull bytes and exactly 70962 bytes. Baseline `OriginalUiArchive50KG::load` authenticates archive identity, exact member table, and every member hash before publishing its immutable storage. This is a genuine-byte trust chain, not a caller-supplied rank or pointer claim.
2. Row preparation (`.cpp:45-72`) uses only space/CR/LF delimiters, checks the complete 3235-token count, and stores the first matching token's identity/rank/offset. Its only duplicate exception preserves `pod_wndw` rank 2490 when the later match is rank 3221 at byte 70650. It never replaces the first key. The retained original table hashes to `a3b786f57a1a5f468e649a7f1b3d076a95c318a2fab2f622b754038d01dedd35`, matching the catalog. The earlier independent original-byte audit records first byte offset 57229 and confirms source lookup 298C80 scans increasing rows and returns the first match.
3. Every case-sensitive `member()` literal resolves in baseline `original_ui_archive_catalog50kg.inc`: `AppStart\\Mat_Stbl.txt` (line 19), `UI\\UIRider\\Widgets.Dat` (441), `ui\\geometry\\19.psg` (444), `ui\\geometry\\20.psg` (766), `UI\\Anim\\19c.mcf` (407), **`ui\\anim\\20c.mcf`** (767), `ui\\textures\\binder.psm` (454), and `ui\\textures\\binder.atm` (405). The table/pod/character/binder gates explicitly reject missing expected identities, so null-to-null equality cannot authenticate an asset.
4. Factories admit only genuine loaded 19/20 pod/character assets or the binder asset. The baseline asset classes privately retain immutable decoded scenes and authentic widget/model/MCF sources; there is no public mutable scene injection. Character PSG selection is derived by that authenticated MCF loader. `model()` is private, and factories construct a local candidate before replacing output.
5. Actual scene material slots determine rows and occurrence counts. Retained fixed records match pod19's six slots/five rows, character19's three slots/two rows, pod20's four slots/four rows, and character20's five slots/five rows. Repeated `19_2` and `19c_2` slots coalesce while retaining occurrence count two. The binder has one row in each helper-domain footprint. Slots do not stand in for all mesh/geometry/wrapper references.

## State-transition and failure-atomicity review

- Successful bind is the only storage allocation/publication and cannot repeat. Public preparation, bind, acquire, release and populate failure paths leave their output objects and ledger state unchanged; only the diagnostic changes. Partial footprint construction occurs only in an unpublished local candidate.
- Acquire planning (`.cpp:160-182`) checks the exact prior sequence, footprint shape/provenance, all positive occurrences and their bounded total, duplicate rows, row capacity, free owner slot, owner serial capacity, row-presence overflow, journal room and sequence room before mutation. The occurrence subtraction cannot underflow: each prior accepted occurrence leaves the running sum at or below the declared slot total. New row slots are distinct, bounded below 256, and never recycle an old identity.
- Commit (`.cpp:242-256`) contains only bounded fixed-array/scalar assignments. It rejects an already-issued output token. Each unique footprint row gains one owner-domain presence and one acquisition event; occurrence metadata remains separate. First acquisition and reacquisition after retirement leave epoch zero. An already-positive row retains its epoch.
- Release planning (`.cpp:184-192`) follows exact owner validation and budgets both all release events and every required live-epoch retirement before any decrement. Commit (`.cpp:258-273`) emits release then explicit retirement for each last live row, clears that epoch, and finally invalidates the owner slot. Pending positive-to-zero release emits no fabricated retirement. Releasing a copied old token again fails.
- Population (`.cpp:194-210,275-283`) first identifies only positive epoch-zero rows, checks all epoch and event/sequence capacity, then sorts the bounded plan by canonical table rank without allocation. Commit gives each pending row a fresh monotonic epoch in that order and appends one explicit population boundary, including when no new rows exist. Existing live rows are untouched.
- Capacity/counter failures cannot silently truncate an operation. Row/owner bounds are 256/128; events are bounded by 256 for acquire, 512 for release and 257 for populate. Sequence and epoch additions are guarded by subtraction from the 64-bit maximum. Owner serial never wraps. The commit paths contain no fallible allocation or callback.
- The `can*` methods share the corresponding operation planners. `canAcquire` deliberately has no output-token parameter; callers must additionally satisfy acquire's documented unissued-token precondition. These previews are not reservations or a rollback mechanism for already-performed source effects.

## Tokens, zero gaps and journal

- Owner validity requires this exact ledger address, in-range slot, nonzero serial, active slot and matching serial (`.cpp:229-236`). Slot reuse increments a nonwrapping serial, rejecting prior-generation copies and foreign-ledger tokens.
- Retirement proof is private ledger+row+old epoch+event sequence. Validation looks up the retained event and checks its retirement identity (`.cpp:299-302`). It continues to prove the old retirement after a newer epoch is populated, but fails after acknowledgement. Mutable public event fields are not substitutes for the token's verified row/epoch.
- The preserved invariant is `eventCount == sequence - acknowledged`, with `eventCount <= 4096`. Append uses the next tail slot and never overwrites an unacknowledged entry. Lookup accepts only the retained interval and derives a bounded ring offset. Prefix acknowledgement advances head and acknowledged together, rejects past/future requests, and leaves source sequence, rows and epochs unchanged. Acknowledging the existing prefix endpoint is harmless and idempotent.
- Last release followed by reacquisition/population therefore keeps the old retirement entry, pending epoch-zero acquisition, and newer populated epoch separately observable even if no draw occurred between them. Continuous overlapping ownership preserves the existing epoch.
- These are scoped pointer handles, not globally unique cross-lifetime IDs. The header explicitly requires the ledger to outlive **every** token copy and consumer, including released tokens, and forbids destruction/reconstruction at the same address while any old token survives. Copy is deleted; the user-declared destructor and deleted copy operations also suppress ordinary implicit move construction/assignment. There is no public reset. Actual integration must enforce this lifetime contract rather than rely on allocator address uniqueness.

## Registration and required integration gates

The CMake diff is exactly one source-list entry at line 81 in normal `rr_native_gameplay50dj`, alongside the existing Rider sources. Its dependencies are already in that target. It is not hidden in a test-only target or optional execution path. Source searches found no baseline runtime reference to the new ledger API; the stage contains no caller, session member, callback hook, collector rewrite, or readiness relaxation.

Before any caller is wired, all of these remain mandatory:

- Prove a closed owner graph and containment of each represented domain's explicit mesh tokens, shared geometry and wrapper references. Add authenticated survivor coverage or reject any static/shared/foreign/debug/second-player owner the bounded model does not cover. Cached CPU descriptors and GPU images do not establish live PSM ownership.
- Retain a distinct token in every actual graph/helper lifetime, including helper candidates that fail before publication. Record acquisition only for real references; record release only after the represented domain is truly destroyed. No other domain may interleave inside one coarsely represented operation.
- Serialize mutation and consumer access externally. Expected-sequence checks are chronology checks, not synchronization primitives. Preflight/reserve the whole needed source operation sequence and its unissued token storage before source mutation; per-operation error atomicity cannot undo source callbacks.
- Call populate only after actual successful referenced-missing PSM construction at 14D370. This all-pending-rows API cannot describe partial 14D370 construction failure; such a path needs precise separate accounting and poison/drain ownership before wiring, without replaying completed source work or pretending no PSM existed. Preserve parent+A4/14D440 grouping as a separate boundary. The retained original 14D370 listing confirms increasing table-row iteration, positive-reference filtering and missing-object filtering; 14D500 confirms last-reference destruction/nulling.
- Prove real CPU-borrow drain and renderer completion before owner destruction, then retire collector history by session/ledger+row+pass+epoch using validated retirement tokens. Acknowledge only after all dependent consumers accepted the exact events. Keep accepted-publication binding changes and unexplained-disappearance rejection intact.
- Enforce ledger/token/consumer destruction order and the no-address-reuse contract. Keep existing movement/Confirm rejection and all unrelated profile, orientation, audio/deferred/video, real 20-owner, completion and navigation prerequisites unchanged.

The stage report's distinction between immutable cold admission, projected owner-domain presence, actual PSM population, grouping, retirement and source publication is consistent with the reviewed code and the retained independent lifetime evidence.
