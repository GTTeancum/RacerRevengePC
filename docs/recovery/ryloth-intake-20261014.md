# Original Ryloth source-intake candidate, 2026-10-14

## Scope and status

Source preparation only. No course selection, Windows operation, build, syntax
check, test, executable, extraction, sanitizer, gameplay check or game launch was
performed by this helper. Root owns integration and every validation operation.
The user has not chosen the proposed Ryloth alternative to Malastare. This packet
must not be interpreted as approval to launch a Ryloth course.

The filename `prepare_ry_intake.py` is a descriptive tool name. `RY.RES` is not an
original archive code. The authenticated Full48F generation contains exactly
`TRACKS/RA.RES` and `TRACKS/RC.RES` for this candidate family; it contains no
`TRACKS/RY.RES` or `TRACKS/RB.RES`. The original Course parser recognizes planet
index 2 as `ryloth`, but that alone does not prove the course-slot/archive join.
The separate course-identity investigation owns that join; do not infer it from
these filenames or select a course based on the first letter.

## Authenticated retained identities

Full48F ZIP SHA256:
`60c041bd1acd58f7e19cea50b0c2f6d3ae1083f18d82bcbb591b3182d115aa0c`.
Generation SHA256:
`3cf500075d2f31e3930d5e74896a3068ca596fba3f47475ba9c4643bcbf60b39`.
Generation member:
`evidence/importFull48F-20261003/native-cache/generations/3cf500075d2f31e3930d5e74896a3068ca596fba3f47475ba9c4643bcbf60b39.json`.

- RA original archive: 4,604,561 bytes, 1,010 members, SHA256
  `1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2`.
- RC original archive: 4,655,141 bytes, 1,121 members, SHA256
  `ce1b41b845935210c288c86d76b3ae0b99f198fb2c29190889c005631c7d2a8b`.
- RA reference: `evidence/worldImport48F/ra-tracks_ra.reference.json`, SHA256
  `42520c6334dfa53b79f75ae2db855432d23b49a82fc524301f3c717a20d3924d`.
- RC reference: `evidence/worldImport48F/rc-tracks_rc.reference.json`, SHA256
  `01ef7eb45385a33a98a1e265108c7c52e1deecda40ddeccb1ceca0f81252a1ba`.
- RA SCB: `tracks\ra.scb`, 540,795 bytes, SHA256
  `57a0c9a244714d2f5f7b1ffcfb66fb182808348673b25362482458253b78ae81`;
  retained conversion reports 10 zones, 10 portals and 1,261 placements.
- RC SCB: `tracks\rc.scb`, 468,918 bytes, SHA256
  `1efe553a10cbeb349794c75c6f9e898f2871474a2dd587e0109772ae7cd1e27a`;
  retained conversion reports 15 zones, 15 portals and 1,277 placements.

Member byte totals are 8,586,749 for RA and 8,764,370 for RC. Maximum logical
member ends are 29,664,154 and 31,679,442 respectively. These are minimum decoded
extents, not exact padded RES block extents. Original RES headers were not
available in the cloud workspace; the helper therefore records the actual
native decoder extent and enforces only the authenticated minimum plus the
existing bounded native decoder limits. It does not invent a padded-size pin.
No RA.RES or RC.RES was found under `/workspace`; the user's D: original asset
folder may contain them and must be inspected by the central executor. Cloud
absence is not evidence of missing user assets.

## Native extraction boundary

`tools/assets/extract_ry_members.cpp` adapts the existing MA extraction boundary.
Its explicit arguments are `RA_OR_RC EXACT_ARCHIVE NEW_OUTPUT`. It calls the
existing native `decodeResource48C` and `extractResMember48C`, uses numbered
exclusive output files, and reports member names, offsets, lengths, source
indices, track code and native decoded extent. Archive names never become native
output paths at that boundary. It runs native host extraction only; it never
executes SLUS, MIPS or guest instructions and invokes no GPU/gameplay owner.

