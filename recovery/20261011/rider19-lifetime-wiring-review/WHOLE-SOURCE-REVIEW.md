# Coordinated Rider19 dynamic-lifetime source review

Review date: 2026-10-11 UTC.

Baseline: `b11246118e9859f83bd75a86f5f453288be4e749` in `/workspace/shared/rr-recovery-git`.

Scope: the coordinated candidate overlay in `candidates/`, compared with `preimages/`, and the bounded wiring plan at `../lifetime-wiring-plan/REPORT.md`. This review covers the complete dynamic-lifetime path, including its unchanged renderer, visibility, material-binding, frontend, and character-lifecycle dependencies. It does not certify original instruction-level failure behavior or complete global/static ownership.

## Current conclusion

No remaining source-review blocker was identified in the reviewed bounded Rider19 initial-attachment and whole-manager-teardown path after the explicit renderer-identity correction described below. This conclusion applies only to the frozen snapshot identities recorded at the end of this report. Parent integration and any later execution/publication remain separate decisions.

This was source inspection only: source reads, textual differences, ordinary listings, and content hashing. No builds, tests, syntax checks, project parsers/importers/decoders, sanitizers, native/GL/gameplay/guest execution, emulation, dependency changes, or Git publication were performed.

## Blocker found and corrected during review

The first snapshot tracked the collector and attempted generation but did not explicitly retain the `VehicleWindowGl50DS` that actually called the collector submission entry point. The existing renderer retirement method permits an idle renderer with no current source owner, so its successful finish alone was not an explicit proof about the submitting renderer.

The corrected coordinated API passes `*this` from the sole real renderer submission call to `beginSourceSubmission`. The collector records the actual renderer and rejects identity changes. Session teardown rejects a supplied renderer unless it matches that retained identity before invoking real retirement. The collector's attempted generation and the current ledger frontier are captured before the synchronous retirement, then checked against the same backend consumer, submitted watermark, and source sequence before ownership can be destroyed. The actual renderer performs `gl.finish` and checks completion. No new GL behavior or fabricated readiness receipt was added.

## Cross-file acceptance findings

### Stable ownership and bounded closure

- Backend storage declares the ledger before entries, live row instances, visibility, and helper holders. Session declares backend before dynamic owner and collector. Explicit teardown runs while all remain alive.
- Canonical static closure is prepared from the actual retained Rider draw material records, stat resources/portraits, predecessor Main material records, and Main particle record. The canonical row factory uses the authenticated APPSTART table and preserves first-match `pod_wndw` identity. Dynamic cold-row admission rejects every static intersection.
- Static inventories mint neither owner tokens nor row epochs. Static collector registrations remain a separate category with their existing continuous-history treatment. This does not claim cross-manager static lifetime reconstruction.
- Public session bind remains `selected19`. The private optional 19+20 cold path merely prepares immutable descriptors and bank policy. Graph footprint, actual scene adoption, attach, and dynamic preflight admit only widgets 27 and 53 for live graphs. No cold20 row becomes live merely because its image or descriptor exists.
- The enabled batch is still one Rider19 pod graph, up to two genuine named binder helpers, and one character graph, each at most once. The fixed budget is conservative for that closed initial batch. It is not a reservation solution for future movement or arbitrary callback batches.

### Actual graph/helper acquisition and cleanup

- Pod and character adopt their domain after a complete native graph build, before later binding/sample/attachment work. Build failure before adoption destroys partial value state without asserting a complete domain.
- Character initial-load failure after adoption explicitly destroys/releases the local candidate. If cleanup itself fails, storage and its exact phase/token are retained. Reload also routes post-build failure through explicit destruction rather than relying on the registered flag.
- Domain cleanup checks release entitlement/capacity before invalidation, invalidates graph/IK/playback state before releasing the domain, and clears its token only after successful ledger release. `owns`, rather than `issued`, determines ledger ownership.
- Each successfully initialized binder candidate acquires its own wrapper domain before the next helper or membership operation can fail. Local failure cleanup walks candidates in the retained reverse pair order. A failed candidate cleanup moves the surviving holder into the backend entry for explicit retry rather than silently dropping it.
- Detach reports failure and checks completion entitlement before membership/helper destruction. Dynamic cleanup retains a completed detach phase so a graph-release retry does not repeat membership/helper effects. An unregistered but retained graph domain still counts as cleanup work.
- Visibility-only operations do not release domains. Helper release precedes graph release in the actual bounded cleanup flow. The implementation does not claim that this proves all original internal destructor traversal order.

### Truthful row construction, partial failure, and capacity

