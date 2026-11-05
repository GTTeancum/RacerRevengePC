# Native-adapted Water allocation entry and first arena boundaries

This packet extends the explicit native-adapted Water path through the NumZones request, when selected, and the common new[] / operator-new / malloc entry. It stops before either first current arena read: 3059EC for small normalized requests, or 305AC4 in the large-bin branch's pending delay slot. Original Config and aggregate source cursors remain 30492C, and original Wake selection return remains absent. There is no allocation, allocation return, pointer publication, source heap receipt, Water completion or GPU readiness.

## Explicit entry and predecessor custody

Owner::enterWaterAllocationNativeOnly accepts only nativeRetiredBackingOnly mode and the same live Config, Water provider and Application, owning thread, no incompatible borrows, no closure/withdrawal, and full existing source/native preflight. Its predecessor must have either a completed pointer-array request at adapted 2E8CA0 or a zero-pools branch at 2B9D98. Unknown or incomplete predecessor count reads do not qualify. The entry is one-shot; repeated calls report retained state without replaying reads or arithmetic. No normal-source continuation calls it.

For a signature-wave or pool request, the exact original request word and sign-extended a0 are taken from the preceding call boundary. The retained caller return site is respectively 2B98DC or 2B9C58. This does not reread the count prematurely or report a completed call.

For the zero-pools arm, the next body starts at 2B9D98 and reads current same-provider numZones4A8. It does not use the saved NumZones getter result as the new read, assume zero or require a positive count. Existing owner invariants presently admit no intervening count writer; this bounded custody remains unchanged and is not a source equality requirement. A future supported writer must extend that ownership before changed or newly unknown values can be admitted between these boundaries.

## Exact NumZones arithmetic

The new state preserves the original count word and its sign-extended register value, then every low32 intermediate:

- 2B9D9C: t8 = count << 3.
- 2B9DA0: t9 = t8 + count.
- 2B9DA4: t36 = t9 << 2.
- 2B9DA8: JAL new[] 2E8CA0, caller return site 2B9DB0.
- 2B9DAC delay: original request = t36 + 16, sign-extended into a0.

All shifts/adds wrap to low32. Unsigned host operations and explicit upper-word composition represent the source SLL/ADDU/ADDIU results without signed overflow or signed shifts. Zero zones still requests 16 bytes. Other count patterns can wrap the request to zero; they are neither clamped nor rejected. An unavailable current count stops at 2B9D98 before any request or wrapper effect.

Later element-constructor wrapper 2E8990, its cookie and element lifecycle, Water +D0 publication, the separate later current +4A8 reread and +4A4 allocation remain unjoined. Raw allocation would not establish constructed elements.

## Wrapper frames, argument and zero request

Only symbolic caller-stack frame/save roles are recorded. They preserve unknown saved register contents without inventing register bits, guest stack addresses or payload writes. SQ128 and SD64 widths remain explicit:

- new[] at 2E8CA0 uses a 16-byte frame and SQ ra at +0, then calls operator new with a0 unchanged.
- operator new at 2E8D90 uses a 64-byte frame and SQ ra +20, fp +10, s0 +0. POR retains the full a0 in s0. The branch delay at 2E8DA8 sets fp = sp. A zero full request executes 2E8DAC/DB0 and changes effective s0 to one; nonzero requests are unchanged.
- 2E8E1C passes the effective full argument to malloc at 305700. This retains both original and effective requests rather than overwriting the original request.
- malloc preserves that full argument in a1, creates a 16-byte frame before the current-reentrancy load, and only after that admitted load saves ra with SD64 at +0 before calling 305990.

No frame is reported restored or any wrapper returned. The save masks record role/order, not a fully known memory image. They require no native heap allocation.

## Fresh current reentrancy at 30570C

The existing closed-process OriginalConfigProcessGlobals50LA owner is used at the exact source read. Its current-thread/reentrancy-known check is performed afresh; state().reent38544C is then captured. The supported native aperture requires the existing symbolic reent385160 identity. Unknown read and known unsupported identity are distinct refusals, both at adapted 30570C. Earlier frame/save/argument effects remain, but malloc's later ra save and allocator entry do not occur.

This does not reuse a previous Config/retirement read, reset poisoned knownness, invoke the sprintf-specific 308C98 bridge, fabricate raw EE pointer bits or infer arena/errno authority from a reent pointer. The current identity is a historical captured argument after this point; later preflight does not recapture it as if it were a new source read.

## Allocator normalization before the first arena read

After supported reentrancy, 305990 creates a 96-byte frame. SD64 saves occur in source order: s3 at +30, ra +50, s4 +40, s2 +20, s1 +10, s0 +0. The last save is the normalization branch delay. Reent is retained symbolically in s3 at 3059A4. Arithmetic is interleaved with these saves as in the original words.

