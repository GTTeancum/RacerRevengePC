# Native-adapted Water pointer-array request prefixes

This source-only packet follows the reviewed range-scale branch without joining the original EE heap. Original Config and aggregate source PCs remain 30492C and original Wake selection return remains absent. The new explicit Owner::prepareWaterArraysNativeOnly entry advances only its own native-adapted state. It ends before entering new[] at 2E8CA0, or before NumZones at 2B9D98. Neither outcome is Water completion or GPU readiness.

## Ownership and separate branch history

Admission requires the nativeRetiredBackingOnly mode, a completed scale store and zero/nonzero signature branch, the same live Config, Water provider and Application, the owning thread, no incompatible borrows, no withdrawal or closure, and the existing source preflight/cursors. Repeated calls report alreadyAttempted without repeating stores, rereading fields or issuing another allocation call. No normal-source continuation invokes this method.

The preceding scale branch snapshot chooses this entry's arm. It does not supply the later count. The signature arm reads current same-provider +6C again. The pools arm reads current same-provider +64. Both are actual typed owner fields, not saved getter results, assumed zero defaults or raw ELF data. There is no extra pools reread between its branch and allocation argument. Existing provider ownership presently admits no intervening writer to these count fields, so preflight keeps its established current-field invariants. The new code neither relaxes those invariants nor substitutes those historical observations for the separate reads. A future supported writer must extend versioned custody before differing current values are admitted.

## Signature-wave arm at 2B98A8

The exact eight caller-stack stores are represented in original order, with per-lane knownness and the store count:

1. 2B98A8: stack +820 = 0.
2. 2B98AC: stack +830 = 0.
3. 2B98B0: stack +824 = 0.
4. 2B98B4: stack +834 = 0.
5. After LUI at 2B98B8, 2B98BC: stack +828 = 0.
6. 2B98C0: stack +838 = 0x3F800000.
7. 2B98C4: stack +82C = 0x3F800000.
8. 2B98C8: stack +83C = 0x3F800000.

Only these eight words become known. Their final vectors are (+0,+0,+0,1) and (+0,+0,1,1). They are this caller occurrence's stack data, not provider constructor vectors or Water globals. Nothing initializes adjacent stack. These stores do not perform floating arithmetic.

At 2B98CC the code reads current +6C. Unknown native custody stops there with all eight stores retained and no count/request/call receipt. An engaged zero is a valid read. At 2B98D0 ADDIU produces the low32 count+1 word. JAL at 2B98D4 and SLL delay at 2B98D8 produce the low32 byte request (count+1)<<2, then stop at callee entry 2E8CA0. The request is not required to be positive, nonzero, nonwrapping or consistent with a safe native allocation size. No extra source check is fabricated.

## Pools arm at 2B9C40

This arm has no signature-stack stores. The new current +64 read at 2B9C40 can stop without a count. BEQ at 2B9C44 compares its word exactly with zero and executes the NOP at 2B9C48 on both arms. Zero stops at 2B9D98 without a request or call. That target begins NumZones loading, not function completion.

Every nonzero word, including high-bit words, uses the same +64 snapshot for ADDIU at 2B9C4C and the JAL/SLL pair at 2B9C50/54. The prefix stops at 2E8CA0 with its original byte request. It does not reread +64 prematurely or range-check it as a signed count.

## Request representation and unresolved allocator

Both requesting arms retain the uint32 count, low32 increment, low32 shifted request, and sign-extended 64-bit a0 value produced by SLL. The arithmetic uses unsigned integer operations and explicit high-word composition, avoiding host signed overflow or signed-shift behavior. For example, wrap is retained by construction; no allocation is actually attempted by this packet.

callIssued means only that the JAL and its delay have been represented and the adapted PC is the callee entry. It is not a new[] receipt, returned pointer, native backing allocation or original execution claim. Water +C8 and +68 are unchanged, and no fabricated pointer, terminator slot, allocation size, handler state, errno or exception appears.

Static evidence shows new[] 2E8CA0 forwards to operator new 2E8D90. That later body changes an original zero byte request to effective malloc request one before calling 305700. This packet stops before those operations and does not conflate that contract with the earlier Config direct malloc(0) adaptation. Later current new_handler/policy and original allocator metadata remain unjoined. Even a future success-only native adapter must preserve original versus effective request and remain distinct from original allocation failure, retry, throw and return receipts.

## Partial-state and cleanup invariants

The new state is untouched before explicit entry. A signature current-read refusal retains precisely the eight stores; a pools current-read refusal retains no stack effects. Zero pools retains the read/branch/NOP but no call. Either allocation request retains count and exact modular arithmetic without entering the callee. No fallible operation separates the recorded non-failing stack stores or integer instructions, so intermediate per-instruction states are not exposed as public refusal points.

Preflight checks the selected arm against the historical scale outcome, each known stack lane and count, absence of effects on the other arm, exact request/sign extension, and the relevant native stop. Optional zero and false remain engaged values. While backing is live, captured count custody is checked against the same provider. After withdrawal it is never dereferenced. The prefix owns no heap allocation or independent provider reference; existing native cleanup retains historical receipts without freeing any source heap or resetting Application globals.

## Provenance and scope

Immediate predecessor is /tmp/racer-water-scale-20261105-0243, final manifest cb79b6e083e799c6dc8ce1d9d5605f8021a5d8ff6be4e6c2abe7240a57ee033b, patch bd4919074092485113de3afef4a497c5a4e253f116e6079c732ada52ecc025bb. Its reviewed source manifest 85d2cc19b0944d19307fe05be5e5ff68bb1a545d2a8351a0df57f573e7766297 is retained separately. The new trace references that reviewed identity before final metadata; this does not assert a different source predecessor.

Published main remains e566993fc25f628ce5b4aa5def424c6003f53b75. All unpublished predecessor dependencies and the narrow process-globals/vector-header rebase qualification remain. Publication is paused.

Verification is static source/ELF inspection, file-byte/Git-blob identity and strict isolated text-patch reconstruction. No builds, compilation, syntax/type checks, tests, probes, helper/model/target execution, emulation, runtime, gameplay or publication are performed or claimed.
