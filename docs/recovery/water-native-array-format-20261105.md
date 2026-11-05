# Native-adapted first array-section sprintf

This packet projects the original fixed sprintf occurrence for the first signature-wave or pool section, using the existing same-process Water formatting bridges and original ELF evidence. It does not invoke the private AudioStream formatter, call host snprintf, substitute a preformatted answer or reuse a previous formatting receipt. Owner::formatWaterArraySectionNativeOnly stops at a NEW same-Config selection entry 157D00, before its lookup, teardown or section loading.

Original Config and aggregate source PCs remain 30492C. Original allocator arena stops 3059EC/305AC4 and pending branch remain unchanged. All formatter and caller return progress here is explicitly native-adapted after the earlier native allocation boundary, not an original allocator return, Water completion or GPU readiness.

## Exact occurrence and output footprint

Admission requires nativeSuccessBackingOnly mode, the same live provider/Config/Application and thread, inherited borrow/closure/withdrawal guards, full preflight, and the predecessor's exact first formatter call at 308C90. Role, immutable literal, argument index one, symbolic stack +710 destination and caller return identify the occurrence:

- Signature literal 3C0AF0, “Signature Wave %d”, return 2B98FC.
- Pool literal 3C0C20, “Pool %d”, return 2B9C78.

Only index one is admitted. Future ordinals need a distinct occurrence and the appropriate original integer-format path, not a replay of this one-shot method.

The retained output owner covers exactly the maximum footprint needed here: 17 bytes. Signature writes 15 prefix bytes plus digit one plus NUL, making 17 known bytes. Pool writes five prefix bytes plus digit one plus NUL, making seven known bytes; the remaining ten bytes are unknown and never read. This is not a claim that the original caller declared a 17-byte or 128-byte C array. No neighboring stack is zero-filled. This buffer is separate from Config name/scratch storage and from native pointer-array payload, and survives through future selection-name consumption.

## Fresh shared authority and formatter frames

308C90 forms the reent global address, 308C94 establishes the symbolic 224-byte wrapper frame, and 308C98 performs a fresh current-reent read BEFORE temporary FILE stores. The exact existing WaterSprintfAccess bridges are used with a local native PC sink:

- readWaterReent308C98.
- initializeWaterSprintf30B528.
- waterLocale305328.
- waterByte306108.

They access the same closed-process owner and shared knownness, not a new FILE/global snapshot. The copied globals reference byte-matches the inherited colors-packet header, SHA256 f1fba582f0f6f3d7efc6dca439ecdf106400b240befa1687030adf38f9cd33cf. No process-global header change is made.

After current reent succeeds, the wrapper saves the known a2 argument and retains save roles for the unused unknown a3/t0..t3 registers and ra. Unused values are never invented or read. It then performs the actual selected temporary FILE stores in order: flags 0x208, output buffer role, capacity 0x7FFFFFFF, captured reent at +54, cursor offset zero, and remaining count 0x7FFFFFFF in the call delay. The source FILE capacity is distinct from the bounded native output footprint.

The 64-byte 30B4F0 frame/save roles precede stdio admission. Shared first initialization happens only if the original owner says +38 is zero and its required state is known; an already initialized occurrence does not reset or pretend to initialize again. Unsupported initialization preserves the local FILE and earlier frame effects. Its failure PCs remain 30B50C, 30B528 or 30B534.

The 720-byte 30B568 core frame/save roles then precede the fresh locale read. The fixed reent/locale/decimal-point authority is checked even though decimal punctuation is not consumed by this integer format. Failure retains the earlier state at 305330, 3052F0 or 30B5A8. Historical captured identities are not asserted to remain current forever; every new bridge read performs its own admission.

Frame masks preserve symbolic save/restore roles and widths without inventing saved register bits, stack addresses or unrelated local fields. The existing shared initializer owns its internal proven effects.

## Scanner and exact single-digit path

The scanner invokes the real Water byte bridge in order for every literal byte through percent: signature offsets 0..15, or pool offsets 0..5. The following d at signature offset 16 or pool offset 6 is read directly by format dispatch, not given another 306108 scan receipt. The original d table entry at 3C68F0 selects 30B920; its signed argument path loads low32 argument one at 30B948 and takes the nonnegative branch.

The original decimal classification takes 30BCE4/30BCE8 to 30BD64 with the less-than-ten condition true. The fast path at 30BDB8..30BDD0 forms 0x30 + argument, decrements its local digit position, masks the byte and establishes the single digit. There is no quotient/remainder call and no division-table dependency. The digit is derived from the retained argument, not copied from a fabricated completed output string.

