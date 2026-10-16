# Wreck B4/B8 zone identity and D0/D4 construction

Source-only increment againstbd155bf. No builds, tests, syntax checks,
sanitizers, project decoder executions or gameplay were performed. Source
inspection used raw read-only authenticated SLUS words and recovered native
registration leaves. No source formatter or guest instructions were executed.

## Proved B4/B8 semantics

The previous setup boundary is resolved by the actual consumers:

-16672C loads pieceB8 into a1;166730 calls120070 with a2=2 for E4.
-16673C loads the SAME pieceB8;166740 calls120070 with a2=2 for E0.
-16674C loads pieceB4;166750 calls28E8A0 with a2=2 for model14C.

120070 consumes a collision zone in the application4C4 namespace.28E8A0
consumes a visibility zone in the separate application4C0 namespace. The
existing native `OriginalWreckQueries50LA::attach` and
`OriginalWreckVisibility50LA::attach` already project those original zone
identities as a retained canonical world plus CollectionRef50BX. Neither
field is a callback, an Entity owner, a fabricated completion word, or a
scalar active-state flag.

Their constructor argument forwarding remains the chain recorded in
original-wreck-model-append50la.md. Source165EC8/165ED0 retain B4/B8; this patch
represents each as its actual typed world and zone reference. It does not
substitute one namespace for the other or require numeric equality without
source proof. The caller must supply actual source-zone field borrows, never
a host camera zone or a chosen default.

## Genuine continuation

`setupFirstBody165E90(session,racer,OriginalWreckSetupZones50LA)` requires the
first canonical occurrence stopped at165E90. The collision world/lifecycle
must be exactly those already borrowed by its backing-only query owner.
Visibility must be a distinct namespace. Both source field values must be
nonzero and in their real lifecycle/portal zone extents; the bounded API does
not admit a null-zone source path.

It stores the represented B4/B8 identities in source order, then:

1. Constructs the actual stable D0 Entity/body using real423F60 and the
   same constructor globals captured at initial piece construction.
2. Performs the literal141C20 slot30 no-op and actual1652E0 model binding,
   body320=1,1653C0(0), using the same retained graph/model cache. It does
   not refresh world100 after the earlier translation invalidation.
3. Appends D0 with maskFFFFFFFF through the real141960 component leaf.
4. Constructs actual D4 gravity, with capacity1, one borrowed own-D0 target
   and the original4431C0 force-vector input; appends with mask0.

Original call anchors are165EE0/165EE8 body construction/publication,
165F10 model binding,165F3C body append,165F58 gravity construction and
165F94 gravity append. The subsequent filename scratch names the already
retained authentic collision member; no project decoder is invoked here.
Success stops at166018 before E0 allocation/construction and11BBC0 publication.
It is the existing `publishedLifetimeRequired` phase, not active/completed.

Body and gravity use the existing source-native constructor helpers; no
separate body copy, invented counter, forced awake state or callback receipt is
introduced. Actual global/vector pointers are retained from the earlier binding
and must outlive this reached prefix. A reached failure is terminal and keeps
all earlier source references, fields, caches and child links.

## Retirement and borrow gates

The existing bounded tail-first D4/D0 retirement helper can now operate on this
canonical completed pre-E0 prefix. It still stops before loaded model retirement;
it cannot dispose actual material/geometry/cache/packet/PSG ownership. It does
not erase B4/B8 or release their worlds.

Once B4/B8 are retained, releaseUnpublishedQueries rejects cancellation: even
though no query row is published, the piece itself still borrows the collision
world through B8. A query-only cancellation cannot certify that world dead.
Visibility likewise must remain alive through the source piece lifetime. A
future authentic piece/model retirement join must end these borrows, then permit
cold backing cancellation; no native revocation is substituted for source
unregister/destruction.

The first piece remains unappended to its collection. Other pieces, E0/DC/POINT/
D8/E4/E8 construction, effects, activation, death and KO remain gated. No driver
calls are added. Legacy one-piece construction is unchanged.
