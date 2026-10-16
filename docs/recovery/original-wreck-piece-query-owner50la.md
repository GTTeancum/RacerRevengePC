# Genuine per-piece query and callback ownership

Increment against95a8e53. Static source implementation/review only: no builds,
tests, syntax checks, sanitizer, project decoder execution, gameplay or guest
execution.

## Ownership join

`OriginalWreckPieceQueryOwner50LA` is one stable typed receiver for one actual
piece occurrence. It borrows that piece's original assets, canonical physical
owner, piece fields and exact piece component storage. Cold binding creates no
source Entity or callback completion. Later coherence requires the real piece
component kind/reference/key/session and reached body-model graph binding.
Bindings cannot be replaced or moved to another occurrence. Only the actual
one-piece and full racer allocating owners may bind these canonical borrows;
external callers cannot combine fields from different occurrences.

`OriginalWreckQueries50LA` no longer depends on `OriginalOnePieceWreckOwner50LA`.
Its cold preparation derives assets, physical owner and session from the real
per-piece receiver instead of accepting independent possibly mismatched owners.
All three initial9C fields refer to that same receiver. The reached2955F0 phase
still replaces ONLY E0.9C with child DC and installs294BB0; E0.A0 remains its
then-own D0. E4 and E8 retain the per-piece receiver; E8.A0 stays null.

The existing one-piece owner embeds one receiver and binds it to its genuine
components[1]/assets/physical/piece. Its existing constructor sequence is unchanged.
The complete ordered racer storage also embeds one receiver and one query owner
per actual retained occurrence. The cold inventory transaction binds only the
receiver, with actual component0, fields and assets. Query/world preparation and
source construction remain uncalled; there is no group-per-piece substitution.

Full-piece member declaration order destroys queries before receiver/component
storage. Existing legacy query-domain teardown remains token-only, preserving
its component destruction safety. Full cold inspection remains const-correct;
no mutable query/body or constructor-completion view is exposed by that owner.

## Callback ownership

`respondFromQuery2CB420` is a bounded adapter on the actual target E4 query owner.
It accepts an actual querying wreck E0/E4/E8 owner/index in the same canonical
world/object table and session, with both query bindings coherent. Its other
body is that querying object's real A0, including genuine null for E8.

The receiver's body is E4.A0, its OWN D0. It must not redirect to the shared
primary named by child DC.30. Restitution is read from the receiver piece's own
DC.C0. The adapter calls the existing original2CB420 numeric leaf with those
canonical fields. It never supplies a fabricated null other body or changes
wake30/enabled204 to reach a supported branch.

A nonzero piece.wake30 still returns the exact165948 bodyEnableRequired boundary,
even when other A0 is null. Source body activation/linked propagation and piece
virtual20 remain required. Consumed self-body aliasing retains an explicit native
binding boundary rather than violating the existing scalar leaf's disjoint-input
contract. Other body families require their genuine typed A0 resolver; they are
not coerced into wreck query owners.

The adapter does not invoke querying-object E4 or report whole collector dispatch
completion. The real dispatcher must first run that callback, then invoke the
target object's E8, and preserve any prefix effects/failure. No driver invokes
this new adapter yet. It is a callback ownership join, not active collision flow.

E8 movement admission now also requires its caller's piece component to equal
the canonical9C receiver component. The previous loose kind/session check alone
could not establish that source receiver identity.

## Source anchors

Authenticated SLUS_202.68 SHA256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

-165C30/165E90 constructs/appends model, D0, gravity, E0, DC, D8, E4 and E8
 in that order.
 The existing split query publication/attachment API and constructor call order
 are preserved; E0 scene identity still publishes before POINT construction.
-295604 overwrites E0.9C with DC;295610..295614 stores its then-own D0 into A0.
-2CB5A0 installs the derived E4 response2CB420 only after the base165C30 route;
 before that, the existing165920 callback tag remains distinct.
-121CF8..121D0C copies hit data to candidate;121D10..121D24 runs the querying
 object's E4 first;121D28..121D3C invokes candidate.E8 with candidate as receiver
 and querying collision object as other.122878..122888 has the same reversal.
-165930..165948 tests actual piece30 before other-body null can bypass work.
-2CB43C reads receiver E4.A0.2CB458/2CB468 obtains restitution from
 E4.9C -> piece.DC+C0, not from DC.30's bound body.
-2CB464..2CB498 writes (1+DC.C0)*otherBody70 XYZ and W=1 to receiver70 on the
 admitted no-wake/disabled-body path. The existing numeric leaf is unchanged.

## Remaining gates

A full constructor must now prepare each real query owner with actual shared
world slots, planned model/E0/E4/E8 component addresses and shared model domain,
then run the original ordered source constructors/append operations. That join
can use the new receiver directly; it no longer needs fake one-piece collections.

Full294BB0 world dispatch still has a legacy owner adapter and needs the same
per-piece receiver/detector inputs. The general candidate-E8 dispatcher must
resolve non-wreck other A0 owners and complete wake/linked-body continuations.
Actual2CC060 effects, group activation/exclusions/2CA620 recomputation, and
published query/effect/reference retirement remain required before death/KO.
No existing activation or retirement guard is weakened.
