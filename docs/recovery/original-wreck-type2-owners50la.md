# Wreck E0 typed shape/result allocation owners

Source-only incremental ownership implementation against dc0b360. No builds,
tests, syntax checks, decoder runs, gameplay or guest execution were performed.
Original SLUS words were read as data using the existing VA/file mapping.

## What this patch actually supplies

Each existing E0 type2 storage now contains one non-copyable source allocation
owner, distinct from its live native transaction state, scratch and two banks.
The owner's private constructor methods are accessible only to the genuine
query owner. No full-constructor publication call is enabled by this patch.
Cold construction creates an empty owner, with no claimed source allocations.

The implementation can allocate the genuine shape, load ordered node/endpoint
arrays from the same retained decoded COL occurrence, allocate all seven result
arrays in source order, and release the completed shape/results in exact source
order. It exposes a const resource view for the later11F570 numerical join.
This copies the authenticated runtime nodes, including their recovered radius
and adjusted half extents, and endpoint fourth words already explicitly supplied
to the existing decoder. It does not infer new W words or run another decoder.

1253DC allocates shape8C;1253F0 stores the actual model identity;1253F8 initializes
normal-mode54 to zero.126640 loads matrix/count, then nodeCount*36 at12687C,
then rayCount*32 at126D3C.126E18 writes root40=0. The retained admitted decoder
requires the independent source count48 and ray count to match and nodeCount+1
to equal ray count. This bounded owner preserves that existing admission.

11BBC0 overwrites mode54 with its actual argument after the shape loader.
11BBE8 allocates the result object;1048E0 constructs two vf0 vectors at00/10,
and direct stores initialize20/30/40 to vf0. Shape48 becomes result5C. The
seven allocations are:

-64,68,6C: count*4 each, in that order.
-70: count*2.
-50: count*32, constructed as count*2 vec4 values.
-54,58: count*16 each, constructed as count vec4 values.

The source vector allocations include a16-byte header and store the adjusted
user pointer. Typed host arrays own their allocation directly; no guest allocator
header is emulated. Each native owner is the authority for its corresponding
source allocation, not an assertion of guest pointer-layout equivalence.
Only completion at11BD18 certifies source90 publication. Earlier owned result
storage is the constructor's local pending object. Native fresh scalar backing
is zero-initialized for safety, without claiming original reset/hit writes.

## Matching release

The query caller must first establish real11B4A0 eligibility: reference zero,
no remaining Entity links, genuine absent/removed registration, and retired
filters. This allocation owner cannot grant any of those receipts.

11B600 type2 releases shape4C then50; result6C,50,70,54,68,64,58; shape8C;
result90. Important source detail: the free of result6C is followed by a zero
store to result74 at11B834, not to6C. Its source pointer remains stale until
the result object dies. The native allocation owner releases that allocation
and writes74, without exposing the dead source pointer as usable storage.
11B900/11B904 finally clear the enclosing shape/result fields.

No canonical row is erased or restored. No model reference is released by this
allocation owner. A reached allocation cannot silently disappear in a C++
destructor: destruction with live ownership terminates, matching the existing
explicit-lifetime discipline. Allocation failures retain the reached prefix and
source boundary. Partial-allocation failure retirement remains unsupported;
no automatic rollback is reported as original cleanup.

## Remaining query-owner join before publication

The new owner deliberately has no public mutation entry point. Root may include
its header in the next normal native build; all constructor calls remain gated.
A subsequent query-owner join must:

1. Admit the same canonical model/component/session and reserve domain capacity.
2. Run shape allocation, publish the actual model44 identity at1253F0, then
   load shape arrays and allocate results. Preserve any reached failure prefix.
3. Run per-E0 preparation11F570 AFTER11BBC0, then append at166098. Do not use
   the legacy helper that prepares E0 and E4 before publication.
4. On retirement, prove real registration/filter lifetime, run11B600, end the
   model-domain borrow only at that source shape lifetime boundary, and perform
   Entity1413E0 with the actual object counter. Preserve canonical row history.
5. Distinguish source-retired backing release from never-published cancellation;
   neither may simply revoke a token to bypass a live query.

The native contact transaction arrays still need their eventual explicit
projection into/out of these seven source arrays before active queries use the
new full-constructor route. This patch neither claims that projection nor
activates E0, wrecks, death, callbacks or KO credit.
