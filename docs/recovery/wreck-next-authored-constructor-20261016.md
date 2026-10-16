# Next authored wreck occurrence: constructor cursor and cold model load

Incremental source-only change against cadea76. No execution validation.

## Ordered cursor

`constructOrderedPiecePrefix(0,pieceIndex,binding)` selects the actual retained
inventory occurrence. `constructNextOrderedPrefix(binding)` selects the
collection's actual completed-piece count; `constructFirstOrderedPrefix` remains
a wrapper for (0,0). Collection advancement is still gated until its genuine
group constructor exists.

Admission requires sourceSlots38 == completedPieceConstructors == pieceIndex.
For a later piece, its predecessor must have completed the real WATER suffix
and stopped at nextPieceRequired /22C0A0, remain appended, and have begun no
query/contact/piece retirement. The live canonical forest is checked first.
No first-piece state, receipt, component, model graph or source count is reset.

Source22C25C/22C260 advances the array offset/index after both WATER additions;
22C270 branches back to22C0A0 while another authored part remains. Actual
piece initialization follows1659CC /2CB4B0;22C16C publishes its collection38
slot before2CB5A0 /165C30 setup. This patch advances sourceSlots38 at that same
reached boundary. The collection's22BF00 construction and sourceCount30 write
occur only for its first piece. A private per-piece constructorAttempted flag
records partial failure independently of the enclosing collection attempt.

All eight caller keys must match the selected occurrence's real component
addresses. Later pieces use the same object-count, attachment-lock, constructor
globals, gravity vector and type registry authority as the first piece.

Before any source-side write, admission requires retained prefix-history count
plus the selected piece's full sceneCount+8 canonical component capacity to fit
64 entries (plus the collection Entity on the first piece). Failure reports
capacity at22C0A0 without changing attempts, cursor, references, model graph,
source count or prior receipts. This is a bound, not a claim that every original
collection fits64. There is no truncation or replacement mesh.

## Actual independent model route

Each selected occurrence uses its own retained graph, authored hierarchy,
standard-model payload and canonical owner. The existing direct-root2403A0 /
26E934 path resolves its real model-file identity in the retained resolver.
An actual geometry cache hit stops at warmModelRequired /26E9F0 before any
geometry retain or clone. A real miss executes28CAC0 on that occurrence's
source hierarchy and retains its actual canonical rows in the shared history.
Later authored pieces are not assumed to hit the first piece's cache entry.

`loadFreshModelOccurrence26EE1C(collectionIndex,pieceIndex,...)` continues only
that selected genuine cold miss. It revalidates the actual file association
before loading and preserves the existing order:26E640 material acquisition,
240570 quantization,240110 geometry/prototype,1326A0 cache publication,
26EE9C source row table, then240740 packets with original a3=false. Resource
references belong to that selected model owner. Partial-failure receipts and
loader diagnostic PCs remain per occurrence. The old first loader is a wrapper.

## Exact remaining boundary

A successful next-piece cold route stops at modelAppendRequired /165CF0.
The new occurrence has not appended its model, extracted/reset authored local
position, constructed D0/D4, published queries, created effects, appended its
piece or added WATER. Those entrypoints still select the first piece and must
be migrated before the next piece can complete and advance the loop again.

The stage3 retirement APIs already select occurrences, but their complete
loaded-model retirement path requires the later append/body/gravity phases.
A new prefix stopped before those phases is not claimed safely retired; its
retained source ownership must survive the stop. Warm hierarchy cloning and
earlier partial model-allocation retirement remain explicit unsupported
boundaries. No source lifetime is synthesized to dispose of those cases.

No driver activation, destructive gameplay, group completion or external
particle-manager cancellation is introduced.
