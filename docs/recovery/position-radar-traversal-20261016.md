# Original radar route and actual pose integration

Source-only candidate against4c56471. No builds, tests, syntax checks,
sanitizer checks, gameplay or independent decoder checks were performed.
Original SLUS was read only as source bytes; no guest instructions or VU
programs were executed. Central checkout was not modified.

## Concrete native integration

Application::preparePositionStatsRadar50LA now joins the existing resource
owner to the actual same-Application camera and ALL8 manager. It performs
original layout, own-leaf visibility and map preference gates before touching
the radar pose or route. It remains uncalled; CPU geometry is not GPU readiness.

The RA camera factory supplies actualRadarMapB8 from its retained authenticated
RA source map, alongside its existing source-owned particles/height/models.
The optional field defaults null for other camera constructors; no other map
or generic caller-supplied readiness flag is accepted by this preparation API.
The existing camera/RA ownership must outlive this synchronous call.

The adapter observes Application view0's racer, which is independently driven
by the original ALL8 AI. It does not alter or select that racer's controls.
An actual Bank visit resolves body340->scene288, calls the existing native
refreshSceneWorld49P and reads its world100 translation, then body cachedAnglesE0
lane1 (E4). This reproduces28B1A0(relative=null); local coordinates would be
incorrect for a parented body. Lazy world-cache effects are canonical writes,
not a copied/recomputed host camera pose. As in the original, they remain if
later preparation fails. Before/after camera publication and identity checks
keep the observation within the same published session frame.

Original13BBA0 cosine and13BAD0 sine polynomials now supply owner32D4/32D8
from actual E4; there is no host trigonometry or angle from a route tangent.
Native finite[-pi,pi] admission is explicit. Source reflection and separate
binary32 multiply/add boundaries are retained. Owner32C0 XYZ is updated while
its constructor W stays unchanged; source28B1A0 writes XYZ only.

## Exact source traversal

192568..1927E0 visits authored map nodes in increasing order.2A2600 extracts
node79 bit5, SecretBranch, and skips these nodes. It does not test Pretty or
Ugly. Each eligible candidate first emits at the current uncounted point;
alpha0 leaves the count and previous accepted node unchanged. Those writes
still occur and can clip a prior endpoint, exactly as192C70 specifies.

A positive-alpha candidate asks2A26E0 for its predecessor. Same branch as the
prior authored node returns node-1. Node0 scans increasing authored Next0 links
for the first link to0. Other nodes search all links descending from that node
through1, excluding0. This is a predecessor query, not a group-ID query.

If that predecessor differs from the previous accepted node, the existing strip
is closed by emitting Next0(previous), counted and finalized. A new strip emits
the predecessor and current node in that order, counting both regardless of
the second call's alpha. Otherwise only the candidate count is incremented.
The final nonempty strip also appends Next0(previous) before finalization.
After16 completed elements the original stops the map loop; elementLimit records
that boundary explicitly. The118-point native capacity fails with diagnostics
before an out-of-bounds write. The authored map graph is validated first.

Finalization writes actual count08=pointCount*2 and dirty3C=1 in the original
primitive owner. The source2E2450 type5 branch validates at least4/even vertices and
then updates active packet counts/header words. The incremental lowering
patch implements these count effects; it does not treat validation as complete. The subsequent2E21D0 call remains deliberately
unimplemented here. The per-element native submission ordering must be recovered
when enabling the actual renderer; completed CPU geometry is not that receipt.

Successful writes before a later failure remain, including the old source point
and both overlapping packet aliases. The route diagnostic retains completed
strip count, pending point count/node and sourcePC. No rollback or silent reset
of geometry is attempted; the resource's complete-route record is cleared on
entry and published only after the whole admitted traversal succeeds.

## Remaining blockers

-2E21D0 draw and its type5 helpers2E2DD0/2E2ED0 must lower the actual authored/STQ
 pairs and active count to native renderer vertices, preserving original state.
 The existing STQ fields must not simply be reinterpreted as ready host XYZ.
-Original zero-map RadarOverlay canonical pass/state/FINAL representative and
 native ScreenObject submission must be joined. No fabricated map0 is allowed.
-1927F0..192C2C racer-marker textures, skipped-current-view/state40 gates and
 subtype3 special marker remain absent. The marker coordinates apply layout
 offsets after projection; route points keep their source screen coordinates.
-Root must activate staging and call the new preparation API at the original
 frame phase, then integrate renderer order with other HUD elements and base
 Entity draw. No host panel has been replaced and no visible success claimed.

Changes to actual racer simulation, rank, lap authority, controls and retirement
are absent. All existing display pins, two drains and reverse release remain.
