# Racer source-normal lighting

Status: source integrated into the cloud candidate, uncompiled and untested. This recovery branch is based on guarded original-AI source 93201627. It preserves exactly eight lighting source replacements; no assets, images, binaries or test receipts are included. No visual improvement is claimed before a real render check.

The imported scenes already retain authored local normals, but the prior racer draw path flattened only positions and bones. Its texture/vertex-color shader had no illumination. The track renderer had a separate illumination path.

The source change retains semantic normals without altering the 24-byte position/UV/color vertex layout, mesh bytes, topology, physics or animation. Normals use the same current per-node palette as positions and the existing inverse-transpose helper. Authored signs and hard-edge splits are preserved. Explicit zero normals and undefined singular transforms use geometric fallback.

Player banks and individual opponents receive separate CPU normal storage, with independent renderer normal buffers. Counts and finite unit-or-zero values are admitted before upload. Allocation remains cold; bounded per-frame transforms have unmeasured CPU cost.

Opaque, non-unlit racer materials use the track's host directional and hemisphere/diffuse factors. Transparent/source-alpha effects, additive effects, energy cables, flames, sky and diagnostic defaults retain their existing treatment. This is host lighting and does not establish original shader parity.

The next centrally authorized check must compile/link the integrated source and shaders, then inspect player/opponent normals through animation and turning. Confirm orientation-dependent body shading and intact hard edges; verify cables, flames, transparency, track, sky, menus, missing-normal fallback and ordinary renderer cleanup. No compiler, syntax check, GL probe or runtime was run for this backup.
