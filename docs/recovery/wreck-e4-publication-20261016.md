# E4 initialization, canonical geometry and retirement

Incremental source-only patch against8d6c85b. No builds, tests, syntax checks,
project decoder execution, guest execution, or gameplay.

## Constructor continuation

`preparePublishAppendFirstE4()` continues the existing first piece at1662A0:

1.1662A8 sets responseE8=165920;1662B0 binds the actual piece9C;
  1662BC binds that piece's real D0 bodyA0.
2.1662C0 calls11F570. Its type0 branch loads model110 at11F5B0,
  calls28B550(model,0,out) at11F5B4, then11ED60 at11F5C0.
3.11EDB4..11EE04 initializes BOTH previousC0 and world80 from this actual
  refreshed model matrix.11ED60 computes preliminary shape values, then11EF24
  calls11F680 with the SAME matrix. Final histories are therefore both known
  model world, and final bounds/composition have ordinary11F680 semantics.
4.The native continuation derives local40 and root bounds from the SAME live
  shared resource-owned mesh, initializes both histories, and reuses the existing
  bounded numeric preparation. It then publishes the existing reserved canonical
  E4 row with its actual identity, flags, bounds and resource-owned mesh pointer.
  No second graph, model Entity, or model-domain identity is minted.
5.1662DC appends E4 to the actual piece after D8, source mask0. Success reports
  `e8ConstructorRequired` at1662E4, before the next allocation.

The two matrix assignments are genuine11ED60 initialization, not an invented
history reset. Later ordinary model preparation and bound views select the
resource-derived source for the full route; the legacy path retains its existing
source selection. Full-route coherence additionally checks the live resource
and exact canonical geometry pointer. The public activation gate remains closed.

## Failure and retirement

The constructor's callback stores and completed graph/history prefix survive a
later numeric failure. Canonical geometry is published only on successful numeric
preparation. An append failure preserves this completed publication. No failed
constructor call is replayed.

`retireFirstPreparedE4()` accepts the actual appended tail or a proven unlinked
pre-append failure. It requires no E4 zone membership, registration, live entry,
mutual/group/filter continuation, E8 publication, or activation. Before callbacks
were reached, it uses the existing genuine unpublished-prefix destructor.

For reached callbacks, the root performs1424E0 tail unlink and104970 reference
transition. The query then executes actual11B600: same-resolver cache removal,
source ref60 decrement, shared array release if final, and per-shape disposal.
Only after that completed source lifetime does it clear canonical hasShape and
end the native mesh/callback aliases. Canonical identity, shape tag, filters,
bounds, world matrix, and query matrix histories remain; no row is erased or
reinitialized. Native receiver/body borrows end after Entity destruction.

E4's model110 is a borrow, not an extra source retain. Existing guards prevent
D8/DC and E0/model retirement while E4 survives. After genuine E4 retirement the
existing retirement chain proceeds, with published E4 history separately checked
before world-backing release. The semantic shared arrays and source ref/cache
ordering remain the preceding patch's owners.

## Remaining boundaries

-E8 allocation/constructor11B190, actual single-pair shape/result ownership,
 category/filter setup and append still follow1662E4.
-Mutual exclusions, groups, later body/effects and actual active-world
 retirement remain unjoined. This patch does not enable callback dispatch or add a driver.
-The first-occurrence root cursor remains bounded. Other ordered occurrences
 must use their authentic constructor cursors and existing shared resource owner.
-The existing supported COL subset is unchanged. No source decoder was changed.
