RA immutable/current vertex streams: source-only rationale

Status: untested. No build, compiler, syntax check, linter, gameplay run, benchmark,
or new instrumentation execution was performed. A byte-count reduction does not
establish an FPS improvement.

Scope
- Split the renderer-private RA vertex stream into immutable position/UV and
  current RGBA/fog streams. Both retain their original float values and precision.
- Keep the shader source, attribute locations and component counts, original
  index buffer, source draw order, clipping math and material math unchanged.
- No original geometry, asset, AI, physics, handling or HUD data is removed.

Layout and cost
- Previously every retained vertex uploaded ten floats, 40 bytes, each frame.
- Immutable stream: float position[3], uv[2]; 20-byte stride, offsets 0 and 12.
- Current stream: float color[4], fog; 20-byte stride, offsets 0 and 16.
- sizeof/offsetof drive allocations, attribute bindings, uploads and readback.
  Static assertions require both exact compact layouts.
- The immutable stream uploads once per renderer creation with STATIC_DRAW.
  The current stream retains one full-range bufferSubData per drawRaWorld call.
- For N retained vertices, per-frame transfer becomes 20N instead of 40N bytes.
  CPU vertex payload and GPU vertex capacity each remain 40N bytes, split between
  two allocations. One additional CPU vector header and GPU buffer name are used.
- No per-vertex GL writes, per-frame allocation, repacking pass, dirty-range
  bookkeeping, persistent mapping or cross-frame validity cache is introduced.
- Existing receipts do not isolate buffer-upload time. Changes in driver/fetch
  behavior and actual frame-time impact remain unmeasured.

Value and lifetime proof
- createRaWorld sizes both private CPU vectors from the same retained cold-world
  vertex count before GL allocation. Its existing caught allocation block rejects
  either allocation failure before the renderer can become initialized.
- The same position/UV float copies populate the immutable stream at creation.
  The retained renderer publication prevents replacing/closing the cold owner.
  Crowd animation selects images; it does not mutate these geometry lanes.
- The existing current color/fog assignments are unchanged. Their destination
  now contains only those five floats. Preparation still sets framePrepared false
  first and true only after every original source-publication gate succeeds.
- CPU clipping and raster bounds read the immutable copies. Pixel provenance
  combines the same-index immutable and current records without changing math.
- Both arrays remain private, fixed-size for renderer lifetime, and are rebuilt
  for a fresh renderer. All retained vertices and original indices remain present.

GL ownership and failures
- Both VBO names initialize to zero, are recorded immediately on generation, and
  participate in hasRaWorldResources and deleteRaWorldResources.
- Attribute 0/1 associations capture the immutable VBO; attribute 2/3 capture the
  current VBO. All use GL_FLOAT, normalization disabled and the original sizes.
- Both VBO extents and the original EBO extent are checked before shader creation
  and successful initialization. The existing allocation/error gate is retained;
  an additional cold error check covers the added extent query/buffer rebinding.
- Partial creation leaves every generated name discoverable by ordinary cleanup.
  Existing deleteName clears a name only after checked successful deletion, so
  deletion failures remain retryable and source-lease retirement stays gated on
  complete GL-resource release.
- drawRaWorld retains its source-currentness/target gate and uploads only after
  that gate. Its existing pre-draw error check, primitive checks and final state
  restoration are unchanged. An upload failure still admits no RA primitives.

Actual GPU diagnostic
- Fog attestation still queries actual VAO attribute state and actual VBO bytes.
  It now expects the current VBO, stride sizeof(RaWorldFrameVertex), fog offset
  offsetof(RaWorldFrameVertex,fog), and the corresponding indexed byte address.
- CPU/GPU float-bit comparison and real prior VAO/ARRAY_BUFFER snapshot and
  restoration are retained. No expected value is substituted for a GL result.

Review limits
- Independent and integrating source reviews found no identified blocker in
  layout, readers, ownership, cleanup, publication gates or GPU attestation.
- A repository-wide source search found no references to the private vertex
  record or current frame array outside the changed renderer include.
- Source inspection can establish the intended ownership and dataflow, but it
  cannot establish compilation, driver behavior, pixel equivalence or speedup.
