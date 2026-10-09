# Original spring detector preparation: 48O

## What this establishes

The 288 fixtures execute the original `0x295630` detector from its entry point,
using explicit, initialized synthetic objects. The original point transform,
AABB calculation, optional spring reset, common first-endpoint writes and type-3
pair staging execute normally. Active paths stop **before** their actual query
or spatial-registration call. Absent-spring and disabled-query paths return
naturally with result 1.

This is a bounded numeric preparation proof. It is not a full detector/query
tick, spatial-index proof, current vehicle-object binding, RAM replay, or live
gameplay claim. The separate 48N query proof remains unchanged.

## Provenance and execution discipline

- Source: the immutable `SLUS_202.68` ELF, SHA-256
  `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
- Address mapping: ELF file offset = virtual address - `0x100000` + `0x80`.
- Exact instruction ranges and independently pinned hashes are in
  `evidence/springDetector48O/original-words.json` and the generator.
- The generator inherits `SpringOriginal` / `Original`, with corrected scalar
  MIN/MAX semantics described below. The subsequent audit also corrected the
  active shared spring oracle; its historical source is preserved separately.
  No original instruction word or historical fixture byte is changed.
- `data=b''`, an empty permitted-source-address set and strict uninitialized
  read failures are enforced. **Every fixture has zero source-byte reads.**
- There are no instruction patches, intercepted calls, helper stubs, forced
  returns, native-generated expected values, RAM retrieval or reconstructed
  RAM images.
- All 120 PCs shared with the independently frozen `queryPrepare48O` helpers
  were checked for exact instruction-word equality.
- The numeric model is the existing individually rounded host IEEE binary32
  interpreter. This does not establish every physical EE/VU exceptional mode.

The checked-in generator reads only authorized source ELF bytes or its frozen
instruction evidence. Importing the existing oracle modules does not invoke
their historical standalone RAM-replay entry points.

## Exact boundaries

| Status | Stop | Meaning |
| --- | --- | --- |
| 0 | Natural return | Detector has no spring; returns 1 without reading spring state |
| 1 | Natural return | Query pointer absent or query-enable word zero; returns 1 |
| 2 | `0x11FF74` | Before the original `0x120230` registration call and its delay slot |
| 3 | `0x295A08` | Before the original explicit-list `0x122660` query call and its delay slot |
| 4 | `0x295A1C` | Before the original `0x121920` query-dispatch call and its delay slot |

The full fixtures never execute `0x120230`, `0x122660`, `0x121920`,
`0x2E83D0` or `0x2897B0`. JSON also captures complete intermediate state at
`0x295944` (numeric preparation done), `0x2959D4` (reset and common endpoints
done), and `0x2959E4` (type-3 staging done), when each point is reached.

The no-pair-buffer active path is deliberately constrained to initial query
type 3. The ELF-verified jump-table cell `0x388A2C = 0x11FF4C` makes the real
`0x11FE50` helper return without a type-specific scene/sphere update. It then
reaches `0x11FF74` naturally. Type-2 query world/sphere preparation requires
additional fields and is proved separately in `queryPrepare48O`; this compact
detector schema does not claim that type-2 path.

## Scene and record semantics

Each fixture has four initialized record slots and four independent scene
slots. Four is the test backing size, not an inferred game or runtime limit.

- A scene whose byte `+0x190` has bit 0 set uses its prepared `+0x100` matrix.
  `0x28BAB0` returns naturally; no refresh helper is intercepted.
- A dirty scene with parent `+0x180 = 0` naturally copies all 16 words of its
  local `+0xC0` matrix into `+0x100` and sets bit 0, preserving other flag bits.
- The point transform uses local XYZ and matrix XYZ rows. Local W and all
  matrix W lanes do not enter the calculation. The result W is explicitly 1.
- Output arithmetic follows the original VU operation order:
  `((column0 * x + column1 * y) + column2 * z) + translation`, with individual
  binary32 rounding at the modeled instructions.
- All 24 words of every source record are retained and compared. Their
  non-point fields are explicit stale sentinels, not implicitly zero-filled.
  Source records and scene-local matrices remain unchanged in supported paths.

The compact numeric fixtures use independent scenes. Shared-scene/alias wiring
is not established by this schema. A separate boundary probe confirms a
prepared scene can carry an opaque nonzero parent without reading that parent.
Dirty parented scenes stop at `0x28BAF4`, before the real refresh call, and are
outside this supported seam.

## Gates and preserved state

The numeric preparation gate is:

`pair buffer present OR (spring +0x48 nonzero AND detector +0x34 nonzero)`.

There is no AABB fallback when the pair buffer is absent and the provider gate
is inactive: the previous detector center and half-extents remain unchanged.
An active provider can call `0x2E83D0`; the proof stops before that call in
separate dependency probes. It does not invent provider output or stub a return.

With a pair buffer and positive signed spring point count, each source record
produces one second endpoint. Scalar minimum/maximum use signed-word ordering,
with reversed ordering for two negative operands. Opposite signed zeros are
ordered rather than treated as floating-comparison ties. The detector receives complete center and
half-extent Vec4 values, both with W = 1. Query absence or disablement does not
skip those earlier numeric writes.

Detector `+0x6C` independently controls the authentic `0x296490` reset. It
clears the proven hit/selection/count fields and sets spring `+0x38` to 1;
point count `+0x7C`, capacity `+0x88`, material `+0xD4`, records and all
unwritten sentinels are preserved.

Only a present query and nonzero detector query-enable `+0x68` copy the center
into first endpoints and continue to query staging. `0x11CA80` always sets
query type to 3, even with a null shape. A present shape independently supplies
its own count; exactly twice that count of full Vec4 entries are copied and
their auxiliary words zeroed. Shape count is **not** assumed to equal spring
point count. Smaller and larger counts use explicit initialized backing,
preserve unused pair/staging/auxiliary slots, and expose stale copied slots
when the shape requests more entries than numeric preparation updated.

## Scalar MIN/MAX model correction

The preserved historical `SpringOriginal` extension used host floating comparisons for
scalar MIN/MAX. That loses the distinction between opposite signed zeros.
For 48O only, the new subclass evaluates those authentic scalar opcodes by
signed-word ordering, reversing the order when both inputs are negative. This
agrees with the existing `contact_oracle48g.h` implementation and the primary
[PCSX2 FPU implementation](https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/FPU.cpp)
(`fp_min` / `fp_max`, lines 83–90 and their opcode users at lines 265–276,
checked 2026-10-06). No external implementation is copied into the evidence.

The new fixture corpus runs both models where their scalar results differ.
`minmax-model-delta.json` records 10 cases with differing intermediate scalar
results and 4 with differing final output words. These final differences are
zero signs in detector centers and the first endpoints/staged copies derived
from those centers. The old model is used only for this disclosed comparison;
it does not produce the frozen expected rows. The active shared spring oracle
is now corrected too, while its prior source and historical fixtures are preserved.
See [the prior-checkpoint audit](minmax-audit48o.md) for supersession details.

This correction does not import a complete EE floating-point implementation.
The remaining arithmetic still uses the stated individually rounded host
binary32 model, and the overflow probes remain out of the finite-domain rows.

## Fixture encoding

`fixtures.bin` and `fixtures.h` contain the same little-endian uint32 words.
The five-word header is magic `0x4F383452`, version 1, case count 288, input
width 250 and expected-output width 160. Each row is 250 input words, one
status word and 160 expected words. No expected values come from native code.

Input word ranges:

| Words | Contents |
| --- | --- |
| 0–10 | spring present, pair buffer present, provider present, spring provider enabled, query present, query enabled, reset, explicit list, shape present, shape count, initial query type |
| 11–18 | Previous detector center Vec4 and half-extents Vec4 |
| 19–29 | Spring fields in the order below |
| 30–177 | Four records, each: local Vec4, scene world matrix16, scene local matrix16, scene flag byte represented as a word |
| 178–209 | All four previous pairs, including every W lane |
| 210–241 | All four previous staged pairs, including every W lane |
| 242–249 | Eight previous auxiliary words |

Spring field order is `+38,+3C,+40,+44,+74,+78,+130,+7C,+84,+88,+D4`.
Point count is input word 26 (spring-state index 7).

Output word ranges:

| Words | Contents |
| --- | --- |
| 0–7 | Detector center and half-extents Vec4 |
| 8–18 | Spring fields in the same order |
| 19–50 | All four pairs |
| 51 | Query type |
| 52–83 | All four staged pairs |
| 84–91 | Eight auxiliary words |
| 92–159 | Four scenes, each: all 16 world-cache words and flag byte |

## Coverage and validation

- 288 ordinary-finite fixtures.
- 159,569 authentic executed instructions; 357 unique instruction PCs.
- Status counts 0/1/2/3/4: **19 / 44 / 26 / 128 / 71**.
- 128 deterministic randomized cases, plus all principal gate combinations,
  independent point/shape counts, null shapes, zero shape count, dirty/root and
  prepared scene flags, rotations, nonorthogonal and degenerate bases, signed
  zeros (including eight explicit opposite-zero cases), adjacent float words,
  repeated points and stale unused fields.
- Eight separately reported dependency/edge probes.
- ELF regeneration and `--frozen` replay both pass with identical artifacts.

The three nonpositive active-point-count probes (`0`, `0xFFFFFFFF`,
`0x80000000`) are intentionally outside the ordinary-finite fixture rows. The
original numeric loop performs no point iterations but still calculates from
its initial ±FLT_MAX bounds. Under the stated interpreter model this yields
center `[+0,+0,+0,1]` and half-extents `[-infinity,-infinity,-infinity,1]`.
This supplies an explicit reason for a finite-domain runtime to reject that
input rather than invent a finite AABB. With no pair buffer and the provider
gate inactive, those signed counts are harmless and included in ordinary rows.

The remaining probes stop at active provider or dirty-parent dependencies,
exercise a prepared scene's opaque parent, and let both original static-global
initialization branches execute normally. The static initializer probe writes
its real global sentinels and flags; no initialization helper is skipped by a
patch.

Reproduce from the authorized ELF:

```sh
python tools/gameplay/spring_detector_fixtures48o.py
```

Replay only the pinned, self-contained instruction evidence:

```sh
python tools/gameplay/spring_detector_fixtures48o.py --frozen
```

The binary is 473,492 bytes with SHA-256
`642a4fc189eb199a383d4dd73681baf9ad7afaa77a5daff99985d2bed9ce6082`.
The header SHA-256 is
`96ff3fb2234801dd0925202a99e2cb21b571350e33cba430133f33f7ee014365`.
JSON hashes, complete PC coverage, branch outcomes and original source hashes
are retained in `evidence/springDetector48O/proof.json`.

Native integration and its tests are separate from this original-execution
evidence. This document records no native build or test claim.
