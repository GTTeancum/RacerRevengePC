# Water backing allocation and callback boundary

These additions are detached native preparation, not source owner activation. The numeric allocation recipe and two separately retained aligned buffers support the already-reviewed water packet/fixup leaves. No live caller, World field, source process global, callback, source PC or hardware event is changed. No compilation, tests or runtime were performed.

## Implemented prerequisite

`original_water_readback_layout50la.h` preserves the original274570 dimension tables and exact32-bit constructor/allocation arithmetic. Modes0..2 use four table pairs each; unsupported table indices return no numeric selection instead of simulating an out-of-bounds original read. All other raw modes select the original45/2 default without consuming the table index. Initializer T0 and raw F12 bits retain the original120/8.0 choice for mode0 and90/10.0 for the others. This arithmetic does not enter the factory's environment, file-existence, resource or allocation branches.

Constructor2B8370 writes columns384/DC=low32(3*A2+1), field388=low32(23*columns), bandsD8=A3 and E0=low32(20*bands). Derived initialization2B8660 passes allocator305500 the low32 byte count368*(columns+1)*bands, not368*columns*bands and not a rounded transfer height. Those raw results remain observable even when wrapping. A separate checked native extent requires positive bands and nonnegative unwrapped column arithmetic, with total bytes representable by uint32 and size_t. It does not establish source success.

`original_water_readback_storage_native50la.h` provides two independent, noncopyable native lifetimes:

- Packet backing uses64-aligned1024-byte allocation, initialized to zero, matching2592D0(64,1). Staging the immutable reviewed packet copies only its128-byte prefix. Clearing the native prepared-prefix length does not erase backing. This is not the source32-byte metadata object, shared443D98 publication, guest uncached alias, or Video+88.
- Depth backing uses128-aligned storage with the checked368*(columns+1)*bands size. Its bytes are initially unknown and not read or exposed. The pure terminal fixup may be applied to this independent native storage. Only the exact bytes written by that leaf become readable: each band's final three368-byte records at[0x10,0x100) and[0x110,0x160). Preserved gaps, other records and the extra allocation suffix remain unreadable/unknown. The readable values describe this detached numeric transform, not data received from the GS.

Both allocation APIs require an explicit byte budget. Their failure statuses describe native admission/allocation only. Host allocation failure is not an original305500 result or a2E8D90 new-handler result. No mutable/raw buffer is exported, no foreign completion Boolean is accepted, and no asynchronous operation retains either buffer. A bounded depth window is count-only, uses the reviewed original unrounded band stride and rounded transfer height, and checks the whole window against this allocation. It conveys no source callback index or destination authority.

## Actual ownership and failure ordering

The source has separate owners and separate publications, not an atomic pair:

1. Factory274570 allocates a0x4B0 geometry provider and calls2B8370. Its base2B0800 and member-subobject construction precede the numeric shape writes; remaining constructor fields/globals follow before return. World400 is published at274744. The factory subsequently invokes virtual+74, which resolves to2B8660, and ignores its return.
2. Derived2B8660 calls full base initializer2B0B70 at2B8714. The base loads water/WTT/wake resources and publishes provider443B80 only at2B0F14 on its successful path. Failures leave that global unchanged.
3. Derived2B8660 ignores the base return. It proceeds to allocate depth bytes at2B8740, stores provider+140 at2B8748 and publishes443CE8 at2B8764. There is no null guard and no depth-buffer memset here. This can publish a new depth allocation while443B80 still describes an earlier provider; a future source continuation must preserve that distinction.
4. Provider destructor2B85B0 frees its+140 allocation at2B85E4. It does not clear443CE8. Native RAII destruction here does not claim an original global clear.
5. Factory2747C0 allocates a0x5E0 renderer and enters2B1E60. After base2BA8C0, matrix/vector/array construction,2B1F28 examines shared443D98. Only if absent does it call2E8D90 for32-byte metadata and2592D0(object,64,1) for64-aligned1024-byte backing. The actual source2E8D90 allocator includes retry/new-handler/throw-or-null behavior. A genuine null object result skips2592D0 and publishes null443D98, then the renderer constructor continues.2592D0 itself has no null-backing guard before its guest uncached alias and memset.
6. Renderer destructor2B2230 frees the shared packet through259380 at2B239C and then clears443D98 at2B23A8. There is no reference-count ownership supplied by that global. It must not be replaced with Video+88 or a convenient per-renderer authentic allocation.

These detached native buffers intentionally do not model original constructor returns, allocator retries, partial-failure publication or process-sharing. Those histories require their genuine upstream owner.

## Callback installation is later than allocation

Actual water render2B4C80 selects Camera443D80 from renderer442F70+0 and independently clears443DC8 at2B4D04. Its early exit when rendering/provider state is unavailable occurs before callback installation. The live path updates camera and water render state, obtains color destination443CE0 from the selected texture's virtual+44 view, and runs water renderer/texture/geometry work. Near the end, the selected renderer's virtual operations precede244D50 at2B5644 or2B56B0.244D50 stores callback2B4500 in renderer+DC.

There is no allocation-only path to that callback publication. A zero native busy count, available buffer, prepared packet, or signaled GLsync cannot stand in for the complete source render invocation. Neither allocation helper installs or removes the callback.

Once installed,2B4500 itself reads Video262E90, examines443DC8, queries actual VIF1 DMA status through242D60, and chooses color/depth receive work. Transfer2B80D0 reaches the first real hardware boundary at2B8298: CSR=2,2596A0(packet,1,0), CSR FINISH polling, VIF1_STAT/BUSDIR configuration and receive QWC/MADR/CHCR. Continuation2B46D0 first resets VIF1_STAT/BUSDIR, then schedules another band or executes terminal fixups. Only its terminal branch clears busy and removes the callback at2B488C. Native packet storage, depth storage, numeric fixup and fence completion establish none of those effects, source CSR/INTC/frame readiness, or sky initialization.

## Remaining implementation boundary

The active native water path represents wake/height/signature and its own explicit presentation policy. It does not own the source geometry provider, water renderer, their resource-loading returns, real Camera/texture view, shared packet443D98, source callbackDC, or DMA/GS event path. These new files are reusable memory/arithmetic prerequisites with no active include/caller added. The next authentic join must retain the actual factories/base initializers and their partial-failure behavior before source global/callback publication, then supply a genuine renderer/DMA/GS platform implementation. No readiness gate was added to conceal those missing effects.
