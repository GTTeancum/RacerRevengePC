# Authenticated Ryloth cold visual assembly source, 2026-10-14

This packet adds `tools/assets/assemble_ryloth_runtime.py`. It is an unexecuted
source candidate. No build, syntax check, test, converter, client, shader,
gameplay, guest instruction, or emulator was executed for this work. The root
integrator owns review, cold preparation, all validation, and any later unlock.
This packet does not establish that either original Ryloth course is ready.

## Exact source findings

The original archives were read with bounded static zlib decompression and the
small source metadata slices were matched to the retained Full48F member hashes.
The authenticated Full48F generation supplies the world reference and every
original member identity, not inferred names from another track.

- RA: original archive 4,604,561 bytes, 1,010 entries; world 10 zones, 10 portals,
  1,261 placements. There are 710 distinct referenced present PSG members;
  583 of them were unsupported in retained48F. Exactly 176 placements reference
  originally absent PSGs. Exactly 178 placements have present COL members.
- RC: original archive 4,655,141 bytes, 1,121 entries; world 15 zones, 15 portals,
  1,277 placements. There are 912 distinct referenced present PSG members;
  one was unsupported in retained48F. Exactly 107 placements reference
  originally absent PSGs. Exactly 112 placements have present COL members.
- RA sky is `skies\ra.cub`, 161,825 bytes; RC sky is `skies\rcSky.cub`,
  167,607 bytes. Both have the actual existing 48-tile CUB/IPUM framing;
  the palette counts are respectively 226 and 126, not MA/SA values.
- RA authentic ATM members are `crowd` and `fholobeam`; RC adds `arrowred`.
  Their archive identities remain track-specific even where names coincide.
- RA fog is authored only on some original zones and is preserved per zone.
  RC has no original fog fields. No global default fog is invented.

Source cube semantics are already recovered in
`tests/native/original_track_sky50kx_fixture.h`: loader 0x276D7C defaults the
original value to 1 and 0x276E24 subtracts 1; presentation 0x275914 skips a
negative cube index. RA authored zero disables sky in zones 1, 2, 8; zones
3, 4, 5, 6, 7, 9, 10 select cube zero. RC's absent per-zone keys retain the
original first-cube default for all 15 zones. This is original loader semantics,
not an inference from colors. Runtime must select using the actual scene-camera
zone; the assembly tool does not run or validate that owner.

## Reused source and scope

The actual existing assembly script is `assemble_ma_sa_visual.py`, rather than
`assemble_ma_sa_runtime.py`. The new script carries its established placement,
record, material pass, mesh and include layouts, with independent RA/RC intake
admission. It uses hash-pinned existing source modules for RRSCENE1 framing,
exact SCB decoding, PST/ATM decoding, and bounded CUB framing. It never reuses
MA/SA/GA pixels, source inventories, placements, global fog or track limits.

The static scene reader currently admits the existing single-node RRSCENE1
subset. Root's later bounded original-data inspection confirmed all actual RA
and RC scenes are single-node, and the existing signed24 decoder handles the
583 retained-unsupported RA scenes. Native regeneration remains required. Any present unsupported geometry, identity-palette/multinode scene,
missing converted material, unsupported texture frame, missing authored ATM
controls, or conflicting original ATM start ownership produces a clear failure.
An originally absent PSG remains explicitly non-drawn with its exact SCB index,
source pose and collision-member presence. A present PSG can never be treated
as missing to reduce a budget. Every material pass is retained; runtime support
for all referenced programs is still required and cannot be silently replaced
with a default material or dropped pass.

## Input contract for root-owned future cold preparation

The tool requires:

1. `--track RA` or `--track RC` explicitly, with no course-selection guess.
2. `--repo` pointing to the reviewed source tree.
3. `--intake` containing the completed `prepare_ry_intake.py` output:
   exact original archive, canonical retained-generation inventory, every
   original member, reference world and intake completion provenance.
4. `--full48f` pointing to the exact retained evidence ZIP with SHA-256
   `60c041bd1acd58f7e19cea50b0c2f6d3ae1083f18d82bcbb591b3182d115aa0c`.
5. `--native-import` containing a completed reviewed native importer output
   using `RR_ORIGINAL_POD_IMPORT_V1` and the established filenames and fields.
   Source copies, member offsets and extents must equal the exact intake.
6. `--import-report-sha256`, the root-reviewed exact `import.json` digest.
7. `--native-output-index-sha256`, the root-reviewed exact
   `native-output-index.json` digest. That index has schema
   `RR_REVIEWED_RYLOTH_NATIVE_OUTPUTS_V1`, `track`, `archive_sha256`,
   `import_report_sha256`, and `files`, each entry containing `path`, `bytes`,
   and `sha256`. It must cover every importer source copy, RRSCENE, RRMESH,
   RRTEX and native mip referenced by the import report. The root creates and
   reviews this index after its authorized import, not this helper.
8. An explicitly new, private, nonsymlink `--output-root`, separate from the
   repository, original intake and native import. Parent traversal is rejected,
   the existing nonsymlink parent is resolved canonically, and every output
   write uses that exact normalized root; plus
   `--execute-preparation` after root authorization.

The original archive and retained-generation hashes are checked independently
of the completion marker. Every original intake member is authenticated before
native output begins. The current decoded SCB must equal the entire retained
world reference in the JSON domain, including all source field provenance,
placements and matrices. Native output identities are rooted in the explicitly
reviewed output index; no caller-chosen filename is enough by itself.

The tool does not build or launch the native importer. Its future authorized
execution would use the existing scalar FFmpeg source IPUM image decode for
original sky/texture data, on the native host CPU. This is a cold image codec,
not guest instruction execution or chipset emulation; its IDCT precision is
not claimed to match a PS2 framebuffer.

## Output contract and gates

Future successful execution would emit `evidence/visualRA` or `visualRC`,
`evidence/texturesRA` or `texturesRC`, `evidence/skyRA` or `skyRC`, and complete
`evidence/atmRA` or `atmRC` banks, plus track-specific include fragments under
`tests/native`. It preserves the existing visual profile layout, exact native
cache records and placed normal mesh report. The retained original world
reference is included for provenance. Sky manifests retain exact environment
fields, original cube bindings and authored per-zone fog fields. ATM manifests
retain every source frame, timing/start bindings and original material hashes.

Only `ASSEMBLY-COMPLETE.json`, emitted after every closure finishes, identifies
a successful assembly. A directory, partial mesh, `asset-selection.json` or
profile include alone is not completion. Failure leaves partial private output
without that sentinel; discard or quarantine it rather than consuming it.
Success explicitly records `gameplay_ready=false` and `game_launched=false`.

Remaining blockers: review and execution of the exact original intake and
native importer; authenticated native regeneration of actual RA/RC PSGs;
renderer program/pass compatibility; source-camera sky-zone ownership; runtime
collision/navigation gates; root-owned cold conversion and independent native
checks. No launch or unlock is authorized by this source-only packet.
