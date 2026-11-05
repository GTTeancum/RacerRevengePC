# Native-adapted Water zone elements and D0 publication

This packet continues the zoneRaw branch from the accepted native caller prefix. Owner::constructWaterZoneElementsNativeOnly projects the verified 2461E0 constructor, its 299AA0 base and the bounded 2E8990 loop into the existing native raw block. It then publishes a private same-provider element view at Water +D0 and stops at 2B9DD4, before the fresh NumZones read and distinct +4A4 allocation.

Original Config and aggregate source PCs remain 30492C. The original allocator arena stop stays 3059EC or 305AC4 with its pending branch unchanged. No original malloc/new[] return, arena ownership, source destructor execution, Water completion or GPU readiness is established.

## Admission and inherited scope

The method requires the explicit nativeSuccessBackingOnly mode, same live provider/Config/Application and owning thread, existing borrow/closure/withdrawal guards and full preflight. The native backing must be zoneRaw epoch 1, with the accepted exact 36*count+16 payload relationship and count at most 16,384. These remain native capacity limits, not source validation. The cookie must already contain size 36 and retained count, with element offset 16.

For a positive count, the predecessor has issued the first constructor call and stopped at 2461E0 before its body. For zero, it has already returned the wrapper to 2B9DD0 without any element call. The new method distinguishes those cases rather than replaying an already completed wrapper. It is one-shot; repeats return the retained outcome without rewriting or publishing again. Pointer-array roles are not admitted by this entry.

The count is the wrapper's retained argument, not a new NumZones read or a saved getter substituted for one. Prior caller and allocator histories stay unchanged. This descendant owns a separate native PC and completion record.

## Source-backed constructor stores

Each admitted element occupies 36 bytes at logical offset 16 + ordinal*36 within the same raw allocation. The source prologue creates a symbolic 32-byte frame and saves ra/s0 with SQ. The first JALR belongs to the preceding packet; this method enters its body without issuing that call again. Later loop iterations issue their own call and delay before entering the body.

Base constructor 299AA0 performs these exact stores:

- +8: original vtable word 0x003D36B0.
- +0: 0xFFFFFFFF.
- +4: zero.

It returns the same symbolic element identity. Derived constructor 2461E0 then performs its ordered stores:

- +8: vtable word 0x003D20C0, replacing the base word.
- +0C and +0D: zero bytes; +0E: byte 255; +0F: zero byte.
- +18 and +1C: words 0x3F800000. These are exact stored bits, with no host floating arithmetic.
- +10: zero word.
- Read the already-written +0F byte, clear bit zero with the source mask, OR the zero selected by ANDI of zero, then write the resulting byte back at +0F.
- +14: zero word; +20 and +22: zero halfwords.

The original vtable words are verified image constants serialized as little-endian data, never cast to host function pointers or C++ vptrs. A separate symbolic NativeWaterZoneVtable distinguishes the base and final derived identities. Dynamic EE allocation addresses are not fabricated.

The only constructor payload read is the +0F byte just written within that same constructor. Unknown raw bytes are not used as inputs. All 36 bytes are established by the supported stores before completion is recorded. Cookie bytes +8..15 remain unknown and untouched.

The source restores ra/s0 and releases the frame in the JR delay before returning to wrapper 2E8A20. No guest function pointer is executed. This is an authored native projection of verified source instructions, not an invocation of original machine code.

## Completed-prefix custody and loop

After a constructor returns, the native completed-prefix count advances exactly once. The backing records the same count and the derived vtable identity for every completed element. Elements at or beyond the completed prefix are never read by preflight. The final constructor's ordinal, offset, save/restore roles, known flag read and symbolic vtable are retained, along with total entered bodies, base returns and constructor returns.

Wrapper continuation reads its ordinal, advances the logical element view by 36, increments/stores ordinal, and performs the unsigned repeat comparison with the retained count. Its branch delay sets a1 to one. With the admitted bounded count these register values are nonnegative and exactly represented; no high-bit domain is silently added. Logical role offsets remain within the actual native payload or at its end. They do not claim a numeric EE base address or prove how an unjoined guest allocation address would wrap.

