# Linux EGL offscreen context owner 50DC

Status: **source-only, uncompiled and untested**. No compiler, build, test,
executable, device query, runtime capability probe or install was run for this
addition. Reading headers/directories did not establish EGL library, driver,
context, GPU or framebuffer availability. These files do not prove pixels,
presentation, GPU completion, frame rate or a playable race.

## Files and integration

- `offscreen_gl_context50dc.h`: explicit request, actual context/config info,
  structured failure diagnostics and noncopyable/nonmovable RAII owner.
- `offscreen_gl_context50dc.cpp`: narrow runtime-loaded public Linux EGL ABI,
  desktop OpenGL version/profile verification and pbuffer lifecycle.

Add the source to the future Linux client target with C++17, the platform's
thread support (`Threads::Threads`/`-pthread` as appropriate), and
`${CMAKE_DL_LIBS}` (`-ldl` where required). No existing build file or target was
changed. EGL/GL development headers or link-time EGL libraries are not required.
The installed runtime must provide `libEGL.so.1`, its required public EGL entry
points, desktop OpenGL and a config supporting both OpenGL and pbuffers.
EGL 1.4 additionally needs `EGL_KHR_create_context`; EGL 1.5 has the required
version/profile attributes in core.

The optional core-symbol loader tries `libOpenGL.so.0`, then `libGL.so.1` only
when the first library is unavailable. It never substitutes a lower GL version,
profile, OpenGL ES, a different requested platform or a dummy context. Neither
library is required if `eglGetProcAddress` supplies all requested GL functions.
The helper does not force a hardware or software driver. The installed EGL
implementation may itself use software; success does not prove acceleration.
No environment variables, network/security settings or device permissions are
changed, and no window system is installed or started.

## Explicit creation request

Construct `rr::platform::OffscreenGlContext50DC` on the rendering thread and
fill `OffscreenGlRequest50DC` deliberately:

- `platform`: `MesaSurfaceless` or `DefaultDisplay`; unspecified is rejected.
  The first requires the advertised `EGL_MESA_platform_surfaceless` client
  extension and a usable core or EXT platform-display entry point. It uses a
  real EGL pbuffer on the surfaceless platform, without a native window.
  The second passes `EGL_DEFAULT_DISPLAY` to the runtime; the runtime may need
  an existing display connection. Failure does not fall back between platforms.
- Positive `width`/`height`; the actual pbuffer dimensions must match exactly.
- `major`/`minor`: minimum desktop OpenGL version, at least 3.2. Undefined or
  unsupported later versions are rejected by the runtime, not guessed.
- `profile`: explicitly `Core` or `Compatibility`. The resulting GL version
  and profile are queried and checked after the context is current.
- `redBits`, `greenBits`, `blueBits`, `alphaBits`, `depthBits`, `stencilBits`:
  nonnegative minimum sizes. Zero accepts the runtime's size for that component;
  it does not promise the component is absent. Inspect `info()` for actual sizes.
- `samples`: zero requires no multisample buffers; a positive value requires
  at least that many samples. The first EGL-selected config is checked; the
  helper does not run a config-fallback search or relax requirements.

`create(request, diagnostic)` succeeds only after a real initialized display,
matching config, exact-size pbuffer, desktop OpenGL context and actual
version/profile checks. It leaves that context current. `info()` reports actual
sizes/versions after successful creation; values during failed cleanup are
partial information, not a successful context. Neither pbuffer creation nor
OpenGL default framebuffer initialization establishes any authored render state.
No viewport, camera, shaders, textures, blend/depth policy or rendering loop is
provided here. There is no swap/present API and no context sharing.

## Current context and 50CV/50CZ integration

Load GL pointers only after successful creation or binding, with
`procAddress("glBindBuffer")` and the other actual GL names. It returns the
EGL-compatible `void (*)()` function-pointer type. Cast to the exact Linux
function signature used by `VehicleGlFunctions50CV`/the shader component and
reject every null result. Pointer availability alone is not a capability proof.
Keep the owner alive as long as any of these function pointers or GL objects are
used. Function lookup does not open libraries or allocate application storage.

`current()` checks the owner thread, selected EGL client API, context identity,
display identity and both read/draw pbuffer bindings. This supplies the context
identity portion of 50CV's required readiness hook, not its remaining buffer,
index provenance, shader/material, camera, target or synchronization checks.

`release(diagnostic)` unbinds this context from its thread without destroying it.
`bind(diagnostic)` makes its pbuffer and context current again. Both reject a
foreign current EGL context or an externally changed EGL client API. Releasing
is not permission to migrate the owner or its GL resources to another thread.
The class performs one bounded application allocation at creation, retains its
state, and allocates no application storage in bind/release/current/proc lookup.
Driver-internal allocation behavior is outside this guarantee.

## Ownership, partial failure and shutdown

The caller must dedicate this rendering thread and the **entire initialized
EGL display lifetime** to this owner. Do not overlap another initialized EGL
owner/client using that display, including on another thread. `eglInitialize`
and `eglTerminate` do not provide private/refcounted display ownership; terminating
a shared display can invalidate other clients. This exclusivity is a caller
precondition, not a property the helper can infer from a handle. Do not use GLX
or another rendering API context on this thread during ownership. Do not switch
EGL APIs or raw context bindings behind the owner's back.

Destroy buffers, VAOs, programs, textures and FBOs while this context is current,
then call `close(diagnostic)` on the construction thread. Teardown unbinds,
destroys the context, destroys the pbuffer, terminates the exclusively owned
display, releases the dedicated thread's EGL state with `eglReleaseThread`, then
unloads its library references. Releasing EGL thread state resets the selected
client API to the runtime's initial default. A preexisting EGL context rejected
before ownership is acquired is left bound; only the API selection is restored.
There is no flush/finish or frame publication promise in destruction.

Every failed creation follows the same partial teardown path. An uninitialized
display is not terminated; missing symbols cannot be called in cleanup. If a
teardown step fails, already-released handles are cleared and the remaining
handles/library references stay owned for a subsequent `close` attempt.
`diagnostic` preserves the original creation failure and adds a separate
`cleanupError`, `cleanupOperation`, `cleanupEglError`. `ownsResources()` tells
whether state is still retained on the owner thread. A failed create or close
never presents an initialized, usable context. Do not call create again until
close succeeds; do not assume a failed creation owns nothing.

`close` reports the exact failed operation and raw EGL error when applicable;
a non-EGL precondition/loader failure may have no EGL error. An unavailable
extension can yield EGL_SUCCESS in that field, while the structured error still
reports the missing requirement. Destroying on the wrong thread or allowing the
RAII destructor to reach an unresolved cleanup failure calls `std::terminate`.
This explicit fail-fast contract avoids silently leaking a bound context or
unloading live driver code. Applications needing a teardown diagnostic must
call `close` before destruction and resolve any failure on the owning thread.

## Public API references and limits

The narrow declarations/constants follow the public Khronos APIs:

- [EGL API registry](https://registry.khronos.org/EGL/)
- [EGL_KHR_create_context](https://registry.khronos.org/EGL/extensions/KHR/EGL_KHR_create_context.txt)
- [EGL_MESA_platform_surfaceless](https://registry.khronos.org/EGL/extensions/MESA/EGL_MESA_platform_surfaceless.txt)

Only new 50DC Linux source/header/note files were added. No existing files,
registries, tests, gameplay source or PC configuration were changed. Sealed 49W,
held 48U, excluded 49C and unapproved `ee.bin` remain untouched. Runtime support,
compilation, error-path behavior and resource lifetime behavior all remain
unverified until the parent's separately authorized verification slot.
