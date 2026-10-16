# Full first-piece DC, POINT and D8 constructor/lifetime

Source-only incremental implementation against b542221. Static inspection of
native source and authentic SLUS raw words only. No build, test, syntax check,
decoder execution, gameplay or guest execution was performed.

## Actual source phases

constructFirstContactSuffix takes actual initialized443550/4435A0 vector fields
and the address of the actual terrain330 pointer field. A missing field is
rejected; a genuinely null terrain value follows2953D8 and disables DC48.
It operates on the same canonical piece, DC/D8 Entity slots, D0 physical owner,
E0 source-owned type2 geometry and model-domain identity already constructed.

-1660B4 constructs DC295A40. The helper is now split: actual defaults have
  capacity88/counts7C/84 and body30 zero, mode34=0,4C=1,64=0,CC=0x3d4ccccd.
  The existing legacy combined helper still produces its previous final state.
-1660C4 invokes295C20(50), storing capacity first and allocating POINT80 before
  generated8C. Both arrays have50 records of source stride0x60. The reached
  literal141C20 identity return has no state changes to implement.
-1660D0 invokes295C80, borrowing the same D0 body. No reference increment.
-1660E4 invokes296190 with actual E0, argument1 and canonical model14C identity.
  The authentic type2 tree is visited child0 before child1, and each leaf's
  SECOND endpoint is considered. Saturation at50 precedes duplicate lookup.
  Duplicates compare XYZ plus scene identity, not W. A new record writes full
  localPoint10, scene40, flags50 bit0, margin48, normal20/surfacePoint30 and7C;
  all other record bytes remain outside this source write's validity claim.
-166100..16615C overrides mode34,54,4C,64,50,AC/B0/B4/C4,CC,D0 and98 only after
  POINT generation, then166164 appends DC with mask0.
-166180 constructs D8.2953A0 at1661A8 receives actual DC/terrain with a3/t0=0,
  so no private query or pairs60 is allocated.1661B8 sets enabled64=1.
-1661C0 invokes2955F0: D8.38 borrows actual E0; E0.9C borrows actual DC,
  A0 borrows DC.30, E4 names294BB0;123EA0 stores1 to filterBC. These are typed
  borrows and the real callback enum, not a completed callback invocation.
-1661D0 appends D8 with mask0. The cursor stops at1661D8 before E4 construction.

The original child order now reaches model,D0,D4,E0,DC,D8. The complete-piece
query-ready flag remains false, so registration, active callbacks, frame update
and response still cannot consume this unfinished full constructor. An explicit
component-liveness check also rejects a retired full DC/D8 callback receiver.

## Record ownership and partial failures

OriginalWreckRecordArray50LA preserves observed legacy data/size/index access.
Legacy one-piece callers retain embedded backing. The full route selects its
exclusive source owner before DC construction, initially with genuine null
allocations, and performs the two independent allocations at295C20. Only one
backing is accessible at a time; there is no second mutable record authority.
Fresh host bytes remain a native initialization policy, never fabricated hits.
The DFS stack is cold native workspace sized from the retained resource, not a
source POINT allocation or runtime guest allocator reproduction.

A failure of either allocation or later bounded POINT traversal keeps its
reached source count/record/allocation prefix. Unlike the earlier type2 failure
boundary, these partial allocations can retire:295A40 genuinely initialized
both pointers to null, and295B90 conditionally frees80 then8C. Destruction of
an owned unreleased native allocation is guarded rather than substituted for
source retirement. An unreached D8 is never fabricated merely to close cleanup.

## Retirement

retireFirstContactSuffix proves actual leaf references and sibling order before
performing tail-first1424E0/142590/104970. A reached D8 retires first through
295320: the actual source60 pointer is null, so no pair-array free is invented.
Then DC295B90 frees POINT80 before generated8C. Each actual Entity reference
and application object count decrements once. Borrowed DC/E0/D0 are not retained
or released merely because their pointers occur in another component.

D8/DC destructors do NOT clear borrowed fields or E0 callback words. Their stale
source links, DC counts and body borrow remain historical. Native allocation
owners reset on actual free, and native detector liveness ends, without claiming
source pointer-clear stores. E0 retirement is blocked until this tail finishes;
it admits the now-retired actual DC/D8, destroys its shape/results, and only then
ends its native callback/model-domain borrows. Existing D4/D0/model/PSG/material/
cache/piece/collection retirement follows, with exact additional release counts.
Only explicitly retired DC/D8/E0 are removed from live Entity-domain validation.

## Remaining joins

Next is authentic E4 type0 construction/resource-cache ownership and matching
11AEE0/11B600 retirement, followed by E8, effect children, tuning, group/query
registration, full source result-array projection and active-world retirement.
The API remains inactive in the driver. This patch creates no synthetic contact,
terrain provider, callback success, death completion or KO credit.