The first call was issued by the predecessor. Therefore a positive count produces count entered bodies and returns, count post-return comparisons, and count-1 additional JALR calls in this descendant. No heap allocation, callback, input parsing, floating conversion or other fallible operation occurs inside this fixed source constructor loop. Intermediate instruction-local states are not exposed as public refusal points. Completion is tracked per returned element rather than by default-constructing a vector of host objects.

## Wrapper completion and current D0 view

For a positive count, ordinal finally equals count. The wrapper's following comparison is false; the branch delay puts the element-base view in v0. The ten saved wrapper roles are restored and JR/stack-release delay return to 2B9DD0. The new record calls this wrapperReturnProjected. The predecessor's original first-constructor frontier remains historical.

For zero count, no constructor, base call, repeat comparison or new wrapper restore occurs. The already returned predecessor is recorded as wrapperAlreadyReturned. Its element view at offset 16 is a zero-length/end view and is not dereferenced.

At 2B9DD0, the projected SW publishes current nativeArrayD0 as the same provider's zoneRaw role, epoch 1, logical offset 16. This is an adapted semantic view, not raw host pointer bytes or an original EE address. Absence before the projected store was not a fabricated source NULL value. The original constructor zeroWords remain unchanged.

The new native PC stops at 2B9DD4 BEFORE its fresh current NumZones read. The subsequent pointer-array request for +4A4 is a separate allocation occurrence; neither this first count snapshot nor the zone raw role authorizes it.

## Preflight and native cleanup

Preflight is linked through the caller stage. Before this descendant is attempted, it forbids premature D0 publication, element completion or vtable metadata. Afterwards it verifies the exact positive/zero entry, completed count and logical extents, call/return totals, source frontier separation, final constructor state, wrapper completion mode and current D0 role.

For live backing, only the completed elements' known bytes are checked. The nine final little-endian words per element are FFFFFFFF, 00000000, 003D20C0, 00FF0000, 00000000, 00000000, 3F800000, 3F800000, 00000000. The cookie remains checked by the preceding stage; its unknown padding is not read. A zero-count array causes no element-byte read.

The backing's pristine check now also rejects premature zone-element metadata. Earlier pointer sidecars stay unchanged and unknown. No additional native heap allocation or source-counted entity is created by this constructor projection; the supported element constructor only establishes raw field data and symbolic identities.

Before provider backing release, nativeArrayD0 is revoked with the existing +C8/+68 private views. Historical completion data remains, but preflight after native withdrawal never dereferences the retired provider or payload. No source NULL store or original zone destructor is claimed. These default projected elements acquire no external resource lease; their +10/+14 pointer words are established as zero. If later zone parsing introduces resources or aliases, that later work must add its own ownership and native cleanup before release. Raw native RAII cleanup here is not a proof of the full original destructor path.

## Remaining boundaries and provenance

Pointer-path sprintf/section selection is unchanged by this packet. Later zone reads, zone parsing, +4A4 allocation, resource publication, renderer access and complete source heap/destructor behavior remain unjoined.

Immediate predecessor: /tmp/racer-water-array-caller-20261105-0329, final manifest a34b2e97ad102baa45bdcdd96ed2fa1444c9333e73ffffb08412ab05b13a52f6, patch 058c3e5d01eb695e19d6343d35bcba2f2520ac2496ce90eef1b95048e9edd248. Existing inherited array-caller evidence contains exact base/derived constructor words, wrapper loop/restores and caller +D0 store. No new remote source or model is assumed.

Published main remains e566993fc25f628ce5b4aa5def424c6003f53b75. All unpublished dependencies and the narrow process-globals/vector-header rebase qualification remain; publication stays paused.

Verification is static source/ELF inspection, file-byte/Git-blob identity and strict isolated text-patch reconstruction only. The authored constructor and allocator code is never executed here. No compilation, syntax/type checks, builds, tests, probes, helper/model/target execution, emulation, runtime, gameplay or publication are performed or claimed.
