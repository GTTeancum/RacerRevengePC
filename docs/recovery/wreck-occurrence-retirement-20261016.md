# Wreck occurrence retirement, stage 3

Source-only incremental change against b2b793e. No construction selector,
next-piece activation, runtime callbacks, or execution validation is added.

## Native join

Eight earlier retirement operations now select an actual retained collection /
piece occurrence: E8, published E4, unpublished E4, DC/D8 contact suffix,
published E0, unpublished E0, D4/D0 children, and loaded model. Existing
`retireFirst...` entrypoints are compatibility wrappers selecting (0,0).
All phase guards, query owners, counters, reference writes and receipts use
that occurrence's private progress. Source publication and retirement history
is retained. Completed history remains readable after collection38 disposal.

Selection authenticates the collection's existing ordered38 slot. A mutating
retirement is rejected until every later reached occurrence in that collection
has completed its piece lifetime. This preserves the tail-first1424E0 order;
it does not skip an unfinished later constructor.

Live domains are borrowed from the actual canonical component slots of all
retained pieces, collections and groups. They exclude only cold/unconstructed
or genuinely zero-reference Entity history; zero-reference objects retaining
another concrete kind reject. The existing64-entry bound rejects overflow
before mutation. Full views verify link membership and adjacent/parent/head
reciprocity. Thus older live siblings and ancestors remain present while a
later occurrence retires. No duplicate Entity or replacement graph is made.
Cold canonical PSG slots may remain NULL until the existing model loader
creates them; other ownership stays private to the retained inventory.

Model retirement first verifies the full live topology. Its outside view then
excludes only the selected owner's model/PSG slots. The canonical model owner
joins its own model Entity exactly once; its separate scene destructor owns
PSG children. Other occurrences' live canonical rows remain in that view.

## Source order and resource proof

The source sequences and guards already recovered in the preceding retirement
patches are preserved:

-1424E0 /142590: tail children first, actual sibling repair, then104970.
-11B4A0 /11B600 and11AEE0: actual query arrays, results, shape/cache references
 and model-domain retirement remain owned by the same per-piece query owner.
-295B90: contact POINT /pair backing release follows actual DC/D8 reference
 transitions; body and callback aliases are retained until their own lifetime.
-2826D0 and1679D0 ->165260: the admitted one-target gravity and NULL28C body
 constructor route retires D4 then D0. Generic body teardown is not admitted.
-240450 ->26E460: model reference release, packet disposal,26E4C0 material
 references,26E598 geometry reference,26E5CC row table,26E5EC material array,
 then2896D0 canonical PSG/scene retirement.

Previously the final piece gate required geometry.constructed240110()==false.
That proves final resource disposal, but would reject a real warm borrower
whose prototype still owns the same geometry. The new private occurrence
receipt is written only after successful actual release26E598. Piece end also
requires that exact model owner to be absent from the geometry's actual
retained-model array. The resource's existing canReleaseForModel guard still
requires prototype-last order. A successful warm borrower release can leave
the resource/cache live; no fake zero reference or cache reset is introduced.
Packets, materials, canonical model lifetime and query/world gates still must
complete independently before piece borrows can end.

Session query-backing closure now examines every actual published E0 history
and requires its own completed source retirement. Cold occurrences retain the
existing unpublished-world gate. Canonical history closes only after all
world borrows pass their real owners' closure conditions.

## Remaining gates

Next-piece construction is still fixed to the first occurrence. The future
constructor must append genuine canonical objects to retained history, resolve
each authored PSG's actual cache independently, and complete fresh loading or
the proven warm clone path. This patch does not implement warm hierarchy
cloning or claim that any existing first piece is a warm borrower.

The admitted full domain remains64 components. Larger authentic collections
need a separately reviewed capacity/ownership extension. Incomplete model
loading and unrelated body/group/effect runtime teardown remain explicitly
unsupported. External particle parents retain their own lifetime; no manager
row cancellation is introduced. Full collection/group and destructive gameplay
activation remain gated.
