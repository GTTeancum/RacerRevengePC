# Positive wreck effect tables and retained model anchors

Source-only continuation on ee15e80; inactive and untested. The existing effect
input stage now reaches actual positive-count allocations and model lookups.
No external child constructor is enabled.

## Connected allocation and lookup

`constructFirstPositiveEffectArrays` requires the actual committed positive174,
both retained non-null context values, stable caller-field bindings, and the
same canonical loaded model. It does not reinterpret a missing field as null.

`OriginalWreckEffectArrays50LA` owns three distinct source tables in source order:

- 2CB634: allocate160, count174 pointer slots for smoke children.
- 2CB644: allocate164, count174 pointer slots for fire children.
- 2CB660: allocate170, count174 model-node pointers. Actual fresh2CB4B0 initialized
  170=NULL; this is the real reached allocation branch, not a replaced table.
- 2CB694: lookup28C590 against D0.model288; 2CB6AC stores each result in170.
- Stop at2CB6CC, before the first external particle child allocation.

Literal3C2670 is exactly `damage` followed by NUL, not `damage%d`. The source
passes the loop index as a formatting argument but the literal consumes none.
Each slot therefore performs the same name lookup. Native traversal uses the
existing canonical model/source-row objects and actual graph links, comparing
names case-insensitively and searching nextSibling188 before firstChild184.
A real name miss stores NULL. Invalid/unrepresented graph ownership stops the
prefix instead of becoming a fake miss. No graph or Entity is cloned.

Only written170 entries are counted. Value-initialized host smoke/fire slots
are not exposed or claimed as source-initialized NULL child pointers. The first
child's real constructor has not written160[0] or164[0]. On native allocation or
lookup failure, completed arrays and anchor writes are retained, without retry.
The native OOM boundary is explicit and does not claim retail malloc behavior.

## Matched retirement

Existing tail-first query/body/model retirement remains unchanged. At the real
piece104970→2CB500 lifetime boundary, the owner frees actual160, then164, then170,
matching2CB534/548/55C, before the existing165BD0/1413E0 completion. Unreached
allocations retain genuine constructor-NULL branches. No pointee is inspected
or released by this array owner. This is safe for a partial allocation prefix
and does not certify any external effect destruction.

The model-node addresses stay in their native occurrence's backing while the
model is retired; no lookup or anchor read is admitted afterward. The table is
then freed by the piece. Numeric count and written-prefix history survive.
Destruction of an owner with live arrays is a hard lifetime failure rather than
silent native cleanup. Existing world/query cancellation gates are preserved.

## Actual context and external forest dependency

Independent source audit now resolves the prior resource-argument uncertainty:
2107B0 saves incoming t2 (the seventh integer argument of210770) into s1;
212A94 forwards it to22BFD0 as t3. It is an incoming racer-setup context, not a
racer-owned field at a guessed offset. Its actual stable argument owner must
supply the existing binding. Particle parent is the actual racer7FC value.

Positive children are:

-160:19A5E0/19A720 from literal3C2680 `podWreckSmoke.emt`.
-164:15DE00/15DEF0 from literal3C2698 `podFire.emt`.
-Both append to the external particle parent through141960(mask0402), at2CB718
 and2CB770. They are not children owned by the piece tables.

Original emitter retirement follows external tree104970 to the actual virtual
slot08 only when references reach zero:

- Smoke19A6C0 calls common15A720 at19A6E8; common frees emitter1C4 at15A754 and
  calls1413E0 at15A760.
- Fire15DE60 frees1C4 at15DEA4 and calls1413E0 at15DEB0.
- Neither destructor contains particle-manager cancellation. Longer-lived
  particles and emitter controls must retain their true manager ownership.

Next required work is external forest lifetime, genuine emitter1C4/control
ownership and constructor failure cleanup, then per-index smoke/fire setup,
stop84, and the always-reached dust/splash suffix. Positive arrays alone do not
close these dependencies or allow activation. Existing count-zero effect owner
is not duplicated or mislabeled as positive-count support.
