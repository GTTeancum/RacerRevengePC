# Inactive canonical Rider material lifetime ledger

Baseline:0ee94e6707c5088bd99c01f46cb3e984c50dac0c. Staged source only,2026-10-11. Root integrates.

## Scope and disposition

This stage adds one inactive native primitive, `OriginalUiRiderMaterialLifetime50KG`, and registers its translation unit in the normal native CMake source list. There is **no runtime caller, session member, callback hook, collector-history change, movement activation, renderer-completion change, or readiness gate relaxation**.

The ledger represents ordered ownership-domain presence and PSM construction/retirement. It deliberately does not invent the original numeric material reference count. Actual original mesh, geometry and wrapper references must be proven to remain within each represented runtime domain before any call site is connected.

No build, test, syntax check, project parser/decoder/importer, probe, GL/native/guest launch, sanitizer or emulation ran. Editing and review use source text and retained original-byte evidence only. Added calls that inspect existing authenticated asset objects are unexecuted staged source.

## Files

- New `src/game/original_ui_rider_material_lifetime50kg.h`
- New `src/game/original_ui_rider_material_lifetime50kg.cpp`
- One source-list addition in `platform/linux/native/CMakeLists.txt`

The new source files were absent at the baseline. The CMake preimage is preserved exactly. No generator or build-system command was executed.

## Canonical row keys

`OriginalUiRiderMaterialRow50KG::prepare` requires the actual loaded APPSTART archive and the canonical `AppStart\\Mat_Stbl.txt` member. The member size is70962 bytes. Tokenization retains the original269B20 delimiters: only space,CR,LF. It requires3235 tokens and returns the first matching row rank and byte offset. Case and slash folding match the existing native material-table consumers.

The only admitted repeated matching name is the authenticated pod_wndw exception: first rank2490, later rank3221 at byte70650. It never returns the later row. Other matching-name collisions reject. Keys are privately populated and contain canonical table identity, rank and byte offset; no caller-chosen numeric row constructor exists.

Keys contain no image index, texture ID, material-pass index or native shared_ptr count. One PSM lifetime is a row lifetime shared by all its passes. Actual shader/PSM provenance compatibility remains the later collector integration's responsibility.

The case-sensitive catalog helper uses exact catalog spellings and every footprint factory rejects a missing expected canonical identity before comparison. In particular,19c uses `UI\\Anim\\19c.mcf`, while20c uses `ui\\anim\\20c.mcf`. Both pod PSG names and binder material/ATM names are lowercase; the widget catalog name is `UI\\UIRider\\Widgets.Dat`.

## Authenticated footprints and honest multiplicities

Footprints can currently be constructed only from genuine loaded19/20 pod assets, genuine loaded19/20 character assets, or the genuine binder asset owner. Widget/model/MCF/material identities are checked against the canonical catalog. Their construction records immutable evidence only; it never calls ledger.acquire.

Model rows are derived from the actual loaded Scene48D material slots. Repeated slots collapse to the same canonical row, with the original slot occurrence count retained separately:

- Pod19:6 slots,5 unique rows.19_2 occurs twice;19_1,cable,pod_wndw,19_shdw occur once each.
- Character19:3 slots,2 unique rows.19c_2 occurs twice;19c_1 occurs once.
- Pod20:4 slots and4 unique rows:20_1,20_2,cable,20_shdw.
- Character20:5 slots and5 unique rows:20c_1,20c_2,20c_4,20c_3,20c_5.
- Binder helper:one canonical binder row per actual helper ownership domain. Its cold PSM/ATM footprint may be shared by multiple real helpers, but each actual helper requires its own live owner token.

Original evidence: mesh acquisition26E7D8 and cached/cloned-mesh acquisition26EB90 increment each material-token slot; mesh destruction26E4C0..26E4E8 releases each token. Geometry wrappers separately release at299AF8 through14D500. Binder23BEB0→150450 creates its material wrapper, and23BCF0→1503B0→299AC0 releases it. See `../independent-lifetime-review/REPORT.md` and its raw-word evidence. The original fixed-record material lists are in `../evidence/fixed-records.json`.

