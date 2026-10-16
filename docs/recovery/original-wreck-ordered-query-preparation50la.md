# Ordered wreck query/world preparation

Source-only increment against6c7a5cd. No builds, tests, syntax checks,
project-decoder execution or gameplay were performed.

## New concrete join

`OriginalWreckRacerStorage50LA::prepareOrderedQueriesCold` prepares every
retained piece occurrence in collection-major/piece-major order. The caller
supplies one actual canonical world binding and one E0/E4/E8 slot/identity
triple per piece. There is no copied world or alternate component forest.

The common binding must have empty per-piece refs/identities, no model domain,
and the default group capacity. Each real collection supplies its own authored
piece count for E0 exclusions. All refs and identities are checked for global
uniqueness before the first per-piece preparation. The existing per-piece
preparer additionally validates canonical world registrations, identity conflicts,
resource shapes and static type2 capacity. Nested worldQuery/mutableQuery/
queryWorkspace contents remain runtime admission gates.

The racer storage owns one model scene domain with capacity equal to the total
actual piece count. The existing512-entry limit is an honest admission boundary;
no ID truncation or fabricated model scene key is used. The domain is declared
before collections, so all querying borrowers are destroyed before it. Actual
model IDs remain inputs of the later11BBC0/1253F0 publication phase.

Canonical addresses are derived internally from each retained occurrence:

- piece component0
- model/PSG components1 through sceneCount
- D0 at1+sceneCount
- D4,E0,DC,D8,E4,E8 at the next six positions

The per-piece query owner receives actual model component1 and query components
D0+2, D0+5 and D0+6. Its privately bound9C receiver continues to name that same
piece's fields, physical state and assets. No one-piece group owner is created.

## Failure and lifetime

Preparation is one-shot once domain allocation is attempted. A reached failure
reports collection/piece ordinals, the original query result/source PC, and the
number of successfully prepared predecessors. Earlier prepared owners stay
retained. The existing28B550 graph refresh and shape-staging prefix can also
survive failure; this operation does not promise rollback or zero model-cache
writes. It does not publish canonical query rows or run source constructors.

`releaseUnpublishedQueries` preflights the entire prepared prefix with the
existing genuine unpublished-world guard, then drops those native borrows and
closes the now-unpublished domain. Any published query, reached callback binding,
registration or other existing guard rejects release. This is terminal native
cold cancellation and does not stand in for source11B4A0, unregister, effects
retirement, reference release, or world terminal-lifetime authorization.

## Source sequence and next boundary

Existing authenticated source provenance remains in
`original-wreck-piece-query-owner50la.md` and
`original-wreck-scene-domain50la.md`.

The actual165C30/165E90 sequence remains piece/model, D0, D4, E0, DC/POINT,
D8 with2955F0, E4, E8, mutual exclusions, and derived2CB420. In particular:

- model publication11BBC0/1253F0 is reached at16606C..166078;
- E0 append is166098;
- DC constructor1660B4 and own-D0 binding1660D0 precede POINT1660E4;
- D8 constructor166180 and2955F0 at1661C0 follow DC;
- E4 and E8 keep the real piece9C, while E0 changes to DC;
- PSG hierarchy rows remain separate from Entity child18 links.

This patch joins full ordered world preparation only. It deliberately does not
label that outcome source construction. The next full constructor must execute
those component/physical/POINT phases in order and retain their reached prefixes.
Piece effects2CB618 onward must use their actual authored context, followed by
the genuine per-piece collection append, group2CC060 construction/effects,
22CE10 inactive sequence,22C310 tuning and22C610 gravity. The ordered owner must
gain actual component/global identities,
visibility, effect ownership and published-query/reference retirement before
activation or destructive gameplay can be admitted. No death/KO gate changes.
