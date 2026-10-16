# Separate actual piece and collection retirement: stage2

Source-only increment against stage1 localc993fc4 (centrally701a077).
No build, test, syntax check, gameplay, project decoder or guest instruction
execution was performed. No later authored piece is constructed or activated.

## Piece lifetime and immutable receipt

retirePieceOccurrence(collectionIndex,pieceIndex,actualWorld) operates on the
same retained occurrence's model/body/query/effect storage and progress.
It requires the existing real child/model retirement to have reached
pieceRetirementRequired. It does not infer retirement from an unlinked
component, allocated backing, or another occurrence's receipt.

The selected piece must be the last unretired reached source slot. Every
later reached occurrence must already have its actual pieceRetired receipt
and zero reference. The full collection child chain is validated against
actual ordered38 occurrences in authored order. Only genuinely retired or
never-appended pieces are absent. All actual children of this selected piece
must have zero references, and its query owner must admit source-retired
world backing (or its genuinely unpublished branch). Older live sibling
pieces are neither decremented nor required to masquerade as retired.

Original142590 disposes own children first, then repairs sibling/head links
before104970. For this now-childless tail, predecessor.next14 is set NULL,
or collection.firstChild18 is set NULL if there is no predecessor. An
actually unlinked partial piece instead leaves the existing chain untouched.
The piece's genuine reference1 becomes0,2CB500 frees its own160/164/170
pointer arrays, then the Entity count/type transition completes. Its actual
world/context borrows end here. Source parent1C, previous10 and ancestor24
history remain untouched; external emitter owners are not disposed by it.

The occurrence now stores an immutable completed pieceRetired receipt at
141418. Repeated retirement returns that receipt even after collection38 is
freed. The enclosing collection never overwrites it or adds its own count.
The existing constructor receipt remains unchanged.

## Collection lifetime and distinct receipt

retireCollectionOccurrence(collectionIndex) requires empty firstChild18,
source reference1 and a still-live collection22BF00. All reached source38
slots require their actual pieceRetired receipts and zero child references.
Unreached occurrences must remain genuine unconstructed backing: allocated
canonical components have kindunbound/reference0, and only unallocated PSG
child-row slots may be NULL. No NULL constructed row is accepted as retired.
The group must still be truly unconstructed; its future lifetime is gated.

Only then104970 reaches zero,22BF84 frees38 without releasing pointees, and
22BF90 reaches1413E0. A distinct collection receipt reports exactly one
released reference/object. Collection count/slot/progress history and stable
occurrence backing survive. SourceRetirement exposes this receipt read-only.

The existing retireFirstPieceAndCollection API remains a sole-reached-piece
compatibility wrapper. It runs the two real operations and composes their
returned totals without mutating either receipt. If collection admission
stops after the piece retired, it returns the completed piece receipt; it
never implies the collection retired. Wider collections use the explicit
occurrence APIs rather than misleading first-piece aggregate totals.

## Preserved gates

Earlier query/contact/body/model retirement entry points still address the
established first piece. Per-occurrence selection of those stages, complete
live sibling/ancestor domain views, and later-piece construction remain the
next review stage. This split does not manufacture those histories.

The existing modelGeometry.constructed240110()==false admission is retained
for today's sole fresh prototype. It is deliberately not generalized to
warm borrowers: a future shared-geometry piece needs a real per-model
reference-release proof while its shared geometry/prototype may stay alive.
Prototype-last ownership and partial material/row-table/packet cleanup must
be preserved before enabling such occurrences. Group/outer racer ownership
and gameplay activation remain gated.
