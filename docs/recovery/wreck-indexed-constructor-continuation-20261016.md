# Indexed wreck constructor continuation

Source-only incremental patch against1a7ca47. No execution validation or driver
activation. All17 existing first-piece entrypoints remain wrappers selecting
(0,0); corresponding `Occurrence` entrypoints accept collection/piece indices.

## Cohesive continuation

The same actual selected cold-loaded occurrence can now continue through:

1.165CF0 model append, actual local-position extraction and165D8C reset.
2.165E90 actual B4/B8 world/zone binding, D0 body and D4 gravity construction.
3.166018 E0 construction,11BBC0 canonical model publication,11F570 preparation,
  and166098 append, using its own query receiver, type2 shape/results and model.
4.295A40 contact constructor, actual POINT records and D8 detector append.
5.E4 type0 cache acquisition, actual9C/A0 callback binding and RESOURCE geometry
  publication, followed by E8 type3 result/shape/category construction.
6.The actual mutual-filter allocations and2CB610 derived producer binding.
7.The selected authored piece's EmitSmoke/count174, actual effect inputs,
  positive smoke/fire loop, Stop84 order, dust and splash owners.
8.22C1B8 append to the actual collection after preceding authored siblings,
  source DC/body tuning writes, and real source-owned WATER categories.

All constructor state, canonical graph, body, callbacks, query owners,
allocations, effect arrays and failure receipts remain with that occurrence.
Source order within each operation is unchanged. Existing partial-failure
returns retain their reached side effects, and no completed receipt is reset.

Common selection requires sourceSlots38==pieceIndex+1 and the collection's
completed-piece cursor==pieceIndex. Earlier linked siblings must be precisely
the authored canonical sequence, with no retirement begun. The selected piece
must likewise have no retirement begun. Full live topology and retained
canonical history must remain valid. The private source objects are reused;
no per-piece replacement forest, body, receiver or model is created.

WATER completion advances completedPieceConstructors to pieceIndex+1 only after
both real additions. The next authored piece may then enter the stage4 loader;
the final piece stops at groupConstructorRequired /22C278. The actual collection
append API verifies existing earlier sibling order rather than requiring an
empty collection. The stage3 retirement APIs already handle these exact
occurrences in reverse reached order with independent retained receipts.

## Authored effects and the real zero-count branch

The effect input operation now reads authoring.collections[collectionIndex]
and pieces[pieceIndex].emitSmoke. Applying piece0's EmitSmoke to another part
would change original174 and is not admitted. MaxNumberOfDamageEmitters remains
the owning collection's actual authored word. Genuine null resource/parent
branches retain the existing source return behavior.

For non-null inputs, authenticated SLUS words at2CB628..2CB638 are:

    8e820174 1840006d 24040260 0c0ba328 00022080

2CB628 loads174;2CB62C BLEZ targets2CB7E4, with size260 in the2CB630 delay slot.
Thus a genuine count174==0 skips160/164/170 allocation and the smoke/fire loop,
but still reaches dust allocation. At2CB7D0..2CB7E8 the authenticated words are:

    8e820174 0202102a 1440ffbc 00000000 24040260 0c0ba364 00000000

The positive loop reaches2CB7E0's size load; the zero branch arrives at2CB7E4
with that same size already set. The continuation records these distinct PCs.
It neither fabricates empty allocated arrays nor indexes a nonexistent last
fire child. Dust construction requires the exact untouched array state on the
zero route. Existing genuine dust/splash constructors, setup/Stop, append,
external ownership and retirement are reused. No forced-set defaults change.

All occurrences with created external children must use the same actual
racer7FC particle forest, storage and capacity. This keeps earlier controls in
the real domain scanned by the existing key/control-ID uniqueness admission.
Fresh namespaces per piece cannot hide an earlier live control. The caller
still must provide the full actual external forest, including unrelated live
children; this is not a process-global registry. External parent lifetime
remains independent of piece retirement, and particle-manager rows remain.

On genuine piece2CB500/1413E0 lifetime end, native external-domain borrows end
also for the zero-count case whose source pointer arrays stayed NULL. This
native borrow cleanup destroys no external child and cancels no manager row.

## Remaining gates

Warm geometry/hierarchy cloning still stops at26E9F0. Early incomplete model
loads and unsupported partial allocation lifetimes stay retained at their
exact boundary. Successful cold occurrences now have the full admitted
constructor/retirement connection through WATER, but this is not a runtime
success claim. Effect/source asset admissions and allocation failures can
still stop the original sequence honestly.

The retained64-component capacity bound remains preflighted by stage4 before
starting the next source occurrence. The collection group constructor22C278,
subsequent collection advancement, world registration and destructive gameplay
activation remain unjoined. No all-collections-complete claim is made.
