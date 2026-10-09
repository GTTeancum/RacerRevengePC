# 48O: bounded query preparation

The two isolated native helpers pass **20,446 checks against 332 independently
executed original-word cases**, in final Linux x64 release and ASan/UBSan builds.
All measured helper calls allocate zero times. This is numeric/copy-component
validation, not a complete detector, race, Xbox build or Xbox performance result.

## Crucial shape split

`11CA80` and `11FF60` are alternatives in detector `295630`, not consecutive
stages of one type-2 query:

- Detector `+60` nonnull: at `2959DC`, call `11CA80` to set query type **3** and
  copy world endpoint vectors; then `2959E4` skips `11FF60` and goes to dispatch.
- Detector `+60` null: at `2959EC`, call `11FF60` to refresh whatever query shape
  is already configured and then spatial registration.

The existing 48N collision producer is a prepared **type-2** query. Staging
these type-3 world vectors does not establish a valid input to that producer.
A caller must not silently relabel one shape as the other. This helper packet
neither makes that conversion nor supplies type-3 collision dispatch.

## Original source mapping

All words are frozen from the already authorized immutable ELF, SHA-256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`, using
`file_offset = VA - 0x100000 + 0x80`. Shared 48H words are cross-checked. Exact
blocks, pinned hashes and the two necessary dispatch-table words are in
`evidence/queryPrepare48O/original-words.json`; sibling scene mapping matches
its `28B550..28B694` block hash.

### Natural-return type-3 staging: `11CA80..11CB20`

- `11CA80..11CA88`: set `query+88 = 3`. The branch compares the literal 3 to
  zero and cannot skip this store.
- `11CA8C..11CA94`: if `query+8C` is null, return naturally with only tag changed.
- Shape `+0` is the logical pair count; `+4` is a per-vector word destination;
  `+8` is a Vec4 destination. Inputs are `a1 = Vec4 source`, `a2 = optional
  per-vector words`. The two array lengths used are twice the pair count.
- `11CAAC..11CAD4`: for each vector, copy one metadata word if `a2` is nonnull;
  otherwise write zero. There is no floating-point interpretation.
- `11CAD8..11CAFC`: copy one complete 16-byte vector, including its fourth lane.
- `11CB00..11CB14`: signed comparison against the 32-bit shifted count.
  Zero count performs no source or destination reads/writes.
- `11CB18..11CB1C`: natural return. No sphere, matrix, registration or collision
  helper is called. Existing sphere fields and all destination tails remain.

### Type-2 scene/matrix/sphere prefix: `11FF60` to before `11FF74`

- `11FF60..11FF70` calls `11FE50`.
- `11FE50` type-2 dispatch uses original table word `388A28 = 11FED8`.
  The type-3 and type-4 table entries go straight to the return epilogue, so this
  is not a hidden type-3 sphere update.
- `11FED8..11FEE8`: load shape `+44` scene; call `28B550(scene, 0, matrixOut)`.
- Proven `28B550` path: source scene is nonnull, relative scene is null and
  `scene+190 & 1` is set. It copies the sixteen words at `scene+100` and returns.
  Scene parent/local matrices are neither supplied nor read by these fixtures.
- `11FEEC..11FEF4`: call `11F680(query, retrievedMatrix)`; original type-2 table
  word `388A08 = 11FA30` selects its branch.
- Shape `+4C` nonnull: `11FA40..11FB14` copies the matrix to shape `+0`, reads
  the root AABB center at root `+0C/+10/+14` and half extents at `+18/+1C/+20`.
  Query radius is the ordered binary32 square-root length of these local halves.
- Shape `+4C` null: `11FB1C..11FC7C` copies the matrix, reads first endpoint XYZ
  at shape `+50` and second endpoint XYZ 16 bytes later. Midpoint is
  `0.5 * (second + first)` and half is `0.5 * (second - first)`, with individual
  binary32 rounding. Radius is `sqrt((hx*hx + hy*hy) + hz*hz)`.
- Both branches transform only the center through the matrix and set its W to
  1. The radius is **not scaled** for a nonorthogonal basis.
- `11FE28..11FE40` and `11FF4C..11FF58` return the inner helpers normally.
  Execution stops at **`11FF74` before its `jal 120230`**.

The different supplied-matrix wrapper `11FF90` has a registration call at
`11FFA4`, the 48M seam. These addresses must not be interchanged in a proof.
`120230` would build a query AABB using `1202E0` when `query+E0` is nonnull and
then call spatial-index helper `280D40`. This packet does not execute those
calls. Its opaque registration identity is deliberately nonnull and unread;
registration is never bypassed, stubbed or allowed to return artificially.
The detector's endpoint AABB belongs to the separate detector-preparation proof.

## Native APIs and safety domain

`src/game/query_prepare48o.h/.cpp` exposes two allocation-free helpers with
ordinary native storage and no guest pointers or interpreter dependencies.

`stageQueryPairs48O` accepts caller-owned source and destination spans of existing
`SpringVector4_48N`, plus optional source metadata and mandatory destination
metadata for nonzero copies. There is no fixed gameplay capacity. Tests exercise
17 and 1,024 pairs as well as the 0..16-pair reference cases. Pair count and
whatever detector point count produced the source remain independent inputs.

Null shape returns `NoShape` after setting tag 3, without consuming count or any
span. Zero pairs return `Staged` after setting tag 3, likewise without consuming
spans. The nonzero domain requires a doubled count representable as positive
signed32, enough declared backing, aligned/representable complete declared
ranges, and disjoint mutable ranges. Mutable ranges may not overlap either
source range, input descriptor or tag; tag may not overlap a source range.
Read-only source ranges may overlap each other. All failures precede any write.
These native guards intentionally reject unsafe/unsupported inputs instead of
claiming the original handled malformed counts or overlapping arrays safely.

`prepareQuerySphere48O` exposes the proven prepared-scene/type-2 arithmetic seam.
It supports one null-tree float pair, or a supplied pair-tree root center/half.
The original root branch does not consume pair count or endpoints; the API
preserves that distinction. The null-tree branch is deliberately scoped to one
logical pair even though its setter itself reads only the first pair.

Scenes must be finite affine matrices. Consumed XYZ values and every arithmetic
intermediate must be finite; an invalid input or overflow leaves output unchanged.
The unused endpoint W lanes, unused shape branch and stale prior output may hold
arbitrary bits. Dirty scenes, missing shapes and other shape tags are unsupported.
The output is 84 bytes: world16, center4 and radius. On Linux x64 the staging
input descriptor is 40 bytes and the sphere input is 132 bytes. Widths of span
pointers/size_t are platform-dependent; these are not Xbox memory measurements.

## Independent evidence and checks

`tools/gameplay/query_prepare_fixtures48o.py` starts `Original(data=b'')` with
only explicitly initialized tiny synthetic objects. Every missing read fails.
It uses the unchanged `query_oracle48h.Original`; no instruction interception,
helper stubs, patched instructions, native expected values, RAM reads, dump
reconstruction or game process is involved.

The 332 cases execute 83,592 original instructions, cover 283 unique words and
perform zero source-byte reads:

- 84 staging cases: null/present shape, null/present metadata source, zero and
  varying pair counts, old tags 2/3/all-ones, arbitrary raw payloads including
  signaling-NaN bits, signed zero, full W lanes, and retained destination tails
- 248 sphere cases: both root-AABB and float-pair branches, identity/translated/
  rotated/nonorthogonal/zero/signed-zero affine bases, zero-length/reversed/
  tiny pairs, prepared-scene flag values 1 and 255, and 128 deterministic cases

Frozen replay (`python3 tools/gameplay/query_prepare_fixtures48o.py --frozen`)
reproduces fixtures, original-word manifest and proof byte-for-byte without
reading the ELF. The generator's unmodified source reads the ELF only to extract
instructions when `--frozen` is absent. Exact hashes are in `proof.json` and
`native-validation.json`.

`tests/native/query_prepare48o.cpp` compares every exposed result bit, full
staging tails and stale query sphere sentinels; separately checks native guards,
null-shape/zero-count nonconsumption, variable capacity, overlap, misalignment,
size overflow, unchanged failed outputs, unused nonfinite values and allocations.
Both direct builds use C++17, strict binary32 flags (`-fno-fast-math`,
`-ffp-contract=off`), `-Wall -Wextra -Werror`. The sanitizer build adds ASan and
UBSan; LeakSanitizer is explicitly disabled because tracing blocks it here.
No leak-check result is claimed.

Five compiled native mutations are rejected by the same fixed original fixtures:
wrong shape tag, nonzero null-source metadata, omitted final vector, doubled
radius and missing center translation. `sensitivity.py` and `sensitivity.json`
retain commands/output evidence. The mutants never supply expected values.

## Remaining scope

No live scene/mesh binding, registration maintenance, candidate discovery,
material/category binding, dynamic-body synchronization, type-3 collision
producer, vehicle scheduling, controller/clock mapping or end-to-end tick is
claimed. The separate detector proof may compose the type-3 staging helper with
its authenticated prefix, but type-2 sphere proof remains an independent branch.
Arithmetic results describe ordinary host binary32, not every physical PS2 EE/VU
exceptional or denormal mode. No build or performance result for original Xbox
hardware follows from these Linux x64 checks.

Final aggregate reports are in `evidence/nativeDetector48O-corrected-linux-20261006/`.
The initial standalone validation remains historical evidence; final runner
reports include the additional declared-tail and descriptor-alias rejection tests.
