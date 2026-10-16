# Ryloth geometry format recovery inspection, 2026-10-14

## Scope and evidence boundary

Source and retained-data inspection only. No build, test, syntax check,
sanitizer, project executable/helper, game, guest instruction execution, PS2/VU
emulation, commit or user-computer operation was performed. Root owns
integration and all validation. No implementation change is proposed for
publication before actual raw-source inspection. This packet does not select,
unlock or enable a course.

The inspected Full48F ZIP is
`RacerRevenge-Checkpoint48F-NativeWorldCacheAndRendering-Evidence.zip`, with
previously authenticated SHA256
`60c041bd1acd58f7e19cea50b0c2f6d3ae1083f18d82bcbb591b3182d115aa0c`.
Counts below were read directly from its generation member
`evidence/importFull48F-20261003/native-cache/generations/3cf500075d2f31e3930d5e74896a3068ca596fba3f47475ba9c4643bcbf60b39.json`
and RA/RC reference JSONs under `evidence/worldImport48F/`.
This inspection did not independently rerun whole-ZIP authentication.

RA archive identity: `TRACKS/RA.RES`, 4,604,561 bytes, 1,010 entries,
SHA256 `1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2`.
RC archive identity: `TRACKS/RC.RES`, 4,655,141 bytes, 1,121 entries,
SHA256 `ce1b41b845935210c288c86d76b3ae0b99f198fb2c29190889c005631c7d2a8b`.

Raw original RA/RC PSG and PSM payloads were not available to this helper.
The retained generation does not include rejected command words, component
words or error byte offsets. Therefore a historical `scene.attribute_command`
status cannot establish which attribute failed, nor prove V4_32 encoding,
compressed-position semantics or current decoder success.

## Exact historical status categories

RA: 127 converted geometry, 45 converted materials, 44 converted textures,
1 converted world, 793 unsupported = 1,010 entries.

RA unsupported breakdown:
- 580 PSG: `scene.attribute_command: Unsupported original attribute command shape`
- 3 PSG: `scene.attribute_variant: Unknown original surface attribute variant`
- 3 PSM: `psm.sections: Extra inactive passes or Prologue/Epilogue need separate original semantics`
- 2 PSM: `psm.map_count: Count is outside supported bounds`
- 205 outside the converter: 178 COL, 7 TGA, 7 VUB, 2 SPL, 2 ATM, 2 INI,
  and one each TOB, AUB, PRT, EMT, CUB, POS, TUN

Thus RA has exactly 710 PSGs, of which 583 were historically rejected. The
other 210 unsupported entries are not PSG geometry failures. COL rejection
here means outside that converter's scope, not an absent source or proof that
the existing native collision decoder cannot handle it.

RC: 911 converted geometry, 43 converted materials, 19 converted textures,
1 converted world, 147 unsupported = 1,121 entries.

RC unsupported breakdown:
- 1 PSG: `scene.attribute_variant`
- 19 PST: `pst.kind: Only 256-entry indexed kind 1 is supported`
- 2 PSM: `psm.sections`
- 1 PSM: `psm.map_count`
- 124 outside the converter: 93 COL, 10 TGA, 5 VUB, 3 EMT, 3 ATM, 2 SPL,
  2 INI, and one each TOB, AUB, PRT, CUB, POS, TUN

RC has 912 PSGs. Its 19 texture-kind statuses are not geometry failures.

## Placement relevance, without inventing missing visuals

Normalized member-name comparison of the retained reference's derived
`geometry_member` candidates to the actual generation inventory gives:

RA, 1,261 placement records:
- 468 reference records name 127 converted PSGs
- 614 name all 580 command-rejected PSGs
- 3 name the three variant-rejected PSGs, one record each
- 176 name 176 absent derived PSG candidates

RC, 1,277 placement records:
- 1,169 reference records name all 911 converted PSGs
- 1 names the variant-rejected PSG
- 107 name 88 absent derived PSG candidates

Every absent candidate record, all 176 RA and all 107 RC, has its associated
exact `collision_member` present in the inventory. These facts strongly support
checking collider-only ownership rather than declaring missing geometry.
They do not alone prove the original render-owner classification. Never add
synthetic meshes for absent derived PSG candidates.

All present PSGs occur among the retained placement candidates. The RA failures
therefore affect concrete source candidates rather than merely unused inventory
names. Present PSG occurrence alone still does not establish live visual-owner
classification, material dispatch or draw eligibility.

Exact historically variant-rejected members:
- RA `geometry\\loft01.psg`, 1,968 bytes,
  SHA256 `592300e0deacb20b957d13a7d1a1b3895b53d53cb3fe98a8ed733444b443b1e8`
- RA `geometry\\loft02.psg`, 1,968 bytes,
  SHA256 `16e7f4a0bd5c9cdd993e0551777e9fcdf801303343788d18e0b7aadc1ccca351`
