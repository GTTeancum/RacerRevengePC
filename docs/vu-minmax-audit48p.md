# 48P finite VU MINI/MAX audit and historical preservation

2026-10-06. This is a bounded correction to the original-word Python reference
used for recovered native gameplay. It covers ordinary finite binary32 lane
selection, including signed zero, lane masks, and the hardwired VF0 destination.
It does not certify every physical EE/VU exceptional arithmetic mode or audit
the historical compatibility runtime. No native builds, installations, or game
processes were used. No original RAM was read or reconstructed. Every replay
uses `data=b''` and explicitly initialized fixture bytes.

## Finding and independent semantic check

The pre-48P `query_oracle48h.py` used Python `min`/`max`, which retain the first
operand on a floating-point tie. It also wrote the destination vector even when
that destination was VF0. These are incorrect for VU MINI/MAX.

[PCSX2 VUops.cpp](https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/VUops.cpp),
lines 721–773, checked 2026-10-06, provides an independent VU-specific check:
`fp_min`/`fp_max` select by signed raw words, reversing ordering when both
operands are negative. `applyMinMax` writes only enabled lanes and skips VF0.
Thus MINI selects negative zero and MAX selects positive zero for opposite-zero
operands, irrespective of operand order. This verification is separate from the
48O scalar EE MIN.S/MAX.S audit.

The active reference now uses this finite selection rule for function codes
`0x2f` and `0x2b`, and suppresses those instructions' VF0 writes. No other VU
opcode, scalar operation, original instruction word, or native source was edited.
The API `Original(..., legacy_vu_minmax=True)`, or the equivalent instance flag,
explicitly restores the old VU behavior for delta audits only. It does not undo
the scalar48O correction inherited by `SpringOriginal`.

The exact old source is preserved at
`evidence/vuMinMax48P/query_oracle48h-legacy.py`:

- Archived SHA-256: `2873908631c84908efe8388d779b2b985b31b176ba11b088f554a43b7f9e5b99`
- Corrected SHA-256: `ff6a9069093e7f157aea7b143a3a72581791af937fe1b6dde7e3d52271f72c79`
- Unchanged scalar source SHA-256: `32f2d7e3d392df8fbf8ea66e218777d38c6e0a9853bda73e8e495686d8dafe53`

## Instruction inventory and recovered historical scope

The broad frozen48H extraction contains four affected instructions, all with
`vf6` destination, `vf4`/`vf5` operands and XYZW mask:

| PC | Word | Operation | Role in newly executed type3 preparation |
|---|---|---|---|
| 00107924 | 4BE521AF | MINI | Vector-min helper00107910 |
| 0011E120 | 4BE521AF | MINI | Inline minimum reduction |
| 0011E15C | 4BE521AB | MAX | Inline maximum reduction |
| 0011E314 | 4BE521AB | MAX | Vector-max helper0011E300 |

They are genuinely executed by original type3 preparation11DD00 in new48P.
The previously generated183-case48P corpus executes each site238 times. No
opposite-zero lane pair occurs there, so all those earlier selected lane words
and all recorded outputs remain unchanged under the correction.

None of the four sites, or any vector/broadcast VU min/max opcode, occurs in
recorded execution coverage for these earlier corpora:

- Friction48J (362 covered words), impact48K (1,416), force48L (317), secondary48L (45)
- Synthetic coordinator48K (1,967), extended coordinator48L (2,370)
- Terrain preparation48M (237)
- Static callback48N (175), spring records48N (51), full static query48N (2,775)
- Query preparation48O (283), detector48O (357)

The complete recovered body-wrapper48H code array and its earlier48C–48G
code-array dependencies contain no VU min/max opcode. The inventory identifies
code arrays through their paired start-address constants and excludes saved data
arrays. Independent C++ interpreters were not rewritten merely because they may
contain generic float-selection code. No earlier recovered native VU-minmax
counterpart or signed-zero counterexample was found in this bounded scope.

For query48H, the direct leaf10B840–10E080 word range contains no VU min/max.
The implemented ordered type2 path from48I and its terrain/static-query use are
covered by the complete48N query proof; the full132-case48N corpus was additionally
replayed under both models with no VU min/max executions and no output changes.
The earlier raw-RAM48H/48I/48J generators were not rerun. This is not a new claim
about arbitrary unimplemented dispatch modes or all code in the broad48H freeze.

## Reproducible tests and delta evidence

Run from the repository:

`python3 evidence/vuMinMax48P/audit.py`

The script writes only its additive audit outputs. It imports initialization and
execution helpers, never historical generator mains. Frozen word loaders verify
their saved hashes. A constructor guard rejects nonempty backing data during
corpus replays, and every resulting machine is checked for zero source reads.

### Direct instruction regression

12,800 instruction cases compare three models: corrected, explicit legacy flag,
and exact archived source. They cover:

- MINI and MAX
- Both operand orders of ±0, equal values, finite signs and reversed-negative ordering
- Ten finite words including positive/negative minimum normals and maximum finite values
- All16 lane masks, including empty mask
- VF0 destination, ordinary destination, and both source/destination aliases
- Every unwritten register/lane retaining its original word

Expected values use an independently expressed monotonic sign/magnitude order
key, rather than the corrected signed-integer implementation. All12,800 pass;
2,990 instruction cases expose an old selection or VF0-write difference. The
explicit legacy flag exactly matches archived-source behavior in every case.
These are instruction-unit tests, not claims that every synthetic opcode encoding
appears in the recovered game.

### Full routine replay

