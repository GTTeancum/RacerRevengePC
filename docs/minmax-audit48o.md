# 48O scalar MIN/MAX audit and historical-source preservation

2026-10-06. Scope: ordinary finite, individually rounded binary32, including
positive and negative zero. This is not an EE/VU exceptional-mode certification.
No RAM image, reconstructed snapshot, native game process, build, or package
installation was used for this audit. The original instruction words are unchanged.

## Finding and rule

The pre-48O `SpringOriginal` extension selected the second operand whenever the
host floats compared equal. That loses the EE scalar ordering of opposite zeros:
MIN(-0,+0) must select -0; MAX(+0,-0) must select +0. The corrected rule compares
signed raw words and reverses the comparison when both operands are negative.
This agrees with the existing independent `contact_oracle48g.h:140..145` and
[PCSX2 FPU.cpp](https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/FPU.cpp),
`fp_min`/`fp_max` at lines83–90 and MIN_S/MAX_S dispatch at lines265–276, checked
2026-10-06. This source check establishes the scalar selection rule only.

The active `tools/gameplay/spring_fixture48j.py` is now corrected. Its original
bytes are retained at `evidence/minmaxAudit48O/spring_fixture48j-legacy.py`, SHA-256
`8e3c678940bceebe3e2e423a1f092b593d58ae8cfbaa825a5adeeb382627f95e`.
The corrected source SHA-256 is
`32f2d7e3d392df8fbf8ea66e218777d38c6e0a9853bda73e8e495686d8dafe53`.
The new detector generator explicitly implements both selection rules, so its
legacy-model delta evidence no longer depends on an unfixed inherited oracle.

## Every scalar operation reached by the audited response corpora

| Original PC | Original operation | Native correspondence | Signed-zero consequence |
|---|---|---|---|
| 00297534 | MIN.S(scale,+1) | spring_contact48j.cpp friction magnitude | No finite result change: a zero never ties +1. |
| 00165694,00165854 | MAX.S(excess,+0) | spring_impulse48k.cpp correction helper | Existing `excess>0?excess:0` already selects +0 for both zero signs. No finite difference. |
| 0029724C | MIN.S(penetration,maxPenetration) | spring_force48l.cpp depth | A -0/+0 tie can change its selected word. |
| 00297294 | MAX.S(speed,-maxSpeed) | spring_force48l.cpp lower speed clamp | A +0/-0 tie can change its selected word. |
| 002972AC | MIN.S(lower,maxSpeed) | spring_force48l.cpp upper speed clamp | A -0/+0 tie can change its selected word. |
| 002972C8 | MAX.S(damped,+0) | spring_force48l.cpp nonnegative magnitude | Existing scalar expression was already finite-safe; this clamp erases upstream zero-sign differences before multiplication by the mass ratio. |
| 002983F0 | MIN.S(cached velocity dot normal,+0) | spring_force48l.cpp removeSpringInwardVelocity48L | Output-sensitive: a -0 dot must stay -0 before negation; the old model selected +0. |

For the current force API, changing only the zero signs of depth/speed arithmetic
cannot change nonzero intermediate magnitudes. The final MAX(damped,+0) turns
both signs of zero into +0 before the common mass-ratio and normal products.
The six additional force fixtures expose intermediate errors, but deliberately
do not claim final force differences. The secondary API does expose differences.
A shared finite scalar helper is appropriate for all four force clamps and the
secondary clamp; changing ordinary C++ branch predicates or VU instructions is
not justified by this finding.

The old 48K generated coordinator reaches the friction and impact operations.
The 48L full coordinator additionally reaches all four force operations and the
secondary operation. There are no separate scalar MIN/MAX instructions in the
coordinator's response-selection predicates.

## Query/terrain48H–48N and detector distinction

The broadly frozen query48H words include these scalar operations outside the
implemented type2-pair/mode1 query path:

- 00108210,00108224: MIN.S; 00108248,0010825C: MAX.S, in the frozen
  caseD_0_0x107e60 path
- 00121B2C,00121B40,00121B54: MIN.S, in entry_00121a6c

None appears in the complete 48M numeric-preparation or 48N full static-query
executed-word coverage. Callback48N and records48N have no scalar MIN/MAX in their
executed paths. The active `collision_query48h.cpp`, bounded ordered tree query,
terrain preparation/query and spring static-query numerical paths have no native
scalar-minmax counterpart to correct. Their ordering/selection checks are source
branch predicates. `query_oracle48h.Original` rejects scalar MIN/MAX rather than
silently executing them; its existing successful H/I/J fixtures therefore do not
exercise these frozen-only words. No old RAM-reading query generator was run.

The old orchestration word range also contains six detector AABB reductions:
MIN.S at00295790/002957B0/002957D0 and MAX.S at002957F0/00295810/00295830. They are
not reached by the 48K/48L response corpora. They are implemented and exercised by
the new48O detector, whose signed-zero correction and delta proof are separate.
No assertion about unimplemented query modes, VU MIN/MAX, or arbitrary frozen
code follows from this audit.

## Saved synthetic-corpus replay

Run `python3 evidence/minmaxAudit48O/audit.py` from the repository. The script
imports initialization helpers but never calls an old generator main. It runs
with empty data backing, checks `source_reads == 0` for every direct case and
uses the existing source-read assertions for regenerated coordinator cases.
Both the explicit historical comparator and corrected comparator are executed.
The historical result is first checked against the exact preserved expected
words/case objects, then the corrected result is compared to the same data.
Original-word hashes are validated by the existing loaders where available.

