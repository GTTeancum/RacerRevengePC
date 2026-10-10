# Static type3 spring callback 48P

## Supported original scope

This recovery executes the original `294BB0` callback with static categories and query tag3. It freezes the full original callback, `2963E0` record append, scalar-path `3093A8` strlen, `295260` XYZ-copy/one-W helper and `106790` XYZ-constructor/one-W helper. ELF SHA-256 is `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`; extraction uses `file_offset = virtual_address - 0x100000 + 0x80`.

The type3 callback operates after query preparation and narrowphase. This component does not execute query dispatch, refresh a scene, establish live detector ownership, run dynamic virtual-body calls, register a world query, or handle tags2/5.

## Exact callback reads and order

1. The common prefix tests candidate category against skipMask before any spring/query data use. A skipped call changes nothing.
2. It replaces spring+130 only when the prior identity is zero or the prior candidate category matches dynamicMask. Static material parsing is identical to48N: the first dot or NUL terminates the suffix, and lowercase `frX` immediately before it yields signed-byte(X) minus48, without digit validation.
3. Type3 enters at294E84. It iterates the **signed spring+7C point count**, not shape+0 or a result count. For each ascending index `i`, it reads result+68[i]. A zero hit word skips the entire record and its arithmetic.
4. At294EC0 it loads the pointer stored at shape+14. Helper295260 copies exactly the three floats at offsets0/4/8 and supplies W1. The pointer is **not advanced by i**. Query preparation identifies this storage as the per-pair length array. Therefore every hit samples **length[0], length[1], length[2]**, including initialized stale backing when the shape has fewer than three pairs.
5. It loads `worldDirection[i]` through shape+1C and `worldStart[i]` through shape+18. Each reconstructed endpoint coordinate is individually rounded `worldStart[i][axis] + worldDirection[i][axis] * length[axis]`. It does not substitute length[i] or a nearest-hit fraction.
6. For the reconstructed endpoint first and start second, it loads the cached body scene matrix from `(query+A0)->+288`, then scene+100. It performs a transpose-3x3 transform after subtracting translation. There is no dirty-flag test or scene-refresh call.
7. The original VU sequence adds positive zero to matrix columns before transposition, including translation. The six off-diagonal transposed copies also add positive zero. Thus negative matrix zeros become positive zero before use. XYZ subtraction, multiplication and each accumulation are separately rounded binary32 operations. Both local output fourth lanes are explicitly1, regardless of source or matrix fourth lanes.
8. It calls original2963E0 with localStart, localEndpoint, result+54[i] normal and result+58[i] surface point. Normal and surface are copied as complete vec4 bit patterns, including arbitrary fourth lanes and nonfinite payloads. Scene identity is the **i-th 32-bit word at shape+4**, not the 2*i metadata word used by query preparation; other-body identity is0. Material is the common-prefix result.
9. All nonzero hit words cause an append attempt, including when record capacity is saturated. Record order is hit index order. Original logical saturation succeeds without changing record/state bytes; successful appends preserve margin48, flags50 and bytes51..5F. No callback reset occurs.

## Original-execution fixtures

Run `python tools/gameplay/spring_type3_callback_fixtures48p.py --frozen` to reproduce using the self-contained, hash-pinned original instruction files without requiring the ELF.

The768 fixed-seed cases execute209401 original instructions, starting at294BB0 and returning normally. All data objects, record storage, names and global mask words are explicit bounded synthetic inputs, with `data=b''` and zero source reads. No original instruction is changed and no helper is replaced or intercepted. Expected outputs come only from the original instruction interpreter. The model is ordinary individually rounded host IEEE binary32, not a claim about every physical EE exceptional arithmetic mode.

Coverage includes all static prefix branches, hit and missed pairs, positive/zero/negative signed point counts, unusual nonzero hit words, nonorthogonal matrices, signed-zero matrix copies, arbitrary ignored W values, raw nonfinite normal/surface payloads, stale bytes, previous static/dynamic identities, material suffix variants, and initial record counts below/at/above capacity.

