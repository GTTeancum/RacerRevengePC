# Gameplay cold collector and representative-map restoration

Source-only replacement, 2026-10-13. The exact restored base is
27ac934596b5f73865013f4ff4cbc2c084180075, tree
7df3dc7566b394ed081c268b3fdfde751b101707. No build, test, syntax check,
scanner, decoder, asset loader, guest execution or gameplay was run.

## Exact recovery and new replacement boundary

The complete table header and implementation are exact recovered postimages
of the reviewed representative-binding layer. Their SHA-256 identities are:

- header: debd736b5e0567efbc2c980a253b8e98d3ab35c94a834763c2771172bef2410a
- implementation: beb969c3fcb97880f8c8ba404210250f80e8cfce02c2b3f334e8d30e8ae0b30a

The domain, material-object pair, texture-resource pair and right Visual
consumer are NEW bounded replacements. Their original complete deltas were
lost. They are not byte-exact recovered versions of the former six files.
The retained old target hashes are kept in the external recovery receipt,
separately from this packet's actual pre/postimage manifest.

## Actual production joins

The existing Application material domain now invokes14D440 only after its
actual14D370 material and29E340 texture population calls succeed. It retains
the table-issued cold receipt without owning additional pass references.
The exact table implementation walks every nonnull canonical row and declared
pass in source order, caps registrations at4096, preserves redirect/barrier
ordering, and seals only a complete rebuild. Clear/rebuild failure leaves its
partial registry unsealed. Publication and final row destruction invalidate
the seal and retire borrowed nodes before any actual pass can disappear.
The domain preserves population counts, texture details and committed-effects
evidence when a later collector operation fails; the collector retains the
reported failure location.

Private material-object methods clear declared5C mappings to source zero and publish the
real registration index. They cannot be called by arbitrary consumers. All
existing pass references remain unchanged; nodes borrow the table-owned
passes and do not manufacture source retains. The small private guards check
the already-established owner invariants and terminate on internal misuse.

The existing cold PST0/1 and TGA resource routes now expose their actual
constructor-cleared F8 bit0 and a known-bit mask. Only bit0 is claimed known;
zero native backing does not assert values for other source bits. The IPU
kind2 setter and other resource families remain outside this constructor.

Right-engine cold Visual lowering asks the sealed table for each actual
final representative before choosing its native image. It requires the
returned live resource to be the canonical token's same object and consumes
the prepared per-map context, including the proven249610 TEXA policy. Both
base and second-map native consumers use those values. The prepared copies
and resource borrows remain local to the cold call; existing immutable RGBA,
sampler and renderer descriptors remain the published native data.

No per-frame collector scan, extra model owner, later live image-bank append,
clock update, RNG use, AI/physics change or lower/chariot/window material
layer is added by this restoration.

## Source evidence and limits

Original executable identity:
c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.

The recovered table source and its matching hashes retain the original
14D440,29BEA0,29BED0,29C490 and249BA0 source review. In the restored repository,
recovery/20261011/RIDER-COLD-MATERIAL-EQUIVALENCE.md documents the complete
constructor-state comparator and the separate post-draw history problem;
RIDER-MATERIAL-REGISTRY-LIFETIME.md documents global registration ownership;
RIDER-SOURCE-RGBA-COMPOSITION.md documents the24EA40 kind2 setter and the
249818..24983C one-map TEXA overwrite. The recovered table also preserves
the previously reviewed dual-map overwrite selection.

The24E310..24E320 constructor-cleared bit0 finding for admitted PST0/1 and TGA
is retained in docs/recovery/original-racer-source-color50la.md and the
pre-loss source review. The complete original constructor
listing is currently unavailable, so this replacement does not claim a new
independent reread of that listing. The implementation never derives F8 from
a resource filename or from the AC pixel format.

This is prospective cold binding preparation, not accepted texture-binding
history, resident mip selection, a full241280 flags/LOD producer, global FIFO
submission/drain, or gameplay window-pass2 admission. Map2 suppression does
not alter cold preparation of either map. Existing native filtering,
source-alpha equations, topology repair, performance invariants, pause and
release order are retained. Exact6a matrix/origin payload and45 distance
restorations are separate already-integrated prerequisites in the base.
