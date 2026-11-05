# Separate success-only native Water array backing

This packet deliberately adapts the unresolved allocation boundary. It does not implement the original EE heap. Owner::allocateWaterArrayBackingNativeOnly requires the distinct WaterArrayBackingMode::nativeSuccessBackingOnly opt-in. It creates private native payload ownership and, for pointer arrays, unknown semantic slot sidecars. It stops before the original caller store or zone-constructor instruction. No original malloc/new[] return, arena read, frame unwind, source pointer bits, Water completion or GPU readiness is claimed.

## Entry, exact retained inputs and distinct resume

Admission requires the fully reviewed allocation-entry state at its pending first arena read, 3059EC or 305AC4; its supported original reent capture and no-op lock history; the same live Config, Water provider and Application; owning thread; no incompatible borrows; no closure/withdrawal; and full existing preflight. It is one-shot. Repeating a request reports alreadyAttempted without allocating, rereading or changing any prior effects. No normal-source continuation calls this adapter.

The original requested word, effective request after operator-new zero coercion, source full register argument, normalization and pending branch remain in allocation-entry history. The adapter separately retains its selected role, allocation count and exact request words. It uses the count that produced that allocation request, not an invented later current count read: the preceding +6C/+64 request snapshot, or the allocation-entry NumZones snapshot.

A successful private native allocation records only its own nativeResumePC:

- Signature pointer array: 2B98DC, before Water +C8 store.
- Pool pointer array: 2B9C58, before Water +68 store.
- Raw zone block: 2B9DB0, before argument setup and wrapper 2E8990.

Original Config and all aggregate source cursors stay 30492C. The allocation-entry PC stays 3059EC or 305AC4, with its large-path pending target unchanged. Wrapper frame/save histories are not marked restored or returned. The new resume site is explicitly a native adaptation across that unresolved boundary, not evidence that an original call returned.

## Explicit native size aperture

The adapter currently admits count at most 16,384 and payload at most 1,048,576 bytes. These are practical native capacity limits, not retail constants, original validation, parsed errors or inferred track sizes. Nothing claims all original inputs fit them.

Using widened unsigned arithmetic, pointer-array payload must equal 4*(count+1); raw zone payload must equal 36*count+16. The result must fit uint32 and size_t, equal the retained original request and equal its effective request. The practical byte cap is separately checked. Count cap, byte cap and request mismatch are separate native refusal categories. A wrapped original request is retained in history and refused rather than silently resized. With the present count cap, accepted requests are positive; the byte/mismatch checks remain explicit even when a stronger earlier cap makes a subcase unreachable.

Zero zones is supported as a 16-byte raw block. A zero pointer-array count would mathematically need one four-byte slot if its preceding source branch/custody admits it. Original zero-request coercion to one remains in history, but wrapped inputs that produced such requests fall outside this reusable count-driven native aperture. The adapter never replaces those original requests with a guessed safe size.

## Fresh native admission, not another source LW

Before preparing storage, the existing process-global owner is checked for current thread and reentrancy knownness. A fresh nativeAdmissionReent is captured separately and must have the supported symbolic reent385160 identity. Unknown and known unsupported identity have distinct native refusal categories.

This is native custody admission, not a replay of source 30570C, not a replacement of its historical capture, and not an errno/arena/new-handler proof. No global knownness is reset or current handler/policy consulted. The sprintf-specific reentrancy bridge is not called.

## Raw payload, semantic slots and roles

The private NativeWaterArrayBacking lives inside the SAME SourceWaterProviderOwner. It is not tied to Config section lifetime. It owns malloc payload bytes through a native free deleter, and pointer-slot sidecars through native array RAII. Native metadata records role, exact count/requests, slot capacity and occurrence epoch 1. The role is signaturePointers, poolPointers or zoneRaw. Role/epoch is scoped to this private same-provider occurrence, distinct from the host allocation address and any EE pointer. The one-shot operation cannot recycle that epoch or overwrite another allocation.

Only exact effective payload bytes are allocated. The normalized allocator chunk is not used as a host payload size. No extra EE header is invented. Payload is not zero-filled and no raw payload byte is read or treated as initialized.

