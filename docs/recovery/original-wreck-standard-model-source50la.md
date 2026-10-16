# Authentic wreck Standard-model source ownership

Source-only increment againsta81d750. No build, test, syntax check, project
asset decoder, sanitizer or gameplay was executed during this work.

## Implemented load-path prerequisite

Each genuine wreck occurrence now retains an immutable
OriginalStandardModelResource50LA prepared from its authenticated archive
member's exact resolved name and retained PSG bytes. This replaces the separate
wreck Scene48D decode, rather than adding a second decoded scene. Existing
numeric graph/bounds preparation consumes this resource's actual source rows.
The scene accessor returns the same retained Standard payload's scene.

OriginalWreckAssetInventory50LA::bindModelSources accepts the real prepared
application material domain and uses its existing resolver3E1A70. It visits
actual collection/piece order and binds each exact PSG member/byte sequence
through bindModelMember. Every occurrence stores that actual file ID. Identical
members share the resolver's existing row; conflicting bytes fail there instead
of overwriting the application's source. No stem-only deduplication merges
piece owners or graphs.

The inventory pins that same resolver until after occurrence storage is gone.
Binding is one-shot; a failure retains the true successful prefix and original
resolver associations, and cannot silently switch to a different application.
Call this after loadNextCold completes and before transferring the inventory
into the ordered racer owner. OriginalStandardModelResource50LA supplies the
payload later accepted by construct240110 without rebuilding another mesh.

This is file-source/payload ownership only. It does not create source resource
reference00, publish geometry240110, acquire material handles, construct model
Entities, build packets or certify model retirement. A resolver geometry hit
must be inspected at the actual future26E820 phase, not captured now as a
promise that the same fresh/warm branch will still apply.

## Why the full model owner cannot be wrapped around current projections

The current wreck loader has already prepared its graph's hierarchy, extracted
and reset root position, refreshed worlds and computed bounds. Existing query
prepareCold also captures initial E0/E4 world histories from that graph. The
reusable OriginalRacerModelSceneOwner50LA instead owns actual model/PSG Entity
objects and must execute source construction on the canonical graph before
those later steps. Its topology-only freshNode guard is not an adoption seal;
a childless prepared graph could pass while still having the wrong lifetime.

Therefore the next genuine integration must:

1. Introduce one private per-occurrence model facet containing the actual
   ModelSceneOwner, material references, geometry owner and packet owner.
2. Replace projected model/PSG component rows with a stable slot map borrowing
   that facet's real embedded Entity objects. Piece/D0/D4/query addresses and
   the genuine per-piece9C receiver remain unchanged. Query preparation must
   capture the real model Entity address, never a copied row.
3. Split backing-only query allocation from numeric initial shape/history
   preparation, or defer the latter until genuine model loading/root reset.
   Do not reset an already-prepared graph and leave stale query histories.
4. Run actual26E820 fresh/warm cache decisions and resource/material/packet
   construction against that canonical facet, preserving any reached failure.

## Exact cache/reference order and limits

165CD4 invokes240740 with a2=0, so source PSG row0 is the model itself.
26E934 sets the resolved name. For a real fresh miss,28CAC0 loads hierarchy,
26E640 acquires materials,240570 loads quantization, then240110 constructs the
geometry resource.26EE50 publishes model.geometry1B4 and26EE80 publishes the
non-owning1326A0 cache association.26EE9C allocates the row table.240798/2407AC
creates packet1F0 and2407A4/2407B4 stores it. Immutable shared_ptr lifetime is
not a replacement for source104960/104970 reference arithmetic.

For a real warm hit,26E9F0..26EA00 installs/retains the actual geometry; its
prototype controls scene copying. Wreck a2=0 with prototype child184 nonnull
reaches virtual98 at26EB10..26EB20. The reusable code currently supports only
a private childless direct-root warm branch, not that general clone. Such a
hit must remain an explicit boundary. Material handles then use clone26EB44
from the actual prototype; fresh acquire26E640 cannot substitute for it.

Only after genuine packet240450 and material/resource26E460 disposal can
2896D0 retire model/PSG ownership. The existing source-reference destructor
guards and the pre-E0 construction boundary remain intact. No active driver,
death or KO path is enabled by this patch.
