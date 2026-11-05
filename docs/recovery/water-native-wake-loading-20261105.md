# Wake Froth loading after explicit native retirement

This packet extends only the explicit native-adapted route. Original Config and aggregate source PCs remain30492C, before the unjoined EE heap header read. Original Wake selection return remains absent. Native loading has its own PC, phase and completion result; it does not establish original free, allocator, selection return, caller continuation, Water completion or GPU readiness. No normal source route invokes either native adapter.

## Admission and operation order

Owner::loadWakeFrothNativeOnly requires WaterWakeLoadMode::nativeRetiredBackingOnly. The public entry checks exact Config/World/Application identity, thread, all source cursors at30492C, no closed or withdrawn native owner, and no outstanding runtime, constructor or particle-invocation borrows. Private Config admission also requires the reviewed native retirement to have completed both old backing releases and projected clears, with historical adaptedResumePC157FE8, the same found Wake row/position and earlier same-stream seek. Missing Wake, NULL-old-child, partially retired or already closed paths cannot enter. No retirement or initial seek is replayed.

The loading body follows the original157FE8..158238 operations as a native projection:

1. Read selected row.line08 before changing the name. Unknown span stops at adapted157FE8. The signed high-bit span means zero loop iterations, rather than a giant unsigned loop.
2. Copy the ten Wake Froth bytes and terminating NUL into name28 indices0..10. Preserve General Information's known tail indices11..19, including its final NUL; do not read or initialize unknown bytes20..1023. Historical first name remains represented by its copied-name receipt and retained General Information query; its buffers, counts and getters are unchanged.
3. Count across the original physical-line span using the same source stream and the existing12EE90 reader/159440 split semantics. Blank, comment and EOF lines still consume physical iterations. Only nonzero/nonsemicolon split[0] increments selected20. Reader's real EOF outcome does not end the loop; native transport/knownness refusal stops the projection without pretending it is a guest reader return.
4. Reload the same text row's position and seek the same selectedRoot/Config/handle once, corresponding to158130. This is the second Wake seek, after counting; no file reopen, alternate cache or first-seek replay occurs.
5. Allocate new table and data success-only native backing with separate current reentrancy captures. Record original low32 requests count<<2 and count<<10. Publish current child24 only after table backing succeeds. Data admission failure retains that table, name, count, stream and all earlier retirement effects.
6. Write semantic pointer0 to the new data role even at count0, then relative offsets i*1024 for positive remaining slots. No native pointer is cast to original EE bits.
7. Read/split the same physical span again and copy retained lines through NUL into the new data backing and matching known-byte row sidecars. The same per-Wake scratch persists across both passes. The bounded byte-result copy does not prove original strcpy alignment, word overreads or EE addresses.
8. Project field428=0 and mark nativeLoaded at adapted158238. Do not create wakeSelection_.returned, advance any original source cursor, call MinForce or resume the caller.

## Separate current ownership and immutable history

NativeWakeLoad owns fresh input/split buffers, read records, phases, span, counters, second-seek state, allocation records, row sidecars and native backing. Scratch starts unknown because intervening getters prevent assuming prior General stack contents. Within this invocation, known suffixes are retained across reads, splits, rewinding and allocations.

New immutable per-Config table/data role tokens use epochs3 and4. The retired General roles1/2 are never reused, even if native malloc happens to reuse an address. pointerZeroRole records epoch4 explicitly. New raw buffers remain private unique_ptr owners; no caller receives a storage pointer or borrow.

Current logical selected20/child24 are derived from the active Wake projection once it starts. Before then they retain the prior source/native-retirement rules. The original General rows, pointer offsets, counts, name/query receipt, allocation attempts/results, stream history and completed getter values stay unchanged. Existing getter historical bounds still use the original sectionCounted_. Retained General sidecars are not reused as live Wake rows, and the old source field-lookup entry remains blocked after retirement.

Native retirement's data/table release receipts and its157FE8 resume marker remain historical. Its preflight no longer mistakes later Wake allocations/counts for resurrection of retired General ownership. Original Wake lookup, first seek and source free-prefix receipts likewise remain historical, including the unjoined30492C boundary.

## PC and stream custody

readTextLine12EE90 has a private optional adapted-PC output. All old source callers omit it and retain exactly the earlier pc_ behavior. New Wake calls supply only NativeWakeLoad.pc; the reader never temporarily writes original pc_ on this route, including refusal and EOF paths. Diagnostic addresses on the adapted operation describe its native source correspondence, not an original source receipt.

The same resolver, root, handle, member and Config borrow are used throughout. Each new read or second seek records the actual current cursor. Config expectedCursor comparison prefers the latest Wake-loading operation over the historical first-selection/first-Wake-seek cursors. Native refusal retains any transport cursor movement already recorded. This is not rollback, a guest I/O failure branch or a fabricated EOF outcome.

## Bounded allocation and partial-state contract

The existing success-only allocation aperture is preserved: exact original requested low32 sizes; minimum eight native bytes for a zero request; at least one offset slot; zero row sidecars when count0; current reentrancy captured independently per allocation. Counts beyond the safe count*1024 aperture stop before oversized native operations. Sidecar or backing allocation refusal is a native admission boundary, not original mallocNULL, errno, new-handler or allocator failure behavior. EE chunk headers, bins, coalescing and allocation addresses remain unknown.

Preflights distinguish an unentered body, unknown span before name mutation, count-read/split refusal, second-seek boundary, table admission refusal, live-table/data admission refusal, bounded copy refusal and completed native loading. Counts and scan ordinals retain their own phases; the latest read need not equal the cursor after the subsequent seek. Every created native allocation remains singly owned on failure. Old retired owners remain NULL. Native Config/provider withdrawal still runs combined preflight before releasing stream custody, and ordinary destruction later releases new backing exactly once. No source destructor is implied.

The entry is one-shot. Repeated requests return alreadyAttempted with the retained adapted PC/completion flag, without reads, allocations or replay. A completed native selection is deliberately not consumed by any new getter in this packet.

## Evidence, predecessor and limits

The focused static trace covers157FE8..158238,12EE90/split ordering, allocation wrappers, exact name-tail effects and partial states. Immediate predecessor: /tmp/racer-water-retirement-20261105-0158, patch111351970226083e3067b3dfc633d93656e79562d3605741d7e5402a37f27f59, final manifest36a5b766f64781bc293646a6770d210ad1b54a389277c5f034dcb6ceea5719f3. The exact unpublished Water predecessor chain is preserved. Published main remains e566993fc25f628ce5b4aa5def424c6003f53b75.

Inherited process-global changes require a narrow reviewed rebase if the pending vectors/section checkpoint lands first; preserve its vector-owner changes. This packet does not change that process-global header. Publication remains paused.

Validation is restricted to static source/ELF inspection, file-byte/Git-blob identities and strict text-patch reconstruction. No compilation, syntax/type checks, builds, tests, probes, helper/model/target execution, emulation, runtime or publication are performed or claimed.
