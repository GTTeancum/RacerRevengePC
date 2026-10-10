# 48M: bounded numeric terrain-query preparation

The direct Linux x64 normal and ASan/UBSan checks both pass **15,674 checks over
223 original-word cases**. The API makes zero counted allocations; state is
268 bytes and input is 360 bytes. This is component validation, not a native
race or Xbox performance result.

`prepareTerrainSinglePair48M` closes the numeric seam from a supplied body scene
to the prepared type-2 single-pair matrix, sphere and reset terrain state. Its
reference boundary stops immediately before the original spatial-registration
call at `0x11FFA4`. It does not establish the complete `0x1B1800` phase, a live
vehicle tick, world candidates, or spring-contact detection.

## Typed, fixed-storage contract

`src/game/terrain_prepare48m.h/.cpp` uses ordinary native types already used by
48G/48I/48J. There are no guest pointers, interpreter calls, generated fixture
dependencies, heap allocations or unbounded input lists in production. One
local endpoint pair and the existing sixteen collected-identity slots are the
entire capacity. The component's lower endpoint `+140` is a separate input from
the shape leaf's second endpoint; the original stores them separately.

The five return statuses have distinct meanings:

- `NoBody`: original `+128` is absent; copy the state unchanged, without consuming
  scene, endpoints, auxiliary-provider gates or shape scope.
- `QueryDisabled`: body exists, auxiliary providers are absent and `+100` is
  zero. Perform resets and transform component `+140` through the full scene,
  but retain the previous segment matrix and sphere. Do not run 48J afterward.
- `QueryReady`: additionally select the query matrix and compute the single-pair
  sphere. The caller still owns spatial registration and candidate selection.
- `UnsupportedScope`: an auxiliary provider is requested, or an enabled query
  has a shape other than type 2, one pair and no pair-tree node. Output unchanged.
- `InvalidInput`: a consumed value or intermediate is outside the supported
  finite affine arithmetic domain. Output unchanged.

Successful output may alias `input.state`. Rejected output is transactional,
including aliasing. Disabled queries deliberately do not validate unused shape
endpoints, old matrix/sphere or pair-tree metadata. No-body calls preserve all
state, even values that would be rejected on an active path. These native domain
guards do not claim that the original game rejected malformed input identically.

Preparation clears `+108/+110/+114/+118`, collected count `+1E8`, exclusion owner
`+12C`, and largest velocity change `+1D0`. It preserves active `+104`, pending
`+10C`, detached `+11C`, previous point/normal, current candidate `+F4`, normal
`+170`, and all sixteen collected slots. In particular, 48J's saved fixture sets
current candidate to zero explicitly; this is not an original preparation reset.

The lower-endpoint seed always uses the full scene. Separately, any nonzero
active flag `+104` selects the full scene as the query matrix; zero selects an
identity basis with the scene's translation. There is no orientation inference
from a rendering mesh. The original midpoint is `0.5 * (second + first)`, and
the radius is the square root of the ordered squared sum of
`0.5 * (second - first)`. Only the midpoint is transformed. The original does not
scale the radius for a nonorthogonal scene basis, and this API does not repair it.

## Original boundaries and reference dependencies

- `1B1810..1B184C`: body gate and resets; scene is `body+288`, matrix at `scene+100`.
- `1B1854..1B1888`: full-scene transform of component lower endpoint to `+150`.
- `1B188C..1B1900`: optional-provider gates; both must be null in this slice.
- `1B1904..1B1980`: enable/prior-contact gates and matrix selection.
- `14D840..14D8B8`: original identity-matrix helper on the no-contact branch.
- `11FF90..11FFA0`: call the matrix/sphere setter. Stop at `11FFA4`, before the
  call to spatial-index refresh `120230`.
- `11F680..11F6C4`, ELF dispatch word `388A08 = 11FA30`: type-2 dispatch.
- `11FA30..11FA3C`, `11FB1C..11FC7C`: null pair-tree/single-leaf matrix copy,
  midpoint, radius and transformed center.
