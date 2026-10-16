# Actual primary left/right paired specular consumer

New source-only implementation against
8d59bec3efac2b5daded6bd76413fa52f6d6d23d, tree
68ae9e8ca9b15ecf612df96d166bed564cba29f7. The earlier held primary-pair
candidate was lost with the previous filesystem. This packet is independently
implemented and reviewed against the recovered current source; it is not an
exact reconstruction of that earlier candidate.

## Connected cold publication

The real50DU caller now supplies its existing right and left model scene,
geometry and canonical material-reference owners to one cold Visual method.
It prepares right then left into two private drafts. Each draft authenticates
its own PSG member/hash, scene index, source-node mapping, palette, material
ordinals, PSMs, program and actual shared Application382B38/383400 resources.
The left model never borrows right geometry or a right source-color matrix.
Both primary flame hooks still bind to their original model clones in right
then left order. Lower-engine and chariot callers are unchanged.

Every consumed primary map obtains its current final representative through
the same canonical sealed collector and prepareColdRepresentativeMap249610.
The returned resource must equal the canonical token's actual object, and
native lowering consumes the copied per-map context including F8/TEXA policy.
This retains the recovered binding producer rather than returning to a cold
PSM-only context or deriving state from texture filenames.

The one immutable second-map bank is loaded only after both drafts are complete.
Resource identity, selected level and actual TA0/TA1/AEM/wrap form the cold
deduplication key. Component/material/pass/map slot keys remain distinct.
At most32 distinct64x64 projections are admitted, with the existing explicit
native largest-level/linear policy and original packed1555 lane expansion.
Neither shared names nor matching pixels substitute for canonical ownership.

All allocation and source admission precede the final bank load. A rejected
draft cannot publish either engine's changed bindings/mask or replace a live
bank. After successful transactional load, only no-throw vector swaps and
Boolean publication remain. The existing player/opponent GPU owners upload
this combined bank once; no later live-bank append or shadow bank is introduced.

The existing source-lighting opt-out prepares both unchanged base-map routes
and acquires no second-map bank. Runtime material overrides, non-dual body
programs, one-node bounds, unknown parameter/animation behavior and unsupported
texture/context branches remain rejected rather than substituted.

## Each actual model reaches its own native draw

The combined source-color descriptor requests source-specular geometry for
left and right independently. The existing actual240740 matrix producer
already consumes both model owners; this increment preserves that join.
The bounded49O adapter now admits either primary component while still
requiring its own multi-node semantic boxes, current locals/flags, source
parent map and model/root matrix. Source radius198/center160 and localF0
remain distinct from native display bounds.

Renderer cold storage contains exactly two primary slots, left/component1 and
right/component2. Each has its own checked original PSG group table, separate
double-buffered map2 UV stream and mesh/VAO extent. The original strip/group
repair and every semantic/EBO ordering guard are retained. No new import or
geometry decoder is run or added.

Each frame and opponent descriptor borrows an array of its two real model
packet owners, in left/right order. For every presentation, including stopped
opponents, the renderer uses each successful retained component's own bounds,
local position, model owner and current source camera to prepare the two
admitted241280 distance bits. The UV leaf consumes that model's result and its
own rootWorld, authored qNormal and raw source normals. Per-racer/per-body UV
and map2-gate storage cannot overwrite another body's prepared result.
The actual50DU caller clears both borrowed pointers immediately after submit
returns on success or failure, before display completion or Session release.

## Draw, error and cleanup invariants

Existing component/span/group/triangle order is unchanged. An admitted group
still draws its complete base before that group's second triangles. Each
second triangle retains its preceding feedback barrier. The existing byte
MODULATE, destination-alpha blend, GREATER0/KEEP, F255 fog and depth equations
are unchanged. Distance suppression omits only map2 and does not suppress cold
preparation of either resource.

The restored group-level error polling and span/group state reuse remain in
place. No per-triangle error/status query, extra generic per-frame identity
scan or new synchronization is introduced. The additional real left draw and
UV work is functional work; no measured performance improvement is claimed.
Partial initialization cleanup visits both primary UV owners, and existing
retryable GL-name cleanup/context rules remain intact. Shader and target
ownership, private source alpha and opaque final presentation alpha are unchanged.

No original AI, physics, animation/material clock, RNG, pause gate or model
alias/reference/cleanup order is changed. The shared Rig, graph, source corpus,
caches, material domain and authentic model lifetimes are reused.

## Retained evidence and honest limits

Original executable SHA256:
c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.

- docs/recovery/original-racer-left-source-color50la.md establishes the actual
  left canonical program and distinct PSG/geometry ownership. The per-pod body
  material correspondence corroborates the dual program, never geometry reuse.
- docs/recovery/original-racer-source-color50la.md and
  docs/original-racer-lighting50km.md retain registration150B3C selecting the
  dual32C3F0 packet and its root-camera/raw-normal/native arithmetic contract.
- docs/recovery/original-racer-specular-pair50la.md retains group-order anchors
  32CC28/32CD18 and32D670/32D750, the16-vertex group bound, map2 flag gate and
  original second-context TEXA interpretation. This implementation reuses that
  existing native program path; no guest instructions are executed or emulated.
- docs/recovery/original-racer-packet-matrix-producer50la.md and
  original-racer-distance-flags50la.md describe the restored actual owner joins.
- docs/recovery/gameplay-collector-binding-restoration50la.md records the exact
  recovered collector/binding source and the explicitly new small replacements.

The original full visibility/LOD/light-cache scheduler, accepted binding history
and global FIFO/drain remain incomplete. This pair continues the bounded local
group-alpha route and does not certify global gameplay alpha or window-pass2.
Lower-engine source-color/specular, chariot dual-map/multipass and window layers
are not included. Native filtering/raster/depth precision, historical specmap5
raw-header evidence and fog-evidence limitations remain explicit; this source
extension is not a hardware pixel-parity claim or a new fidelity verification.

Only manual source/evidence/diff review and file hashes were used. No build,
test, syntax/lint/scanner/sanitizer, asset decoder/loader, runtime, gameplay,
profiling or guest execution was performed for this candidate.
