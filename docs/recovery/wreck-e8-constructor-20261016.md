# E8 one-pair constructor and reached retirement

Incremental source-only change against the frozen E4 publication result
(localbae1b37, equivalent parentb683f2b). The E4 export is unchanged.
No build, test, syntax check, project decoder execution, guest execution or
runtime/gameplay inspection was performed.

## Actual constructor order

`constructFirstE8ToMutualFilters()` uses the existing planned E8 component and
canonical world slot. It stops at166390 before mutual exclusions.

-1662E4 allocation;1662F8 calls11B190(1);166300 stores E8.
-166308 calls11B570(3).
-166324 appends E8 with mask0 BEFORE any shape/result construction.
-16633C stores actual piece9C;166348 sets actual PHYSICSBODY3E0718 categoryA4.
-166350 calls11C710(query,1,NULL,NULL). There is no11F570 call here.
-16635C calls124160(1), writing category modeD4. This repeats11B190's initial1.
-166368 calls124170(PHYSICSBODY). The actual one-entry D8 allocation is distinct
 from the preexisting cold two-slot category backing; DC high-water becomes1.
-16638C stores F8.bit0=1; next166390 begins the mutual-exclusion suffix.

The canonical E8 shape row is published after completed11C710 and before category
allocation. Thus a category allocation failure retains its actual completed shape
and can retire it. No query radius, shape radius, hit count, fraction or ray length
is invented. Source vectors mirror their real typed allocation values; native
transaction scalars retain their prior backing and cannot dispatch while the
existing full-path activation gate remains closed.

## Typed11C710 ownership

The new owner represents ONE source endpoint pair, with these actual allocation
orders and types:

-shape8C0x50 at11C740; shape20/30 constructed to vf0; shape00=1.
-shape04: two metadata32 words;08: two Vec4 endpoints;0C/10: one Vec4 each;
 14: one float length;18/1C: one Vec4 each.
-source array calls11C774,11C78C,11C7BC,11C7EC,11C818,11C82C,11C85C.
-shape44=0 at11C898; the unrelated global3DF5B0 is not changed here.
-result0x80 at11C934, then scalar arrays64,68,6C,70, then vector arrays50,54,58.
-result5C=1; object90 publication is11CA5C.

1048E0 constructs every vector as vf0=(0,0,0,1). Metadata04 becomes zero because
a3=NULL. Endpoints08 are copied from actual retained3E02C0 because a2=NULL.
Source shape40,query80,result60/74/78/7C, lengths14 and scalar result arrays
are not initialized by this constructor. Host allocation zeroes are not source
initialization claims. The owned endpoint arrays are used by the existing staging
and bound-view interfaces on this full route; legacy inline backing is unchanged.

## Retirement and honest allocation failure

`retireFirstE8Prefix()` handles the actual appended tail or the proven unlinked
append-failure prefix. It requires unregistered/inactive membership and no later
mutual/group/filter continuation. It performs the real tail unlink and reference
transition, then11B4A0. Identity arraysC0/CC are genuinely NULL here; categoryD8
is released at11B51C BEFORE11B600. Its native alias ends while high-water/mode
history remains. The canonical identity/shape tag/cache history is not erased.

Type3 release order is shape04,08,0C,10,14,18,1C; then result6C,50,70,54,68,64,58;
then shape object and result object. Source6C cleanup writes result74=0; it does
not clear the stale source6C pointer before object disposal. Native typed owners
release their actual allocations in this order.

An incomplete shape/result allocation is deliberately terminal and retained:
11C710's shape constructor initializes20/30, not every pointer slot, so native
NULL bookkeeping cannot certify an original partial11B600 destructor. Before the
shape allocation exists, the genuine NULL shape prefix can retire. Completed
shape/result ownership and later category-allocation failure can also retire.
No earlier E4/D8/DC/E0 lifetime may end while a reached E8 remains alive.

## Remaining joins

-166394 onward: actual E0/E4 exclusion mode setters and four ordered mutual
 identity exclusions, including their source allocation and retirement ownership.
-Complete full type3 ray/result producer/transaction binding before activation;
 owned endpoints alone do not make result storage active or certified. Existing
 query activation remains closed, and no driver has been added.
-Later collection/group/body/effect construction and real active-world retirement
 remain separate. Only the first retained occurrence's existing constructor cursor
 is extended here; no synthetic collection completion or KO credit is introduced.

Independent source review covered the new owner, constructor order, matching
retirement, endpoint access and private signatures; no blocking findings remained
within this constructor-only scope.

Root review adds object-count overflow preflight before the E8 attempt. An
initializer rejection leaves the constructor cursor and attempt state unchanged;
no reached-E8 retirement requirement is fabricated before a real count.
