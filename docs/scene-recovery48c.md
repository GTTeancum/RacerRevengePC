# Original scene recovery 48C

48C expands the offline, semantic PSG3 decoder. It does not execute PS2 code or
play back captured graphics packets. Frozen 48B remains unchanged. This work is
the next bounded foundation for the planned built-in PC importer, not a complete
importer or a playable scene. Xbox packaging remains a later target.

## Results and acceptance

| Original container | PSG members | Structurally decoded | Rejected | LOD0 vertices | LOD0 triangles |
| --- | ---: | ---: | ---: | ---: | ---: |
| TA.RES | 230 | 228 | 2 | 104,554 | 66,164 |
| POD01.RES | 13 | 13 | 0 | 8,332 | 5,282 |

The candidate totals include local meshes and faces with unresolved normal
alignment. They are **not** a full-track acceptance claim. All 140 meshes accepted
by the frozen 48B decoder produce byte-identical RRMESH01 output in 48C.

The conservative world-space aggregate grows from 20 meshes / 1,414 triangles to
26 meshes / 6,035 triangles (9,981 vertices). It includes only original world-space
geometry with one identity node and no opposing-normal triangles. The six added
members are `zone01groundstrips01`, `zone01groundstrips02`,
`zone01groundstrips17`, `ta_short`, `ta_cave`, and `ta_cavetop` under `geometry`.
Their authored positions are unchanged. No local prop has been placed by inference.

The authored Pod01 setup selects `chariot.slt`, `englt1.slt`, and `engrt1.slt`.
Their matching PSG files provide 4 LODs each and 18/14/14 hierarchy nodes. Applying
the original node matrices and joining their LOD0 surfaces gives an authored
bind-pose candidate with 5,755 vertices and 3,571 triangles. No alignment offsets
were added. This includes the original flame surfaces; runtime animation,
vehicle physics, cable effects, damage visibility and flame shading are absent.
The separate vehicle parameters (including engine positions in Pod01.phy) have
not been interpreted as scene placement or applied a second time.

Useful outputs:

- `evidence/assets48C/TA/world-static-neutral.rrmesh`: conservative world subset.
- `evidence/assets48C/POD01/pod01-bind-neutral.rrmesh`: three authored components.
- `evidence/assets48C/POD01/assembly.json`: original setup hash and component spans.
- Each numbered `.rrmesh` has a corresponding `.json` containing all original
  nodes, material names, LODs, surfaces, local vertices, normals, bone IDs, UVs,
  original color presence, topology and stored-bounds checks.
- Each `inventory.json` contains member/container hashes, logical RES offsets,
  named rejections and output hashes. `manifest.json` freezes this packet.

## Why the 47 strip-count failures are resolved

48B counted every packed strip restart as a distinct original strip. Original
long strips are split into groups of at most 16 vertices, repeating the last two
vertices at the next group. Thus the on-disk debug count counts source strips,
while ADC/restart signs count packed strips.

48C requires each continuation to be adjacent, start at the beginning of the
next group, follow a full 16-vertex strip, and repeat **every** endpoint attribute:
quantized position, rigid bone ID, normal, UV and color. It then requires
`packed strips - proved continuations == original source-strip count`. It retains
the original triangle emission and alternating strip winding. No debug count or
attribute mismatch is waived. Across the 47 historical failures, exactly 166
continuations account for every discrepancy. Across all supported TA surfaces,
444 continuations are recorded; POD01 adds 36.

The supplied later CarsTools source corroborates this mechanism in
`StripMesh.cpp:661–740` (`CreateStripList`): advancing to the next chunk repeats
the final two indices. The original bytes and header reconciliation establish
that this specific mechanism applies here. Later PSG versions, packet-reordering
rules and ADC encodings are not assumed compatible.

## Original hierarchy and material evidence

The original ELF `SLUS_202.68` SHA256 is
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
The evidence script preserves instruction comments and verifies each opcode
against the ELF load segment: **751 distinct instructions** across eight source
files. `source-evidence/anchors.json` records the generated-source hashes and
the precise excerpts of CarsTools used as reference.

- `FUN_0026e820`, original VA `26EBE0/26EBFC`, reads magic and version. Its
  `26ED24/26ED44` calls `28CAC0` to read the hierarchy.
- `28CB04` reads node count. `28CB68/28CB88` read a 128-byte node name and a
  64-byte matrix. `28CBFC/28CC1C/28CC3C/28CC58` read center, half-extent, radius
  and parent index. This independently establishes the 224-byte node record.
- `28CC90–28CCC8` interprets parent `-1` as root and otherwise indexes the
  object array, storing the parent object pointer at object offset `0x180`.
- `FUN_002897b0` walks that parent relation. At `289804–289810`, it calls
  `289940` with destination child world matrix (`child+0x100`), parent world
  matrix (`a1`) and child local matrix (`a2=child+0xC0`).
