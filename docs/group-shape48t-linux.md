# 48T group shape updates and bounded collection efficiency

This packet closes original `135CF0 -> 11F680` group member tag 0 in the
explicit-spatial collector. It also improves two safety-validation costs while
retaining full malformed-input, alias, capacity and transaction guarantees.
48S and earlier recovery archives remain intact; all prior evidence is unchanged.

## Correctness and scope

The group caller prewrites translation before the setter copies the previous
matrix cache. Consequently the completed previous and current matrices contain
the same new translation. Full XYZW matrix composition preserves ordered
binary32 operations. Member sphere center uses the incoming world matrix;
radius is the unscaled geometry-half length. Group ratios are still calculated
but do not scale this tag-0 branch. Repeated/shared shape updates retain source
order and each member's separate sphere. The collector still emits outer-group
identities, including original duplicates and nominal-capacity overshoot.

Optional shape-indexed tables avoid adding matrices to every object. Their
initial/scratch/destination spans and geometry backing join the full-span alias
checks. Unsupported paths and numeric/backing errors preserve every destination
and result byte, even after earlier scratch mutations. Zero shape references
retain the old unsupported boundary, so the original 239-case 48S suite is kept.

See `group-shape48t.md` for the API and exact arithmetic, and
`group-shape-proof48t.md` for original-execution evidence. The latter has 187
helper and 110 collector cases, 264,764 instructions, 33 semantic assertions
and zero source-memory reads. The native helper covers 91 completed group-entry
cases and one pointer boundary; 95 direct-setter/comparison cases remain
explicitly original-only. Collector coverage is 108 completed worlds plus two
pointer boundaries, each tested with four hint modes. No new ISA extension was
needed; all 239 previous collector and 338 vehicle-contact cases reproduce.

The model covers ordinary finite binary32 behavior and signed zeros, not
physical EE/VU exceptional modes. General matrix setters, query shape-0 bounds,
outer-group/compound query expansion, external allocation and unknown callbacks
remain separate boundaries. Inputs are source-derived/synthetic; live world
registration/ownership and the complete vehicle scheduler remain unverified.
No PC or RAM access, reconstruction, transfer, new installation or publication.

## Setup and per-query efficiency

A load/setup caller may provide sorted identity and block reference arrays.
They require four bytes per object or block. Every query verifies exact length,
reference range, current strict ordering and all current physical intervals;
this proves a full permutation without a bitmap or unchecked validation token.
Nested spans and all mutable-state checks remain. If a descriptor changes,
still-valid hints remain safe and invalidated hints reject. Without hints, the
old guarded pairwise path remains available independently for each table.

After bounded acyclic validation, two singly linked block chains intersect
exactly when they share a nonnull terminal reference. This replaces active-by-
spare cross-products without additional storage or changed accepted topology.

Serial synthetic Linux measurements, excluding caller setup:

- 8,192-object disabled-query validation: 25.76 ms unhinted versus 396.7 us
  hinted, with 32 KiB of borrowed references.
- 4,096-block disabled-query validation: 5.59 ms unhinted versus 12.1 us hinted,
  with 16,388 bytes of references including the single object.
- 512 one-entry spare-block selection: frozen 48S 41.42 ms versus 48T 1.375 ms
  without hints, or 1.291 ms with hints.

All 78 baseline/current profile rows pass with zero observed allocation calls.
The synthetic inputs already arrive in sorted order; generic sorting cost is
not measured. See `collection-validation48t.md` and its exact source hashes.
Remaining repeated free-entry-by-spare scans can still produce aggregate cubic
work; ancestor/selected-entry guards remain quadratic. These are isolated host
costs, not a world-scale frame guarantee, Xbox FPS or measured console budget.

## Final native validation

The complete serial retained sweep passes **6,038,250 checks per configuration**
in release and ASan/UBSan. New helper/collector tests contribute 1,308,308.
All 16 new semantic controls fail ordinary assertions with exit 1: thirteen
setter/integration/tail mutants and three supplemental hint-validation mutants.
No crash, harness failure or sanitizer error is counted as a detected mutant.
Zero hot-scope ordinary, array, aligned and aligned-array allocations are observed.
There are no new compiler warnings; three old terrain warnings remain per mode.

LeakSanitizer is explicitly disabled because this executor's ptrace route is
unsupported. CMake, Windows, Xbox, rendering and whole-game execution are not
validated here. The preserved Windows target is extended additively.

Reproduce with new output directories outside the source tree:

    python3 tools/test_native_group_shape48t.py --stack-usage --output-dir /tmp/rr48t-release
    python3 tools/test_native_group_shape48t.py --stack-usage --sanitizers --no-leak-check --output-dir /tmp/rr48t-sanitized

The 48S collector runner still checks all its retained cases and now links the
new helper. Dedicated mutation runners and evidence are described in
`group-shape-tests48t.md`; independent review is `group-shape-review48t.md`.
The portable aggregate is `evidence/nativeGroupShape48T-linux-20261006/validation.json`.
Earlier development runs retain their original inputs/logs and are not final runs.

## Memory and recovery

Each supplied shape uses 192 bytes per initial/scratch/destination state; its
borrowed source is 72 bytes and geometry is 24 bytes on this host. No matrices
are added to each generic object. GCC individual function frames are 464 bytes
for the helper, 1,408 for collection, 208 for open and 48 for pool validation;
nested frames and caller storage are additional, not a measured target stack.
A separate complete collector test process peaks at 15,020 KiB RSS, including
JSON, fixtures, guard tests and diagnostics. Sanitizer compilation peaks around
1 GiB; that is tooling memory, not the shipped runtime or console ceiling.

All generated executables/objects remain outside the source tree. The recovery
ZIP includes source, frozen fixtures, logs, mutant source, stack records and
hashes, with no new compiled build products. Prior baseline files are preserved.

## Next bounded packet

Map and independently prove the outer grouped-collision branch of `122660`,
reusing the now-closed `135CF0` setter and explicit group inputs. This is separate
from candidate shape-1 compound meshes. Existing ELF is sufficient for source
mapping; verify secondary dispatch and callback dependencies before integration.
Further amortization of repeated pool checks is a separately provable efficiency
item. Do not jump from collection/detection to response across unclosed vehicle
stages. Vehicle +344 remains gravity and +354 remains spring; the withdrawn 48J
impact replay remains withdrawn.
