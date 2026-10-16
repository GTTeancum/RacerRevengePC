# Loaded canonical wreck model retirement

Source-only increment against232c57c. No builds, tests, syntax checks,
sanitizers, project decoder executions or gameplay were performed. Source
inspection used existing recovered owners and raw read-only SLUS words.

## Actual ordered continuation

`retireFirstLoadedModel()` is admitted only after the bounded tail-first
retirement actually released first-piece D4 and D0. It requires their real
references to be zero and the piece's remaining first/only child to be the
canonical loaded model. It does not manufacture a null-resource branch.

The canonical model owner joins its own Entity exactly once. Its outside
Entity domain contains the real collection and piece, excluding already
retired D0/D4 and the separately owned PSG hierarchy. Complete preflight
covers the attachment lock, live parent/sibling topology, model/PSG references,
observer/configuration absence, packet ownership, material reference release
and resource/prototype lifetime. A warm user prevents early prototype disposal
through the existing canReleaseForModel check.

After preflight, the admitted sequence is terminal and preserves each reached
prefix on rejection:

1. Actual142590 current-parent repair and104970 model decrement to zero.
2. Actual240450 packet release, while geometry/palette ownership is still live.
3. Actual26E4C0 material reference release in original slot order.
4. Actual26E598 geometry reference release, last-resource surface cleanup and
   original1327D0 cache removal after resource destruction.
5. Actual26E5CC source-row-table disposal.
6. Actual26E5EC material-array disposal.
7. Actual2896D0 PSG retirement, using its real first-child/next-sibling order.

The existing scene owner decrements each actual child reference and423F60
counter. Model-as-row0 is released once. Aggregate result counts merely report
those completed operations; they do not perform extra counter/reference writes.
The original stale model1F0/1B4/1B8/1C4 values are not cleared to fake disposal.
Native owner phase flags prevent reuse.

## Material backing boundary

Source26E5E0 reads1C4;26E5EC calls its array free;26E5F8 calls2896D0. The
previous material owner released references at26E4C0 but retained its allocation
until host destruction. New freeAllocation26E5EC requires that actual release
and frees the native handle allocation at the explicit source phase. Read-only
handle/allocation inspection thereafter returns no live storage. Count1C0 and
the model's stale source pointer remain separate observations.

Existing callers retain their prior deferred backing behavior unless they call
this new phase. The wreck continuation calls it only after geometry and source
row-table disposal. No global material-table release or cache-clear shortcut is
introduced.

## Remaining genuine lifetime boundary

Successful model retirement returns `pieceRetirementRequired` at2CB500.
The actual piece and collection references are still live. PieceB4/B8 still
borrow their visibility/collision worlds, and setupZonesBound remains true.
Consequently query-only cold cancellation remains rejected. Freeing a model
is not evidence that its enclosing piece or its source-world fields died.

The next bounded join must execute the actual reached piece2CB500/165BD0/
1413E0 path and collection22BF50/1413E0 path, with exact reference transitions
and constructor-prefix facts. Only actual piece lifetime completion can end
B4/B8 borrows. The query backing has never published11BBC0 rows in this route;
its later native cancellation must remain distinct from source11B4A0.

No destructor guard is weakened. The enclosing storage remains non-destructible
while piece/collection source references survive. Construction failures before
the completed pre-E0 prefix, observer-bearing models, warm cloning, effect
ownership, query publication, runtime activation, death and KO remain outside
this bounded retirement path.
