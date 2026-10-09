# Static type2 spring callback (48N)

This is an allocation-free, caller-bounded recovery of the static type2 branch
of original `294BB0`, with original `2963E0` record append. It is a component
boundary, not a complete detector, a full vehicle step, or a playable race.
It does not depend on a live memory image, emulation at runtime, or a PC setup.

## Verified source and ordering

The preserved ELF is `SLUS_202.68`, SHA-256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
The byte ranges, per-instruction words, and hashes are in
`evidence/springCallback48N/original-words.json`. Extraction uses the original
PT_LOAD mapping, file offset = virtual address - 0x100000 + 0x80. Original
`295448..295458` forms `294BB0` and stores it at the detector query's +E4.

The recovered callback sequence is:

1. `294BD8..294BE4`: category & global3E0728 skips everything, before loading
   the spring object or touching its saved candidate identity.
2. `294BF4..294C18`: replace spring+130 only when it is zero or the previous
   candidate's category intersects global3E0718. A previous static candidate
   persists even when later candidates append more records.
3. `294C28`: incoming dynamic categories take virtual-body paths +A4/+A0/+98.
   That branch is explicitly unsupported by this API.
4. `294D30..294DD0`: static material categories intersect global3E0720. A null
   candidate+9C scene gives material0. Otherwise scan scene+38 to the first dot
   or NUL. The immediately preceding three bytes must be `frX`; then material
   is signed-byte(X) minus 48, interpreted as the original 32-bit word. There
   is no digit validation: `frA` gives17, `fr/` givesFFFFFFFF, and a byteFF
   givesFFFFFFCF. Uppercase `FR` does not match. A later dot is irrelevant.
5. `294DDC..294E7C`: only query tag2 is supported here. Iterate every current
   result+60 pair, in order. Endpoint vec4s from result+50 go directly to
   record00/10, result+54 normals to20, result+58 surface points to30. Scene
   identity is query-shape+44, not the material scene; other-body is zero.
   There is no per-pair hit check, transformation, normalization, or dedupe.
6. `2963E0`: append until original signed capacity is saturated. Saturation
   neither updates materialD4 nor initializes margin48/flags50/tail bytes.

No reset is inside `294BB0`. The query owner invokes the separately recovered
`resetSpringRecords48N` only where the original query orchestration does so.

## Cumulative pair and fourth-lane contract

`SpringCumulativePair48N` deliberately carries all four lanes. The original
callback and append copy raw 128-bit vectors. Even unusual W values, signed
zeros, infinities and NaN payloads pass through unchanged; no floating-point
arithmetic is performed by this callback.

Original `10882C..10885C` inserts the query's endpoint vec4s into the pair
result unchanged. `1087A4..1087E4` updates a strictly-nearer existing pair's
normal/point but leaves endpoints unchanged. Thus ordered candidate callbacks
must consume cumulative per-pair results. Taking only the latest candidate's
hit, or only the final globally selected hit, is not equivalent. Later
callbacks can re-append earlier pairs under the later callback's material.

`world-lane-probes.json` executes the authentic `121590..121634` world-result
loop at a fully initialized boundary, covering sixteen incoming-W/flag
combinations. It establishes:

- Already-world-transformed pairs are not rewritten; their W bits persist.
- An unflagged normal is transformed in XYZ; `1215D8` adds vf0 to all four
  vf9 lanes. Its W comes from incoming vf9.W + 1, not its input normal.W.
- `1215FC` explicitly sets vf9.W=1 before storing the surface point.
- Subsequent unflagged pair normals therefore receive W=2.

`10838C` and `1083C0` set vf9.W=1 in the frozen leaf-initialization block.
The boundary probes alone do not claim full query execution; an integrating
query oracle should verify the incoming vf9.W chain for its supported paths.
Do not manufacture W=1 for every vector. XYZ-only tree APIs cannot by
themselves preserve caller-supplied endpoint W values.

## Native safety boundary

The API is in `src/game/spring_callback48n.h`. It consumes explicit native IDs,
category masks, previous-candidate category metadata, the bounded cumulative
pair span, a bounded material-name span, and caller-owned record storage.
These identities are not guest addresses. Query eligibility filters and
candidate production belong to the integrating query owner.

- `Skipped`: skip mask matched; all writable arguments remain untouched,
  even when unconsumed fields are invalid or unsupported.
- `Complete`: static type2 callback completed. Diagnostics report appends,
  original-capacity saturation, and the parsed material.
- `UnsupportedScope`: incoming dynamic category, non-type2 tag, or a first
  dot/NUL before byte3. The short-name case could read preceding scene bytes
  in the original and is not guessed from nonexistent caller storage.
- `InvalidInput`: invalid pointer extent/alignment/overlap, zero candidate
  identity, negative logical record metadata, excessive pair count, or a
  required name span with no bounded dot/NUL.
- `InsufficientCapacity`: caller storage cannot hold the complete logical
  record capacity. It is not reported as original saturation.

All errors are transactional. Pair/name spans and descriptors must be disjoint
from record/state/result writable ranges; writable ranges must be mutually
disjoint. No heap storage, lock, runtime interpreter, or mutable global is used.
The append API preserves the complete96-byte original record, including stale
margin48, flags50 and tail bytes. Caller lifetimes and nonconcurrent ownership
remain ordinary C++ preconditions.

## Verification

`spring_callback_fixtures48n.py` froze original callback, append, strlen,
initializer anchor, pair transform, and leaf W initialization directly from
the hash-verified ELF. Its640 fixtures execute the original callback and append
end to end on explicit synthetic objects, with109,259 original instructions,
zero snapshot/source-data reads, no patched instructions and no native expected
values. No RAM file is read or reconstructed. Original strlen executes its
unaligned-name scalar path; its optimized aligned paths are preserved but not
claimed as executed by this corpus.

The binary fixture SHA-256 is
`004699580bd29f073daf42898c472c3bff473a27f0c92d47a281a638bc071683`.
`--frozen` regenerates byte-identical fixtures using pinned frozen block hashes
without reading the ELF. It does not silently substitute expected outputs.

The standalone native test passed87,176 checks, including all state words and
all120 record words per case. The corpus yields297 appends,694 saturated
attempts and144 skipped callbacks. Native safety checks cover unsupported
branches, capacities, short/unterminated names, invalid spans, aliases, stale
bytes, repeated callback order, and skip-before-validation behavior.

Normal and AddressSanitizer/UndefinedBehaviorSanitizer runs pass. LeakSanitizer
cannot operate under this container's tracing setup; it was disabled for the
successful ASan/UBSan run, rather than reported as passing. Instrumented
allocation counts remain zero through200,000 serial callback repetitions.
Host timings are recorded in `native-validation.json`; they are not original
Xbox timing, gameplay FPS, or a native race benchmark.

Six mutations of the native implementation were each rejected: ignoring skip,
replacing an earlier static identity, forcing normal.W=1, appending only the
last pair, clamping materials to digits, and clearing stale record flags. The
original oracle instructions were not modified for mutation testing.

From the repository root, using the existing compiler only:

```
python3 tools/gameplay/spring_callback_fixtures48n.py --frozen
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -Werror \
  -fno-fast-math -ffp-contract=off -I src/game \
  src/game/spring_records48n.cpp src/game/spring_callback48n.cpp \
  tests/native/spring_callback48n.cpp -o /tmp/spring-callback48n-test
/tmp/spring-callback48n-test .
```

Dynamic-body virtual callbacks, tags3/5, material-name underreads, spatial
registration/world candidate discovery, object construction, a saved live
spring-to-body binding, and full-race performance are not established here.
