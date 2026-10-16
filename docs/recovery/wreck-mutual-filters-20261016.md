# Mutual C0 ownership and derived callback continuation

Incremental source-only patch againstd958698. No builds, tests, syntax checks,
project decoder execution, guest execution or gameplay were performed.

`constructFirstMutualAndDerived()` continues the first retained piece's real
constructor at166390 and stops2CB614 before effect-setup inputs are consumed.

## Source order and storage

-166394:123EA0(E0,1), then1663A0:123EA0(E4,1). These set BC mode only.
-1663AC:123EB0(E0,E4);1663B8:123EB0(E0,E8).
-1663C4:123EB0(E4,E0);1663D0:123EB0(E4,E8).
-1663D8 returns from165E90, then165C30 returns through165DB0..165DD0.
-2CB610 changes actual E4.responseE8 from165920 to2CB420.
-2CB614 is the next stop, before loading the incoming effect count.

The source123EB0 scans duplicates across C4's whole extent, then fills the first
zero hole, otherwise reallocates4*(C4+1), appends at oldC4 and incrementsC4.
The new typed owner holds actual native C0 arrays, separately from preallocated
cold transaction capacity. Each fresh list grows0→1→2, with the actual canonical
pointer/count updated at each completed source call. E8 gets no identity list.
Source object addresses are represented by the same canonical native identities
already owned by these E0/E4/E8 objects, not invented handles or guest pointers.

Native allocation uses allocate/copy/commit. On failure, the previous completed
prefix remains owned and the source-call cursor does not advance. This is bounded
native failure handling, not a claim that retail realloc would preserve its old
pointer on failure. No failed constructor cursor is replayed.

## Matched lifetime

The full source path validates each live list against its owned pointer, exact
extent, mode and expected ordered identities. Retirement remains tail-first:
E8, E4, D8/DC, E0. Exclusion entries are numeric identities and remain history
when their referenced target dies; no deletion or compaction is fabricated.

E4 and E0 free their own C0 at11B4F4, before the genuine NULL CC/D8 branches and
before11B600 shape/result disposal. Only the native pointer alias ends; C4
high-water/mode/capacity history is retained. The retired owner is recorded and
cannot be reused. Canonical rows are not erased or reset. World-backing release
requires both real C0 lifetimes to have ended. The original legacy fixed-capacity
path is unchanged.

The derived callback is accepted by retirement only when the actual four-call
prefix completed. Its native dispatch binding ends with the actual E4 lifetime.
The source callback enum/history is not replaced by a fake completion receipt.
Existing initialShapesReady remains false; constructor completeness does not
enable query dispatch, group changes or destructive gameplay.

## Next precise boundary

2CB614 loads the incoming stack+E0 count.2CB618 tests resource-context s3, but its
delay slot2CB61C ALWAYS stores that count into piece174.2CB620 tests particle
parent s2.2CB628 onward branches on count174 and enters effect allocations.
A missing context does not justify skipping174 or inventing a null-resource
route. Continuing requires those genuine count/context inputs and actual effect
ownership/retirement. The earlier2CB4B0 constructor cleared160/164/168/16C/170/
178/17C, but did NOT initialize174.

Remaining activation work includes the full type3 ray/result producer join,
ordered later collection/group/effect construction, and active-world lifetime
retirement. This patch changes none of those gates and adds no driver.

Independent source review found no blockers in this bounded ownership,
constructor and retirement scope.
