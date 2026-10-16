# Full E8 allocation owner to genuine movement producer

Source-only incremental join on fbd6ccd. No activation, build, test or runtime
success is claimed.

## Connection

The existing `queryOriginalWreckMovement50LA` path stages real endpoints,
updates its real world membership, and calls the existing native121920 producer.
Its transaction now imports the full E8's allocated source shape/result state
before that call and publishes completed results back to the same allocations.
The inline q.type3/q.ray records remain transaction mirrors. Legacy one-piece
queries retain their existing inline representation through explicit admission
when no full type3 allocation was attempted.

Only the actual movement function is a friend of the private import/commit join;
there is no new public caller-supplied receipt API. No pointers to transaction
locals are retained. Existing coherent/initialShapesReady and registration gates
remain unchanged, so this does not admit unfinished full constructors to runtime.

## Source fields

| Native producer value | Allocated source owner |
| --- | --- |
| localStart / localDirection XYZ | shape0C / shape10 |
| length | shape14 |
| worldStart / worldDirection | shape18 / shape1C |
| localCenter XYZ / worldCenter / radius | shape20 / shape30 / shape40 |
| result00 / result10 | result00 / result10 |
| selectedNormal / selectedPoint / selectedCorrection | result20 / result30 / result40 |
| ray normal / point | result54 / result58 |
| hitCount / hit / fraction / selectedFraction | result60 / result68 / result6C / result74 |

QueryCenter/queryRadius stay with the existing canonical query60/80 producer.
The join does not confuse hit array68 with unused words64, and does not fabricate
writes to pairs50, words64, tags70, or opaque78/7C.

Original source anchors, independently reviewed from raw SLUS words:

- 11E298..11E2D4 (11DD00 continuation): center/radii, result00/10 sentinel,
  selectedFraction74=1 and hitCount60=0.
- 126FA4: load result68 for per-pair clearing, before inner eligibility/coarse
  rejection. Existing normalModeWritten proves this dispatcher was reached.
- 127058..127088: after inner-sphere admission, reset selectedFraction74,
  fractions6C and hitCount60.
- 127260..127268: load hit68, points58 and normals54 for hit-conditioned world
  transforms.
- 10EC20..10EC58: pair hit, cumulative hitCount and selected result updates.

## Uninitialized and preserved state

Preparation alone does not initialize per-pair hit/fraction words. The owner
therefore tracks actual committed producer preparation, hit writes, and fraction
writes separately. Before the relevant producer executes, it does not import a
fresh host allocation's zero as original source scalar state.

The existing internal dispatcher scratch now reports one additional actual
branch fact, fractionsWritten. It resets at each invocation, becomes true only
at the existing fraction-reset loop, and is accumulated by the real world query
only after a successful dispatch. This changes no arithmetic, candidate order,
filtering or callbacks. In a completed query with no inner-coarse admission,
result6C remains untouched. Disabled queries commit no shape/result writes.

Candidate-local native vectors expose XYZ only. Copies into shape20/0C/10 update
those lanes without claiming the unused original W/register history. Retained
constructor W values are not certified as the runtime original W result. Full
world and result vectors retain their complete native producer lanes.

Normal/point/selected vectors carry forward their actual prior owned values when
no hit changes them. Source result50/64/70 allocations remain owned and retired
by the existing11B600 path, but this producer does not supply values for them.

## Lifetime and remaining gates

No new allocation or borrow is created. The same type3 owner still owns and
retires its arrays through11B4A0/11B600. The completed producer history is retained
through retirement; access continues to reject retired storage. World-query
failures do not publish staged arrays; existing endpoint/registration prefix
behavior remains unchanged.

Full wreck activation still requires actual effect construction and retirement,
remaining collection append/tuning/group phases, and genuine registration and
runtime ownership completion. This patch does not set initialShapesReady or
provide an active driver. Positive emitter160/164/170 ownership remains separate.
