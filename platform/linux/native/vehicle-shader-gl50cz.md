# Native-viewer compatibility shader 50CZ

Status: **source-only, uncompiled and untested**. No compiler, build, test,
script, executable validation, GL/context probe or rendering was run. This
addition does not establish shader acceptance, pixels, presentation or FPS.

## Scope and source

`vehicle_shader_gl50cz.{h,cpp}` supplies GLSL 150 sources and a cold program
owner for the Linux `vehicle_draw_gl50cv` caller. The shader behavior comes from
`platform/windows/native/main48b.cpp` at its `shader` string and input layout:

- Position is `vec4(position, 1) * worldViewProjection`, retaining the viewer's
  row-vector multiplication. UV and normalized vertex color pass through.
- With explicitly selected `useTexture = 1`, output RGB is sampled texture RGB
  times vertex RGB. Both texture alpha and vertex alpha are ignored; output
  alpha is always one.
- With explicitly selected `useTexture = 0`, output is the viewer's constant
  `(0.68, 0.72, 0.78, 1)`, independent of the texture and vertex color.

These are diagnostic inspection shaders, not recovered original-game material
programs. No lighting, fog, skinning, original material passes or PS2 viewport
conversion is introduced. A missing texture never selects the neutral branch
automatically. Material resolution must reject missing required data or make an
explicitly documented viewer-neutral choice in the caller.

## Explicit GL interface

Use desktop OpenGL 3.2 / GLSL 150, not OpenGL ES. The supplied function table
uses Linux GL scalar-width signatures and introduces no loader or link
dependency. Its entries correspond directly to the named `gl*` operations.
The context owner must load them and establish a correct live/current context
before any call, including error queries and cleanup.

`initialize` binds these locations before linking:

- Attribute 0, `rrPosition`: three `GL_FLOAT` values at byte 0
- Attribute 1, `rrUv`: two `GL_FLOAT` values at byte 12
- Attribute 2, `rrColor`: four normalized `GL_UNSIGNED_BYTE` values at byte 20
- Fragment output 0, `viewerOutput`: RGBA

All attributes use a 24-byte `Vertex48B` stride, zero vertex base offset and
ordinary non-instanced vertex fetching. Configure the six 50CV VAOs accordingly;
50CZ does not configure or bind VAOs, buffers or render targets.

After successful initialization, `program()` borrows the linked name and
`uniforms()` supplies locations for `worldViewProjection` (`mat4`), `useTexture`
(`int`) and `image` (`sampler2D`). The caller binds the program with `glUseProgram`
and explicitly writes all three uniforms before drawing. Do not rely on GL's
initial zero uniforms: a zero matrix is unusable, and zero `useTexture` must not
become a silent material fallback. `image` receives a texture-unit index, not a
texture object name or `GL_TEXTURE0 + index`.

The matrix has row-vector mathematical semantics. For example, uploading with
`glUniformMatrix4fv(location, 1, GL_FALSE, data)` requires column-major storage
of the caller's row-vector transform. A conventional column-vector transform
must be transposed mathematically for this shader. Do not blindly reuse the
Windows constant-buffer bytes: matrix packing and clip-depth conventions are
separate concerns. 50CV vertices are already world-space, so this matrix must
not apply the vehicle root/model transform again. The caller owns the camera,
depth convention and any D3D/PS2-to-GL conversion; 50CZ performs none.

Texture uploads, UV/image orientation, color space, texture-unit binding,
texture completeness and filtering/wrap/LOD state remain caller-owned. The
Windows viewer uses non-sRGB RGBA8 textures with linear filtering, wrap and
maximum LOD zero. To reproduce that view, explicitly prepare equivalent GL
sampling state; the GLSL deliberately uses ordinary `texture()` rather than
silently overriding caller sampling with `textureLod()`. Framebuffer color
space, blending, depth state, culling, viewport and clear policy are also
external. Forced shader alpha alone does not establish an opaque render state.

## Ownership and failures

Create a persistent, noncopyable `VehicleGlShader50CZ` with the function table,
then call `initialize` once on the cold path. It owns the program and both
attached shader objects until `release`; it intentionally retains these small
shader objects instead of adding a detach lifecycle. No per-frame operation,
heap allocation, shader recompilation or hot uniform lookup is introduced.
Driver compilation/resource allocation occurs only during initialization.

Missing entry points are rejected before GL calls. Existing ownership rejects
reinitialization without disturbing a successful program. Each creation,
source upload, compilation, attachment, binding, link and uniform lookup is
checked. Failed compilation/linking returns a bounded NUL-terminated log; GL
errors are reported separately. All three uniforms must be present.

Any initialization failure after creation attempts cleanup of every owned
object: program first, then both shaders, including partial compile/link
failures. Successful deletion clears that name; a reported deletion error
retains it. The original failure stage/error/log is preserved alongside a
separate cleanup error. `cleanupIncomplete` and `hasResources()` identify
remaining ownership; `program()` is zero after a failed new initialization.
The `alreadyOwned` rejection instead preserves the previous program/interface.

Error checking consumes error state. Enter with no pending error; an entry
error rejects the operation rather than accepting it after clearing. Residual
errors are drained with a fixed bound before cleanup so they are not attributed
to deletion. If the bound is exhausted, cleanup stops, keeps remaining names,
and reports `errorStateUnclear`. This component cannot repair context loss or
prove the validity of a broken GL function table. Keep the owner alive and let
the context owner resolve such failures; do not assume deleting names succeeded
or blindly transfer them to a different share group.

Call `release` on the same live/current share group before destroying the C++
owner or its context. Unbind the program from every context using it first,
otherwise GL can defer actual deletion. `release` is harmless when empty and
invalidates the borrowed program/uniform interface before attempting cleanup.
If it returns false, ownership remains available for explicit context-owner
recovery and later release. No destructor calls GL against an unknown context.
Do not externally delete/relink these objects, and do not retain their borrowed
program/uniform interface after release. All access is synchronous,
single-threaded and non-reentrant.

## Connection boundary

Add the new `.cpp` only to a future context-owning client target. No existing
build registry, source, test or launch path is changed here. In the 50CV hooks,
`ready` verifies the prepared program, all explicit uniforms/camera, resources
and unpublished target; `materialReady` resolves original material indices;
`bindMaterial` selects the program and resolved texture/untextured choice while
preserving the current VAO/EBO/target contract. No hook implementation or
material resolver is supplied by 50CZ.

Context/window creation, resource/texture preparation, camera, sampler/state,
unpublished color/depth targets, synchronization, commit/presentation and
recovery remain the caller's responsibilities. Sealed 49W, held 48U, excluded
49C, unapproved `ee.bin` and the user's PC remain untouched.
