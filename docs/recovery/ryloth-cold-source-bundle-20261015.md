# RA/Brightlands isolated cold-source bundle

Candidate only; no central edits, execution, tests, syntax checks, conversion runs,
generated assets, Windows access or course admission. Prepared against95ffd70.

## Concrete API

`ra_cold_sources50LA::Owner::decode(originalArchiveBytes, actualSharedLibm,
actualCollisionRegistryMasks, diagnostic)` is one-shot. It uses actual current
native resource/world/collision/navigation/AI/TUN readers, retaining real decoded
storage. `sourcesDecoded()` returns a const borrow only after all represented
parsers succeed. That name means decoded source data only; it is never a session,
renderer, course or gameplay capability. Owner is noncopyable/nonmovable.

The caller must read the original RA archive through its genuine resource/root
policy. There is no input pathname or default asset path in this bundle and no
fallback to another course. It authenticates the entire original4604561-byte
archive and four unchanged members by length, offset and SHA256. Every selected
COL is authenticated transitively by that exact archive and unique member table;
no generated collision hash or visual output hash is invented.

## Owned represented work

- Exact1010-member/29687808-decoded-byte archive; distinct case-insensitive names.
- Exact SCB10zones/10portals/1261placements and source List collision methods.
- Existing fresh-game height projection, rejecting any unexpected present provider.
  Authored terrain remainsnat13.trn; source fresh270550/1A9EE0 omits terrain
  construction, and absent WaterFile genuinely inheritsnone. No terrain data is
  fabricated or relabeled.
-178 source-order placed collision owners. Each decodes actual COL, builds the
  existing native in-memory collision representation and original numeric placement.
  WorldInstance.zone is originalONE-BASED; the checked per-zone array subtracts1.
  Stable reserved storage survives until owner destruction; no deduplication or
  sorting, geometry repairs, collider registry identities or body/query objects.
- Exact176 collision-only placements; none missing both PSG andCOL; category
  counts109static-only/69static+floor use actual supplied registry masks.
- Actual M/A gates with115/113nodes and ordered node0 predecessors103/101,
  plus both AI metadata owners. Caller supplies the actual sharedSourceLibm49T.
- AI environment impact, dynamic impact, final impact and AI packing/driving
  tuning from actual RA.TUN. Existing source fresh selection produces descriptor7,
  order4,classA,recordpod9, independently of any profile or selected racer.

## Failure/lifetime contract

Every failure is terminal for this owner and retains partial storage privately;
no source-data pointer is published. Diagnostic includes failed stage, placement,
actual parser diagnostics, and whether each navigation gate parser committed.
A successful M parse may update actual shared libm before a later A/TUN failure;
this bundle never restores or substitutes that owner. Do not retry a failed owner
as a fresh initialization. Exceptions are caught into a terminal failure.

Returned const borrows must not outlive this owner. Collision mesh/name backing
must stay unchanged while future candidates borrow them. No destructor registers,
removes or repairs scene objects; only privately owned portal backing exists.

## Deliberately pending actual-owner joins

This is a source-decoding candidate, NOT the complete RA cold/runtime owner.
There is noSessionAssets row or host/Course enum/capability. No renderer inputs,
POD/PHY defaults, grid pose, lap cursor, racer list, human/AI mutation or fabricated
native registry identity is supplied.

The candidate now owns the real same-SCB portal topology: 10 heads, 20
source-ordered directional links, 10 portal geometries and 10 parsed inputs.
All four vertices are reparsed from the exact SCB labelled fields through
parseTrackNumber50AD, following track_runtime_storage50IG's existing path.
prepareLiveZoneWorld50EN preserves ascending original portal visitation and
head-prepending links (272B3C..272C70/27F3B0). Original one-based zone ordinals
are topology references; no collider or Entity identity is fabricated.