- `289940–2899B0` forms each output row as child-row components multiplying
  the parent's rows. This independently confirms the row-vector, row-major
  composition `world = local * parent_world` used by 48C. This is static
  original instruction evidence, not an executed guest fixture.
- `FUN_0026e640`, `26E670/26E6A4`, reads the material count and each 64-byte
  material name. Surface material indices in the original PSG select this table.

For rigid position W, 48C accepts only signed multiples of16 with absolute
values in the available node range. The sign encodes the existing strip ADC
suppression; `abs(W)/16-1` selects a rigid bone. Multi-node files additionally
must carry the observed identity matrix-palette trailer. Nonidentity palettes
are rejected. Every bone's decoded local bounds, across every LOD, must agree
with its independent stored hierarchy bounds within one position quantum plus
the small float allowance. All decoded TA/POD01 bones pass. In the three full
pod models, maximum errors are less than 0.00027 game units. Empty attachment
nodes have zero stored extents and remain in the hierarchy.

The later Cars source independently describes parent-relative matrices in
`BaseConvert.cpp:1054–1084` and per-bone extent accumulation in
`StripMesh.cpp:300–375`; its hierarchy writer is at `BaseConvert.cpp:955–1049`.
We use these correspondences specifically. Original PSG3 and later Cars PSG5
are not treated as the same format wholesale.

## Supported bounded format and remaining gates

The decoder accepts only original `psg\0`, version 3, one/four LOD layouts,
observed rigid surfaces `0x00ff010f` (position/normal/UV/color) and `0x00000107`
(position/normal/UV), signed16 positions, signed8 normals, signed16 UVs, and the
exact supported group templates. It checks counts, sizes, finite values, affine
matrices, parent ordering, material IDs, payload extents, attribute padding,
matrix-palette identity, full byte consumption and a total-vertex cap before
export. The per-asset parser is importable and throws `FormatError` with a named
reason. Repository paths are derived from the module location; no drive letter
is embedded in conversion logic. The inventory command accepts an output path.

The two remaining TA rejections, `anim\xsailbarge.psg` and `anim\x_sab.psg`, use
position command `0x6c` (V4_32), while accepted meshes use `0x6d` (V4_16). They
remain rejected at byte676 and byte1124, respectively. Their floating-position
and animated hierarchy semantics have not been broadened based on appearance.

The RRMESH01 diagnostic format stores only positions, UVs, RGBA and indices.
Full LOD/material/node provenance is retained in adjacent JSON. Where color is
absent on disk, semantic JSON records null; the expressly neutral diagnostic
export fills required RGBA with white. This is a display convention, never a
claim about original material color. For colors present on disk, bytes remain
unchanged. 48C does not extend the unsettled PST1 palette/swizzle interpretation
or claim accepted original lighting, alpha, texture mapping or shaders.

17 TA candidates contain43 triangles whose geometric winding opposes the sum
of their original local normals. These are preserved and reported, never
flipped to pass a check. POD01 contains111 opposing-normal triangles after
excluding tests that would compare different bone coordinate spaces; the
three full models account for111 of those (left105, right1, chariot5 across
all LODs). Mixed-bone triangles are reported as unchecked by the local-normal
test. These facts limit material/culling acceptance; matching bounds alone
does not prove a final rendering match.

The 197 local-space TA models have authored model transforms but no validated
track instance placements here. Original `tracks\ta.tob` and `tracks\ta.scb`
exist with `BINARY\0` version1, but their scene-instance semantics remain
unparsed. `geometry\Zone9floor.col` is an original collision source, also not
converted here. A world geometry overview must not be described as a complete
track or used as a collision substitute.

## Validation and next work

`python -B tools/assets/test_scene48c.py` passes **6,130 checks**. Tests compare
all 140 compatible members against frozen 48B output byte-for-byte; independently
reconcile original strip/vertex/material/bone-bound records; prove the 47/166
historical discrepancy totals; check analytical strip winding and matrix order;
reject altered repeated attributes; exhaust every strict prefix of the small
ground fixture; and attack hierarchy/surface/palette counts, nonfinite values,
parent cycles, full-byte consumption, and sampled payload boundaries of the
real pod and long-strip meshes. No native race or benchmark was run for this
packet. `tests.json` records the original candidate member hashes.

Dependency order from here:

1. Inspect neutral world/pod outputs in the native viewer, with reported camera
   placement and no changes to recovered vertices. Verify that broad geometry
   and the authored hierarchy are coherent; retain culling/shading caveats.
2. Port only frozen/proved 48C semantics into the native importer. Keep named
   rejection diagnostics and cache identity/versioning. Support material/LOD
   spans rather than flattening every asset into an undifferentiated mesh.
3. Prove original material/PST interpretation and visibility rules against
   source/bytes or controlled original loader output, then add actual materials.
4. Decode scene instance placement, original collision and race splines. Match
   source transforms and stored invariants before assembling a full track.
5. Apply original pod runtime pose/attachments/visibility in native gameplay.
   Bind-pose rendering alone does not satisfy the playable vehicle gate.
