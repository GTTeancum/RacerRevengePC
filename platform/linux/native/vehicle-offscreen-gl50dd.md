# Offscreen compatibility-viewer owner 50DD

Status: **source-only, uncompiled, untested and unexecuted**. No compiler, build,
test, device probe, executable, install or validation script was run. Existing
build registries and tests are unchanged. This is source integration, with no
rendered-pixel, hardware-acceleration, presentation or FPS evidence.

## Bounded purpose

`VehicleOffscreenGl50DD` joins the loaded `VehicleDrawAssets50CR` lifetime owner,
`VehicleGlBackend50CV`, `VehicleGlShader50CZ` and `OffscreenGlContext50DC` into a
private Linux desktop-GL diagnostic path. It owns one context/program, six VBOs,
six VAOs, three per-component retained EBOs and two separate color/depth targets.
There is no window, main loop, frame source, texture loading, game-camera policy,
original game shader reconstruction, screenshot file writer or live presentation.

Creation requires the caller to select `viewerNeutralUntextured`. This selects
the existing viewer's explicit constant `(0.68, 0.72, 0.78, 1)` shader branch.
Unspecified and other mode values reject; no missing texture chooses this mode.
Every original material index must still fall inside its actual loaded scene's
material array. The CPU scene, mesh, material names, span order and IDs remain
unchanged. Future textured support needs an explicit texture/material binder;
this component does not pretend it exists.

## API usage

1. Load one persistent, nonmovable `assets::VehicleDrawAssets50CR` using authentic
   inputs. Keep it unchanged and alive until after the renderer is closed.
2. Fill `VehicleOffscreenRequest50DD.context` with the caller's actual platform,
   width, height, minimum desktop GL version (at least 3.2), profile and pbuffer
   channel/depth/stencil/sample requirements. Select
   `VehicleOffscreenMode50DD::viewerNeutralUntextured` explicitly. No resolution,
   display/platform, camera, profile or GL version is synthesized here.
3. Call `renderer.create(request, assets, diagnostic)` on the dedicated rendering
   thread. Missing EGL, unavailable platform/config/version or missing functions
   produce real failure diagnostics; no command-recorder or dummy-success fallback.
4. Build a successful `VehicleDrawBatch50CO` from those exact 50CR prepared meshes,
   preserving the required canonical frame and caller-selected render cut. Pass
   `VehicleOffscreenFrame50DD` with nonnull pointers to 16 finite matrix floats
   and four finite `[0,1]` clear-color floats to
   `renderer.submit(batch, frame, diagnostic)`.
5. When pixels are actually needed, provide `renderer.rgbaBytes()` bytes of caller
   storage to `renderer.readRgba(destination, capacity, diagnostic)`. This reads
   RGBA8, tightly packed, **bottom row first**. It synchronizes GL through ordinary
   client-memory `glReadPixels`; it does not allocate, vertically flip or encode.
   On readback failure, destination bytes may be partially changed, while the
   retained GPU image remains unchanged. Readback rejects before the first commit.
6. Call `renderer.close(diagnostic)` on the creating thread to inspect teardown
   errors. The destructor also owns cleanup. An unresolved EGL cleanup failure
   retains ownership for another close and inherits 50DC's terminate-on-unresolved-
   teardown destructor contract. Do not destroy an unresolved owner casually.

The 50CZ vertex shader uses `vec4(worldPosition, 1) * worldViewProjection`.
50DD uploads with `transpose=false`: provide that row-vector matrix in OpenGL
column-major storage, including the intended GL clip-depth convention. Positions
are already world-space. No additional vehicle/root/model transform is applied,
and the renderer does not derive a camera or matrix from bounds. The viewport is
exactly `(0,0,request.width,request.height)`; this mapping uses the explicit
resolution, with no alternate aspect-ratio or camera defaults.

Targets are private single-sample linear RGBA8 and depth24 renderbuffers,
independent of the pbuffer's requested default-framebuffer format. Diagnostic
state is solid two-sided opaque geometry, writable LESS depth with clear depth 1,
and no blending, scissor, stencil, multisample, dither or framebuffer-sRGB effect.
This follows the simple inspection-viewer use case, not original material parity.

## Transaction and resource provenance

Both VBO/VAO banks use the unchanged 24-byte `Vertex48B` layout: float3 position
at 0, float2 UV at 12 and normalized unsigned-byte RGBA at 20. Indices are copied
once from the prepared component-local uint32 arrays into `GL_STATIC_DRAW`
buffers. They are immutable by private-owner contract, not GL 4.4 buffer-storage
flags; GL 3.2 remains sufficient. Cold creation queries actual sizes/usage,
verifies every index byte with bounded 1 KiB stack scratch, and checks VAO
attribute offsets/types/stride/normalization/associations and target completeness.
The owner exposes neither raw GL objects nor the context or backend callbacks.
Its exclusive-use contract preserves that verified provenance without repeated
GPU index readback. Submitted index/span pointers and counts must match the
retained prepared mesh, and scene material bounds are checked again per span.

Each `begin` clears and draws only the inactive color AND depth target, with the
inactive VBO/VAO bank. All commands stay on the same private GL context. Ordinary
GL ordering and buffer-update dependency handling protect reuse after earlier
draws, including abandoned work; no cross-context or unsynchronized mapping path
exists. `abort` only drops pending bookkeeping. It never changes the previously
published target or publishes a partially drawn target.

`commit` checks GL errors before its CPU-only target-bank index publication.
There is no fallible step after publication. In particular, neither begin nor
commit calls `glFinish`; a true result means command acceptance, **not GPU
completion**. Explicit readback is the synchronization point. Device/context loss
can make any image unreadable and is not reversible by keeping two targets.

No application heap allocation occurs in submit/readback. Cold allocations are
limited to the owning state, shader/adapter wrappers, context and GL resources.
Driver-internal allocation or implicit stalls are not controlled or benchmarked.
No copies of full CPU image buffers are kept by this owner.

## Diagnostics, lifetime and later integration

`VehicleOffscreenDiagnostic50DD` provides a first-failure category/static
operation, component index, GL error, nested shader and 50CS submission detail,
separate primary/cleanup EGL diagnostics and cleanup status. Successful context
destruction frees remaining private GL resources even when explicit deletion
failed. Initialization failure with unresolved EGL cleanup leaves the owner
non-initialized but retained for explicit close. Wrong-thread close rejects
without discarding it. Inputs, diagnostics and caller output must be separate
live storage; all calls are non-reentrant and single-threaded.

50DC requires exclusive ownership of the rendering thread and the EGL display's
entire initialized lifetime. Do not interleave another EGL display owner, GLX
context, raw API switch or external calls through this implicitly current context.
The wrapper does not restore unrelated GL state or support sharing its objects.

A later explicitly authorized native target must include the new 50DD/50CV/
50CZ/50DC `.cpp` files and the existing 50CR/50CO/50CM/50CS dependencies; 50DC
requires the platform dynamic-loader link option (`dl` on typical Linux).
This addition intentionally changes no existing build or launch registry. Real
GL validation still needs a separately authorized compile/run and suitable EGL
implementation; the current source-only work consumes no test slot.

Existing sources/tests/build registries were not modified. Protected 49W, 48U
and 49C lanes remain untouched; `ee.bin` and the user's PC were not accessed.