**Slot occurrence count is not claimed to equal total original PSM references.** `ownerPresence` counts complete proven-contained actual runtime owner domains. One graph token contributes one positive presence to each distinct row even when that graph has repeated slots and several internal wrappers. That projection is valid only if every represented internal reference is acquired/released inside the same actual graph/geometry domain and no unmodeled survivor can retain it. Shared geometry escaping the domain invalidates that assumption and must be modeled separately or rejected.

The primitive currently provides no arbitrary static/foreign/debug/second-player footprint factory. Future wiring must prove the bounded rows have no unmodeled live owners, or add separately authenticated owner coverage first. Matching images or cold cache pointers do not establish such coverage.

## Ordered operations and fresh epochs

All mutation methods require the exact current source-event sequence. Calls must be synchronous, single-threaded and nonreentrant; sequence checking is logical staleness protection, not synchronization for concurrent access. No reset, rewind or sequence override API exists.

1. `acquire(footprint, expectedSequence, unissuedToken, receipt)` mints one unique active owner token and records one ownerAcquired event for each distinct row. It increments projected owner presence, not original numeric references. A previously unreferenced row remains epoch0: acquisition does not construct a PSM.
2. `populate(expectedSequence, receipt)` is for the actual successful14D370 population boundary. It gives each positively referenced epoch0 row a fresh nonzero epoch, in ascending authenticated material-table rank, then appends a populationBoundary event. Already live rows retain their epoch. Calling it with no new rows still records that explicit boundary; it does not claim14D440 or a draw occurred.
3. `release(token, expectedSequence, receipt)` consumes exactly that active owner generation. It records ownerReleased for each distinct footprint row. A positive-to-zero live row additionally emits a rowRetired event with a private retirement token and clears its live epoch. A pending row that never reached actual PSM population returns to zero without fabricating a live-epoch retirement.

Within one operation, owner-row events use the footprint's first-occurrence order. This is bookkeeping order for a complete domain event, not a claim about the order of every original internal mesh/wrapper destructor call. Future callers must forbid interleaving another ownership domain inside a coarsely represented operation. Population order is explicitly the original table-row order, independent of footprint/acquisition order.

A zero gap remains visible even if reacquisition and population occur before the next draw. The old retirement event remains in the ordered journal, new acquisition has epoch0, and the subsequent actual population receives a strictly newer epoch. An owner that overlaps continuously keeps the row positive, preserving its existing epoch.

Epoch serials are unique within this ledger lifetime. Future history keys must also remain scoped to the actual ledger/session owner; do not compare epoch1 of unrelated destroyed/recreated sessions.

## Explicit owner and retirement tokens

Owner tokens are privately minted and retain ledger identity, active owner-slot index and monotonic owner serial. Default, foreign, released, or reused-slot old tokens reject. Acquire refuses to overwrite any issued token. A future runtime owner should retain its one token directly until verified release; a cold asset object must never carry a live source token merely because its footprint was prepared.

Retirement tokens carry the ledger identity, exact row, old live epoch and event sequence. `retirementCurrent` validates them against a retained actual rowRetired journal entry. They remain valid for retiring that old epoch even if the same row already has a newer epoch. Default/foreign/acknowledged tokens reject. Consumers must use the token's verified row+epoch, not untrusted mutable fields copied into a public event struct.

These are **scoped native handles**. The ledger must outlive every token copy and consumer, including released tokens and copied journal events. It cannot be copied, moved or reset. Destroying it and constructing another ledger at the same address while any old token survives is forbidden; pointer identity is not a cross-lifetime globally unique ID. This contract must be enforced by actual session/consumer ownership before wiring.

## Capacity, overflow and failure atomicity

Fixed bounds are256 distinct rows,128 simultaneous owner domains, and4096 unacknowledged journal events. These are fail-closed native storage limits, not inferred original population counts. Seen rows retain their identity after retirement; active owner slots may be reused only with new serials. No mutation allocates after bind.

- `canAcquire`, `canRelease`, `canPopulate` are nonmutating previews of the same identity/sequence/capacity/overflow checks used by their respective mutation.
- Acquire verifies all unique rows, table provenance, slot multiplicity totals, row capacity, owner capacity, owner serial capacity, journal capacity and sequence capacity before changing storage.
- Release validates the exact active token, every row presence and the combined release-plus-retirement event budget before changing storage.
- Population counts every new epoch, checks epoch/sequence/journal capacity, and prepares a bounded allocation-free table-order list before changing storage.
- Counters never wrap. Output tokens/receipts and ledger state remain unchanged on failure; only the diagnostic can change. Footprint/key preparation similarly replaces its output only on success.
- Mutation commits use only fixed-array/scalar assignments. Repeated release, stale expected sequence, exhausted owner/row/journal space and counter overflow reject rather than dropping an event or guessing successful work.