- Acquisition reserves worst-case owner release plus row-retirement journal and sequence capacity. Population cannot consume that reserve. The initial whole-batch bound and fixed live-handle capacity are checked during cold sealing, before graph/helper construction.
- Population fixes sorted pending canonical rows, forbids acquire/release interleaving, and reserves separate population and successful grouping boundaries.
- The backend constructs fresh mutable per-pass binding state for one complete row before committing that row's epoch. Immutable descriptor caches do not stand in for live state. A pass-construction failure destroys its incomplete candidate and cannot stamp that row.
- Earlier complete rows are inserted and journaled immediately. A later failure preserves their epochs and events, aborts the remaining scan without a successful scan/group event, poisons new acquisition, and leaves reserved cleanup releases available.
- No allocating operation remains between a row commit and its insertion into pre-reserved live-instance storage: the insertion moves a shared pointer into existing capacity. Thus the implementation does not create a journaled epoch and then lose its native instance on vector growth.
- Population completion, actual synchronous collector grouping, recorded group rebuild, coarse generation increment, and final consumer acceptance are distinct ordered steps. Two initial attaches produce two genuine synchronization/grouping boundaries before character post-attachment capture/IK completes.

### Collector identity, transactional consumption, and accepted-state commit

- Every dynamic registration/history identity includes its ledger, canonical row, pass ordinal, and nonzero epoch. Source/provenance equivalence remains a compatibility check. Static registrations have no fabricated dynamic epoch.
- Cold aliases carry policy only. Live canonical lookup promotes aliases to the same row instance, including compatible aliases whose own model remains cold. This grants no cold-model geometry permission.
- Lifecycle processing stages history, bindings, observations, and representatives. Failed validation/allocation leaves the collector cursor and accepted projection unchanged. The backend retains the journal until both consumers have accepted the exact prefix.
- Partial population followed immediately by cleanup is representable: the consumer can observe the committed row, then its authenticated retirement, even if no successful grouping or frame ever occurred. Pending rows that never obtained epochs simply return to zero through owner-release events.
- Retirement requires a still-current ledger token for the exact old row/epoch, removes only matching dynamic registrations, and preserves static history. When retirement changes identity without a real rebuild, representative/leaf indices are invalidated rather than reused after compaction.
- Parent synchronization groups synchronously at the actual population frontier, then verifies the next `groupRebuilt` event. Rendering requires matching ledger cursor, registry generation, valid groups, and no awaited grouping record.
- Frame composition proposes effective binding history and per-live-instance state copies. Only accepted source publication swaps them into the exact retained histories/instances. CPU drain, lifecycle acknowledgment, and rejected source work never certify an accepted material bind. Interrupted/partly executed source submission poisons binding history.

### Completion, explicit teardown, and retained failures

- Actual renderer submission records the conservative attempted generation before source work, including attempts later rejected before any drawing. A nonzero watermark therefore requires retirement; a zero watermark derives from the real submission entry point rather than a bank/frame/update heuristic.
- Teardown first closes new session work, then retires the exact renderer if any submission was attempted, drains CPU borrows, verifies consumer/generation/frontier, explicitly destroys dynamic owners, consumes remaining lifecycle events, and finalizes native rows/journal before session reset.
- A failed real retirement returns before helper or graph release. A failed later cleanup keeps the exact remaining owners or unacknowledged event prefix accessible for an explicit close retry. Successful source construction or selection is never replayed as recovery.
- The new terminal-destructor containment deliberately retains the complete frontend storage when an unclosed Rider source lifetime remains, preserving the Manager callback context, session, ledger, cold/application owners, and GPU owners together. This is documented leak-on-contract-misuse containment, not reported cleanup success and not a substitute for a normal explicit close retry.
- The existing real renderer retirement clears its source-owner borrow only after checked completion. No fake Boolean, immutable bank, composed frame, or CPU drain supplies completion authority.

### Gates and unchanged prerequisites

- Directional input still returns `unavailableDestination` before callback/transform/audio/deferred movement effects, even when a wrap would finish on Rider19.
- Confirm/course, page2, nonzero player/controller extension, foreign model owners, popup/reset admission, full static-domain conversion, and general intra-Rider mutation completion remain outside the slice.
- No IPUM allowlist, decoder path, dependency, build registration, or gameplay enablement change is part of this overlay. The one renderer edit is the existing submission handshake's identity argument.
- Original numeric material refcounts, foreign/shared escaping graph domains, complete low-memory instruction behavior, and runtime/render equivalence remain unproved. The source review validates the stated contained native semantic projection only.

## Snapshot identity

Final source snapshot reviewed at 2026-10-11 12:37 UTC: 16 coordinated candidate files, including the renderer call-site identity change and the session's final cleanup-retry snapshot correction. After successful dynamic drain, `selectionAttached` is cleared before fallible lifecycle consumption/finalization, so a retry snapshot does not claim the destroyed graphs remain attached.

All 16 preimages were independently hashed against source bytes read from immutable baseline commit `b11246118e9859f83bd75a86f5f453288be4e749`; every pair matched.

Exact candidate identities are in `WHOLE-SOURCE-REVIEW-SHA256SUMS`. Its own SHA-256 is:

`a660f878bdf39adb600de92b609d4ea32491dfe94352fd495d3fd829c4867cd1`

The final session source identity is `e4bc96143ade6562fe49acbcd36a3796a9b9ed4fe2732f33a5ee4143fdc5afd5`; this includes the final post-drain `selectionAttached=false` correction. Earlier session hash `7ed813364c1c15468e70dcfa748867181d98c7b381325b7d3aa4dcdb5bd634a3` is superseded. Rehash the candidate bytes before integration; subsequent edits invalidate this exact-snapshot review until inspected.