| Corpus | Cases per model | VU selection differences | Output differences |
|---|---:|---:|---:|
| Full static query48N | 132 | 0 | 0 |
| Query preparation48O | 332 | 0 | 0 |
| Archived first type3 query48P | 183 | 0 | 0 |
| New mixed-zero type3 queries | 24 | All24 cases | 20 cases |

The first183-case48P fixture, report and executed-word file are preserved in
`evidence/vuMinMax48P/type3Query48P-before/`. Their fixture SHA-256 is
`091ab4bb3da577d458f3c4c677b99e4d19635a8265f4759c1da6297507a82ffd`.
The audit replays these expected objects using the current generator. It strips
only the new `requires_mode2` classification field if absent from the archived
object; every pre-existing field and word must compare exactly. The earlier
48P generator source was not separately archived, and no claim is made that the
current generator source is that historical generation source.

The signed-zero audit stage contains209cases:183previous cases,24new
mixed-zero endpoint-order cases, and two additional lazy mode2 dispatch cases.
That exact stage is preserved under
`evidence/type3Query48P/before-transform-extension/`. The new fixture SHA-256 is
`4cee7d2a492a75f34ac65c5823432fe957783979747f8066b459b0ce946c1a14`.
Its `vu-minmax-impact.json` records every changed selector and output path:
64 changed selected lanes and296 changed snapshot words across20cases.
This audit independently replays all24new cases under both modes and checks the
corrected full expected objects exactly.

For example, `mixed-zero-crossing-0-reverse-0` has first endpoint
`(+0,+1,+0)` and second `(-0,-1,-0)`. At0011E15C, X and Z compare first-operand
`80000000` with second-operand`00000000`. Legacy MAX retains`80000000`;
corrected MAX selects`00000000`. The changes survive in prepared centers[4,6]
and query_sphere[0,2], and their later recorded snapshots. Four other mixed-zero
cases have intermediate differences that subsequent arithmetic erases. The
full-routine evidence is therefore needed alongside direct selector tests.

## Native integration and immutable history

The new type3 preparation must use the independently verified finite VU ordering.
Reusing an already correct bit-order helper is valid, but its justification here
is the VU-specific source check, not an inference that scalar and VU instructions
must be identical. Its native differential tests should consume the corrected
`expected.after_prepare` objects, including the new opposite-zero cases.

No historical48H–48O fixture was regenerated. `historical-preservation.json`
checks165 preserved evidence/source files and permits only the active query
reference change. The old scalar48O evidence and oracle remain untouched.
Historical reports that record the previous query oracle hash retain that hash;
they describe the original generation event and resolve through the archived
source above. Newly regenerated48P reports identify the corrected source.

Files under `evidence/vuMinMax48P/` include the source archive, historical hash
manifest, instruction/coverage inventory, direct regressions, complete dual-model
replay report, new mixed-zero replay, preservation report and execution logs.
Native validation belongs to the separate48P preparation and full-query reports.

## NEW48P proof provenance refresh

A later provenance-only check found one new48P source map still carrying the
pre-correction interpreter: `springCallback48P/native-validation.json`.
Callback and leaf generation reports did not originally record interpreter source
hashes; node and full-query reports already recorded the corrected interpreter.

`python3 evidence/vuMinMax48P/refresh_provenance.py` reruns only the existing
callback frozen generator and leaf frozen-input generator, checks exact original
report/word/payload reproduction, then appends explicit current generation-source
provenance to their new48P reports. On2026-10-06:

- Callback:768cases,209,401original instructions; payload SHA-256 remains
  `63f307d35cfa51109863c1bd247510a0190c725ffe72c424a0252b27a90c2728`
- Leaf:904cases,525,757original instructions; payload SHA-256 remains
  `2066b9c1e914fecc6dfc36071cebfb2eb4fbed455c4b045680786e3e7794bb5a`
- Both have zero source reads, empty backing data, and no executed VU min/max
- 164 historical files other than the already authorized active query oracle still
  match their pre-audit hashes

The callback validation report identifies the fresh reference-generation source
pairing. Its original native-run source map and exact original report are retained
separately, and it explicitly states that no native binary or test was rerun.
All its native sources, original words, fixture bytes and previously recorded
native test results are unchanged. This is a refreshed reference proof of the
same inputs/expected outputs, not a new native validation event.

Preserved reports, command logs and the outcome are under
`evidence/vuMinMax48P/provenance-refresh/`. The refresh neither edits nor reruns
the full-query generator, so independently added query fixtures must retain their
own freshly generated provenance. The209-case counts above describe the
signed-zero audit stage; later additive query expansions are recorded in the
current `evidence/type3Query48P/report.json`.


### Final expanded query mapping

The final additive query proof contains 245 cases, fixture SHA-256
`a87159b1b858f6351b17de655152ad4d6fb69aed228132e1f5f27ea37b1c7f3f`.
A read-only provenance check verified that all 209 prior complete case objects
(inputs and expected outputs), and all prior mesh objects, are identical in this
expanded fixture. Every source hash in its report matches the current file,
including the corrected interpreter. Callback, leaf and node proof reports also
record that same interpreter hash.

This mapping does not claim a legacy-VU dual-model replay for the 36 added
transform/gate/tree cases. The earlier audit remains the archived 183-case
baseline replay plus the 24 signed-zero cases, with two mode2 classification
controls present in the preserved 209-case snapshot. Exact paths, hashes and
scope are recorded in
`evidence/vuMinMax48P/provenance-refresh/final-query-mapping.json`.