Journal events are retained in a fixed ring with monotonically increasing sequence. `event(sequence)` returns only unacknowledged entries. `acknowledgeThrough` may consume only a retained prefix, never a future sequence; it changes journal storage only, not row state, epochs or source sequence. It must run only after all real dependent consumers accepted those lifecycle events. It is not a fabricated source draw acknowledgement or GPU completion signal.

A preview is not an atomic multi-callback navigation plan. Future integration must reserve/check the whole required operation sequence before source mutation and recheck the live sequence before each actual source boundary. This primitive's error atomicity does not roll back already-created or destroyed source objects.

## Mandatory call-site prerequisites before any wiring

1. Establish the actual closed one-player owner graph and prove whole-domain containment of mesh material tokens, geometry references and surface-wrapper tokens. Repeated-slot metadata alone is insufficient. Prove no static/shared/foreign survivor was omitted.
2. Prepare immutable footprints in the private cold candidate only; that contributes zero source presence. The existing bank seal and GPU residency remain independent.
3. Store one owner token in each actual graph/helper lifetime. Acquire at the real reference-owning creation boundary, not when descriptors are warmed or images uploaded. Cover helper candidates that may fail before attachment publication.
4. Invoke populate only after the actual referenced missing PSMs have successfully been constructed at14D370. A backend registration Boolean is not proof. Partial14D370 failure is not represented by this all-pending-rows population API: any such path needs separately precise partial-construction accounting and poison/drain ownership before wiring; do not pretend that no PSM existed or retry completed source work. Keep parent+A4/14D440 synchronization and collector grouping as separate actual events.
5. On failed attachment before population, release only genuinely acquired pending owners. After successful population followed by failed capture/IK, record release and actual retirement for those created lifetimes. Never replay a partial source transaction.
6. Drain CPU borrows and obtain actual renderer completion before destroying source owners. The ledger does not do this. Current backend.detach resets a lease and precedes modelDestroyedAfterDetach; neither is enough to place graph retirement correctly.
7. Release a graph only after its real graph/geometry/wrapper ownership is destroyed; release a binder only after its real helper is destroyed. Do not release merely because a model is hidden/deselected if ownership persists.
8. Integrate collector history by actual ledger/session+row+pass+epoch and consume explicit retirement tokens. Preserve unexplained-disappearance rejection, surviving static histories, and accepted-source-publication binding updates. A journal acknowledgement does not substitute for any of those operations.
9. Preserve the existing movement/Confirm rejection until all movement, source chronology, actual20 owners, profile, orientation, audio/deferred/video and completion dependencies are separately closed.

## Source-level scenarios, not executed checks

- Acquire19, actual populate, final19 release, acquire20, actual populate: old cable/binder epoch retires and the new epoch is fresh if no actual owner survived the zero gap.
- Acquire another actual overlapping owner before release: shared row presence stays positive and its epoch survives.
- Repeated19_2/19c_2 slots: one row epoch, multiplicity retained in each graph event, no duplicate PSM instance.
- Acquire then release before population: no source PSM epoch/retirement is fabricated.
- Release then reacquire before consumer catch-up: both the old retirement token and new epoch remain distinguishable.
- Journal full or64-bit counter exhausted: source-facing caller must stop before the corresponding source action; no silent event eviction/reset.
- Acknowledged retirement token: cannot be newly validated or replayed to retire current history.

## Artifacts and validation limits

`SOURCE_PREIMAGES.json` distinguishes the two new files from the existing CMake preimage. `STAGED_FILES.json` records exact staged hashes. `material-lifetime-ledger.patch` is the unified source patch. `INDEPENDENT-REVIEW.md` records the independent source review and exact reviewed identities. `DELIVERABLE-MANIFEST.json` and `SHA256SUMS` complete the package.

No runtime correctness, original numeric reference parity, build success, source integration, or readiness is claimed. Root must reconcile any newer CMake edits when applying this inactive slice.
