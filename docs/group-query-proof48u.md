# 48U original grouped-query proof

## Executed boundary

The new corpus starts at genuine original `122660` for every case and runs
preparation, outer-list ordering, XZ group gate, `135CD0`, `135CF0`, the
shape-0 `11F680` setter, secondary query switch, `126F40`, mesh traversal and
static `294BB0`/`2963E0` callbacks naturally. It never starts after the grouped
branch to manufacture entry registers. Exact unsupported pointer/callback
boundaries stop immediately before the first unsupported original operation.

The sole source of expected values is execution of authenticated original
instructions. Data backing is `b''`; all bytes are explicitly initialized sparse
overlay bytes or writes by original instructions. Every fixture asserts zero
source reads, no fallback and a read-byte subset of initialized/written bytes.
All instructions and dispatch words match the ELF SHA listed in the source map.
No word is patched, helper replaced, return forced, RAM retrieved or reconstructed,
or result computed by the native implementation.

## Interpreter composition audit

The unchanged 48P interpreter lacked the REGIMM64 branch needed by the group's
unsigned-position conversion sequence. The new local subclass reuses the exact
already-covered 48S REGIMM64 condition and scalar-load upper-64 sign/zero
extension. Existing interpreter files are unchanged. Before grouped execution,
all 245 retained 48P cases are executed with this local subclass and compared
against every stored expected result, including byte audits: zero differences.
See `isa-adapter-audit.json`. No new floating-point or VU operation is added.
This is individually rounded ordinary-finite IEEE binary32 source modeling,
including signed zeros, not physical EE/VU exceptional-mode or timing proof.

## Final corpus

There are 128 distinct original full-entry cases: 120 native-complete projections
and 8 explicit native-unsupported cases. The source naturally returns in 123
cases; five stop at exact boundaries. New-corpus execution totals 917,559
instructions, 3,381 unique instruction addresses, 402 dispatcher visits and 303
completed static callbacks. All 296 semantic checks pass. The fixture SHA-256 is
`65d184ae5cb91b053fe6dd899d52d2b4a00740ef04c5777d4d52fd24b7794cf1`.
The saved first-122-case artifact remains in `before-final-extension`; all its
case objects are unchanged and five additional cases close the stale-shared-
sphere and callback-owner boundaries. A subsequent single case closes a
demonstrated cached-outer-sphere mutation sensitivity gap. The preceding 127-case
artifact remains in `before-direct-sphere-extension`, with every case object
unchanged. `direct-sphere-extension.json` records exactly one new original
execution of 3,392 instructions. These counts exclude the separate 245-case
adapter audit and all native repeated-hint/configuration invocations.

## Fixture schema

`evidence/groupQuery48U/native-fixtures.json` extends the 48P query fixture schema.
It keeps pairs, scene matrices, candidate geometry, filters, copied vectors,
initial hit/fraction/length arrays, spring state and full record backing.

- `outer_order` is ordered stable object IDs, independent of candidate table order.
- `groups` contains stable ID, unsigned position triplet, raw signed-byte encodings
  `width_byte`/`height_byte`, four raw binary32 dimension words, and ordered stable
  member IDs. Optional `count` independently encodes the signed source count.
- `group_scale` is raw binary32 world position scale.
- Candidates add raw 16-word `local_matrix`, `base_matrix`, `previous_matrix`;
  optional `shape_alias_id` denotes physical shared shape backing. Root node0 is
  the actual shape+114 geometry read by both the setter and traversal.
- `expected.initial.candidates` and final `expected.candidates` contain all 72
  shape words, full member sphere XYZW+radius, flags and copied vectors.
  Synthetic descriptor pointer words remain explicit opaque fixture values.
  `expected.initial.groups`/`expected.groups` prove outer group state unchanged.
- Pair/preparation/record/spring state and callback/dispatch snapshots retain the
  existing 48P schema. New `group_events`, `full_calls` and `setter_writes` expose
  order and original source PCs. Observational snapshots do not change execution.
- `native_scope` and `native_reason` distinguish complete native projections,
  source-complete unsupported cases and source boundary cases.

The metadata pointer input is always explicit. Nonzero scene metadata is source
proven separately and stays outside the bounded composed native entry. Raw W
lanes are retained; full-W matrices are not silently replaced with affine ones.
Outer collision-tag groups are not candidate shape-1 compound records.

## Discriminating coverage

Cases distinguish ordinary collision-tag precedence, query bit-0 suppression,
wrong group tag, inclusive XZ tangency and Y omission, strict member XYZ tangency,
signed empty/negative member counts, helper mutation before inactive rejection,
member list-tag/filter bypass, repeated members/groups, mixed direct/group order,
shared shape backing, same-query membership and late state resets.

They also cover shape-4/5 signed ratio/translation writes and shared compact
backing; nonzero no-op tags; full-W, rotated, scaled, sheared and signed-zero
world scales; multiple rays; callback enable/skip/material behavior; prior,
saturated and zero record capacity; unknown member callback ownership/order;
null member shape and null root geometry; a dynamic294BB0 callback boundary
at `294C34`, before its first body-vtable dependency; prior dynamic/static
callback-owner identity; late guarded compound/callback and
early rejected invalid backing; mode1/mode2 normal interpolation reachability;
and seeded finite order/member/matrix variations.

The decisive shared-state case first updates all aliased members, moves only one
member and the shared matrix far away, then visits an unrefreshed member as an
ordinary candidate. Its retained sphere admits the dispatch, which resets hit
count from one to zero before the moved geometry misses. Refreshing every
member sphere would skip that dispatch and leave the old count, so this is an
observable behavioral distinction, not just two equal matrices.

A second, independently executed source contrast moves only member 10 to Y=100
through the group setter, obtains an intervening ordinary member 11 hit, then
visits member 10 ordinarily. Its updated cached sphere rejects before 126F40:
dispatch IDs are exactly [10,11], final current-candidate is 10, and the earlier
hit flag remains 1. Substituting an immutable input-descriptor sphere in that
ordinary gate would reach another dispatcher and clear the retained hit flag.
This case directly source-backs the native mutant-killing assertion.

The JSON proof report is authoritative for current case, instruction, callback,
partition and semantic-check counts. These are original executions, separate
from repeated native invocations with different validation hints or buffers.
Native safety tests and mutations are separate reports, not original evidence.

## Preservation and reproduction

Run from repository root:

    PYTHONDONTWRITEBYTECODE=1 python tools/gameplay/group_query_fixtures48u.py

Writes are limited to new 48U evidence. `preserved-files.json` hashes all retained
48S/48T evidence and prior interpreter/fixture sources before and after generation.
All 239 48S cases and all 297 48T cases (187 helper, 110 collector) remain intact.
The proof does not edit or regenerate those corpora. It also does not build,
install, retrieve guest RAM or run a game/PC session.

`map/original-words.json`, `map/dispatch-data.json`,
`executed-original-words.json`, `semantic-checks.json`, `isa-adapter-audit.json`
and `proof-report.json` provide immutable-word, order, boundary and provenance
checks for the new seam. Consult the native test/review reports for runtime
status rather than assuming source execution alone proves the translation.
