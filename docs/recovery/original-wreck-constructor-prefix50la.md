# Full inventory: first genuine constructor prefix

Source-only increment against0ddcde4. No build, test, syntax check, decoder,
sanitizer or gameplay execution was performed.

## Reached implementation

`OriginalWreckRacerStorage50LA::constructFirstOrderedPrefix` now runs the actual
first collection/piece component constructor sequence over the already retained
full inventory. It consumes real application component identities, the original
423F60 counter and423F58 lock, body constructor globals,4431C0 gravity vector,
SoulTree category and actual water field. Constructor vectors are borrowed from
the exact inventory loading binding rather than recreated by the caller.

It constructs the collection22BF00 component and writes actual authored count30,
then the first piece2CB4B0 fields, original model/PSG component rows, own D0 body
and model binding, and D4 gravity. Piece child18 links are appended in exact
model(mask0), body(maskFFFFFFFF), gravity(mask0) order. PSG hierarchy rows are
never appended as component children. Piece authored50 comes from the actual
selected occurrence. The body model graph is that same occurrence's retained
mutable graph, satisfying the per-piece query receiver's identity checks.

The source phase stops at166018 before E0 allocation/construction (11B190 at
16602C), hence also before the later11BBC0 call at166074. This
is reported as `publishedLifetimeRequired`, never constructed/active. Body
constructor defaults and original component flags are preserved. No invented
inactive writes are used to make the partial owner appear complete.

The first piece is still not appended to the collection. Its setup has not
returned to22BFD0, so the second piece and every later collection remain
unreached. The full prefilled ordered38/374 arrays remain native planning storage. The
first source collection38 slot has nevertheless been stored at22C16C before
setup; reachedSourcePieceSlotCount records that separate extent of one.
The API has no ordinal selector that could skip this unresolved first suffix.

All component keys are checked before source writes. Existing64-component
kernel capacity is an explicit bound on the first reached component domain.
A reached failure is terminal and retains every already constructed component,
actual counter increment, body/cache write and child link. Repeated calls report
stopped with the retained PC/count. Allocated scene byte backing precedes source
construction. No active driver calls this API.

## Honest lifetime boundary

The ordered storage destructor now rejects any unreleased source reference in
its reached prefix, matching OriginalRacerMainBodyOwner50LA and
OriginalRacerGravityOwner50LA. Cold allocation remains freely destructible.
Neither query cold cancellation nor native memory destruction certifies source
104970/reference-zero/slot08 retirement. Therefore this constructor API remains
inactive until a real prefix retirement join exists. It cannot be invoked as a
throwaway diagnostic and then silently discard source ownership.

The pre-publication boundary is necessary because later canonical query rows
borrow retained memory: E4 geometry.mesh borrows the asset mesh; E0/E4 exclusion
lists and E8 categories borrow query-owned arrays. Current query destruction
only revokes the scene-domain token. That token alone does not invalidate all
canonical row borrows. `releaseUnpublishedQueries` rightly rejects publication.
This patch does not invent a published-row cleanup or substitute native cleanup
for11B4A0.

## Exact next source dependencies

After genuine lifetime ownership is joined, continue:

1. E0 allocation166018 and11B190 construction16602C precede pointer store166034.
 11BBC0/1253F0 at166074 publishes E0 using the actual model scene ID;
  then append E0 at166098.
2. DC295A40 at1660B4, own-D0 binding at1660D0, and POINT296190 at1660E4.
  POINT uses the already published E0 model identity, source child-first DFS,
  exact duplicate test and50-point saturation.
3. D8 at166180 and2955F0 at1661C0, then E4/E8 construction/append, mutual
  exclusions and derived2CB420. E4 keeps its own D0; E0.9C becomes actual DC.
4. Actual piece effects at2CB618; only after setup returns can22C1B8 append the
  piece and advance the original loop. Group effects2CC060,22CE10 inactivity,
 22C310 tuning and22C610 gravity follow in their source order.

Retirement anchors already recovered:22CE10 calls query120290 in E0/E4/E8
order;120290 unregisters through281490/280920 and clears registrationE0 but does
not destroy shape storage.11B4A0 separately handles remaining registration,
filters and11B600 shape/results (type2 shape cleanup11B7E4; results8C/90 free and
clear11B8E0..11B904), then1413E0. Entity142590/1424E0 releases children tail-first
through104970 and calls slot08 only when the actual reference reaches zero.
The native join must preserve these separate ownership transitions and ensure
canonical borrows are unreachable only after genuine retirement or an actual
terminal-world lifetime boundary. No activation/death/KO gate is relaxed.

In particular E4 retirement cannot simply delete its numerical shape projection:
11B648 reaches manager3E1A70 cache removal1327D0 using shape124, then shared
shape release11AEE0. E8 shape3 frees its seven shape arrays before the common
result-array sequence; E0 shape2 frees4C/50. See
`original-wreck-query-cold-retirement50la.md` for the exact retained addresses.
The resource-cache/reference owner remains part of the published retirement gap.