The binary has a two-word count/width header and378 words per case:

-12 scalar input fields
-11 initial spring state words
-8 name words
-4 common-length words (only first3 consumed)
-16 body-scene matrix words
-4 pair descriptors of18 words each: start4, direction4, normal4, surface4, hit1, scene1
-120 initial record words (five96-byte records)
-11 expected spring state words
-120 expected record words, including every preserved byte
-4 expected summary words: skipped, appended, saturated, material

Fixtures represent callback inputs independently of query preparation. Query-composition evidence must separately establish every derived length, direction, mask and per-pair scene value. Repeated static callbacks are not cumulative type2 callbacks: a type3 dispatcher resets candidate-local hit masks before each candidate narrowphase.

## Native boundary

`applySpringStaticType3Callback48P` is allocation-free and uses zero-copy bounded arrays. Its two descriptors live in `src/game/spring_type3_callback48p.h`. It reuses the existing native48N static prefix with no pairs on a staged state, preserving material and current-candidate semantics without duplicating them. This native implementation reuse does not alter the original-execution oracle, which runs the authentic type3 branch and original helpers end to end.

After a skip-first test, all written spans are validated for alignment, representable sizes and overlap. For positive signed counts, hit backing must cover the full loop. If any hit exists, other per-pair arrays must cover the highest hit index and lengths must contain at least three words. Entire declared consumed spans, including unused tails, participate in alias checks. No-hit calls ignore body-scene data and non-hit arrays. Negative point counts also ignore hit storage. Read-only spans may overlap one another.

Consumed XYZ values and arithmetic intermediates must be finite. Matrix and vector W lanes are not arithmetic inputs and impose no affine-W restriction. Copied normal/surface payloads remain raw bytes. A first arithmetic pass validates every hit, including saturated append attempts, before state or record mutation. A second bounded pass recomputes the same values and invokes the existing48N append helper. Invalid inputs, unsupported scope and insufficient capacity leave state, every record byte and the result unchanged. Successful calls preserve original logical saturation and stale record fields.

This native callback does not require a fixed gameplay cap; variable spans are caller-owned. Stack use is constant, there is no heap allocation, and calls are serial. The component's arithmetic proof requires strict floating-point compiler flags with contraction and fast math disabled.

## Verification results

Both optimized strict-float and AddressSanitizer/UndefinedBehaviorSanitizer (`ASAN_OPTIONS=detect_leaks=0`) builds passed768 original-execution fixtures and105407 checks, with zero failures. Builds use `-Wall -Wextra -Werror` and produce no warnings. Original outcomes total270 successful appends,533 saturated attempts and176 skip-mask returns. Each native configuration also performs100000 serial repeated callbacks with zero observed heap allocations.

Safety coverage verifies unsupported tags/dynamic categories, material/name bounds, null/misaligned/overflowing spans, insufficient backing, aliases involving records/state/result/descriptors/data, finite arithmetic and overflow rejection, no-hit/negative-count bypasses, first-three length sampling, raw metadata indexing, complete fourth-lane behavior, repeated append ordering, saturation and an untouched record canary. Artifacts `native-normal.json`, `native-sanitized.json` and `native-validation.json` contain results and exact source hashes. Host microtiming in the logs is an isolated component observation, not a whole-race performance claim.

## Final metadata sensitivity control

A temporary compiled mutation replacing metadata[i] with metadata[0] was
rejected by 140 runtime assertions in the 768-case standalone callback fixture.
Compilation succeeded; production source was unchanged. This proves a
distinguishable metadata-selection path in the standalone callback and does
not broaden the composed query's zero-scene scope. Exact commands, hashes and
diagnostics are in metadata-sensitivity.json beside the fixture;
check-metadata-sensitivity.py reproduces it outside the source tree.
