# Source-owned collection WATER categories

Source-only increment againsta84a7cc. No build, test, syntax check, gameplay,
project decoder or guest instruction execution was performed.

## Actual owner and order

OriginalWreckCategoryList50LA owns actual D8/DC arrays, separate from cold
transaction backing and the legacy inline filter view. Its bounded capacity
is two actual constructor categories: PHYSICSBODY and WATER.
Authenticated SLUS_202.68 (VA-0x100000+0x80):11B190 writes D4=1 at11B3BC,
D8=NULL at11B3E4 and DC=0 at11B3EC.124160 only writes D4 at124164.
124170 first scans all DC entries for a duplicate, then fills the first
zero hole at1241F0 without changing DC. Otherwise124214 reallocates exactly
4*(DC+1);12421C stores D8,124230 writes the new word at old DC,12423C
increments DC. Typed native allocate/copy/commit preserves this ordering.

The source allows a zero category and does not handle realloc failure.
This bounded host constructor requires the actual nonzero named categories;
that is an admission restriction, not recovered source zero rejection.
Native allocation failure retains the prior allocation and reached cursor,
then stops without claiming completed source realloc or allowing replay.

The existing actual E8 PHYSICSBODY allocation at16636C now uses this same
typed owner from its genuine empty state. Initial allocation failure retains
an empty owned view and remains safely retireable. No second E8 array is
created by the collection suffix.22C21C sets E0 mode1;22C238 appends WATER
to its initially empty array;22C254 appends WATER to E8's existing array.
A duplicate WATER==PHYSICSBODY leaves E8 extent1; otherwise it grows to2.
The retained cursor tracks successful appends, including failure after E0
succeeded but before E8 growth. No failed prefix can be replayed.

## Matching retirement and canonical history

11B4A0 frees source C0 at11B4F4, CC at11B508, D8 at11B51C, then calls
11B600 at11B528 and1413E0 at11B534. E8's category owner is released before
its type3 shape/results. E0 releases actual C0 first, then its category array,
then the actual type2 shape/result/model domain. D4/DC high-water history
remains unchanged; only the native borrowed values pointer ends.

A full-category ownership predicate checks actual values, extents, mode,
published or source-retired canonical objects and the reached append cursor.
This accompanies all existing full E8/E4/contact/E0 retirement gates. Legacy
inline-category routes keep their previous retirement exclusion. Final world
backing cancellation requires both actual category owners retired when the
collection suffix was reached; it does not clear completion history as a
substitute for retirement. The old inline helper rejects full publications.

## Connected ordered continuation

applyFirstCollectionWaterCategories takes the actual WATER4342A8 field and
runs the real full query owner continuation only after22C1B8/scalar writes.
On success22C25C/260 advances the first collection's native ordered constructor
cursor to1. If authored count30 exceeds1, the result is nextPieceRequired at
22C0A0; otherwise it is groupConstructorRequired at22C278. The completed
piece count is exposed separately from the already published source38 slot
count and total authored plan. Neither branch reports a complete collection.

Later piece construction remains gated by genuine warm model occurrence
ownership, with a new piece/model/body/query/effect sequence for each source
slot. Group construction/effects, outer racer370/374 publication and live
activation remain unjoined. This patch neither fabricates those objects nor
reuses the first piece as another authored occurrence.
