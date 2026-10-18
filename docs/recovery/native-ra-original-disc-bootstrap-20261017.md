# Native RA original-disc collision/bootstrap inputs

## Scope

The RA branch of `runSession50DU` no longer reads the historical type2 collision
corpus, numbered loose PSG/VUT/MOT fixtures, loose POD01 PHY, or loose podboxf
COL. It opens the authentic POD01.RES through `RR_ORIGINAL_ASSET_ROOT`, verifies
its existing source pin, extracts original members in memory, and uses the
existing native geometry, motion, physics and collision decoders. No generated
asset files, copied disc tree, cache, captured output or guest execution is
introduced.

Other tracks retain their existing explicit diagnostic corpus path. The existing
Fixture mutable arena and its development initialization remain; this change
removes its external data dependency for RA. It does not claim a new complete
original application constructor or validation of a running race. Genuine RA
roster/model/query/grid construction and all existing owner checks still execute.

## Source verification performed

Read-only examination of the supplied disc verified PODS/POD01/POD01.RES:
349110 bytes, SHA-256
`08b4d7655cb796da8a29d9b9a5318c0364561c659794ac9de6c9dfed7416eca3`.
The bounded RES index and zlib blocks contain all seventeen exact members used
by the bootstrap. No extracted source files were written. The PHY is 6584 bytes
and matches the existing `47fcfbaa...` identity. COL member extents are:

- podboxm.col: 1404 bytes; 24 rays, 23 nodes, 24 leaves
- chariotm.col: 956 bytes; 16 rays, 15 nodes, 16 leaves
- podboxf.col: 1760 bytes
- chariotf.col: 800 bytes

The source executable was read statically and authenticated with the existing
SHA-256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Relevant original stores/calls:

- 11B234..11B244 and 11B444..11B45C initialize query copied vectors to vf0 /
  the existing (0,0,0,1) sentinel
- 11B3A4..11B3EC initialize accept mask, list modes and empty lists
- 212178..212180 and 212764..21276C bind both spring query categories to the
  actual PHYSICSBODY registry value
- 21219C/2121A0 and 212788/21278C pass zero to the type2 normal-mode constructor
- 212790..2127A8 set secondary category-list mode 1 and append POD

The new query initializer uses those native constructor values. Native identities,
scene IDs and type tags are explicitly host namespace bindings, not frozen guest
addresses or recovered registry ordering. Original COL authors no endpoint W
words; the same already-declared host opaque-W policy is preserved, with no claim
that its poison bit patterns came from the source archive.

## Implementation

- `runtime_startup50fl_fixture.h`: explicit `AssetInput::OriginalDisc`; original
  archive authentication and in-memory geometry/motion/PHY/type0 input ownership
- `native_racer_collision50hw_fixture.h`: in-memory archive overload using the
  same atomic prepare/decode/publication path as existing file-based callers
- `original_disc_query_bootstrap50le.h`: typed source-derived query initialization
  and shared existing opaque-W policy
- `native_query_identities50kx.h`: distinct native bootstrap main690 identity
- `terrain_interactive_drive50du.cpp`: RA direct branch, non-RA corpus isolation,
  and scratch sizing from actual owned collision resources
- `original_disc_bootstrap50le.cpp`: cold source-input regression executable

## Verification on October 17, 2026

The Linux native target compiled and linked. The cold bootstrap regression
passed using the original disc directory and an empty generated-data directory.
It checks genuine resource extents, query ownership and constructor fields,
the explicit opaque-W policy, failure atomicity, and malformed archive rejection.
The complete Fixture and all thirteen supported Rig/Visual owners also passed
an independent cold-load test. Gameplay verification is recorded separately in
the current handoff; these cold checks alone do not establish gameplay parity.

For the regression executable, use C++20, `-ffp-contract=off`, `-fno-fast-math`,
the project's existing include directories, and the following existing sources:

- src/assets/model_geometry49o.cpp
- src/assets/model_bindings49o.cpp
- src/assets/import_decode48c.cpp
- src/assets/import_collision48g.cpp
- src/assets/collision_mesh48i.cpp
- src/assets/collision_runtime48h.cpp
- src/game/type2_resource49x.cpp

Link zlib. If using the existing native static gameplay library, link that
instead of listing its source files.

The integrated RA route now constructs roster, visual, cable, effect and image
owners directly from authenticated original disc resources. Non-RA legacy
routes retain their separate preparation requirements.
