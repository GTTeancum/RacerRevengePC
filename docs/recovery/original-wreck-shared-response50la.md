# Original shared wreck contact-response split

Increment against7196be9, source-only. No build, test, syntax check, sanitizer,
project decoder, gameplay, or guest execution was run.

## Implemented boundary

The existing2964C0 static-generated-record implementation is now one internal
kernel with separate contact and body owners. The legacy public wrapper retains
its exact own-body admission, prepared-owner checks, real producer scene key,
point/dynamic-body guards, and numeric operation order.

The new `advanceOriginalWreckGroupContact2964C0_50LA` resolves the child from
an actual group member slot. DC.30 selects only that member's own D0 or the
same group's primary D0. An unrelated body is rejected. For positive generated
count, the target and child model graph identities must each resolve uniquely
to actual retained occurrences of the supplied collection. There is no copy of
body state and no additional authoritative contact-body pointer.

Child DC retains and receives:
- Existing point/generated record backing, flags and counts.
- Contact coefficients, material friction, response switches and custom center.
- World-center cache110 and velocity cache120, including their source W=1 lanes.

Bound D0 supplies and receives:
- Mass/inertia/inverses and ordinary center1E0.
- Actual model288 and its live graph transforms.
- Linear/angular velocities, loads, accumulators and corrections.

Positive records remain serial. The child velocity cache is prepared once before
impact and reused for that record's secondary response, spring and friction.
Only bound-body graph helpers refresh scenes; the child's own D0 remains intact
when the target is the primary. No record is synthesized or discarded.

The no-generated-record route clears pending38 and sets the same sentinel globals
without reading inventory, graphs or scene IDs. A source-prefix failure retains
earlier writes and must stop the caller; it is not permission to replay or run
the integration suffix. Native span checks keep physical bodies, graph backing,
application zero-vector and sentinel storage disjoint.

## Explicit nonzero scene40 boundary

The kernel has a per-record frame resolver. Legacy records resolve through their
existing actual owner/model identity. The new full-group wrapper currently admits
only the original ZERO scene40 fallback to the bound D0 model.

A nonzero full-group record stops at reached296884 with `sceneBinding`, after
whatever original center/sentinel prefix writes already occurred. It is NOT
reinterpreted as a member ordinal, body identity, or primary scene. Therefore
this patch does not claim normal nonzero-query contacts are fully joined yet.
The full cold owner lacks the actual model/query identity publication needed
for that route. Adding a response-only map would fabricate that missing link.

## Source provenance

Authenticated SLUS_202.68 SHA256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

-16606C..166078 passes piece.model14C into11BBC0.
-11BBD8 ->11B920 ->124F80 ->1253F0 retains that model in E0.shape44.
-294E40 loads that SAME shape44;294E58 calls2963E0 to append record.scene40.
-296518..296598 uses bound D0.model288 and either child DC.customCenter100 or
 bound-body center1E0; stores the result in child DC110.
-296884..2968C8 uses a nonnull record.scene40.world100 directly.
-2968CC..29690C uses DC.30 -> D0.model288 only for a null record.scene40.
 Neither point-transform branch refreshes the scene first.
-296FAC..296FB8 applies28BBB0 to the bound-body model;296FCC..296FF8 uses
 bound-body angular velocityB0 and28B9B0 on that same model.
-297004..297038 adds bound-body linear velocity70;29713C..297178 stores
 relative velocity in the child's DC120.
-298438..298470 secondary response writes bound-body linear70.
-297318..297324 and29759C..2975A8 pass bound D0 to force/friction while
 retaining the child's contact settings.
-297CA4 and297DB0 impact continuations modify bound D0 accumulators/velocities.
-295604/295610..295614 bind E0.9C to child DC and E0.A0 to its then-own D0.
 Later295C84 changes only child DC.30. This patch does not rebind E0.A0.

## Next genuine integration dependency

Construct a typed per-piece model/query identity domain in the actual full
constructor. Bind the model once at the real11BBC0/1253F0 shape publication,
and use that SAME stable identity in both294BB0/2963E0 generation and response
resolution. It must authenticate ID, retained graph occurrence and node, reject
duplicates/stale sessions, and preserve every owner's retirement/borrow gates.
The current full cold piece has no such published query owner or identity.

Until that exists, do not enable ordinary full-group contact generation or
claim the nonzero path complete. Point contacts and reached other-body response
remain explicit existing boundaries as well. Actual2CC060 effects, group
activation/exclusions/2CA620 recomputation, and published query/effect/reference
retirement remain prerequisites for destructive gameplay/death/KO activation.

The new header is included through racer storage for the next parent-owned
full native driver build. No active driver caller or gameplay guard is changed.
