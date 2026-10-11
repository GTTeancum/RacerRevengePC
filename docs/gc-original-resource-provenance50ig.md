Historical source/staging notes recovered into main on 2026-10-11. Current main includes later original-resource, roster, compound collision and native Gate work. These notes and the offline visual selector do not establish gameplay validation or expand current runtime admission.

# GC original-resource admission and short-position collision WIP

## Status and scope

This document contains authored findings and source references only. It contains
no original game assets, decoded geometry, texture pixels, instruction listings,
raw instruction/data words, transfer receipts or private machine paths.

The staged change is limited to `src/assets/import_collision48g.cpp` and `.h`.
It has been source-reviewed but has not been compiled, syntax-checked, tested or
run in gameplay. No guest CPU/device execution was used. Original-resource
extraction and conversion were performed as asset processing; that does not
establish native gameplay correctness or complete track support.

## Authored resource closure

The inspected GC archive contains 1,190 original members, including its SCB,
M/A splines, track tuning, sky cube, static geometry and collision source.
The original SCB matches the source already embedded in the retained GC world
fixture. GC is affiliated with Gamorr according to its original track-announcer
references; no localized course subtitle has been established here.

Static SCB closure:

- 12 zones, all authoring List search; 13 portals; 999 ordered static entries.
- No explicit collision-count, COL-name or StaticType overrides in those entries.
- 801 visual placements resolve to 426 distinct original PSG members.
- 198 remaining entries resolve to original collision-only geometry; no static
  entry lacks both its referenced visual geometry and collision member.
- 551 collision placements reference 258 distinct static COL members.
- 257 of those COL members fall within the existing float-position reader.
- The remaining static member, `geometry/tree_branch_d.col`, is COL version 1,
  type 0 with short-quantized positions and no optional vertex normals. Its
  eight placements are zone3-model204 through zone3-model209, plus
  zone8-model37 and zone8-model38. They must not be omitted or substituted.
- The tree-branch source contains 81 vertices, 143 ordered nodes and 144 faces.
  Its reverse flag is zero and its local transform is identity.

The actual M spline contains 188 nodes; the A spline contains 195. Original
start, route, branch, width, speed and progress metadata are present. Resource
presence is not a substitute for native spawn, navigation, AI or lap integration.
The actual sky source is `skies/sky21.cub`; a TA sky is not a valid replacement.

All 426 statically referenced PSG members were accepted by the retained native
scene importer. Across the entire archive, eight other PSG members exceeded
that importer's format support, but none is referenced by the static SCB list.
This does not establish that those other members are irrelevant to animated
owners or environment effects. The selected static material names resolve,
but authored multipass/projected material handling, textures, sky and runtime
rendering remain separate integration responsibilities.

## Unsupported animated collision is actually authored

`anim/opening.col` and `anim/windmill.col` both have original COL version 1,
**type 1**. The staged reader continues to support only type 0 triangle trees;
this change does not reinterpret type 1 as type 0.

Neither type-1 member appears among the 551 static collision placements.
However, GC's TOB/TOY defines two real keyframed animation owners:

- Animation1 uses `opening.mcf`, starts in zone 11, is initially inactive and
  non-looping, and is triggered by the authored gate action.
- Animation2 uses `windmill.mcf`, starts in zone 2, is initially active and loops.

Those MCF files identify matching animation content directories, SLT models
and VUT motion data. Thus there are zero static placements of these type-1
members but two corresponding authored animated owners. They are not proven
unused resources. The actual dynamic collision-instantiation path and moving
owner updates have not been integrated or validated by this work. A completed
static collision layer must not be presented as complete animated collision.

## Original-source conversion basis

The implementation is grounded in the authenticated original executable and
retained source annotations. Exact executable address ranges used:

- `0x125AA0..0x125B94`: quantized flag and six-byte-per-vertex source storage.
- `0x106ED0..0x107104`: position helper called by the type-3 leaf at `0x10E19C`.
- `0x107AE0..0x107D28`: quantized and float position paths feeding the type-2
  node/triangle axis helper.
- `0x108400..0x108678`: type-2 leaf position prefix.
- `0x10B8C8..0x10BAEC`: terrain triangle position prefix; both formats converge
  at `0x10BAF0` before shared geometric calculations.
- `0x10C4C8..0x10C4F4`: absent optional normals select geometric-normal output.
- `0x383640`: original position ScaleFactor, numerically 32767.

Each quantized position component follows the same source sequence: sign-extend
its original 16-bit value, convert to binary32, multiply by the source binary32
reciprocal of 32767, then separately multiply by 32767. Both products retain
individual nearest-even binary32 rounding. A plain integer-to-float cast is not
the source conversion, even when it appears numerically equivalent.

The cold importer performs that source-defined projection once. Existing live
queries receive the same float-coordinate boundary reached after the original
prefixes. This changes storage representation, not authored geometry, topology,
face planes, candidate order, tree traversal or collision selection policy.
It assumes the pinned original scale and ordinary strict binary32 runtime
policy; it is not a claim about arbitrary mutated original globals or all PS2
exceptional floating-point behavior.

## Cache provenance and retained limits

The RRCOL001 layout remains unchanged. It stores expanded float coordinates,
retains the source quantized flag and embeds the complete original source.
Original source offsets follow the six-byte stride. Serialization rereads the
original signed-short words, checks every typed coordinate against its exact
source projection and reproduces the original bytes. No reverse-quantized
estimate or replacement mesh is manufactured.

Admission remains bounded:

- COL version 1, type 0; existing vertex/node/face and byte caps.
- Quantized optional vertex normals remain explicitly unsupported.
- Quantized preparation requires FE_TONEAREST.
- Existing topology, count, extent, finite-plane, source-provenance and
  unsupported-single-leaf boundaries remain enforced.
- The unquantized twelve-byte path remains unchanged.
- No hot decoding, allocation or capacity growth is introduced.

The separately staged cold owner layer counts actual original placements and
reserves complete stable storage once. Its ceiling is 1,021 static owners,
leaving three dynamic/query references within the 1,024-slot SearchList.
The original 64-entry query-selection/merge semantics are not enlarged.

## Review and validation handoff

Importer preimage hashes:

- `src/assets/import_collision48g.cpp`:
  `5ae3bca273f20584a14849a8dc7d3b7bc40caf790344f77ae6ea61859d50d4f2`
- `src/assets/import_collision48g.h`:
  `1264f48a6a3bae24f0fc8f0b1d2994a398d7b4a0c776bfff36fc027a219ea764`

Staged source hashes:

- CPP: `fccec3dd3ea8fcbc60081618f5d9fab254f4dbe70f2d006f71baf111312a6ff6`
- Header: `19e266220a5a1a56f156a1a1934dc9e437603497f2cb895cd2c169cbc2034b3d`

These are WIP source identities, not test results. Before enabling GC, validate
the actual member's decode/cache provenance and complete collision query path,
retain TA equivalence, and exercise the rejection boundaries. Full original
visual/material/sky, spawn/navigation/AI, animated-owner and gameplay/video
acceptance remain independent requirements.