Both M/A owners now bind indices against this exact owned LiveZoneWorld via
bindTrackNavigationZones50EU (1ACF70). The M parser's actual scratch is consumed
by prepareLapProgressTrack50GF before the A parser overwrites it. This preserves
ProgressNode-presence markers (2A1980..19C0) and the real predecessor, without
inventing per-racer lap state, timers or crossing events. Native parser acceptance
and portal numeric traversal remain untested.

DecodedSources is explicitly noncopyable/nonmovable: its world pointers cannot
be copied into a detached backing lifetime. The outer Owner also remains
noncopyable/nonmovable. No const_cast or caller-supplied unrelated world is used.
Root's eventual live membership/registry join must reuse this owned topology
through a narrow exclusive lifecycle adapter. Do not construct a second portal
world and pretend it is the already-bound owner. Live membership can mutate
portal-link distance caches, so the future runtime adapter needs explicit serial
mutable ownership; the current read-only source view does not grant that join.

Static candidate radii/filter/registration preparation can reuse
track_static_candidates50IG::prepare once its input is factored to the actual
stable collision-owner span. This candidate does NOT fabricate a ColdScene with
zero/default pod lift and pose merely to fit that older adapter. Root must supply
actual Application type/category registries, native identities, SourceGrid,
selected authentic podPHY lifts, genuine query owners and all lifetime storage.

Other RA visual/ATM/sky/light/audio/trigger/particle owners remain separate open
requirements. Source closure cannot waive them. All code and parser acceptance
remain unbuilt/untested until root's authorized centralized validation.

## Evidence

`docs/recovery/ryloth-cold-source-metadata-20261014.json` is the exact member,
source-count and navigation-profile authority. Runtime binding/source closure
notes provide original fresh height, collision-zone and metadata provenance.
Actual API contracts were read inimport_decode48c.h,import_world48f.h,
track_collider50ao.h,track_navigation50eu.h,ai_authoring50iu.h,
impact_authoring49y.h andtrack_height_cold50jb_fixture.h. Current central
SessionAssets and admission remain unchanged.

## Follow-on source review

The topology input/storage/output spans are distinct members of the heap-owned
nonmovable DecodedSources. prepareLiveZoneWorld50EN preflights the topology and
geometry before writing these spans. bindTrackNavigationZones50EU stages each
index array before publication; a later A failure leaves an unpublished private
M prefix. Diagnostic stages identify mapZones, mapLap and aiZones separately.
No broad registry, parent hierarchy or scene-readiness claim was added.

## Final manual API/lifetime review

Re-read against current95ffd70 parsers and storage contracts. The original SCB
portal records are enumerated1..10, use one-based connect1/connect2 zone values,
and bind parameter_block to their actual labelled source fields. World50EN's
output, storage arrays and parsed input spans are disjoint, satisfying its Guard
contract. No temporary vector or local storage address escapes: its view points
only into the fixed arrays inside the heap-owned nonmovable DecodedSources.
Collision vectors reserve their complete measured178-owner extent before any
construction; all future mesh/name borrows require this owner's lifetime.

Navigation parser scratch and output are separate. M gate parse commits first,
then genuine M metadata, same-world index binding and scratch lap projection;
only then does A parse clear/reuse scratch. Lap progress uses actual fieldMask
presence bits and checks the actual115-node/103-predecessor projection, without
inventing progress-marker count or values. Partial failure never publishes the
private bundle, and never rolls back the actual shared libm owner.

Correction: fixed diagnostic failure reasons now use a const char pointer, so
reporting an allocation exception does not allocate another string. Existing
parser detail remains in diagnostic.message and nested diagnostics. A second
call is rejected before touching source storage or libm; if the first call
succeeded, that rejection does not revoke or mutate the completed source view.

The returned native view is contractually read-only, not a deep-const security
capability: LiveZoneWorld50EN itself contains mutable typed pointers. No caller
may use those pointers to mutate backing through this source-view API. Root's
future live membership adapter must explicitly acquire exclusive serial mutable
ownership; no runtime registration or admission is certified here.

No compilation, syntax checking, parser execution or data conversion was used
in this review. Central95ffd70 remained untouched.
