# 48S bounded collection query bounds

`prepareCollectionBounds48S` independently recovers original `1202E0`, the query-bounds helper used before candidate collection. It does not call or substitute `prepareType3Rays48P`: the source function has different writes and operation order.

## Source and boundary

The source is the authorized ELF with SHA-256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. The new `evidence/collectionBounds48S/map` retains five file-backed intervals, 798 instructions total, and the seven-word switch at `388AB0`:

- `1202E0..120E68`: bounds function, including all original switch branches and common tail
- `107910..10793C`: VU minimum helper
- `11E300..11E32C`: VU maximum helper
- `106790..1067AC`: XYZ constructor
- `14D840..14D8BC`: identity constructor

Each interval ends with the authenticated JR and its delay slot. `map/freeze_map.py` checks the ELF SHA, actual PT_LOAD, exact words, binary hashes and endpoints. The retained map hash is `1c7ea5b4e5bc4a347a0cafdea35f869aac50888fa078716d4e6d3ffe31d4b66e`. Existing 48R evidence and runtime remain unchanged.

Supported paths are tags 1 and 6 with source matrix/local center/local half, and tag 3 with a positive signed pair count. Other tags return `UnsupportedBounds`. The full source words are retained for attribution; retaining a branch does not claim its native recovery.

## Exact semantics

Tag 1 reads payload center `+00`, half `+10`, matrix `+80`; tag 6 reads center `+40`, half `+50`, matrix `+00`. Both return the common transformed bounds without changing the query's cached sphere. The native projection therefore passes through cached center and radius as raw words, including NaNs.

Tag 3 reads two Vec4 endpoints per pair. Metadata zero uses the original endpoint; a nonzero scene pointer uses only its cached `+100` matrix. The source does not refresh scenes or inspect parent/dirty flags here. Native metadata uses zero for absence and checked one-based indices into a caller-owned cached-matrix table. Repeated indices represent the same scene identity. An absent metadata span expresses an explicitly all-zero source metadata array; it does not claim the original permits a null metadata pointer.

For each pair, minimum and maximum accumulate the second endpoint before the first. The audited VU signed-word selectors preserve their `-0 < +0` behavior. Their finite ordering can share the corrected scalar order key; this is justified by the separate VU audit, not inferred from scalar MIN.S/MAX.S. Endpoint W, matrix W lanes, unused table entries and tails are opaque.

The tag-3 half is `0.5 * (maximum - minimum)`, and its cached center is `0.5 * (maximum + minimum)`, separately rounded per operation. The cached center write at `120880` precedes the cached radius store in the identity-call delay slot at `1208B4`. Radius preserves `(x*x + y*y) + z*z` and the source square root. The common tail then transforms the center and three independent half-axis vectors using the actual identity matrix constructor. It retains all multiply-zero and add-zero terms. Returned half is `abs(zAxis) + (abs(xAxis) + abs(yAxis))` per lane.

Returned center and cached center must not be conflated: all-negative-zero endpoints yield negative-zero cached XYZ but positive-zero returned XYZ after the common tail. Original returned half.W is never written; the native API intentionally exposes a three-lane half.

## Native safety contract

The result is wholly transactional, including late failures after a prior valid endpoint or transform. Source arrays and descriptors never change. The entire result must be disjoint from the input descriptor and every full declared source span, including inactive tails. Read-only spans can overlap. Declared nonzero spans require aligned, representable, caller-backed storage. Every consumed nonzero scene index is checked before dereference. Capacities are caller-owned; no fixed gameplay pair limit or dynamic workspace is introduced.

A nonpositive count is rejected as `InvalidInput`. Independent original execution documents that zero and negative counts skip the loop and proceed from maximum-float extrema into nonfinite arithmetic; they are not silently treated as an empty finite shape. Nonfinite consumed numeric lanes or intermediate overflow return `NonFinite`. Ignored W values, unused values and untouched cached values may contain arbitrary words.

The arithmetic contract is ordinary separately rounded binary32, round-to-nearest, with no FMA contraction or reassociation. It does not certify all physical EE/VU NaN, denormal, saturation or overflow modes. Matrices need not be orthogonal; half extents need not be positive; no affine-W restriction is invented. Runtime is serial, allocation-free and constant-stack, with no guest addresses, retained references, scene ownership, PC/RAM access or emulator dependency.

## Independent evidence and reproduction

Run from the project root:

```
python3 -B evidence/collectionBounds48S/map/freeze_map.py
python3 -B tools/gameplay/collection_bounds_fixtures48s.py
```

`--frozen` replays the already authenticated words without loading the ELF. The interpreter always starts with `data=b''`, uses a strict explicitly initialized byte overlay, and has zero source reads. It naturally executes the authenticated instructions and dependencies, including calls and delay slots. There are no instruction patches, native-produced expectations, helper-return substitutions, virtual stubs or skipped effects. Passive tracing audits every read/write and proves that initialized bytes outside the allowed stack, result XYZ and tag-3 cache writes remain unchanged.

`fixtures.json` contains 273 original cases: 254 supported complete cases, six unsupported-tag boundaries, ten nonfinite probes and three nonpositive-count probes. Coverage includes tags 1/6, transformed and untransformed endpoints, mixed per-endpoint metadata, shared scene identities, arbitrary ignored W/tails, negative halves, affine matrices, signed zeros, overflow, signed nonpositive counts, 240 seeded randomized cases and a 257-pair case. Native tests compare all 12 exposed result words bit-for-bit for complete fixtures; unsupported/domain probes verify transactional status and bytes. The original suite executes 154,992 instructions with zero source reads.

Native safety tests separately cover short spans, null pointers, alignment, multiplication/address wrap, out-of-range scene indices, count limits, source/result aliases including inactive declared tails, and allocation-free repeated execution. Build the standalone fixture test with `src/game/collection_bounds48s.cpp`, include `src/game` and `third_party/native-json`, C++17, and strict floating flags (`-fno-fast-math -ffp-contract=off`). Release and ASan/UBSan summaries are retained beside the fixtures. The final focused runs each passed 4,884 checks, including all 254 differential cases, 19 source boundary/domain probes, 26 extra guard cases and zero observed allocations. The sanitizer run used `ASAN_OPTIONS=detect_leaks=0:halt_on_error=1` and `UBSAN_OPTIONS=halt_on_error=1`; LeakSanitizer is not claimed. The warmed release one-pair case averaged about 92 ns over 25,000 serial calls on this cloud host; this is a small helper benchmark, not game-frame performance.

`native_sensitivity.py` builds the same harness against test-only native variants: conflating cached/returned type-3 center, omitting scene transforms, replacing radius, reassociating the half sum, and skipping inactive full declared spans. It never edits production files or original source words. Run it only in the serial compiler window. All five controls were detected (3, 1,213, 128, 81 and 2 failed checks respectively), while the unchanged baseline passed. Results and compiler stack-usage entries are stored in `native-sensitivity.json`. GCC 14.2 reports 448 static bytes for the main helper and 8 static bytes for its separate transform helper; neither has a dynamic native stack frame. The measured helper frame is bounded independently of pair count.
