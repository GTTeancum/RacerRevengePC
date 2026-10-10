# Group shape-0 setter: original-entry proof 48T

## Scope and evidence boundary

This is an additive continuation of the frozen 48S collection proof. It closes
only the reached group-member tag-0 call `135CF0 -> 11F680`, with explicit
finite matrix/geometry backing. It does not stand in for world initialization,
shape registration, dirty scene traversal, a general shape setter, or all EE/VU
exceptional arithmetic modes.

The source is the authorized `work/native/project/disc/SLUS_202.68`:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
`tools/gameplay/group_shape_fixtures48t.py` independently freezes full
`135CF0..135F24` and `11F680..11FE44` entries and the seven-word setter dispatch
at `388A00` into `evidence/groupShape48T/map/`. Every word is checked against the
ELF and frozen 48S map. Collector dependencies are the unchanged 48S map,
validated by its pinned hash and against the ELF on the default proof run.

The proof uses the unchanged 48S interpreter and sparse initialized overlays:
`data=b''`, empty allowed-source addresses, and zero source reads. Each
execution read must be initialized or previously written by genuine instructions.
There are no guest RAM reads, simulated full RAM image, native expected outputs,
patched words, helper stubs, forced returns, allocator interception, or skipped
helper effects. Tracing is passive. No new ISA semantics were necessary.

All expected word arrays and write traces come from executing original entries.
The 48T script does not write any 48S file. It additionally re-executes all 239
48S collector cases in memory and compares complete case objects exactly, and
re-executes all 338 existing 48R numerical/caller cases against their original
saved expected outputs. Both frozen corpora remain byte-identical.

## Exact group caller contract

The original `135CF0(group,index)` resolves archetype via
`*( *(3E1CE8) + 6C + 4*u8(group+E))`, then member identity through the archetype's
`+204` pointer array. It returns null without shape writes if member `+8C` is
null. The collector later dereferences that null result; null is an explicit
unsupported boundary, not graceful skipping.

For a present shape, group XYZ is unsigned `u16(+8/+A/+C)` converted to float
and individually multiplied by the world float at `+590`. The unsigned-load
negative branches are not taken for these 16-bit inputs. The helper then
executes both signed-byte scale calculations before dispatching by member tag:

- height ratio: `(d18C + ((s8(+11) * (d194-d18C)) / 128)) / d18C`
- width ratio: `(d180 + ((s8(+10) * (d180-d184)) / 128)) / d180`

Each subtraction, multiply, divide and addition is separately rounded in the
bounded ordinary-finite model. Tag 0 does not use either resulting ratio.
Negative bytes do not scale the tag-0 matrix or sphere radius.

The tag-0 route executes the following exact sequence:

1. Copy all 16 words of shape `+80..BF` to its stack temporary.
2. Replace only temporary translation XYZ (`+30/+34/+38`) with group XYZ.
   Preserve all basis words and the fourth row/column W words, including `-0`.
3. Write group XYZ to the live shape `+B0/+B4/+B8`. The last write at `135F0C`
   is the delay-slot instruction of JAL `135F08 -> 11F680`.
4. Call `11F680(member,temporaryMatrix)` naturally.
5. Return the original member identity after the setter returns.

This ordering matters: the setter's previous-world cache captures the world
matrix *after* the group translation prewrites. For a completed group tag-0
call, previous `+C0..FF` and current `+80..BF` are identical. An implementation
that saves the earlier group's translation to `+C0` changes original behavior.

Tag 5 writes height/width ratios at shape `+58/+5C` and XYZ at `+40/+44/+48`.
Tag 4 writes them at `+68/+6C` and `+30/+34/+38`. Other tags return the same
member after the common arithmetic without those shape writes. These existing
routes are retained as comparison cases; tag-0 helper runtime scope does not
silently expand to a general setter API.

## Exact tag-0 setter contract

`11F680` reads member `+88`, indexes `388A00` for tags 0 through 6, and executes
tag 0 at `11F6C8`. Its tag-0 branch has no JAL, virtual call, registration call,
scene traversal, dirty-state check, or other dynamic dependency.

Input and payload layout, with all matrix arrays retaining all 16 raw words:

- member `+60..6F`: cached sphere center vec4
- member `+80`: cached sphere radius
- member `+88`: shape tag
- member `+8C`: shape pointer
- shape `+00..3F`: composed matrix, output
- shape `+40..7F`: local matrix, read-only
- shape `+80..BF`: current input/world matrix
- shape `+C0..FF`: previous input/world matrix
- shape `+114`: geometry pointer
- geometry `+C/+10/+14`: local sphere center XYZ
- geometry `+18/+1C/+20`: local half extents XYZ

The write and arithmetic sequence is:

1. Eight iterations copy two words each from current shape `+80` to previous
   shape `+C0`. Reads precede their corresponding writes exactly as recorded.
2. Eight iterations copy two words each from the input matrix to shape `+80`.
3. Load the input columns and local matrix columns, then calculate all 16
   composed words. For flattened column-major arrays `I` and `L`, the exact
   expression for local column `r`, lane `c` is:

   `C[4*r+c] = (((I[c]*L[4*r]) + I[4+c]*L[4*r+1]) + I[8+c]*L[4*r+2]) + I[12+c]*L[4*r+3]`

   Every multiplication and addition rounds separately; preserve XYZW, sparse
   zero multiplications and signed zeros. Do not transpose, commute the two
   matrices, drop W terms, or replace the chain with a fused dot product.
