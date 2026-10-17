RA single containing-interval upload: source-only rationale

Status: untested. No build, compiler, syntax check, linter, gameplay run, benchmark
or new instrumentation execution was performed. No frame-time gain is claimed.

Scope and cost
- Retain the split immutable/current float streams and all source preparation.
- Replace the full current-stream upload with one contiguous interval containing
  every selected surface's vertex range. Do not split it into per-surface calls.
- For S selected surfaces and N retained vertices, the new local prepass adds
  O(S) comparisons without allocation. Transfer is 20*(maxEnd-minStart) bytes,
  bounded by the previous 20N. A broad selection can save no bytes.
- An empty selection skips only bufferSubData. Existing GL setup, error polling,
  empty draw iteration and final restoration still execute.
- The complete current VBO is initialized from the value-initialized CPU vector
  during cold creation, adding one cold 20N-byte initialization. Its size, usage,
  ownership, attributes and deletion remain unchanged.

Selected-data proof
- createRaWorld already validates every original EBO index against its surface's
  nonempty retained vertex range before GL allocation.
- drawRaWorld retains its source-currentness and target gate before the prepass.
  The prepass checks every selected surface index before plan access, then checks
  nonzero vertex count, first <= frame size and count <= frame size - first.
- Hence first+count cannot overflow and lies within the existing private vector.
  The minimum start and maximum end contain every selected indexed vertex.
- Both byte offset and length fit the existing whole-buffer extent, already
  checked against GL's signed pointer-size limit during cold creation.
- The interval uploads from data()+minStart before any selected draw, using the
  same float bytes produced by unchanged complete selected-surface preparation.
- Existing upload-error checking still gates all primitive submission; a failed
  transfer cannot admit a selected primitive. Existing final restoration remains.
- Endpoints are local and recomputed on every call. No prior selection, LOD,
  successful upload, or failed upload is used as a cache key or freshness proof.
- No external callback intervenes between the prepass and original draw loop.
  The retained source lease prevents replacing the indexed geometry/draw plan.

Failure and diagnostic limits
- New range rejection occurs before drawRaWorld changes GL state. The existing
  publication, preparation, material, primitive-error and cleanup gates remain.
- Fresh Storage owns an empty current vector; cold resize value-initializes its
  float records. Complete initial upload defines even never-selected GPU values.
- An unselected CPU record can change during preparation that is later abandoned.
  If a later successful selection excludes that record from its upload interval,
  the GPU retains its previous defined value. This cannot affect selected draws.
- Actual fog attestation continues to read and compare the real GPU bytes. It
  already labels whether its surface is selected; no mismatch is suppressed or
  replaced with an expected value. A source comment explains the staging case.
- Source inspection does not establish compilation, driver behavior, rendered
  equivalence, or performance. Those remain unverified.
- Independent and integrating source reviews found no identified blocker in
  checked interval arithmetic, selected-data coverage, cold initialization,
  actual diagnostic reporting, or retained upload/restoration gates.
