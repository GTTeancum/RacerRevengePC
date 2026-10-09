# Why action 2255B0 remains blocked

2026-10-06. Existing-ELF investigation after 49A; no action implementation added.

The original helper `[2255B0,225B9C)` cannot be replaced by its caller's timer
stores, by an opaque success callback, or by a guessed spawn/pose. The new audit
closes several access/copy leaves but identifies concrete missing input data and
state-producing dependencies.

## Required authored graphs

The original class is TunnelTrack. Loader `[2A1260,2A1D74)` consumes a separate
parameter document with:
- Track: NumNodes and TrackWidth
- Node0/NodeN: Position, NumNextNodes, ordered NextN references and NodeReverb

Its original caller constructs a map spline name from `%sM%s` plus `.spl`, an AI
spline from `%sA%s` plus `.spl`, and an ordinary `%s%s` plus `.spl` fallback. These
are separate assets, not SCB static-world records. No asset retrieval is attempted.

The retained twelve RRWORLD1 fixtures contain 17,313 original parameter blocks.
Exact case-normalized searches find no Track or NodeN blocks and no NumNodes,
TrackWidth, NumNextNodes, NextN or NodeReverb fields. Ordinary model positions and
portal endpoint pairs cannot replace the missing ordered track graph. Eleven
worlds even share the same supplemental default-start position; these values
must not be invented into race respawn locations.

## Required runtime bindings and effects

TunnelTrack nodes have stride80 hexadecimal. Node position starts at+00, neighbor
pointer at+6C and signed neighbor count at+70. NodeReverb is the word at+74.
The table-index byte at+78 is a different value: the node constructor clears it,
but an independently called post-load helper1ACF70 later produces it through
setter2A2660. That producer is not recovered. Assuming every live value remains
zero would omit real initialization.

2255B0 also needs application table arrays+4C0/+4C4 indexed by that byte, current
scene transforms, exact VU/FPU normalization/selection semantics, and the mandatory
player virtual+78 effect. Supplied cached-valid world transforms resolve only one
subproblem; they do not supply missing graph or action state.

The verified player table maps virtual+78 to1A24F0. Its complete wrapper clears
player+109C and selected bits of+10CC; for this action's a1=0 it skips its optional
branch, may call1A0240 depending on+3C, and then calls216A30(player,0,1). Those
concrete effects cannot be replaced by the preceding BC0/BD0/C30/C34 stores.
Neither downstream target was followed in this bounded investigation.

## Closed leaves and deliberate stop

Six selected helpers are complete raw graph access/copy/vector-construction
leaves. Their original lack of index/null checks does not establish valid live
storage. Scene helper28B2B0 is straightforward only with supplied cached-valid
or parentless-copy state; its cache-rebuild dependencies remain unexpanded.
The audit retains all selected original words and 33 exact verified code/string
blocks. No missing runtime graph or object state was reconstructed from RAM.

Production action work stops here. The independent 49B world-index milestone
uses real retained static-world data and does not claim to close2255B0, recover
race spawn/path behavior, infer visibility, or connect world collision systems.
The existing49A unavailable-effect boundary remains unchanged and explicit.

Evidence: `evidence/worldIndex49B/reposition-blocker/` and the retained authored
world corpus audit. The original ELF SHA-256 remains
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
