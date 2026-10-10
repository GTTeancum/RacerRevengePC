# Vehicle draw submission 50CS

Status: new source only, **uncompiled and untested**. No compiler, build, test,
executable, probe or validation script was run for this addition. This is a
submission bridge and a headless command recorder, not an on-screen renderer or
a performance result.

## Integration contract

Add `src/game/vehicle_draw_submit50cs.cpp` to the chosen native target alongside
50CM/50CO. No existing source, build registry or test is changed here. Consume
only a successful `buildVehicleDrawBatch50CO` result. Preserve its immutable
prepared indices/spans, keep all input arrays alive through the call, and keep
input, diagnostic, recorder and backend working storage disjoint.

`submitVehicleDrawBatch50CS(batch, backend, diagnostic)` performs:

1. Required-callback and structural/capacity checks, including all three
   component descriptors, ordered complete span coverage, triangle counts and
   agreement with batch totals. `preflightVehicleDrawBatch50CS` also exposes this
   check without issuing callbacks. It reports exact upload/index byte lengths
   and a bounded command count: six fixed commands plus the preserved spans,
   at most 198. It trusts the previously checked 50CM vertex/index content rather
   than rewalking every index on each hot submission.
2. The backend's read-only preflight. The caller must check actual resource
   readiness, capacity, exact retained index-buffer identity/content, material
   resolution for every component/span and explicitly selected camera/render
   state here. Backend capacities are counts of vertices, indices and commands;
   zero capacity does not mean unlimited.
3. Begin an unpublished transaction and upload chariot, left engine and right
   engine world-space `Vertex48B` arrays. All uploads precede any bind/draw.
4. For each component in that order, bind its geometry, then issue its original
   material spans in order. The callback receives the component, span ordinal,
   entire unchanged `SceneSpan48D`, and explicit base vertex zero.
5. Commit once. Any false callback result after begin is invoked, including a
   false begin or commit, calls abort exactly once. Preflight failures never
   call begin or abort. A true submit result means backend acceptance only.

Invoke callbacks only through `submitVehicleDrawBatch50CS`. Callbacks are
synchronous, noexcept and single-threaded; they may not retain
transient batch/plan/span references or reenter submission. They must not mutate
the inputs or callback table. After begin, all writes/commands must remain
unpublished until commit. Abort must discard pending work and preserve the last
committed frame, even after a failed begin/commit. Device removal or similar
unrecoverable backend failures remain the caller's responsibility; the adapter
cannot roll back arbitrary external device effects.

## Exact viewer mapping

The source contract comes from `platform/windows/native/main48b.cpp`:

- `Vertex48B`: 24-byte stride, POSITION at byte 0, TEXCOORD at byte 12 and COLOR
  at byte 20. Bind vertex/index buffers at byte offset zero.
- Component-local `uint32_t` indices map to R32_UINT, triangle-list topology.
- Bind the material selected by this component's original `material_index`;
  call `DrawIndexed(span.index_count, span.index_start, 0)`.
- Positions are already world-space. The caller supplies view/projection, with
  no model-root multiplication or second placement transform.

The current viewer creates immutable vertex buffers. A live backend needs
updatable buffers created once, separate immutable index resources, and an
inactive buffer/command-recording or render-target transaction strategy. Do
not wire these callbacks directly to visible immediate draws and claim abort
can undo them. Resource/device creation, shaders, textures, materials, camera,
visibility, LOD selection, presentation and GPU-completion synchronization are
explicit caller responsibilities. No first-pass texture, fallback material,
camera placement or visibility rule is invented by 50CS.

## Bounded headless recording

`VehicleDrawRecorder50CS(firstBank, secondBank)` is an optional concrete backend.
Each `VehicleDrawRecordingStorage50CS` supplies three caller-owned vertex arrays
and a command array. Reserve each model's required vertex count in both banks,
and six plus the total span count command entries in both banks. Construct the
recorder once, retain `recorder.backend()`, then use the ordinary submit call.
The recorder is noncopyable/nonmovable because the callback context borrows it.

`recording()` is null until first commit. A successful record contains copied
world-space vertices, borrowed immutable indices, copied span metadata, original
material IDs, zero base vertices, original order and batch bounds/totals. Upload
and bind commands identify the component whose recorded geometry they use.
No pointer to the 50CO batch descriptor or source span array is retained.

Each begin selects the inactive bank; commit publishes that complete bank.
Rejection leaves the last committed record and its bytes intact. Its view is
valid until the next successful commit. Old copied views can be overwritten by
a later submission that reuses their bank; consume them before then. Inputs,
both banks and all their arrays must be mutually disjoint. Do not feed a record's
payload back as input to the same recorder. No heap allocation occurs on submit;
the recorder makes one bounded memcpy per component and appends commands.

This headless backend deliberately has no device, camera or resolved materials:
it preserves command data rather than pretending to execute those choices.
An eventual device backend must perform its own resource/material/camera
preflight. Recorded operations alone establish neither pixels nor frame rate.
Sealed 49W, held 48U, excluded 49C and unapproved `ee.bin` are unchanged.
