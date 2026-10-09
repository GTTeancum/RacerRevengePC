# 48P original mode1 node predicates

## Supported boundary

`type3SphereBox48P` and `type3RayBox48P` in
`src/game/collision_type3_node48p.{h,cpp}` implement the numeric predicates in
original `10E080` mode1. They are pure, fixed-stack, allocation-free, serial
functions returning `Overlap`, `Separated`, or `InvalidInput`. They have no
query/result writes, global state, guest pointers, or temporary segment arrays.

The original predicates can be separated without retaining hidden numeric state:
the overall-center predicate only determines whether to enter the ray loop;
each ray starts with independent start/direction/length data and the node
center/half extents. The original `s2` overlap flag is initialized to zero before
the loop, and the first overlap immediately takes the child path. A caller must:

1. Test the candidate-local overall center and unchanged query radius first.
2. If it passes, test rays in input index order until one overlaps.
3. On that first overlap, traverse child `+8` and then child `+A` exactly once,
   passing **all** rays to both children. Do not repeat traversal per hit ray.
4. With an empty ray sequence, traverse neither child. Array/count validation,
   capacity checks, traversal ordering, result transactionality, and recursion
   replacement belong to the caller, not these single-value predicates.

No mode0, triangle leaf, descriptor lookup, candidate dispatch, dynamic world,
PC installation, saved RAM, or live-game behavior is claimed here.

## Source operation order

The immutable source is `evidence/type3Query48P/map/original-words.json` plus
`mesh_walk.bin`/`.txt`. The ELF identity is
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`;
the complete frozen mesh-walk block hashes to
`addcba4fd67f048e49d4ef76a33848dd5f7462853af38f2f38d9e438006d7737`.

- `10ED7C..10EE40`: compute `boxCenter - localCenter`, then three ordered
  inclusive comparisons `abs(delta) <= boxHalf + radius`, X then Y then Z.
  This is the original radius-expanded box predicate, not an exact Euclidean
  sphere-versus-box distance test. An expanded-box corner can pass.
- `10EE60..10EF34`: compute `boxCenter - start`; an inclusive XYZ inside test
  passes immediately. No positive-length requirement is imposed here.
- Faces run in source order: +X (`10EF38`), -X (`10F004`), +Z (`10F0D0`),
  -Z (`10F19C`), +Y (`10F268`), -Y (`10F334`). Each computes
  `(rounded(center +/- half) - start) / direction` in separate binary32 steps.
- Each face rejects on `time < 0`, then on **not** `time <= length`. It then
  checks `abs((start + rounded(time * direction)) - center) <= half` for the
  other coordinates. X faces check Y then Z; Z faces check Y then X;
  Y faces check Z then X. Each multiply, add, and subtract is separately rounded.
- `10F400` branches on `s2`, the predicate result, before the original recursive
  calls at `10F41C` and `10F434`.

Zero direction components are divided by rather than skipped. From finite
source inputs, division can produce infinity or NaN, and finite operations can
overflow or underflow. These temporary values follow the original comparison
polarity. Replacing `!(time <= length)` with `time > length` would allow an
unordered NaN past that gate. Replacing this sequence with a conventional slab
algorithm or a type2 helper is not the implemented contract.

## Native validation and floating-point contract

Consumed source XYZ values and segment length must be finite. Box half extents
and sphere radius must additionally be nonnegative; negative zero is accepted.
These are native input guards, not inferred source assertions. Segment W lanes
are not read or validated and may contain any raw bits. Finite negative and
zero segment lengths remain accepted inputs: a start inside the box still
passes. A zero-length ray outside may also pass when an original face time
underflows to signed zero; that behavior is preserved rather than repaired.

No new nonfinite-intermediate gate is inserted: e.g. the sphere's overflowed
absolute difference can compare inclusively against an overflowed expanded
extent. This is deliberate bounded **IEEE model** equivalence. The caller owns
round-to-nearest, gradual underflow, and masked floating-point exceptions.
Compile without fast math or FP contraction; explicit volatile scalar helpers
round every arithmetic operation. Floating exception flags and physical EE/VU
exceptional modes are not reproduced or claimed.

## Independent instruction evidence

Run `python3 tools/gameplay/type3_node_fixtures48p.py` from the repository root.
The script verifies pinned frozen words and bytes, starts every case at original
`10E080`, and executes actual instructions with `data=b''` and `source_reads=0`.
All consumed data bytes are explicit fixture inputs or were written by those
instructions. It neither reads guest RAM nor patches instructions, stubs a
helper/recursion, or imports native expected values.

Observations stop **before** the following instruction executes:

- Sphere: `10EE48` means the three comparisons passed; `10F53C` means rejection.
- Ray: `10F400`; `s2==1` means overlap, `s2==0` separation. The overall predicate
  is supplied identical node/local centers and radius zero so it passes.

The observation boundary is not represented as a completed original function
return. Original child traversal is deliberately outside this predicate proof.

Frozen records contain 467 sphere cases and 3,964 ray cases, executing 1,058,124
original instructions. The ray cases include 4,439 infinite and 752 NaN
quotients from finite inputs. They cover all
sphere/start-inside/24 face conditional branches both ways, all six directions,
translated/nonunit rays, inclusive endpoints/edges, zero-size boxes, signed-zero
divisors, finite negative/zero lengths, subnormals, and finite-input overflow.
Random fixtures supplement explicit cases; all expected statuses are obtained
from original instruction execution. `node-fixtures.json` records hashes,
instruction counts, model identity, and division classifications. Per-family
JSON files record every case, observed stop PC, divisions, coverage, and branches.
`InvalidInput` checks are separately labeled native guard tests, not fabricated
original outcomes.

## Native differential verification

The standalone runner is `tests/native/collision_type3_node48p.cpp`. It compares
all 4,431 frozen expected predicates, checks unchanged inputs, exercises
nonfinite consumed lanes and negative extents/radius, and performs 100,000
serial calls of each API while counting allocations. It sets the default
masked floating-point environment and round-to-nearest for the test process.
A focused single-core Linux compile is sufficient; the outer query integration
owns its build-system registration and full traversal checks.

Optimized GCC 14.2 and ASan/UBSan (leak detection disabled) each pass 8,968
checks and both 100,000-call allocation loops. Measured optimized function
frames are 40 bytes for sphere, 128 bytes for ray, plus an 8-byte validation
helper frame. These are compiler-specific measurements, not a cross-platform
stack-size guarantee. Two separate temporary-native-source negative controls
(strict sphere boundary and rejecting nonpositive ray length) both fail the
authentic fixtures as intended; production source and original words are never
changed by those controls.

Commands/results are recorded in `evidence/type3Node48P/native-check.txt` and
`native-sensitivity.json`. All compile/run steps were serial on one Linux CPU.
No broader build or runtime result should be inferred from the focused runner.