- `106790..1067A8`: original float-vector construction helper.
- `11FE28..11FE40`: setter epilogue; `1B1A14..1B1A28` is the natural disabled/
  no-body return path.

All extracted instruction words were read from the hash-verified original ELF,
SHA-256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Words also present in immutable `evidence/query48H/original-words.json` are
cross-checked. `evidence/terrainPrepare48M/original-words.json` and `proof.json`
record exact words, bounded ranges, hashes and executed branch outcomes.

The minimal retained inputs are the original ELF, 48H executor and word proof,
`tools/gameplay/contact_words48g.h` (selected scene/component/constants), and
`tools/gameplay/terrain_query_fixtures48j.h` (previous prepared segment/sphere).
No additional reference files, collision caches or full `ee.bin` are required
for this numeric slice. The generator never reads or reconstructs a RAM dump.
Frozen header values are explicit inputs to synthetic continuations, not a
before/after observation of a real tick.

## Independent fixtures

`tools/gameplay/terrain_prepare_fixtures48m.py` uses the unchanged 48H instruction
executor. It starts with empty source memory, explicitly initializes every
required object/data byte, and forbids all source-data reads. Instruction words,
arithmetic, branches and helpers are not patched. A nonnull opaque registration
identity is present; it is never read, and registration is neither executed nor
suppressed. The active path stops at its declared boundary. Disabled and
no-body paths return naturally through the original epilogue.

The fixture set contains 223 cases, 55,147 executed instructions, 237 unique
original words and zero source-memory reads: 189 ready, 18 disabled and 16
no-body. It covers both prior-contact branches (including non-boolean nonzero
flags), all linked/enabled combinations, rotated/nonorthogonal/zero bases,
zero-length/reversed/signed-zero endpoint pairs, adjacent float endpoints,
old collected counts above capacity (which are reset without being read),
distinct component and leaf lower endpoints, and 128 deterministic variations.
Tests compare all 67 exposed state words, including unchanged values, and check
aliasing, unsupported scope, nonfinite/overflow rejection and allocation counts.

The retained scene's prepared sphere is deliberately different from the old
48J saved prepared sphere. With prior contact active, the refreshed center/radius
words are `459B9C9F 4504438D 4647471E 41780000`; the old prepared values are
`459B9B14 45044366 464746E0 41780000`. Treating the two matrices as interchangeable
would invalidate a purported full-phase proof.

## Integration limits

48J remains the prepared ordered-list query, and 48G consumes its resulting
terrain-following surface. The spring model's collision producer is separate.
This work does not provide world enumeration, spatial registration, material or
category binding, auxiliary surface providers, moving-object synchronization,
vehicle scheduling, input/clock mapping, or a full before/after simulation tick.
The saved component's `+E0` registration pointer was nonnull; a null-index test
must never be relabeled as replay of that saved full prequery.

## Native validation

Both serial builds used C++17, `-fno-fast-math`, `-ffp-contract=off`, and
`-Wall -Wextra -Werror`. The normal build used `-O2`; the sanitizer build used
`-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer`.
`ASAN_OPTIONS=detect_leaks=0` is explicit because the executor environment blocks
LeakSanitizer under tracing; leak checking is not claimed. The independent
allocation counter reports zero calls during the measured API calls.

Logs, exact commands, binary/source/fixture hashes and status are under
`evidence/terrainPrepare48M/`. An initial test-source indentation warning was
fixed; the failed preliminary compile log remains alongside the clean build.
An unavailable `/usr/bin/time` prevented the first resource-wrapper attempt
from invoking a compiler; subsequent recorded commands use Python subprocess
timing. Reported child RSS is cumulative tool-process usage, not game memory.
No x86/Windows/Xbox build or aggregate project build is claimed by these direct
checks. The parent integration owns those wider checks.

The arithmetic comparison is ordinary host binary32, round-to-nearest, without
fast-math, reassociation or contraction. It does not certify all physical PS2
exceptional/denormal modes or Xbox performance. Native validation results are
recorded in `evidence/terrainPrepare48M/validation.json`.