Pointer arrays have count+1 semantic sidecars representing original four-byte slots independently of host pointer width. Every slot starts unknown; its native role pointer and offset defaults are not source NULL bits. Preflight requires all slots to remain unknown and rejects a premature known slot or alias. No host pointer representation is copied into raw four-byte storage. A future genuine projected NULL store could establish four zero bytes; a non-null source store would need semantic target-role custody without fabricated EE address bits. Neither is implemented here.

zoneRaw allocates no pointer-slot or element sidecars in this slice. It is raw backing only. Source wrapper 2E8990 would write element size at raw +0 and count at +4, leave +8..15 unknown, and derive element base raw +16. It would then construct 36-byte elements. All of those writes and constructors remain unentered, including for count zero. No Water +D0 field is published, and the raw base is not confused with a future element-base view.

## Native allocation refusal and transfer

Pointer sidecars are prepared first as unpublished native temporaries. If preparation returns null, the adapter records sidecarAllocation, with no payload attempt or published role. Slot construction only initializes native unknownness metadata; it is not a source element constructor.

The payload allocation is then attempted with std::malloc(exact effective bytes). A null native result records payloadAllocation. Any prepared unpublished sidecars are explicitly reset and their native retirement recorded. The provider remains pristine. This is neither a guest NULL word nor source errno, new-handler, retry, throw or rollback. Earlier source and adapted histories remain intact.

On successful native malloc, all fallible preparation has finished. Payload and optional sidecars transfer into the private provider role, exact metadata is recorded, and nativeResumePC is set. No callback or further fallible work intervenes in that transfer. Native memory may have a reused host address, but it cannot revive another semantic role because role/epoch and same-provider custody are separate.

## Preflight and cleanup

The new preflight participates in the existing chain through allocation-entry. It validates pristine provider backing before an attempt; exact source occurrence/request association; each native capacity/admission/allocation refusal; sidecar preparation and temporary retirement; successful private ownership; exact role/epoch/capacity; and all pointer slots still unknown. It never inspects uninitialized raw payload. Optional zero remains an engaged value.

Before native provider withdrawal, existing code preflights all owned state, withdraws Config file/Track custody and any exact World400 publication, then retires the counted entity. Provider reset destroys these private RAII resources exactly once. A new nativeRetired historical marker is recorded after that reset, without a fallible step in between. After withdrawal, preflight requires the provider to be absent and never dereferences its backing. Failed adapters have no private provider payload and retain their distinct refusal history.

If an earlier withdrawal step refuses, provider backing remains owned. No raw role escapes this minimal pre-store adapter, and no element/resource destructor is needed because no source elements were constructed. Owner destruction also has normal native RAII cleanup. No native cleanup is labelled as original delete[], free, zone destructor or a source NULL field store. Process scalars and all earlier source effects remain unchanged.

## Still missing

Original arena bins/chunks, availability, malloc/free metadata, original return addresses, errno, current new_handler/policy, callback/retry/throw and actual guest pointer bits remain unjoined. The adapter does not solve them. Caller pointer publication, loop current-count rereads, bounds against allocated capacity, signature/pool object construction and Config section reselection remain separate work. Zone cookie/element projection and later destructor/resource ownership also require their own reviewed steps. The later zone-pointer allocation at Water +4A4 is a distinct occurrence and cannot reuse this role or first count snapshot.

## Provenance and limits

Immediate predecessor: /tmp/racer-water-allocation-entry-20261105-0305, final manifest 00eda4cc87f8b89ec3933d7c0fea8a448bae4564d3c3010083aad81769331747, patch d0e66104104742be2d520dce6ee9d7bada82b36f9248e3fb04f9e2b35f2930d0. The provider withdrawal preimage is the exact latest flags-packet source, retained in the predecessor identities; no older provider preimage is substituted. The design evidence's pending-review input references the same unchanged reviewed source identity 4a63ceb537284f6be6640d958aec6abe1b56d1f53e180a1e51a2b62b1afedf4b before final metadata.

Published main remains e566993fc25f628ce5b4aa5def424c6003f53b75. All unpublished dependencies and the narrow process-globals/vector-header rebase qualification remain. Publication stays paused.

Verification is static source/ELF inspection, byte/Git-blob identity and strict isolated text-patch reconstruction only. The new allocation code is authored but never executed here. No compilation, syntax/type checks, builds, tests, probes, helper/model/target execution, emulation, runtime, gameplay or publication are performed or claimed.
