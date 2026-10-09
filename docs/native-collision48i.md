# Native collision data and original placement, 48I

The native application now loads the original static collision world into compact owned records. `--world-collision` uses `RuntimeWorld48I`, `loadCachedCollisionWorld48I`, and `loadCollisionMesh48I`; it no longer keeps a complete source COL copy inside every live mesh. The native cache remains unchanged and retains reversible source data and its hashes. This is preparation for the connected native simulation, not a playable-game claim.

## Records and ownership

Runtime vertices and optional vertex normals are float triples. Nodes retain the original volume, center, half-extents and both ordered children; native high-bit tags identify face indices. Nodes occupy 36 bytes. Faces occupy 16 bytes: original float D, three original-loader signed-short normal components and three unsigned-short vertex indices. Validated source vertex limits make the narrowed indices lossless. No triangles are sorted and no replacement tree is built.

The loader first validates RRCOL001 with the existing canonical parser, including complete source provenance, graph topology and all indices. It transfers vertex ownership and builds runtime nodes/faces. Normal packing uses the verified 48H original-loader calculation. A nonfinite packing intermediate is an explicit transactional failure. The output is unchanged on invalid cache input or allocation failure. The runtime object owns all of its data after the input buffer is destroyed.

This deliberately separates cache/recovery representation from query representation. Load-time staging still includes canonical source bytes and temporary parsing allocations. The reported payload is not peak process memory or Xbox residency. Meshes are shared by native content key across placed instances. Every alias must match the verified cache metadata and original source hash.

## Original placement arithmetic

`placementCollision48I` follows `27B1FC..27B26B`: separately normalize authored Up and Look with the original individually rounded operations; compute Right as Up cross Look; retain authored position. It does not orthogonalize Look. `composeCollisionWorld48I` follows `11F730..11F79F`: original COL-local times instance placement, with each multiplication and addition rounded in original order. The full final addition is retained, including homogeneous coordinates.

The helpers require finite usable inputs, reject zero or parallel placement axes transactionally, and allow composition output to alias either input. This validation boundary is native policy, not a claim about how the original game handles malformed transforms. Their evidence covers ordinary binary32 round-to-nearest, not every physical EE/VU exceptional mode or all host rounding modes.

`tools/assets/collision_placement48i.py` verifies all 56 original instruction words and generated annotations against immutable ELF SHA-256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. Its offline original-word oracle generates 558 comparisons: all 318 authored TA instance placements, plus 240 deterministic nonidentity/nonorthogonal examples. All 16 placement and 16 composed matrix words are compared exactly. Source hashes, original words and fixture hashes are in `evidence/collisionMesh48I/placement-recovery.json`.

## Scope and validation

The loader checks all 38 TA COL fixtures, 14,586 faces and 14,548 nodes. These include both source float normal streams and packed face normals. Tests cover input lifetime, invalid extents/provenance, packing overflow and transactional output. The record payload for these 38 fixture assets is 933,532 bytes, versus 2,191,148 encoded cache bytes; these are different representation sizes, not measured live memory savings.

The application still follows only the evidenced default static member selection: absent `NumberOfCollisionObjects`, `StaticType`, and `COL` overrides. Missing original members are optional as in the recovered loader; present damaged/unconverted members fail. TA loads 37 instances, 36 unique assets and 281 absent original COL members. The query's original category filters, spatial candidate discovery, terrain callback state and moving objects remain separate work. Native query recovery is documented in `terrain-query48i.md`.

Final build and application validation locations are recorded in the adjacent 48I checkpoint and audit manifest. The world viewer submits bounded offscreen frames; it does not measure presented gameplay FPS.
