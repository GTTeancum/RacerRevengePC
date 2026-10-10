# Native scene converter 48D

This stage ports the bounded scene recovery in `tools/assets/decode_scene48c.py` into ordinary portable C++ data. The earlier native decoder 48C remains unchanged. The production scene decoder uses neither Python nor guest execution, and does not depend on a graphics API, Windows, or JSON. Parent-owned application integration serializes the semantic result and renders diagnostic previews.

## Public interface

`src/assets/import_scene48d.h` exposes `decodeScene48D(input, output, error, hooks, limits)`. Input is one original PSG3 member, already extracted from its resource container. The existing 48C resource decoder remains the container interface. Errors reuse `DecodeError48C`: named `unsupported`, `malformed`, `limit`, and `cancelled` outcomes. Output is transactional: a failure leaves the caller's prior `Scene48D` untouched.

`Scene48D` retains these values directly:

- The material name table and original world-space flag.
- Every observed LOD and its original positive cutoff.
- Every surface's material index, local vertices, local normals, UVs, source color presence, rigid bone indices, and triangle indices.
- The hierarchy's names, parents, local matrices, composed bind matrices, original local bounds, and independent decoded bounds checks.
- Original strip spans, validated split continuations, and normal-alignment diagnostics. These are evidence fields, not runtime packet execution.
- A neutral LOD0 `RRMESH01` export and one material span per LOD0 surface.

All semantic coordinates, transforms, normals, and UVs use binary64 values to retain the recovered conversion's arithmetic. No serialized address is a host pointer. The application controls storage; this decoder only returns C++ objects and byte vectors.

`SceneSpan48D.vertex_start` and `index_start` refer to offsets in the concatenated LOD0 vertex and index arrays. Indices in `neutral_lod0` already include each surface's vertex base. Surface-local semantic indices remain local. A span retains its material index and whether colors existed on disk.

## Supported original subset

The parser accepts version 3, 1–256 hierarchy nodes, 1–4096 materials, and the observed one- or four-LOD layouts. Each LOD has 1–4096 surfaces. It supports the two proven attribute variants, with and without original vertex colors. Positions are signed 16-bit packed values, normals signed 8-bit, UVs signed 16-bit, and colors original four-byte values when present. The original surface UV-size field remains explicitly uninterpreted.

Every packed group must match the recovered attribute sequence and address layout, contain 3–16 vertices, and end correctly. Each strip begins with exactly two suppressed vertices. A long-strip continuation is accepted only when a full 16-vertex strip is followed by the next group's strip and the two duplicated boundary vertices match in every attribute, including bone, normal, UV, and color. Original source-strip and emitted-triangle counts must reconcile. The two currently rejected TA members retain the named unsupported attribute-command outcome; the decoder does not skip arbitrary bytes to accept them.

The absolute position-W value selects one rigid hierarchy node after division by 16. Its sign controls strip emission. Parent nodes must precede children. Local and composed matrices must be finite and affine. Multiple nodes require the observed exact identity palette, its padded extent, zero padding, and final node count. Unproven palettes are unsupported.

The parser accumulates each node's decoded local bounds across all LODs and compares them with the stored center/half extents, using the preserved threshold `position_scale * 1.01 + 0.002`. Nodes without points use zero bounds, matching the frozen reference. No runtime placement, vehicle animation, damage visibility, culling, or LOD-selection policy is inferred.

## Transform and color policy

Matrices follow the recovered row-vector convention. Each bind matrix is `local * parent.bind`. Each matrix element uses the ordered binary64 sum over four products. The flattened export uses three ordered position products, then adds the translation, and finally stores binary32. The compilation unit must preserve that operation order (`/fp:strict` in the current MSVC build); parity is verified against frozen reference bytes on both x64 and Win32.

Semantic positions and normals remain local. The neutral mesh applies each vertex's original rigid bind matrix to its position. It preserves original UV and color bytes. Missing colors remain absent semantically and become explicit white only in `RRMESH01`, whose vertex layout requires RGBA. This is a diagnostic color, not a recovered material or lighting result.

Triangles with opposing normals are counted and retained without flipping. Triangles spanning more than one bone are counted as unchecked by the local normal-alignment diagnostic. Runtime pose, normal transformation under that pose, original shading, texture sampling, and scene visibility are separate application work. Decoder coverage and fast preview rendering do not establish gameplay correctness or a gameplay frame rate.

## Bounds and cancellation

The hard input limit is 8 MiB per PSG and the hard total is 1,000,000 vertices across all LODs. Caller limits can only reduce those caps. All reads are checked against file and surface extents; declared counts, alignment, names, finite values, indices, hierarchy relations, palette bytes, padding, and exact file consumption are validated. Arithmetic used for file extents is bounded before addition. Nonfinite composed transforms, normal diagnostics, and nonfinite binary32 exports are rejected. Allocation failure is returned as a named limit error.

Cancellation is checked before parsing, between hierarchy nodes, before surfaces and packed groups, during bounds/export work, and before the final output commit. Progress stages are `scene_nodes`, `scene_surface_vertices`, and `scene_lods`. Hooks must follow the existing 48C callback contract; the caller owns its context and any exception policy. No output file or persistent cache is written by this layer.

## Validation and evidence

`tests/native/import_scene48d.cpp` reads original `TRACKS/TA.RES` and `PODS/POD01/POD01.RES` through the native 48C resource reader. It compares scene data against the frozen JSON and native meshes in `evidence/assets48C/TA` and `evidence/assets48C/POD01`. JSON is a test-only dependency. The native scene library itself requires only the standard library and the 48C error/hook declarations.

The expected original coverage is TA 228 accepted out of 230 and POD01 13 accepted out of 13. For every accepted member the test requires byte-identical neutral LOD0 output and equal materials, hierarchy, matrices, local positions/normals/UVs/colors/bones, all LODs, topology, bounds diagnostics, and material spans. Both TA exclusions must remain named unsupported command variants. Additional mutations cover truncated structure, caps, nonfinite/invalid matrices, cyclic or forward hierarchy, invalid bone indices, altered palette/counts, material bounds, changed continuation attributes, independent bounds disagreement, and transactional cancellation/limit failure.

Accepted integrated build `evidence/nativePc48D-build-20261003-134330-366` passed all seven CTests on x64 and Win32. The native scene decoder passed **7,384 checks per architecture**: TA 228 accepted / 2 unsupported and POD01 13 accepted / 0 unsupported, with all 241 neutral exports byte-identical to Python48C and all tested semantic values equal. The application-owned scene cache additionally passed **121,260 checks per architecture** over those 241 originals, including hierarchy consistency and compatibility-mesh validation.

The decoder's source/reference hashes, accepted test-log hashes, executable hashes, and build-provenance links are saved in `evidence/importScene48D`. These results concern conversion and native-data preservation. The tests do not establish material fidelity, runtime animation, race behavior, or a gameplay frame rate.

## Remaining limits

This is a native asset conversion foundation, not a recovered game loop. The format subset is intentionally limited to proven originals. It does not add material semantics, textures, skeleton animation, physical simulation, track-instance placement, race state, or portable platform services. It does not forecast original Xbox performance. Binary64 import arithmetic and diagnostic data are conversion-time choices; a later runtime layout can compact them after a separately verified conversion.