| Corpus | Replayed synthetic cases | Changed intermediate selections | Changed output words/case fields |
|---|---:|---:|---:|
| Friction48J | 1,024 | 0 | 0 |
| Impact48K | 1,024 | 0 | 0 |
| Force48L | 2,048 | 32 | 0 |
| Secondary48L | 512 | 0 | 0 |
| Coordinator48K | 192 | 0 | 0 |
| Additional full coordinator48L | 256 | 0 | 0 |
| Total | 5,056 | 32 | 0 |

The exact force rows are896–927, zero-based. Each changes PC002972AC with
first word80000000 and second word00000000: legacy selects00000000; corrected
selects80000000. Every final output word remains unchanged. The exact per-row
records and observed execution counts are in `replay-report.json`.

The four saved48K cases were deliberately not rerun against saved backing data.
Their recorded calls and original code establish a narrower no-change conclusion:

- `saved-saved`: no response helper calls
- `saved-positive-contact` and `saved-positive-other-body`: prep296F10,
  impact297610, friction297390
- `saved-positive-two-records`: the same sequence twice

All use toggles `[false,false,true,true,false,false]`; no force/secondary call is
present. Their only possible response scalar selections are MIN(scale,+1) and
MAX(excess,+0), which are finite-safe for signed zero as shown above. This is a
recorded-path/algebraic assessment, not a new saved-state replay. The four rows
remain present verbatim in both historical coordinator fixture files.

## New regression inputs and coverage gaps closed

`evidence/minmaxAudit48O/edge-fixtures.json` is native-ready uint32 input/output
word data from actual original instructions, with zero source reads:

- Eight secondary cases: normal[3], cachedVelocity[3], primaryVelocity[3] inputs,
  and three expected velocity outputs. Six unit-axis normals of both signs
  generate a -0 dot; all three output words differ from the historical model.
  Two controls cover a +0 dot and +0 destination velocity.
- Six force cases use the unchanged76-input/15-output layout from
  spring-force-fixtures.bin. They cover both depth-limit zero orders, both dot
  zero signs, both speed-limit zero signs, and identify the exact changed
  intermediate PC and word. Their final outputs are unchanged.

For example normal=(+0,+1,+0), cached=(-0,-0,-0), velocity=(-0,-0,-0)
produces legacy outputs(80000000,80000000,80000000), corrected outputs
(00000000,00000000,00000000). PC002983F0 changes its selected word from+0 to-0.
Existing secondary fixtures had individual -0 velocity components but not the
all-negative-zero products needed to retain a -0 dot through all additions.
Passing their512 rows was therefore insufficient evidence for this edge.

General helper tests should also cover both operand orders for ±0, equal finite
values, finite sign changes and negative-number ordering. Whole-routine output
tests alone cannot expose the force intermediates because the later clamp masks
them. Native build/test results belong to the separate scalar regression and
48O validation reports; this audit did not build or run native code.

## Provenance and immutable history

No historical fixture binary or JSON file was changed. In particular the old
springQuery48N/report.json source list still records the historical oracle hash;
it describes the original generation event and is resolved by the archived
legacy source above. Old proof/fixture hashes should not be rewritten to pretend
the corrected source was used historically. The newly created detector48O proof
must be regenerated after the active source fix, retaining its explicit legacy
comparison mode and newly computed source hashes.

`inventory.json` records frozen instruction hashes, exact scalar PCs, complete
M/N coverage intersections, saved-case helper paths, and the historical-source
mapping. `audit.stdout.log`/`audit.stderr.log` retain the latest bounded replay.
The active source correction is forward-looking; unchanged historical outputs
are verified only for the listed corpora and bounded saved-case paths.

## Earlier native48B–48I census and corrective supersession

The separate earlier-module read-only audit found one additional active output
mismatch: `control_filter48b.cpp` and its old70-word test oracle both retained a
float-comparison tie. The eight original scalar sites are MAX at2180CC,
218110,218150,218190 and MIN at2180E0,218124,218164,2181A4. Both the70-word control
array and697-word aero array were checked byte-for-byte against the authorized ELF.

With target/prior channels all raw80000000, rates both80000000 and elapsed3F800000,
the old control implementation/model produced four+0 words; corrected original
signed-word selection produces four-0 words. The active control code now uses
the finite scalar helper. Its independent original-word C++ oracle uses signed
word selection and additionally tests zero-sign Cartesian inputs/rates/time in
all four existing host rounding modes. The old Windows48B logs/binary hashes
remain historical; they do not certify this corrected source.

Aero48E already implements correct finite mixed-zero selection, using sign-bit
OR for min and AND for max and an independent signed-word oracle. Its six scalar
sites clamp against literal+1 or+0. No scalar MIN/MAX sites occur in the original
headers for body48C, engine48D, contact/body-scene48G or wrapper48H. Ordinary branch
predicates and VU min/max are separate and were not changed. This census concerns
recovered native game modules, not a new audit of the historical compatibility
runtime under `src/core`.

Current48O supersedes the earlier control clamp, spring force selector and
secondary-velocity zero-sign behavior and their incorrect reference descriptions.
Historical archives/fixtures are preserved. The new tests assert the corrected
original-word outputs rather than accepting the legacy implementation as oracle.
New detector proof/source hashes have been regenerated after the shared oracle
fix; its fixture data bytes are unchanged. Final release/sanitizer provenance is
recorded under `evidence/nativeDetector48O-corrected-linux-20261006/`.
