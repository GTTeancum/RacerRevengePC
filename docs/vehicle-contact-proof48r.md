# 48R: independently executed vehicle-contact branches

## Result and limits

`tools/gameplay/vehicle_contact_fixtures48r.py` executes the genuine entry
`0x219420` with unmodified, hash-verified ELF instructions. The checked-in
corpus has **338 cases**, **324 natural handler returns**, **407 authentic
append calls**, **818 authentic body callbacks**, and **14 reached boundary
cases**. It executes **417,491 instructions / 726 distinct words**.

Supported shape bounds are the original `1202E0` branches for tags **1 and 6**.
Both required scene prologue calls, the bounds helper, dot/constructor helpers,
both inverse-point transforms, both known body callbacks, and the append helper
execute their original words. No helper return is intercepted. There is no
spatial-registration bypass: the accepted branches of `1202E0` perform their
actual local numeric bounds work; other branches stop before their work.

This is a bounded contact handler, not complete `219810` vehicle simulation,
world discovery, arbitrary shape bounds, dirty-parent hierarchy traversal, or
arbitrary virtual dispatch. The separate frozen source map documents the wider
caller ordering without claiming to execute that caller.

## Reproduce

From the repository root:

```sh
python3 -B tools/gameplay/vehicle_contact_fixtures48r.py
python3 -B tools/gameplay/vehicle_contact_fixtures48r.py --frozen
```

The first run rechecks every code/dispatch word against the authorized ELF
`work/native/project/disc/SLUS_202.68`, SHA-256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
The frozen run needs only the checked-in source map, raw code/data blocks, and
the corrected `SpringOriginal` / `query_oracle48h` implementation. The source
map JSON is pinned to
`717452d4d1f70808eda3cf6cba3391da96005c1ef8afe3c1b5d2151e6282d99c`.

Every case creates `SpringOriginal(data=b'', words=...)` with an empty allowed
source-address set. Data consists solely of explicitly initialized byte
overlays and bytes written by executed original instructions. Missing bytes
raise an exception. The reports assert `source_reads=0`. No guest RAM image is
read, reconstructed, or requested. No native output contributes to an expected
value. Prior 48Q source/evidence is not modified or regenerated.

## Source-driven field contract

The synthetic vehicle supplies these independent source bindings:

| Vehicle field | Meaning actually consumed |
| --- | --- |
| `+1B4` | Scene for position and world Y/Z axes; required even for signed count <= 0 |
| `+190` | Scene for the two inverse-point transforms; consumed only after acceptance |
| `+340` | Primary body used by the first virtual contact-list callback |
| `+354` | Spring record state and backing |
| `+364 -> +80` | Own contact radius |
| `+58C`, `+590` | Signed candidate count and ordered candidate pointers |

Scene `+190` bit 0 marks a prepared world matrix. A prepared scene ignores its
parent. A dirty parentless scene copies all 16 local-matrix words at `+C0` to
world `+100`, then sets bit 0 while preserving other flag bits. A dirty
parented scene reaches an unsupported traversal boundary. Position/local scene
indices can identify the same mutable object. The original reads the refreshed
matrix through that alias. The body `+288` scene fields are deliberately
different, initialized, and asserted **unread**; they cannot replace these
vehicle scene bindings.

Each candidate supplies category `+A4`, shape tag `+88`, shape pointer `+8C`,
cached center `+60`, and body `+A0`. Its cached radius `+80` is initialized with
ordinary and nonfinite bit patterns and asserted unread. The category mask at
`3B14B0` is explicit mutable input, not the zero initially stored in the ELF.
Repeated candidates remain ordered and are not deduplicated by the handler.

Tag 1 shape input is local center at `+0`, local half-extents at `+10`, and
world matrix at `+80`. Tag 6 has world matrix at `+0`, local center at `+40`,
and half-extents at `+50`. Bounds-derived center is computed but discarded by
the handler; the independent cached candidate center remains the contact
center. All shape and cached fields are preserved.

The sole supported virtual target is authentic `165430`, bound explicitly by
the ELF-proven rigid-body vtable `3CE000` slot `+98`. Its signed body count is
at `+20C`, with ten identity slots beginning at `+210`. Counts >= 10 are natural
no-ops. Otherwise it scans only the active prefix, suppresses an existing
identity, or appends it. Inactive stale slots remain untouched. Self-body
aliasing runs both real callbacks on one state object and appends only once to
that body's identity list.

Spring state is projected in the order `38,3C,40,44,74,78,130,7C,84,88,D4`.
The handler does not reset it. `2963E0` uses the signed count/capacity at
`+84/+88`; on append it writes four complete vec4s and words `+40/+44/+4C`,
increments count, and updates `+D4`. Existing records, record `+48`, and record
`+50..+5F` remain byte-exact, including nonfinite stale bits. A saturated append
does not undo the two preceding body callbacks.

## Numeric order and boundary evidence

The original computes full bounds transforms, including explicit zero products
and add-zero operations. It overwrites the temporary half-extents' Y lane with
positive zero and computes the other radius as:

`priorScale * sqrt((half.x * half.x + half.y * half.y) + half.z * half.z)`.

The scale at `434718` is not always 0.5: flag `gp-7894 == 0` initializes it to
0.5; any signed nonzero flag preserves its existing scalar. The separate flag
at `gp-7890` initializes `434720` to 1, even though that scalar is never used
by this handler. Cold/warm, signed nonzero flags, negative/zero/positive scales,
and opaque second-scalar values are covered.