4. Copy geometry center XYZ to member sphere center, then write center W=1.
5. Calculate radius from local half extents as
   `sqrt((halfX*halfX + halfY*halfY) + halfZ*halfZ)` with separately rounded
   operations. The scalar instruction order squares X and Y, accumulates their
   sum, then adds Z squared before sqrt. The radius is not matrix-scaled.
6. Transform center XYZ as
   `((I[c]*centerX + I[4+c]*centerY) + I[8+c]*centerZ) + I[12+c]` for `c=0..2`.
   Translation is a plain addition, not multiplication by a stored center W.
   Force output W to positive 1, even when matrix W lanes contain other values.

Only shape `+00..3F`, `+80..BF`, `+C0..FF` and member sphere fields change on
this route. Shape local matrix and metadata, member flags/category/list state,
and all geometry words remain unchanged. Group callers additionally perform
translation prewrites as above; the collector later changes member flag bit 4.

The original direct setter differs from the group caller: its previous cache
is the truly old world matrix, because it has no group translation prewrite.
The direct arbitrary-input-matrix cases are original-only proof, not evidence
that the bounded native group entry exposes that API.

## Fixtures and native comparison partition

`helper-fixtures.json` contains 187 genuine helper-entry cases:

- 101 start at group member `135CF0`
  - 91 completed tag-0 cases are native group-entry comparisons
  - 1 reached null-geometry case exercises the explicit native boundary
  - 8 comparison-tag cases and 1 null-shape case are original-only
- 86 start directly at `11F680`, all original-only
  - 85 natural returns, including the setter's no-op tags 3, 4 and out-of-range
  - 1 reached null-geometry boundary

The helper native-comparable partition is explicit per case with
`native_supported` and `native_status`. Direct cases must never be silently
included in the native group-entry differential count. The 91 completed group
cases include identity, scale, rotation, shear, arbitrary W, signed zero,
negative-zero world scale, extreme u16 positions, signed width/height bytes,
dirty/opaque metadata, center/half variants and 64 seeded finite random matrices.

Each helper fixture stores complete raw `member_words[64]`,
`shape_words[72]`, `shape_geometry_words[9]`, `input_matrix[16]`, position,
world scale, signed-byte encodings and archetype dimensions, plus complete
expected raw arrays, original writes, call targets and sparse-byte audit.
Standalone addresses are member `20000`, shape `30000`, geometry `40000`,
input matrix `50000`; pointer words are identities in fixture data, not native
host pointers.

`collector-fixtures.json` adds 110 cases in the 48S schema without changing any
of the preserved 239 cases. It contains 108 completions and 2 pointer boundaries.
Candidates with shape data carry 72 shape words and 9 geometry words; optional
`shape_alias_id` and `geometry_alias_id` represent shared backing. Synthetic
shape backing is `C00000 + shape_alias_id*200`, geometry backing is
`D00000 + geometry_alias_id*100`. Three explicit shared-backing cases cover
shared shapes, shared geometry, and two groups updating a shared shape. Other
cases cover repeated members and cross-tree repeated groups, nominal-capacity
overrun, disabled and sphere-rejected paths, ordinary direct-member composition,
filter bypass, comparison tags, matrix variants, and 64 seeded finite matrices.

For group expansion, appended identities remain outer group identities,
repeated once per returned member. Query current-candidate and member flags
still refer to the member. All shape/sphere state reflects sequential mutations,
including members shared by different groups. There is no per-member capacity,
filter or sphere test introduced by 48T.

## Unsupported pointers and numerical domain

The source proof stops exactly before the first null-geometry data read,
`11F7B0` (geometry `+C`). By then world/previous/composed matrices have been
written; member sphere has not. That partial original state is recorded for
fidelity. A bounded transactional native failure is expected to publish none
of it and return an explicit missing-geometry/unsupported-shape status.

The null-shape helper naturally returns zero. Its full collector case stops at
`1225B8` before the unsafe member flags dereference. Neither boundary is
represented as a successfully collected member. Arbitrary invalid addresses,
unmapped dynamic dependencies and other shape-setter branches are not invented
or recovered through fallback reads.

The numerical proof is the existing individually rounded IEEE binary32,
ordinary-finite model, including signed zero. Active input matrices, geometry,
group arithmetic and results stay finite. It is not a physical EE/VU proof for
NaNs, infinities, subnormal exceptional behavior, underflow or overflow. Raw
opaque words outside the consumed numeric fields are preserved and need not
be finite floats. Any native rejection/domain tests outside these original
fixtures are separate policy checks, not original/native differential results.

## Reproduction and reports

Run from the repository root:

`PYTHONDONTWRITEBYTECODE=1 python tools/gameplay/group_shape_fixtures48t.py`

This writes only the new 48T evidence directory. `proof-report.json` records
entry/partition counts, native-comparable versus original-only scope, reached
boundaries, complete coverage, steps, fixture hashes and unchanged prior
corpora/interpreter hashes. `executed-original-words.json` lists every executed
word. `semantic-checks.json` records 33 source-backed invariants. Native
execution is not performed by this proof generator; native test results belong
to the separate runtime test report.
