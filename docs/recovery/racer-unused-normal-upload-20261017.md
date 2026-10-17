# Normal uploads for materials that consume normals

Status: source-reviewed optimization; UNTESTED. No build, syntax check, test,
gameplay run or benchmark accompanies this change. No measured performance
improvement is claimed.

## Scope

Only the private racer normal-buffer update in
`platform/linux/native/vehicle_window_gl50ds.cpp` changes. A cold per-component
summary records whether any retained material can enable fragment lighting:
`!unlit && !sourcePrelit && !sourceDiffuseCompressed50LA && blend == opaque`.
The helper used to build that summary also supplies the existing
`applyPodMaterial` lighting selector, preserving its exact Boolean expression.

Both the player and archetype material-copy loops visit every admitted material,
including unused entries. A single normal-dependent entry preserves the upload
for the entire component. A missing material table also preserves the upload.
Only a present complete table with no such entry skips `bufferSubData`.
The existing per-component buffer bind and error check remain unconditional.

## Shader dependency

The linked racer vertex shader assigns `rrWorldNormal` only to
`viewerWorldNormal`. It does not use normals for position, UVs, source colors,
clipping or depth. Its ordinary and coverage-payload variants share that source.

In the fragment shader, every source-diffuse route returns before the normal
calculation: unpaired source color, base map, map 2 and coverage payload. Other
materials can still execute the normal calculation, including normalization of
the varying or geometric fallback, but its result affects output only inside
`useLighting != 0`. It cannot affect discard, alpha, depth, texture coordinates
or coverage when that selector is zero. Native selected-light coefficients are
also consumed only inside that same lighting branch. The separate coverage
composition program reads payload/depth/destination images, not racer normals.

This leaves all CPU normal validation, source-color generation, specular-UV
generation and other CPU normal consumers intact. Shader source, attribute
formats, topology, source passes and draw order are unchanged.

## Ownership and failure boundaries

Each private component in both GPU banks retains its existing normal VBO.
Cold creation initializes its full vertex capacity from the zero normal array;
`createWorldNormals` verifies the buffer extent, every uploaded byte and
attribute 3's source/format/stride/offset. The VAO continues to reference that
defined storage. No normal VBO allocation or deletion is omitted.

Policies and summaries are copied into the same fresh renderer Storage and do
not change during submissions. Frame UV updates and UI bank replacement do not
modify them. Repeated racers using one archetype therefore share the same
normal-consumption policy. A failed creation never admits a submit; replacement
creation allocates fresh Storage. No cross-frame normal-currentness cache or
external pointer lifetime is introduced.

Existing live component/provenance/span bounds and normal validation still run.
The renderer has no per-frame normal-VBO readback or public racer-normal upload
counter to misreport: its normal byte and layout attestations occur at cold
creation, which remains unchanged. Other dynamic-world normal uploads and their
diagnostics are unaffected. All existing upload error gates and frame
publication/abort cleanup remain in place.

## Incidence and cost limits

The canonical engine-material admission visits the complete material table.
With original body colors enabled, admitted body materials select source-diffuse
color and admitted flame materials are additive; neither enables host lighting.
These engine components qualify without treating ordinary opaque chariot
materials or neutral material tables as equivalent.

Each qualifying component update avoids one normal-buffer upload and
`vertexCount * sizeof(WorldNormal50HK)` bytes (12 bytes per vertex). Buffer
allocation, CPU validation, vertex processing and the separate position/color/UV
and specular-UV uploads remain. This does not remove any coverage bridge, and its
effect on driver time or frame rate is unknown.

Source inspection used vehicle renderer blob
`2e3676917e7446e3dab11057cb181019b4b6b1e2` and racer shader blob
`dd273724ccbf9e3c0c7c1ed2514f650d74f5ca3b`.