Define sx32(w) by retaining uint32 w and adding high bits 0xFFFFFFFF00000000 only when bit31 is set. ADDIU at 305994 produces sum = low32(effective request + 19), with full register sx32(sum). SLTIU compares that full unsigned64 value with 31. If smaller, normalized chunk is literal 16; otherwise the original sign-extended mask gives low32 sum & 0xFFFFFFF0 and its canonical sign extension. These are request-normalization observations, not allocated-chunk metadata or a safe native byte size.

No original range, signed-negative, overflow or refusal branch exists before the arena read. Wrap remains part of the represented arithmetic. Unsigned64 comparisons of these canonical values agree with low32 unsigned comparisons, including high-bit inputs; no signed reinterpretation is substituted.

3059D0 calls 306538 with retained reent in its delay slot. The pinned original lock helper is exactly JR ra / NOP and returns to 3059D8. Its separate called/returned roles do not acquire a native mutex, prove synchronization or supply arena authority.

## Small and large exact stop boundaries

The normalized full register is compared unsigned with 504, then the branch executes its LUI arena-base delay at 3059E0. Normalized chunks are multiples of 16. Small results are 16 through 496; the next possible value is 512. No normalized 504 receipt is invented.

Small path: the source computes the bin header from base 385468 plus normalized chunk. The packet records only the pending arena-read offset chunk + 12, then stops at 3059EC BEFORE loading that backward-link word. It does not read the arena, manufacture a pointer or execute the later SRL at 3059F4.

Large path: let q = chunk >> 9. The original branch/delay classification gives:

- q == 0: index = chunk >> 3.
- q < 5: index = (chunk >> 6) + 56.
- q < 21: index = q + 91.
- q < 85: index = (chunk >> 12) + 110.
- q < 341: index = (chunk >> 15) + 119.
- q < 1365: index = (chunk >> 18) + 124.
- Otherwise index = 126.

The first actual large value 512 selects index 64. The q == 0 arm is preserved in the source structure but cannot be reached by an actual normalized large request here. The packet records index and pending arena-read offset index*8 + 12. At 305AC0 the branch targets 305AD4, but the unjoined load is its delay instruction at 305AC4. Therefore the final adapted PC is 305AC4 with pendingBranchTarget = 305AD4, not a completed transfer to 305AD4. No arena word has been read or changed.

## Partial states and cleanup

Preflight distinguishes pristine state; unavailable NumZones at 2B9D98; unknown or unsupported current reentrancy at 30570C with exact earlier frames/arguments retained; and either completed pure-entry prefix at its pending arena read. It validates all retained arithmetic, save masks, distinct original/effective requests, branch selection and pending transfer. Optional zero and false are engaged values. There is no fallible work in the fixed instruction sequences between these public refusal points.

The new prefix owns no heap payload, owner reference or resource requiring separate cleanup. Existing preflight/withdrawal retains history, never rereads a withdrawn Water provider, never executes an original free or resets process globals. All original Config, getter, packed, scale and array-prefix PCs remain historical. This entry's local PC and lock-return role cannot be used as original Config/heap completion receipts.

## Remaining allocator and native-adapter boundary

Current bin links, chunk headers, free-list state, allocator availability, errno and returned pointer are not established. mallocNULL recovery would subsequently read current new_handler/policy and possibly callback, retry or throw; those paths are not entered. Native success-only backing would require a separate explicit adapter and must never turn these pending original arena reads into source receipts. Pointer arrays need original32-bit slot roles and host sidecars; zones additionally need cookie/constructor proof. Such an adapter is not included here.

## Provenance and limits

Immediate predecessor: /tmp/racer-water-array-prefix-20261105-0258, final manifest ddd3fc8d699167e35bb174cba03ad182efa9e0ba6043b7f6b1adeffcb0605036, patch 1db01902ae8ac4c7fff9b82b9410d61f840e656c4bb94969465333ebff929c0f. The trace references reviewed source identity 68afb97bc123441fab6a658eba20af73dd30f7d7aba059fb1549e1e7b66def67 before acceptance metadata; that same reviewed source is retained unchanged. The full unpublished predecessor chain and process-globals/vector-header rebase qualification remain. Published main remains e566993fc25f628ce5b4aa5def424c6003f53b75; publication stays paused.

Verification is static source/ELF inspection, byte/Git-blob identity and strict isolated text-patch reconstruction only. No compilation, syntax/type checks, builds, tests, probes, target/helper/model execution, emulation, runtime, gameplay or publication are performed or claimed.
