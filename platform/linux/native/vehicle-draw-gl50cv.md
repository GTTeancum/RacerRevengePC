# Linux OpenGL submission adapter 50CV

Status: **source-only, uncompiled and untested**. No build, compiler, executable,
test, install, device probe or validation script was run. These files provide
actual GL upload/draw calls for an eventual context-owning Linux client; they do
not establish rendered pixels, presentation, GPU completion or frame rate.

## Connecting to 50CS

Add `platform/linux/native/vehicle_draw_gl50cv.cpp` to the future client target
alongside `src/game/vehicle_draw_submit50cs.cpp` and its 50CM/50CO dependencies.
No existing file/build target is changed by 50CV. Include the matching header,
construct a persistent `rr::platform::VehicleGlBackend50CV` from the caller's
function table, resource descriptors, hooks and explicit initial published bank
(0 or 1), and retain its `backend()` result. Submit a successful 50CO batch with
the unchanged `rr::game::submitVehicleDrawBatch50CS(batch, backend, diagnostic)`.
The adapter object must outlive the returned callback table and must not move.

The seven function pointers correspond to `glBindBuffer`, `glBufferSubData`,
`glBindVertexArray`, `glDrawElementsBaseVertex`, `glIsBuffer`, `glIsVertexArray`
and `glGetError`. Their Linux signatures use standard C++ scalar widths; there
is no GL header, loader, context/window library or direct GL link dependency.
The caller loads them from its actual initialized context. They use the OpenGL
3.2-style API, not OpenGL ES. The adapter does not discover capabilities or load
entry points itself, and it does not select an alternate drawing path.

## Resources the caller must initialize

- Two banks, each with three already allocated updateable VBOs and three
  already configured VAOs. All six VBO names must be distinct, all six VAO names
  must be distinct, and no VBO may alias any retained index buffer. VAO and VBO
  numeric names may coincide because those are different GL object namespaces.
- Three retained immutable index buffers, initialized once from the exact
  prepared 50CM `uint32_t` arrays, and their matching CPU pointers/counts. They
  are shared across banks and never uploaded, resized or modified here. GPU
  content must match the complete retained CPU array, not merely the same count.
- VAO vertex attributes matching `Vertex48B`: 24-byte stride, float3 position at
  byte 0, float2 UV at byte 12, and the four original color bytes at byte 20 as
  normalized `GL_UNSIGNED_BYTE` RGBA, matching the existing native viewer's
  `DXGI_FORMAT_R8G8B8A8_UNORM`. The caller chooses attribute locations to match
  its shader and configures this layout. Each VAO must reference its own
  bank/component VBO, with vertex byte offset zero and no instancing/divisor or
  other vertex-fetch setting that changes the intended ordinary indexed draw.
- A complete material resolver/binder for each component's original
  `material_index`, plus explicitly chosen shaders, textures, sampler behavior,
  view/projection and render state. All positions are already world-space.
  Do not apply the vehicle root or another model transform a second time.
- An unpublished render target/command strategy and synchronization that make
  both failed submissions and bank reuse safe. Double VBOs alone are not an
  unpublished-frame transaction. Depth/stencil and any other writable targets
  must also be private to pending work or equivalently protected.

`ready` is mandatory and runs before the first GL query. It must verify the
correct live/current context and usable GL error reporting, actual VBO byte
capacities/uploadability, both VAO layouts/associations, retained GPU index
identity/content, the chosen camera/device state, and pending target readiness.
Use authoritative resource-creation/upload records and ownership, or suitable
read-only device queries; do not return true merely because IDs are nonzero.
It may reject resources still in use, or let `begin` synchronize before writes.
`materialReady` must resolve every submitted component/material pair without
changing state. Both checks are read-only, bounded and allocation-free. Their
resource/material decisions must remain valid through the synchronous submit.

Preflight independently rejects missing entry points/hooks, invalid bank IDs,
missing/aliased resources, insufficient declared capacity, changed CPU index
identity/count and unsafe GL integer conversions. It also checks live buffer and
VAO objects with GL queries and requires every `materialReady` result. It does
not read back GPU bytes, introspect a shader or prove the caller's hooks honest.
The required `ready` check is what establishes those additional invariants.
GL error queries consume error status: enter with a clean error state. Pending
errors cause rejection rather than being silently cleared and accepted. A failed
hook/object check can return without draining errors; caller recovery owns that.

## Transaction and draw order

50CS preflights before `begin`. The selected bank is always the opposite of the
last committed bank. `begin` must bind an unpublished target and explicitly
establish the selected camera/state, including waiting for resource reuse when
necessary. It must remain abortable even if it returns false.

The adapter uses `glBufferSubData(GL_ARRAY_BUFFER, 0, count * 24, vertices)` once
per component to update only the inactive bank. All three uploads precede any
draw. For each component it binds that bank's VAO and the retained EBO, then
calls the required material binder for every original span before issuing:

`glDrawElementsBaseVertex(GL_TRIANGLES, span.index_count, GL_UNSIGNED_INT,
span.index_start * sizeof(uint32_t), 0)`

The indices are component-local. `span.vertex_start` is not a base-vertex offset.
The binder must preserve the active VAO/EBO, pending target and world-space
camera contract. No material fallback, visibility policy, sorting, LOD choice,
camera defaults, state defaults or index rebasing is introduced.

Every adapter-issued binding/upload/draw is checked for a GL error; the adapter
also checks before entering the commit hook. `glError()` exposes the first error
observed by those checks in the latest backend preflight/submission. A zero
value does not mean success: missing resources, false hooks or 50CS structural
rejection may still fail. A 50CS rejection before invoking backend preflight
leaves the previous `glError()` value unchanged. Inspect the 50CS diagnostic too.

The caller's `commit` hook is the sole publication point. It must do any final
readiness/error/completion checks before publication. False must publish nothing
and leave pending work abortable. Once it publishes, it must return true without
a later failure. The adapter deliberately performs no GL query after successful
commit, because detecting an error after publication could not roll it back.
A swap/present flow that can publish before reporting failure does not satisfy
this hook contract and must not be represented as an atomic commit.

On any callback failure after invoking begin, 50CS invokes abort exactly once.
The adapter calls the caller's abort hook, preserves its published-bank index,
and discards its pending bookkeeping. `abort` must preserve the previously
committed image/resources, restore state as needed, and handle partial uploads,
partial unpublished draws, failed begin and failed commit. Inactive data may
change; previously published data may not. The caller handles device/context
loss and other unrecoverable failures; this adapter cannot reverse them.

All hooks and GL functions must remain synchronous, nonthrowing, single-threaded
and non-reentrant. Keep the context current through the call, including recovery
callbacks. No callback may retain transient batch/plan/span references or mutate
input storage/the callback table. Hooks must avoid per-submit heap allocation.
This adapter copies only bounded metadata and performs no heap allocation.

## What remains outside this addition

Context/window creation, function loading, GPU resource creation/destruction,
VAO configuration, immutable index upload and verification, shader compilation,
material/texture preparation, camera and device-state choice, unpublished color
and depth targets, synchronization, publication/presentation and recovery all
remain caller-owned. A true 50CS result means the hooks accepted and published
the complete submission and the checked GL commands reported no error; it is
not proof of visible pixels, GPU completion or an on-screen game.

No tests or launch wiring are added. Sealed 49W, held 48U, excluded 49C,
unapproved `ee.bin` and the user's PC remain untouched.