Each scan retains offset, actual helper return status, length and wide value only when established. Unknown reent/byte-limit ownership stops at 30B658 or 30B65C. A known zero byte limit returns length -1; this is retained and stops at the unsupported scan branch 30B674. It is never silently accepted as an empty/NUL scan. An unreturned scan never invents length or wide output.

## One flush, two ordered segment copies

Exactly ONE 30C9FC flush contains the queued prefix and one-digit segments. There is no prefix-only flush and no second call for the digit. The actual 30B3F0 flush frame is 32 bytes; its 304DF0 writer frame is 144 bytes. Their save/restore roles and successful zero return values are retained separately.

The current string-FILE flag 0x200 bypasses FILE callbacks. The selected known prefix segment is copied through the bounded 304F40/306370 path first, followed by the known digit segment within the same writer loop. Only copied bytes become known. After each segment, source stores update remaining count and cursor in order; the writer's pending residual is then decremented at 305024. The prefix copy leaves one pending byte, and the digit copy leaves zero. The two native segment receipts remain [15,1] or [5,1].

Native output bounds reserve one byte for the later terminator. A bounded copy refusal stops before that segment at 304F40, retaining any previous copied prefix and FILE effects. It is not a guest printf failure, callback, errno or rollback. The accepted fixed footprint fits this aperture; no broader format is admitted.

The writer returns zero and restores its frame. The single flush wrapper clears its count/residual, returns zero and restores its own frame. The core resumes scanning the format's final NUL at signature offset 17 or pool offset 7. If that scan fails, the already copied output remains known without a terminator or formatter return. A successful NUL sees an empty queue and skips the later 30CA24 flush call entirely.

## Formatter return, NUL and new selection entry

With local FILE flags still 0x208, error bit 0x40 is clear. 30CA4C establishes formatter return length 16 or six. The 720-byte core and 64-byte outer formatter frames restore and release in source order before the sprintf wrapper resumes.

308CEC loads the logical cursor, 308CF0 restores ra, and 308CF4 writes NUL at the cursor. This store does NOT advance FILE cursor or decrement remaining count. JR at 308CF8 and its frame-release delay complete the native-adapted wrapper return, retaining the length independently from termination.

The actual caller then supplies the SAME stack +240 Config identity and stack +710 known name argument:

- Signature: setup at 2B98FC, JAL 2B9900, delay 2B9904, future caller return 2B9908.
- Pool: setup at 2B9C78, JAL 2B9C7C, delay 2B9C80, future caller return 2B9C84.

The new native PC stops at 157D00 ENTRY. No new Config is constructed, no old General/Wake selection receipt is reused, no selected child/count/stream cursor is changed, and no original free or future selection return is minted. The formatted name remains privately owned for a later same-Config operation.

## Partial state, preflight and cleanup

The new state and preflight are linked through the array caller. They validate pristine state, exact occurrence, wrapper/local-FILE/frame custody, each bridge refusal, ordered scan receipts, the single-digit path, partial two-segment copies, the one-flush return, final NUL/return and same-Config call entry. Optional zero values, including writer/flush returns and NUL wide output, are treated as engaged values.

Preflight reads only the known output prefix. It does not refresh old reent/stdio receipts or restore poisoned process knownness. New stdio effects are exactly those admitted by the shared original owner. Repeated calls do not rerun them. Config/provider withdrawal retains private name/history while revoking native provider views; no buffer pointer escapes and no output is sent externally.

The zone-constructor path from the immediate predecessor is unchanged. This formatter introduces no native payload allocation, source entity count, source descriptor or renderer access.

## Provenance and limits

Immediate predecessor: /tmp/racer-water-zone-elements-20261105-0338, final manifest 2637f9cd5a3fc3071b2612ca48220ebc0f7eab9f3b48cc1628878ed8e244b3be, patch fa627d96dea878082b5b9327686b23053912ce21b62b00d63bd8229a205557e1. New formatter evidence includes exact wrapper/core/flush/writer words, dispatch-table entry and the same-owner API reference. The AudioStream implementation is a structural reference only; its private loader-specific formatter is not called or given forged custody.

Published main remains e566993fc25f628ce5b4aa5def424c6003f53b75. All unpublished dependencies and the narrow process-globals/vector-header rebase qualification remain. Publication is paused.

Verification is static source/ELF inspection, byte/Git-blob identity and strict isolated text-patch reconstruction only. The authored formatter and allocator code is never executed here. No compilation, syntax/type checks, builds, tests, probes, helper/model/target execution, emulation, runtime, gameplay or publication are performed or claimed.
