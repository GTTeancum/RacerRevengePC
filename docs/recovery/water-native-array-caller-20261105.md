# Native-adapted Water array caller prefixes

This packet resumes only from the explicit success-only native backing adapter. Original Config and aggregate source PCs remain 30492C. The original allocator's pending arena read remains 3059EC or 305AC4, including its pending branch target. No allocator return or epilogue is manufactured. The new Owner::resumeWaterArrayCallerNativeOnly opt-in projects the supported caller instructions into a separate native history.

Pointer paths stop at the first formatter entry 308C90. The zone path writes the cookie and stops before the first element constructor at 2461E0; zero zones instead completes the native-adapted wrapper return to 2B9DD0, before Water +D0 is written. None is Water completion or GPU readiness.

## Admission, current fields and lifetime

Admission requires the same provider, Config, Application, owning thread and existing borrow/closure/withdrawal gates, a successful live native backing role with epoch 1, full inherited preflight and the exact private native resume site. It is one-shot. Repeated entry reports retained state without replaying field stores, cookie stores, count reads or calls.

SourceWaterProviderOwner now has private native-adapted views for Water +C8 and +68. A view contains this provider's backing role, epoch and logical offset, never raw EE pointer bits or host pointer bytes. Absence means no adapted view has been established; it is not a fabricated source NULL value. The original constructor's zeroWords remain historical and unchanged. No source-exact reader is added for these adapted views, no view escapes the owner, and Water +D0 is not published in this packet.

Before provider backing is reset, these private field views are explicitly revoked and nativeViewsRetired is recorded. This is native authority withdrawal, not source NULL stores. No fallible work separates that revocation from backing reset and its existing native retirement marker. Preflight after withdrawal requires the provider absent and never reads freed storage. If an earlier cleanup step refuses, views and backing stay owned and unchanged.

## Signature and pool pointer paths

At native resume 2B98DC, the signature path projects SW into current Water +C8 as the exact private signaturePointers backing role at offset zero. At 2B9C58, the pool path does the corresponding +68 store. No pointer-array slot is initialized. All count+1 semantic sidecars remain unknown, and raw pointer-array bytes are not read or written.

The signature jump at 2B98E0 and pool jump at 2B9C5C execute their delays, setting full ordinal register s0 to zero. The new current field read then occurs at 2B9BC0 (+6C) or 2B9D78 (+64). This is not the prior branch/getter/allocation count receipt. The read word is sign-extended into a full register and compared unsigned against zero at SLTU. The following BNE executes its NOP delay on both arms.

Existing provider count custody admits no intervening writer and remains intact. Differing values are not newly admitted by this packet. A hypothetical newly unknown count would stop at its current-read frontier with the earlier field publication retained; the full inherited model can reject an unsupported state before entry. The exact not-taken architectural frontiers, 2B9BD0 or 2B9D88, are represented without executing a terminator store. Neither branch is fabricated by pretending the current count changed. Accepted native pointer counts are bounded by the preceding adapter; high-bit cases are not silently added to that accepted domain.

On a taken branch, the original first format call is prepared:

- Signature: a0 is symbolic caller stack +710, a1 is verified literal 3C0AF0, “Signature Wave %d”, a2 is one. JAL 2B98F4 and its delay lead to formatter entry 308C90, with caller return site 2B98FC.
- Pool: a0 is the same occurrence's stack +710, a1 is verified literal 3C0C20, “Pool %d”, a2 is one. JAL 2B9C70 and its delay lead to 308C90, with caller return site 2B9C78.

The role, literal and caller return site distinguish these formatter occurrences. No host formatting is used. The formatter body, current stdio/reentrancy read, output bytes, output length, return and later same-Config section selection are all unentered. Stack +710 is a symbolic destination argument with unknown contents, not a zero-filled fabricated buffer.

## Zone caller arguments and wrapper

At 2B9DB0 the private zoneRaw role supplies the native-adapted raw argument. The caller prepares constructor 2461E0, destructor 246260, element size 36, and retained original NumZones register for the call at 2B9DC8 with delay 2B9DCC. There is no new NumZones read here. The retained count is the same caller s0 captured before allocation, not inferred from current memory or a source allocation return.