`tools/assets/prepare_ry_intake.py` is a standalone bridge with fixed RA/RC
archive/reference identities and fixed existing decoder/JSON dependency pins.
Root must review and pin the new bridge/helper source packet before use. The
bridge requires the SHA256 of the separately root-built native extractor; it
checks that identity before and after execution. Supplying a SHA is not a
substitute for root's source review and build provenance.

The bridge authenticates the entire Full48F ZIP in memory, the exact generation,
the exact reference, the original archive and every extracted member. It
rejects duplicate/colliding retained paths and publishes member paths only
after authentication. Outputs must be in a new private directory outside the
repository. On Windows both output and executable paths must be ASCII because
the native helper uses narrow argv. Partial failures are quarantined with
`FAILURE.json`; `INTAKE-COMPLETE.json` is published only after full authentication.
The completion status authenticates source intake, not playability.

Root-controlled cold invocation after source review/build and choosing the
exact original source archive (illustrative arguments, not executed):

```text
python tools/assets/prepare_ry_intake.py --track RA --repo REPO --archive EXACT_RA.RES --full48f EXACT_FULL48F.zip --extractor ROOT_BUILT_HELPER --extractor-sha256 ROOT_REVIEWED_BINARY_SHA256 --output NEW_PRIVATE_INTAKE --execute-preparation
```

Use `--track RC` with exact RC.RES only if that source is deliberately requested.
This script builds nothing. The helper build should link only the existing
native decoder and its normal host dependency, plus native-json; root must
choose and validate its existing native build command.

## Retained converter limitations and missing integrations

These are authenticated 48F historical converter statuses, not fresh tests and
not evidence of current gameplay failure:

- RA: 127 converted geometry members, 45 converted materials, 44 converted
  textures, 1 converted world and 793 unsupported members. Unsupported reasons
  include 580 `scene.attribute_command`, 3 `scene.attribute_variant`, 3
  `psm.sections`, 2 `psm.map_count` and 205 out-of-converter asset types.
- RC: 911 converted geometry members, 43 converted materials, 19 converted
  textures, 1 converted world and 147 unsupported members. Unsupported reasons
  include 19 `pst.kind`, 2 `psm.sections`, 1 `psm.map_count`, 1
  `scene.attribute_variant` and 124 out-of-converter asset types.

Both authenticated inventories include their own original map/AI splines,
tuning and skies: RA has `tracks\mra.spl`, `tracks\ara.spl`, `tracks\ra.tun`,
`skies\ra.cub`; RC has `tracks\mrc.spl`, `tracks\arc.spl`, `tracks\rc.tun`,
`skies\rcSky.cub`. RA has crowd/fholobeam ATM sources; RC additionally has
arrowred ATM. Existence does not prove material, animation, sky, collision,
lighting, race-owner or audio joins.

Genuine source-integration gaps at inspection time:

1. `track_session_assets50ig_fixture.h` contains RA/RC world metadata but no
   runnable `SessionAssets` binding for either. Existing selectable bindings
   cover TA, GC, GA, MA and SA.
2. No reviewed RA/RC visual assembler, sky recovery or animation-source adapter
   is present. MA/SA tools explicitly restrict their own authenticated profiles;
   they must not be relabeled or used with RA/RC data.
3. No original RA/RC member payload intake was available in this cloud workspace.
   Central-executor source authentication is needed before any payload work.
4. Course identity mapping and user choice remain separate gates. This packet
   does not enable, unlock, substitute or launch a course.

Reference `geometry_member`/`collision_member` fields contain derived candidate
names. Some apparent absent PSGs are collider-only names, and many absent COL
names are ordinary visual-only models. Therefore absence of either inferred
name must not be called a missing original asset without inspecting the original
scene's collision/visual ownership and concrete inventory. No such unsupported
missing-asset claim is made here.
