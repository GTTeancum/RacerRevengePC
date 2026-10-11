Historical source/staging notes recovered into main on 2026-10-11. Current main includes later original-resource, roster, compound collision and native Gate work. These notes and the offline visual selector do not establish gameplay validation or expand current runtime admission.

# Data-driven track storage seam

Source-only follow-on, 2026-10-09. One new header:
tests/native/track_runtime_storage50ig_fixture.h.

No active source, previous staged source, session menu, or Clegg changes were
edited. No build, syntax check, test, decoder, renderer, emulator, or gameplay
execution was performed. This header is untested and not integrated.

## Useful change

The fixture supplies an immovable, cold-created owner for the existing world,
collision selection, movement registry, portal membership and collector
backing. Counts come from the actual loaded Ground world and COL owners.
FixedBuffer permits element mutation but exposes no resizing or relocation.

- N original COL owners yield N candidate bindings, contact-material records,
  movement candidates and target pointers. Query refs are N+1, N+2 and N+3;
  the collector has N+3 objects.
- Z actual zones yield 2Z live registries, 2Z membership-scratch registries,
  2Z collection methods and 4Z SearchList block descriptors.
- P authored portals yield 2P directional links and P prepared portal records.
  Each of the three query memberships and shared membership scratch receives
  P+1 links/entries, preserving the existing original-order portal preparation.
- The source candidate merge and collector nominal limit stay 64. They are not
  expanded to the world size. Selected-copy/index capacity is max(N,64), so a
  valid repeated selection can be represented even if N is smaller than 64.
- The queued cold registrations fit max(128,N+3) events. Every original collider
  is registered in caller order and its actual authored zone, using the
  existing serial transactional registry APIs.
- The helper maps MaterialID through the existing original SCB reader and
  builds sorted lookup handles without reordering the candidate source list.
- Selected terrain filters and gathered movement filters are reserved during
  cold setup. gatherMovement copies through the existing capacity-checked
  member-wise routine instead of assigning vector-owning candidates hot.
- registerMutableOwners registers stable POD backing spans and mutable lanes
  with the existing racer snapshot bank. It never memcpy-registers a vector,
  smart pointer, Storage object or FixedBuffer object.

For existing TA input N=37, Z=9, P=10, query references remain 38/39/40;
objects remain 40, registries/methods 18, blocks 36, portal links 20,
membership capacity 11 and event capacity 128. The source ordering, source
64-selection behavior, original geometry and native query state are unchanged
by design. Compatibility is source-reviewed, not runtime-tested.

## Source admission and ownership

prepare receives the already-loaded Ground plus an ordered Binding array.
Each Binding names an original-owner index and an existing native candidate.
The array must cover every loaded COL owner exactly once. Every candidate
must borrow that owner's actual mesh and bit-equal prepared world matrix and
sphere, retain its original category, and have a unique nonzero native identity.
The helper supplies no geometry, filter values, opaque identities or radii.

The caller must keep Ground, candidate objects, names, filter backing and node
radii alive and at stable addresses for the whole session. Ground's COL decoder
and the caller's radius/query preparation remain the authority for validated
mesh topology and numerical state. This is a storage layer, not asset
authentication, a substitute collision constructor, or other-track admission.

All allocation, filter cloning, sorting, numeric text parsing and preparation
occur before publication. Publication is transactional; on rejection the owner
remains unprepared and all input candidates/ground remain untouched. Once
published, FixedBuffer cannot grow. The helper's hot view/gather methods perform
no allocation; invalid capacities fail explicitly. The caller must not grow
external filter lists, replace descriptors or mutate borrowed geometry.

## Integration outline, not applied

1. Complete the existing original COL/radius preparation and candidate-to-ground
   association. Construct one Binding per candidate using its exact matched
   ground-owner index. Retain current candidate order.
2. Before any query/membership or racer-bank sealing, call Owner.prepare. Use
   the returned Storage as the stable owner; do not move or reconstruct it.
3. Replace corresponding fixed arrays/descriptors with Storage buffers and
   count fields. The header has collectionWorkspace, initialCollection and
   per-membership storage view builders. They bind dimensions, not query
   geometry or source event state.
4. Attach the real main, secondary and terrain query owners to their source
   initial zone, using Storage.lifecycle/world/membershipWork and the proper
   N+1/N+2/N+3 reference. Bind actual filters, type tags, resource and E0 chain
   to collectionInput. The three query slots are intentionally unbound at
   preparation; they cannot silently masquerade as real queries.
5. Replace gatherRegistered with Storage.gatherMovement and pass the returned
   registeredMovement backing/count to the existing movement query.
6. Replace the movement-candidate and world-storage blocks of
   native_racer_owners50hn.inc with registerMutableOwners. Keep the original
   external-candidate lane loop and real vehicle/query registrations. Remove
   the replaced blocks to avoid duplicate overlapping snapshot spans.
7. Every memcpy/loop/capacity argument previously using 37, 9, 18, 36, 40 or
   sizeof a fixed local array must use the corresponding count and data pointer.
   Do not memcpy a FixedBuffer descriptor. Tree-state commits should copy
   count.methods elements. Selection helpers keep the source 64 limit.

This integration is intentionally not inserted into main or the racer owner
include because the session/menu and Clegg work own concurrent changes there.

## Source-reviewed limits and remaining boundaries

- At most 1,024 loaded terrain owners. This is the existing movement
  SearchStorage single-block capacity, not an arbitrary geometry truncation.
  No second-block growth, eviction, dropping owners or spatial repartition is
  invented. A larger world must close the actual SearchList growth boundary.
- At most 256 zones and 4,096 portals, matching current original-world decoder
  limits. Supplied Limits can only lower these ceilings.
- All source collision methods must be authored List. QuadTree or missing
  method authoring is rejected rather than flattened to List.
- Maximum requested backing payload is 32 MiB. The budget accounts fixed
  element arrays and requested cloned/reserved filter elements. It excludes
  allocator overhead, implementation-dependent spare vector capacity, original
  mesh/radius ownership and the separate racer-bank snapshots.
- The current Ground loader still caps its COL owner vector at 64. This new
  storage layer does not remove that upstream boundary. It can consume more
  owners once an independently admitted original loader provides them.
- No map/AI spline, start pose, collision corpus metadata, resource closure,
  sky, textures, track-specific lap-record row or gameplay is added by this
  header. Those remain required before GC admission and gameplay video.
- Lookup/collection calls preserve existing unsupported-source and capacity
  boundaries. Multi-zone duplicate membership still follows the source
  membership adapter, including its UnsupportedLinkRemoval boundary.
- prepare uses a whole-owner coverage requirement. If an integration has
  intentionally excluded original COL owners, it must resolve and document
  that source behavior; this helper will not hide the omission.

## Root-owned checks still required

Within the next authorized test slot: compile/include this header; prepare it
against the actual TA original owner bindings; verify count/ref parity,
registered source order, unchanged candidate bytes, cold budget and no hot
allocations; then check snapshot restore across racers. Exercise rejection for
duplicate/mismatched bindings and incompatible world method/capacity inputs.
Actual other-track execution remains blocked on its original resources and
the other existing TA-specific loaders. No fresh evidence is claimed here.