Wrapper 2E8990 establishes its 192-byte symbolic frame and ten SQ save roles: ra, fp, s7 through s0. Saved unknown register bits and guest stack addresses are not initialized. The private native-success role supplies the nonnull condition for this adapted path. This does not establish an original allocator result. Constructor identity is the verified nonnull source constant; it is never cast to or called as a host function pointer.

The only payload writes in this packet are:

- 2E89D0: four little-endian bytes for element size 36 at raw +0.
- 2E89D4: four little-endian bytes for the low32 retained count at raw +4.

Separate known cookie fields are set only after their actual native byte writes. Preflight checks only these eight established bytes. Raw +8..15 and every element byte remain unknown and are never read. There is no whole-block zero-fill or source element initialization.

At the actual branch delay 2E89DC, elementOffset becomes 16. The raw allocation remains the owning base. For count zero, offset 16 is a valid zero-length/end view and is never dereferenced as an element. The source stack unwind slots are represented separately: element view at +A0, size +A4, count +A8, destructor +AC, and +B0's initial count store followed by its zero ordinal store. Adjacent stack remains unknown.

## First element boundary or zero-count return

The initial ordinal zero is loaded and compared unsigned with the retained count at 2E8A30/34. The BNE at 2E8A38 executes its delay at 2E8A3C, setting a1 to one on both outcomes.

For a positive admitted count, JALR at 2E8A18 and its delay pass the first element view at raw +16 to constructor 2461E0. The new native PC stops at that constructor entry. No constructor body, base constructor, vtable store, element byte or constructor return is projected. No element is marked complete.

For count zero, the wrapper's following comparison also finds zero less than zero false. Its branch at 2E8A48 places the element-base view in v0 in the delay, then restores the ten saved roles. JR at 2E8AD4 and its stack-release delay complete the native-adapted wrapper return to caller 2B9DD0. No constructor call occurs. This is a bounded wrapper return after native allocation adaptation, not an original malloc/new[] or Config return. The caller's +D0 store remains unentered.

The full 36-byte element constructor is separately traced in evidence, but intentionally not implemented here. Its symbolic vtable identities, exact writes, per-element completion, bounds and future destructor/resource ownership need a separate review before extending this boundary.

## Partial state and preflight

The new preflight is linked through native backing. It validates pristine state and absence of premature provider effects, selected role and exact predecessor resume, pointer publication and current-count history, the formatter arguments/frontier, zone argument/save/cookie/unwind state, first-constructor versus zero-return state, and native view retirement. Optional zero and false are engaged values.

The native backing preflight continues to require all pointer slots unknown. Its pristine check also forbids premature cookie fields. Successful backing remains the same private allocation and epoch as caller work progresses. No second payload allocation is made, and raw cookie access is guarded by live payload custody and sufficient size. Fixed non-failing operations between public boundaries expose no additional partial state. There is no rollback of earlier projected writes.

## Remaining work

Signature/pool formatting and subsequent same-Config section replacement, object creation and count-driven array writes are unjoined. Future slot accesses must use actual allocated capacity, not a guessed unchanged loop count. Zone element constructor projection, +D0 publication, the distinct later +4A4 pointer allocation and complete native resource cleanup are also unjoined. Existing pointer views and zone offset do not authorize those operations by themselves.

## Provenance and limits

Immediate predecessor: /tmp/racer-water-array-backing-20261105-0317, final manifest f3f8916289f89b292b3a473f7203d4963908ac4ac03988f292f2ba000dd5980c, patch 0846989308966b7ed40191c84512222af3916d58f8c8d1898cc8ae0e03817c4a. The trace names its unchanged reviewed source identity a5335abf7ff7be246dc91ecb69a637fd59fc1e236af9b81da3b5e3e6e3e94db1 before final acceptance metadata.

Published main remains e566993fc25f628ce5b4aa5def424c6003f53b75. The complete unpublished chain and narrow process-globals/vector-header rebase qualification remain; publication stays paused.

Verification is static source/ELF inspection, byte/Git-blob identity and strict isolated text-patch reconstruction only. The authored caller and allocation code has not been executed. No compilation, syntax/type checks, builds, tests, probes, helper/model/target execution, emulation, runtime, gameplay or publication are performed or claimed.
