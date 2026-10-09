# 48S: isolated first-list-then-second-list candidate merge

## Scope and authenticated boundary

This closes only original `219810`'s merge interval
**`[219B1C,219BD0)`**, after both `122350` calls and before the detector pointer
load. The 45 instructions (180 bytes) have SHA-256
`8c91c494d42df29fb8689683b8d508dee7ef7c6ee1c7c7658a7c86e40def6d59`.
Execution stops before `219BD0`; it does **not** claim a natural return from
`219810`, execute `122350`, enumerate a runtime world, or invoke a detector.

`tools/gameplay/candidate_merge_fixtures48s.py` independently verifies the ELF
SHA-256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`
at `work/native/project/disc/SLUS_202.68`, parses its actual ELF32 little-endian
program header, checks the unique covering file-backed `PT_LOAD`, and extracts
the original words. The segment is `(1,128,1048576,1048576,2972160,3470848,7,128)`;
file offset is `VA - 0x100000 + 0x80`. The prior 48R map supplies word-matched
annotations, not executable substitutions or expected native outputs.
All existing 48R source/evidence files remain unchanged.

## Exact original order and memory consumption

The supplied `s3` is the vehicle. Entry `219B1C` writes zero to vehicle `+384`.
The first loop uses signed count `+58C` and words `+590[i]`; the second uses
signed count `+488` and words `+48C[i]`. Both append to `+388[outputCount]`.

- `219B68` and `219BC0` reload their signed source count on every loop test.
  Zero and negative counts skip the corresponding body and all list reads.
- Each positive-count iteration first reads output count (`219B28/219B80`)
  and compares it signed-less-than 64.
- If below 64, `219B40/219B98` reads the next raw 32-bit source identity,
  writes it to the output, rereads output count and increments it.
- At capacity, the **taken branch-likely** `219B30/219B88` executes its index
  increment delay slot `219B34/219B8C` and jumps past the copy. It does not read
  the skipped source identity. The loop still advances over all remaining
  logical indices and rereads the source count; it does not break early.
- Below capacity the branch-likely delay slot is annulled. The ordinary index
  increment is instead `219B60/219BB8`.
- The first source is exhausted logically before the second loop starts.
  The output is not sorted or deduplicated. Raw zero, repeated values,
  high-bit values and `0xFFFFFFFF` are ordinary copied words; no identity is
  dereferenced by this interval.

For immutable supplied counts, let `a=min(max(firstCount,0),64)` and
`b=min(max(secondCount,0),64-a)`. Actual list loads are precisely
`first[0..a)` followed by `second[0..b)`. Source-count loads are respectively
`max(firstCount,0)+1` and `max(secondCount,0)+1`. Output-count loads are
`max(firstCount,0)+max(secondCount,0)+a+b`. There are `1+a+b` output-count stores
and `a+b` output-slot stores. Output slots from `a+b` through 63 are preserved.
The formulas are checked against logged original instruction accesses.

The distinct original arrays occupy at most 64 source/output words before the
next count field. A logical source count can exceed its initialized backing
without an invalid original read if the 64-copy gate makes that suffix unread.
It cannot excuse an absent word in a reached prefix.

## Native API and explicit safety policy

`src/game/candidate_merge48s.{h,cpp}` provides standalone
`mergeCandidates48S(input,scratch,destination,result)`. It has no runtime-world
dependency. Each input list supplies an immutable signed count, a native pointer
to raw identities, and a separate backing count. Each caller-owned writable
buffer supplies a pointer and capacity. The result supplies the replacement
signed output count. Initial result count has no effect.

The API requires backing for `a` and `b`, not each full logical count. It also
requires scratch and destination capacity for `a+b`, not necessarily 64. A
logical count of `INT_MAX` therefore requires at most 64 source words. This
bounded implementation deliberately collapses original saturated count-only
iterations under an immutable, nonvolatile, callback-free input contract.
It does not preserve instruction count, timing, or concurrently changing counts.

All nonempty declared spans must name real caller-owned, aligned storage with
representable extents. **Full declared extents**, including unread and unwritten
tails, participate in alias validation. Scratch, destination, and result are
mutually disjoint and disjoint from both input arrays and all three input/buffer
descriptors. Read-only spans may alias each other. The caller need initialize
only actually consumed source elements; no unused source tail is read.

Statuses distinguish `Complete`, `InvalidInput`, `InsufficientBacking`, and
`InsufficientCapacity`. Every error preserves **all scratch, destination and
result bytes**. These structural checks and transactional errors are native
safety policy, not an assertion that original instructions roll back writes.
On completion only scratch/destination prefixes and result count are changed;
both writable tails retain their original bits.

Production work is allocation-free, serial, constant-stack and bounded by 64
source-identity reads, 128 list-word writes and one count write. Caller scratch
is at most 256 bytes for the natural cap; output is another 256 bytes. No heap,
growing containers, threads, retained pointers, dynamic registration, guest
addresses, floating-point operations or global runtime state are introduced.

## Independent execution evidence

The generator executes genuine frozen instructions with the hash-pinned
`query_oracle48h.Original` interpreter. Its subclass only logs memory accesses;
it does not override instruction semantics. Every machine is created with
**`data=b''`**, explicit initialized bytes and strict uninitialized-read errors.
No RAM image, helper stub, instruction patch, native implementation or native
expected result participates. Expected output/count are read directly from the
executed original memory overlay.

Current results in `evidence/candidateMerge48S/`:

- 274 deterministic cases, 409,136 executed instructions; all 45 interval words
  covered and both outcomes of all four conditional branches exercised.
- Positive, zero and negative counts including `INT_MIN`; exact combined
  63/64/65 boundaries; first-source and second-source saturation; preserved
  initialized unused tails; raw zero/repeated IDs; and logical counts up to
  4096 with exactly the reached prefixes backed.
- Exact source/count-read audits and three strict-initialization negative
  controls. Missing reached first/second identity or first count raises an
  uninitialized-read error; unbacked saturated suffixes are never read.
- `source_reads=0`, no original call interception, and the same explicit stop
  PC `219BD0` for every successful fixture.

`original-words.json/txt` freezes the interval; `native-fixtures.json` contains
executed expected outputs; `read-audit.json` records each case's access counts,
trace hashes and branch outcomes; `report.json` pins generator/interpreter,
ELF/map, fixture and original-word hashes. Positive `INT_MAX` is tested only
natively: no claim is made to execute billions of original saturated iterations.

## Native verification and reproducibility

`tests/native/candidate_merge48s.cpp` passes **40,903 checks across 323 focused
calls**, plus 180,000 serial benchmark calls: all 274 original differential
cases, full source/tail preservation, structural invalid-input/capacity guards,
full-span aliases, read-only aliases, exact minimal backing, signed extremes and
transactional errors. Linux protected pages immediately after every consumed
source/output extent confirm no eager reads or extra writes. Completely
unconsumed lists can point at inaccessible pages with zero declared backing.
Observed production allocations are zero.

`evidence/candidateMerge48S/run_mutations.py` independently rebuilds the normal
harness and four compiled production mutants. The unchanged baseline passes.
Swapping source order yields 14,534 assertion failures; moving the cap to 63
yields 302; moving it to 65 yields 10,323; clearing the unused destination tail
yields 6,824. **All four exit normally with status 1 and a completed assertion
summary.** Their generated sources, hashes, build logs and test logs are under
`native-validation/mutations/`; production source is never modified.

A fifth mutation adds a second-source read after the first list fills all 64
slots. It uses a separate minimal protected-page probe: the baseline exits 0;
the mutant reaches `PROT_NONE` and terminates with `SIGSEGV` (exit code -11 in
the subprocess API). This is a **memory-access finding, not a semantic mutant
assertion detection**, and is recorded separately in the mutation report.
Core dumps are disabled for that deliberate probe. There are four semantic
detections and one distinct guarded-access finding, not five assertion kills.

GCC 14.2.0 strict release (`-std=c++17 -O2 -Wall -Wextra -Werror -pedantic`)
and AddressSanitizer/UndefinedBehaviorSanitizer runs both pass. LeakSanitizer
cannot run in this container's ptrace environment, so the passing sanitizer
run uses `ASAN_OPTIONS=detect_leaks=0`; its unavailable attempt is retained in
the logs. Allocation instrumentation remains active. No physical-console or
whole-game memory/performance conclusion follows from these host tests.

The release production stack report is **120 static bytes** on this host.
The recorded nine-batch host microbenchmark reports median 83.3315 ns and
maximum batch mean 88.8465 ns per 64-word merge, including span validation.
These are observations, not budgets or Xbox FPS claims. Release compile peak
RSS was 288,664 KiB; sanitizer compile peak RSS was 387,376 KiB, serially.

Reproduce from the repository root:

```sh
python3 tools/gameplay/candidate_merge_fixtures48s.py
g++ -std=c++17 -O2 -Wall -Wextra -Werror -pedantic -fstack-usage \
  -Isrc/game -Ithird_party/native-json tests/native/candidate_merge48s.cpp \
  src/game/candidate_merge48s.cpp -o /tmp/candidate_merge48s
/tmp/candidate_merge48s .
g++ -std=c++17 -O1 -g0 -Wall -Wextra -Werror -pedantic \
  -fsanitize=address,undefined -fno-omit-frame-pointer \
  -Isrc/game -Ithird_party/native-json tests/native/candidate_merge48s.cpp \
  src/game/candidate_merge48s.cpp -o /tmp/candidate_merge48s_sanitized
ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 \
  /tmp/candidate_merge48s_sanitized .
# Run serially; creates only its own evidence subdirectory.
python3 evidence/candidateMerge48S/run_mutations.py
```

## Remaining integration boundary

This API is usable independently by a future first-substep composition, but
does not prove which candidates `122350` produces, the detector's subsequent
use of the merged list, full vehicle-substep sequencing, real world ownership,
runtime geometry coverage, scene registration, rendering, original-clock
behavior, Xbox FPS or whole-game RAM acceptance. Those remain separate work.