The handler subtracts cached candidate center from scene position, projects it
onto the scene's **un-normalized** Y basis using the actual VU dot helper, and
subtracts that projection. Acceptance is strictly `distance < ownRadius +
otherRadius`. Exact tangency and adjacent binary32 values on both sides are
covered. Accepted normals recompute the norm squared through the original VU
route, compute a rounded reciprocal square root, and multiply each lane.
`normalization-route-diagnostics.json` identifies **71 contacts** that differ
from per-lane division by the scalar distance. Those alternate calculations are
sensitivity diagnostics only, never expected outputs.

For each accepted contact, record vectors are:

1. Inverse scene transform of own world position
2. Inverse scene transform of `position - ownRadius * normal`
3. The normal
4. `cachedCandidateCenter + otherRadius * normal`

The inverse helper uses transpose multiplication and negative transformed
translation in its original operation order. It is not a general inverse and
does not replace that order with `transpose(point - translation)`. Rotations,
reflections, scale, shear, signed zero, dirty scene refresh, and scene aliasing
exercise the distinction.

Arithmetic is the inherited individually rounded host binary32 finite model,
not proof of all physical EE/VU exceptional modes. Three accepted zero-lateral
cases are labeled `zero-normalization-finite-model`: the inherited VRSQRT-zero
model produces the largest finite reciprocal, then zero lanes multiply to
zero. These cases do not establish a hardware exceptional-mode contract. Four
`nonfinite-copy-only` cases have nonfinite bits solely in lazily unused or
bit-copied fields; no nonfinite arithmetic is claimed for them.

## Exact unsupported boundaries

The original runs until the reached instruction; no continuation is invented:

| Condition | Stop PC |
| --- | --- |
| Position scene needs dirty-parent traversal | `28B1FC`, before its JAL |
| Accepted contact's inverse scene needs dirty-parent traversal | `28BD94`, before its JAL |
| Unsupported bounds tags 0/2/3/4/5 | `120378/12044C/1205EC/1209F8/1208E4`, after authentic dispatch |
| Out-of-range bounds tag | `120C4C`, after the authentic range branch |
| Unknown primary body target | `219788`, before JALR |
| Unknown other body target | `21979C`, before JALR; first callback effects are already observed |
| Negative body-list count | `165430`, before invalid-offset callback work |
| Negative spring record count | `2963E0`, before invalid-offset append work |

The axes helper's dirty-parent boundary `28A77C` is guarded but unreachable
under these single-threaded genuine-entry fixtures: the mandatory position
helper either prepares the shared source scene first or stops before axes.

Native safety rules are distinct from original prefix observations. In
particular ten cases with signed-negative spring capacity return naturally in
the original because its append saturates, while the established native append
contract rejects negative capacity. Their original results remain in the
corpus; native tests must require the documented invalid-input transaction,
not relabel the original outcome. Similarly, native unsupported/invalid
transactions preserve all destination state, whereas the original evidence
keeps real side effects already executed before each boundary.

## Appended position-axis review coverage

The first 298 complete case objects, generator, report, and replay artifacts
are preserved under `evidence/vehicleContact48R/archive298/`, with a manifest
of their original paths and hashes. Every new regeneration checks the archived
fixture hash and asserts that its first 298 complete case objects are unchanged.

The appended 40 cases directly address the initial position-up-axis coverage
gap. They combine five supplied Y bases with prepared/dirty-parentless position
caches, shared/separate inverse scenes, and both hits and misses:

- Tilted unit-like `(0.36, 0.48, 0.8)`
- Nonunit mixed `(2, 1, -0.5)`
- Nonunit vertical `(0, 2, 0)`
- Mixed cancellation `(0.5, 0.5, 0.5)`
- Negative nonunit `(-0.75, 0.125, 1.5)`

All 40 additions return naturally under the ordinary finite model, with 20
accepted contacts and 20 misses. A passive observation after original106E70
records the actual XYZ operands and projection for these additions only.
`position-axis-diagnostics.json` compares those authentic results with alternate
reassociation, normalized-up and Y-only routes; none supplies expected values.
For example, delta `(2^26,-2^26,2)` and up `(0.5,0.5,0.5)` produce original dot
`1`, while right-associated addition produces `0`. The original lateral Z is
`1.5`, rather than `2`. Normalizing supplied nonunit axes also changes acceptance
in several cases, so the corpus distinguishes that unsupported shortcut, not
merely a harmless output representation change.

## Return semantics

The handler epilogue restores registers and stack without defining `v0`.
`return-semantics.json` records actual residual values separately. Zero-count
fixtures with three different incoming `v0` values all naturally leave the
scratch-address residue `0x01FEFF50`; accepted contacts can leave floating-point
bit patterns. Neither is a source-supported Boolean or result count. A native
`Complete` status is an adapter completion status, not a translation of `v0`.

## Files

- `evidence/vehicleContact48R/native-fixtures.json`: complete explicit inputs,
  genuine-entry expected state, events, exact stop PCs, and per-case audits
- `evidence/vehicleContact48R/report.json`: counts, source hashes, fixture hash,
  coverage and branch outcomes
- `evidence/vehicleContact48R/executed-original-words.json`: exact executed words
- `evidence/vehicleContact48R/return-semantics.json`: residual-return evidence
- `evidence/vehicleContact48R/normalization-route-diagnostics.json`: independent
  route sensitivity evidence
- `evidence/vehicleContact48R/position-axis-diagnostics.json`: appended tilted/nonunit axis and dot-order evidence
- `evidence/vehicleContact48R/archive298/`: exact earlier corpus and source fingerprints
- `evidence/vehicleContact48R/map/`: separately owned frozen source/helper/map
  evidence and `219810` ordering analysis

This proof produces no native pass claim on its own. Native differential,
transactional, allocation, sanitizer, and mutation-sensitivity results are
separate artifacts owned by the native test/validation work.