- RA `geometry\\smokenull01.psg`, 992 bytes,
  SHA256 `b7273c7a28541ccacab3acf3df25e2e2afd98995564cedbe42b6e9d96a67ac1f`
- RC `geometry\\ubershroom4_col.psg`, 1,024 bytes,
  SHA256 `6ecbbe72864b3afad4026562294ef8958373edcc82d6560dd82500ef6d2333b8`

RA section-failed materials: `textures\\gatedoor01.psm`,
`textures\\tile003.psm`, `textures\\gatepost01.psm`.
RA map-count-failed materials: `textures\\flarezretrieve.psm`,
`textures\\gfxtest.psm`. RC section-failed materials:
`textures\\rc_track4.psm`, `textures\\rc_sand02.psm`; map-count failure:
`textures\\gfxtest.psm`. An actual scene material-table join is required before
attributing these inventory failures to a specific rendered surface.

## Current native support versus historical failures

`src/assets/import_scene48d.cpp` already provides:
- Default V4_16 signed positions (`0x6d`, four signed16 lanes)
- Opt-in V4_32 positions (`0x6c`, four signed32 lanes), with XYZ bounded to
  authored signed24 range -8,388,608 through 8,388,607
- Signed8 normals, signed16 UVs, source quantization and source origin
- Exact group tags, count/address command matching, bounded padded extents,
  source ADC/bone words, strip topology and continuation accounting
- Exact components `0x00ff010f`, `0x00000107`, and `0x00ff0107` only with
  one node; absent colors remain absent semantic colors

`SceneLimits48D::allow_signed32_quantized_positions` defaults false. Its header
explicitly requires the caller to establish the original compressed resource
path. Globally enabling it merely because an opcode is `0x6c` is invalid:
floating-position/animated source paths can use the same attribute width.
The failure code arises inside `take`, which checks each exact command; it is
not specific to position. Later range, strip, bounds, palette or trailer gates
can still reject a source after the first historical failure is resolved.

`tools/assets/original_roster/ma_compressed_scene_admission.h` demonstrates a
bounded source-authenticated admission pattern: exact MA archive/scene hashes,
actual scene material names, exact corresponding PSM identities and actual
one-pass `PrelitCompressed` selection before granting scene-specific limits.
It cannot be reused with RA data by changing a label or globally granting the
flag. Its set of 21 scenes and 18 materials is MA-only.

Current `src/assets/import_texture48e.cpp` also differs from the historical
converter: PST kind 0 (16-entry indexed) and kind 1 (256-entry indexed) are now
supported; kind 2 packed1555 remains unsupported. The 19 RC historical kind
errors do not disclose their actual kind and cannot all be claimed recovered.
Inactive higher numbered PSM sections are now retained with source provenance;
Prologue/Epilogue remain unsupported, and map count remains bounded to 1..2.
The old shared `psm.sections` reason does not distinguish inactive sections
from auxiliary sections, so current material recovery counts remain unknown.

## Minimal defensible continuation and exact dependencies

1. Obtain exact RA/RC archives through the approved central desktop task and
   reviewed intake boundary. Authenticate archive, member names/order/extents,
   sizes and all retained member digests before publishing any output.
2. Inspect each of the 580 RA command-failed PSGs as source data. Record the
   actual first failing offset/word, attribute role, group count/address,
   component word, node/LOD counts, quantization, source materials and source
   identity. Distinguish signed quantized positions from float/animated paths.
   No generic VIF decoder or guest program execution is required or permitted.
3. Inspect all four exact variant cases. If a case is the already-supported
   single-node `0x00ff0107`, use existing admission unchanged. Other component
   combinations require their original native reader semantics, complete
   streams and downstream consumers before widening the exact allowed set.
4. Build a separate RA cold admission only for the exact source subset proven
   to select the compressed program through every actual used PSM pass. Reuse
   existing scene decoder limits rather than adding synthetic geometry or
   changing the default decoder. Establish a material/program closure; do not
   assume all RA programs are one-pass `PrelitCompressed` because MA was.
5. Root runs the current native importer for the authenticated subset and
   records fresh per-member errors and output provenance. Root validates
   semantic scene round trips, exact source material joins, extents, original
   bounds, vertex/index/strip accounting and full referenced-present closure.
   An initial-error resolution is not a full conversion or rendering claim.
6. Runtime binding consumes immutable native scenes only after complete
   required PSG/PSM/PST/ATM closure and original owner classification. Preserve
   source positions, normals, authored colors, UVs, material pass semantics,
   world-space root bypass and real scene placement. Diagnostic neutral white
   export is not evidence of an authored color or a default material.

No source-file ownership beyond this document was requested or exercised.
The recommended first change is source-specific authenticated admission if
raw evidence supports it; no new decoder opcode is yet justified by the
available retained evidence. If unsupported streams remain, pause the
specific asset and extend the original semantic decoder owner with source
proof, never a guessed shape, skipped required visual or fallback material.
