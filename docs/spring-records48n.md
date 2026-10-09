# 48N: bounded generated spring record append and reset

This is the native, allocation-free translation of original `002963E0` and
`00296490`, the generated-record append/reset seam. It is not a full detector,
world enumerator, gameplay tick, dynamic contact producer or Xbox runtime test.
Production takes native typed scene/body IDs and caller-owned record storage;
it contains no guest addresses, emulated registers, RAM accessor or interpreter.

## What the original instructions actually do

`2963E0` stages four complete 16-byte vectors before its signed comparison of
generated count `84` and logical capacity `88`. If count is below capacity, it:

1. Copies the four vectors to record offsets `00`, `10`, `20`, and `30`.
2. Copies argument `t1` to other-body field `44`, `t2` to scene field `40`, and
   `t3` to material `4C`.
3. Increments generated count `84` and writes that material to `lastMaterialD4`.

There is no deduplication and no initialization of `margin48`, `flags50` or the
remaining bytes `51..5F`. Existing stale values survive, including every vec4
fourth lane, NaN payloads, signed zero, and any untouched slot. Neither floating
point arithmetic nor finiteness validation belongs to this copy routine. The
meaning of `field00` is left unnamed rather than inferred from its position.
The entire 96-byte record has explicit native storage; unknown tail bytes are
preserved as such, rather than assigned guessed semantics.

`296490` sets `pending38` to **1**, and clears generated count `84`, current
candidate `130`, contact `3C`, point-contact flag `74`, generated-contact flag
`78`, upward flag `40`, and other-body flag `44`. It preserves persistent point
count `7C`, logical generated capacity `88`, last material `D4`, and every record
slot. Reset does not initialize a record or validate append storage.

## Native boundary and failure semantics

`src/game/spring_records48n.h` separates the original signed logical capacity
from the actual size of the caller's backing allocation:

- `Appended`: exactly one slot and the two original metadata fields changed.
- `Saturated`: original `count >= capacity`; nothing changed. Nonnegative
  overfull counts also take this exact no-append branch.
- `InsufficientCapacity`: backing storage does not cover the full declared
  logical capacity, even if this particular call is saturated. This is a native
  workspace error, never accepted partial computation or inferred game policy.
- `InvalidInput`: negative count/capacity, null nonempty storage, an overflowing
  or misaligned declared range, or any input/state/storage overlap. Nothing
  changes. These checks are explicitly native safety restrictions; the original
  machine routine does not validate its pointers, negative metadata or overlap.

Null storage is valid with zero backing capacity and zero logical capacity.
Extra native backing slots are allowed and never touched. The caller owns the
actual objects and must provide storage matching the declared extent; portable
C++ cannot prove that arbitrary non-null pointers denote live allocations.
The caller is also responsible for later resolving nonzero scene/body IDs in its
native registries. Append transports IDs without inventing a validity policy.

All processing is serial. The implementation uses only fixed-size stack locals,
does not allocate or retain caller storage, and has no mutable globals. A record
is 96 bytes; the input is 76 bytes and state is 44 bytes on the tested host. No
desktop-only container, SIMD requirement or CPU-count assumption is introduced.

## Original-word differential evidence

`tools/gameplay/spring_records_fixtures48n.py` verifies the existing 48K evidence
SHA-256 and each extracted block, then executes the unmodified append/reset
words with the existing offline original-word executor. It does not read an ELF
or RAM image, patch instructions, stub calls, or use the native implementation
to calculate expected values. Every byte of the 320-byte original spring
object and every backing slot is initialized explicitly. Unrelated object
bytes are independently checked for preservation after every operation.

The fixtures project fixture-only scene/body tokens into independent native
IDs. This mapping is recorded in `evidence/springRecords48N/report.json`; no
production API uses those machine tokens.

- 264 deterministic cases, including 256 seeded mixed-operation sequences
- 1,566 operations: 600 appends, 556 saturations and 410 resets
- 39,220 executed instructions; all 51 instruction addresses covered
- Both outcomes of the original capacity branch covered
- Zero source-data reads; all reads are initialized fixture bytes or original
  instruction writes
- Bit-exact comparison of all state fields and all 96 record bytes after every
  operation, including unused extra backing slots
- Capacity zero/one/exact/overfull, maximum signed count, stale margin/flags,
  duplicate appends, reset/append reuse, and repeated reset covered

Fixture SHA-256:
`90f123bf67da0e4e80875456a76fa5779ea54cb1fcdd33eaa7c5e3ce1606d562`.
The subset original-word SHA-256 is
`b51c46e984bf9acc25c0ff38ad39cb1cc4751c2d71e461c9d6c23fc162077cad`.

The native test adds transactional guards for malformed count/capacity, short
storage even on saturation, null/overflow/misaligned pointers, all overlap
directions, zero-capacity null storage, and reset's deliberately narrow writes.
Allocation instrumentation encloses every differential append/reset call and a
serial repeated six-record workload. Host microbenchmark numbers are only the
cost of this seam; they are not gameplay or original-Xbox FPS claims.

Both the normal and AddressSanitizer/UndefinedBehaviorSanitizer builds passed
251,985 checks with zero observed allocations. LeakSanitizer alone was disabled
for this executor's ptrace restriction. The release production object has no
undefined external symbols; GCC's host stack report estimates 8 bytes for each
function. Five isolated semantic mutations (wrong pending reset, initialized
stale flags, initialized margin, dropped fourth lanes, and swapped scene/body
IDs) all fail the tests. Detailed hashes, logs and scope caveats are retained in
`evidence/springRecords48N/native-validation.json`.

Reproduce from the repository root:

```sh
python3 tools/gameplay/spring_records_fixtures48n.py
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -fno-fast-math -frounding-math \
  -ffp-contract=off -Isrc/game -Ithird_party/native-json \
  src/game/spring_records48n.cpp tests/native/spring_records48n.cpp \
  -o /tmp/spring_records48n
/tmp/spring_records48n .
```

Use a separate output binary with `-O1 -g -fsanitize=address,undefined
-fno-omit-frame-pointer` for sanitizer checks. Validation output is recorded
separately in `evidence/springRecords48N/native-validation.json` when run.
