# Per-occurrence native continuation ownership: stage1

Source-only refactor against59d4cce. No build, test, syntax check, gameplay,
project decoder or guest instruction execution was performed. This stage
changes ownership of existing continuation metadata only. It does not enable
another piece, select a different model, acquire a new reference or reset a
completed receipt.

## Field ownership and moved reads/writes

OriginalWreckPieceStorage50LA now contains its own private progress_ value:
- modelRetirementAttempted and appended22C1B8 (formerly firstPieceAppended22C1B8).
- e0ConstructorAttempted/e0PrefixRetired/e0PublicationAttempted.
- contactSuffixAttempted/contactSuffixRetired/dcReached/d8Reached and
  contactSuffixDestroyed.
- e4ConstructorAttempted/e4PrefixRetired/e4PublicationAttempted.
- e8ConstructorAttempted/e8PrefixRetired/mutualConstructionAttempted.
- retirementAttempted/freshModelLoadAttempted/modelAppendAttempted/
  pieceSetupAttempted.
- Existing prefixResult and retirementResult, retaining exact defaults,
  update order, statuses, source PCs, totals and failure history.

Every former storage_-> access to those fields now selects firstProgress(),
which is deliberately fixed to collections[0].pieces_[0].progress_. All
existing entry-point admissions and short-circuit storage checks remain.
The actual model/body/query/effect objects have not moved or been copied.
Read-only sourceProgress() exposes the occurrence's own metadata; no setter,
reset operation or active-occurrence selector is added.

The old owner constructorAttempted flag covered22BF00 as well as the first
piece. It is therefore moved to the actual first collection's
constructorAttempted_, not reinterpreted as a piece-only operation. Likewise
the old pieceCollectionRetirementAttempted belongs to that collection's
retirementAttempted_: this currently remains a combined final operation.
The existing final retirement receipt still includes the enclosing collection
tail, preserving current API behavior. Splitting that operation and its
reporting is explicitly stage2; no partial refactor claims it already happened.

Session/racer identity, immutable inventory, shared model scene domain,
world-backing preparation/release state, actual global pointers/counters/lock,
canonical component registry/domain and collection arrays stay with their
existing authority. The source30/38 and completed-piece cursor stay with the
collection. Cold later occurrences receive fresh native metadata but are not
source-constructed by that default initialization.

## Next required refactor, still gated

Before any next-piece source construction, retirement must select the actual
tail occurrence, use a complete live canonical sibling/ancestor view, and
separate piece retirement from collection38 destruction. The existing
first-piece implementation assumes a sole source child. That assumption is
retained rather than silently generalized by this metadata move.

Each next authored PartN must resolve its own actual file cache. A different
part can be a real cache miss; no universal warm-cache assumption is valid.
Warm26E820 uses a2 for its hierarchy/root branch (26E84C saves a2;26EA0C
tests it). Outer240740 preserves a3 for packet mode instead. A hit stores
model1B4 at26E9F0, calls the actual retain at26EA00, and reads that resource's
prototype1C. It then performs the real hierarchy branch, clones materials
26EB44..26EB90, copies parameters via virtualB0 at26EBBC, allocates/copies
source row table26EE9C..26EEB4, then constructs packets.
The existing childless warm helper covers only a2=0, prototype191.bit1 clear
and prototype184 NULL. It cannot replace either virtual98 clone branch.
The canonical geometry owner already requires its original prototype to
retire last. Any future warm occurrence must preserve that dependency and
match partial material/row-table/packet retirement before source publication.
