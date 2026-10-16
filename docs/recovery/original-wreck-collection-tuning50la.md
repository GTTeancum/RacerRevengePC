# Ordered original wreck collection tuning and contact-body prerequisite

Increment against ff39ce9. Source-only work: no builds, tests, syntax checks,
sanitizers, gameplay, project decoder execution, or guest execution.

## Implemented

`original_wreck_collection_tuning50la.h` exposes the first genuine22C310 pass
for one complete retained collection. The caller supplies its actual stable
piece/body/Entity bindings in source order, together with the retained full
inventory and the already primary-bound shared group. It rejects duplicate
piece/body/Entity bindings, wrong count/session/occurrence, missing constructors,
and already-registered groups before numeric work. Actual body-model binding
retains its graph nodes/count as native ownership identity. The tuner compares
that identity with each retained occurrence, so equal graph-local model indices
cannot accidentally admit permuted pieces. This is a native owner borrow, not
a new source field or an additional mutable transform authority.

For each original PartN, ascending:

1. Use that occurrence's own retained model bounds and authored weight/inertia,
   with its collection's original spring/friction/drag parameters.
2. Apply the existing166400 numeric routine and22C53C..22C574 overrides.
3. Call2CA190 with the occurrence's actual BreakRate/BreakScale. Primary piece0
   is skipped by pointer equality; later pieces fill the original ordered slots.
4. After the entire loop, copy primary mass26C to saved group4C and the complete
   primary inertia100 matrix to group60. Saved center50 is deliberately unchanged.

The existing single-piece API retains its one-piece/group admission and calls
exactly the same extracted numeric body, then saves mass/inertia as before.
The new shared routine uses `selectedPiece()`, not collection.pieces[0], so each
full inventory occurrence consumes its actual weight/inertia. Numeric operation
order and finite/FE_TONEAREST admission are unchanged. Bounds remain cold-model
bounds; callers must not substitute an animated/live geometry state.

Failure reports the exact current ordinal, completed prefix, whether the current
piece tuning committed, and the source phase. Earlier source writes survive;
a caller must stop rather than replay a partial pass. This is an initial tuning
join, not a general runtime retuning/rollback service.

`OriginalWreckGroupArrays50LA::bindMemberContactBody295C80` implements the exact
one-store leaf for an actual registered member. Destination is explicitly the
same group's primary D0 or the member's own D0. It validates both typed owners,
constructor/session bindings, and current DC.30 membership in that same pair.
It changes ONLY child.contactBody30. No caller is added from tuning, activation,
or ordinary racing. It does not modify A0, E0.A0, records, coefficients, model
scenes, caches, masks, body enabled state, or query exclusions. It cannot certify
completion of2CA480/2CA2F0 or admit response on a shared body.

The racer-storage header includes this new header, making it reachable by the
parent-owned native driver full build without activating a runtime constructor.

## Source proof

Original SLUS_202.68 SHA256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Static word annotations retained in the source-audit directory as
`tuning22C310.txt`, `group2CA020-2CA190.txt`, and
`group-construction-retirement.txt`.

-22C33C..22C400 reads actual collection tuning values.
-22C410..22C4D4 formats ascending PartN names and reads occurrence-specific
 weight/inertia; BreakRate/BreakScale missing-value defaults are each1.
-22C4D8 supplies zero restitution to166400, with water constants1,200,.5.
-22C504..22C538 invokes each actual piece slot74 (166400).
-22C53C..22C560 writes piece110/114 from authored collection restitution and
 friction torque scale;22C564..22C574 clears DC.B4.
-22C578..22C590 calls2CA190 with that same piece and its break values.
-22C594..22C5A8 advances and compares against actual collection30 count.
-22C5B0..22C5C0 copies mass;22C5C4..22C5F0 copies eight pairs of words,
 i.e. the entire64-byte inertia matrix. There is no saved-center write here.
-295C80/295C84 is exactly the contact object's DC.30 pointer replacement.

## Remaining gates

The cold storage still exposes const pieces/group and does not pretend their
source constructors have run. A full runtime constructor must own the real
collection/piece/Entity/query/effect graph and then call this tuning pass.
Actual2CC060 effects and their retirement remain separate dependencies.

Shared contact response cannot be enabled by changing one guard:
-296518..296598 computes child DC110 from the bound primary D0's model288 and
 center1E0.
-296884..296910 uses each generated record's nonnull scene40 for its point;
 only a zero scene40 falls back to the bound D0.model288.
-296F10 uses the bound primary D0's scene, angular velocityB0 and linear70,
 while the resulting velocity cache is written into the child's DC120.
Thus future response needs independently authenticated primary body/model and
per-record scene ownership. The current own-body response/flight rejection is
preserved; no second authoritative body pointer is stored.

Group activation must still preserve disable/reset, DC binding, ordered E0-only
exclusions (including self), and2CA620's actual aggregate recomputation. Full
published query/effect/reference retirement is required before death or KO.
