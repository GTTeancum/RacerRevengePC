# Linux interactive window host 50DQ

Status: **source-only, uncompiled and untested**. This addition ran no compiler,
build, test, executable, runtime capability probe, device query or install. It
does not establish that SDL2, a display server, a GL driver or a visible working
window exists in the current environment. It adds a host boundary, not a playable
race, original input mapping or a rendering implementation.

## Files and dependencies

- `window_host50dq.h`: request, actual window/context information, raw keyboard
  snapshot, fixed event budget, diagnostics and noncopyable/nonmovable owner.
- `window_host50dq.cpp`: runtime-loaded Linux SDL2 window, desktop OpenGL context,
  event processing, swap submission and cleanup.

Add the source to the future Linux interactive target and link the platform
dynamic-loader library (`${CMAKE_DL_LIBS}` in CMake, normally `-ldl`). Use C++17 or
later. No SDL development headers or link-time SDL dependency are needed. The
runtime loader tries `libSDL2-2.0.so.0`, then `libSDL2.so`; SDL3 is not substituted.
The public API used includes `SDL_GL_ResetAttributes` (SDL 2.0.2 or later).
Missing libraries, functions, video backends and requested GL capabilities fail
explicitly, with copied diagnostics. There is no install, driver/display setup,
headless fallback, alternate GL profile, or implicit vsync downgrade.

SDL's existing environment/backend selection is respected. The implementation
does not set `DISPLAY`, `WAYLAND_DISPLAY`, `SDL_VIDEODRIVER`, EGL or driver
environment variables. Successful creation does not prove hardware acceleration.

## Ownership

Construct, create, poll, render, swap, close and destroy on the process's main
thread. The owner records its construction thread and rejects calls from other
threads; choosing the main thread is the caller's obligation. Reserve the SDL
subsystem lifetime and event queue exclusively for this host. Creation rejects
already-initialized SDL subsystems. Do not create another SDL owner, push custom
events, change event filters, or alter this host's context/window behind it.

The rendering thread must not overlap any foreign SDL, EGL, GLX or other current
context. SDL can check its own current context; this does not discover all raw
EGL/GLX contexts. `bind` and `release` support temporarily unbinding this context,
not sharing the thread with the existing 50DC/50DD offscreen owners. 50DD has its
own private context and cannot simply render into this window. A renderer for
50DQ must accept this host's GL entry-point loader and context ownership.

Destroy the renderer's GL objects while the context is current, then call
`close(diagnostic)` explicitly. Cleanup unbinds, deletes the context, destroys the
window, shuts down SDL, and closes the dynamic library in that order. SDL context
and window deletion APIs are void, so they cannot report driver teardown errors.
If unbinding or library close fails, ownership is retained for an explicit close
retry. A failed create can likewise retain ownership if its cleanup fails:
inspect `cleanupError`, `cleanupOperation` and `cleanupDetail`. The destructor
terminates on wrong-thread destruction or unresolved teardown, matching 50DC's
fail-loud lifetime convention.

## Integration sequence

1. Construct `rr::platform::WindowHost50DQ` and `WindowHostDiagnostic50DQ`.
2. Fill `WindowHostRequest50DQ`: nonempty title, logical width/height, minimum GL
   major/minor, and `WindowGlProfile50DQ::Core` or `Compatibility`. Minimum 3.2 is
   accepted by the host; select at least 3.3 for existing GLSL 330 renderers.
3. Pick `swapInterval` explicitly: `0` immediate, `1` synchronized, or `-1`
   adaptive. The default is `1`; unsupported values/capabilities fail instead of
   changing the pacing choice. Requested color/depth/stencil sizes are minima;
   zero samples forbids multisampling. All framebuffer sizes and actual GL
   version/profile are checked. Logical window size can be constrained by the
   window manager; read actual dimensions from `info()`.
4. Call `create`. It leaves the GL context current. Obtain each GL function with
   `procAddress`, cast it to its actual Linux GL signature, and reject nulls.
   The returned pointers are valid only while the library/context remain owned.
5. Once per outer loop, call `poll(input, diagnostic)`. Stop on failure or latched
   `input.quitRequested`. Escape has no special meaning inside 50DQ; a runner may
   deliberately map a raw key to its own quit behavior.
6. Pause gameplay when unfocused/minimized. When `eventBudgetExhausted` is true,
   defer simulation until a later outer-loop poll catches up; avoid an unbounded
   drain loop. This prevents stale queued key state from advancing gameplay.
7. Read `info().drawableWidth`/`drawableHeight` for GL viewport and presentation
   targets. Logical `width`/`height` need not match physical pixels. Recreate any
   size-dependent renderer targets when dimensions change. Skip rendering when
   minimized or either drawable dimension is nonpositive. 50DQ does not alter GL
   viewport, projection, framebuffer binding or render state.
8. Translate raw input through the runner's separately approved binding layer,
   advance its bounded simulation, draw/blit into this window's default GL
   framebuffer, then call `swap`. The owner supplies no clock or frame limiter;
   a runner may use `std::chrono::steady_clock` and its own bounded frame pacing.
9. Tear down renderer resources, then explicitly close the host.

`swap` checks the context and submits `SDL_GL_SwapWindow`. SDL has no return value
for this operation. Success does not prove a displayed frame, GPU completion,
vertical-sync timing or an FPS target. Do not interpret this host alone as
evidence of a playable or correctly presented native runner.

## Raw input and bounds

`WindowInput50DQ` has 512-element `down`, `pressed` and `released` arrays, indexed
by public SDL2 physical scancode values. Index zero is unknown and ignored. Each
entry is zero or one; OS key repeats do not generate new pressed edges. Press and
release can both be set in one poll. Edges reset each successful poll; held state
persists until a release, focus loss, minimize or host close. Use SDL's published
scancode values in the binding layer; these are not character codes or game
controls. There are no guessed steering, throttle, brake, menu or reset bindings.

Polling pumps SDL once and retrieves at most `windowEventBudget50DQ` (128) events
into fixed stack storage. `eventsProcessed` includes ignored events. Exhausting
the budget conservatively sets `eventBudgetExhausted`; it does not promise that
more events remain. Queued events are left for the next poll, so keyboard state
is the state after the drained prefix, not an invented current snapshot. SDL's
own OS pump/driver work is not hard-time-bounded and may allocate internally;
50DQ itself has no per-poll application heap allocation.

Focus loss clears held keys and reports release edges. The end of each poll also
checks actual window focus/minimize state, so clearing occurs even when the loss
event is still behind the event budget. Regaining focus does not copy SDL's
global held-key array back into controls. Resume only after any event backlog is
drained. The caller should gate gameplay on `focused`/`minimized` even if a pressed
edge was observed earlier in the same batch. Quit requests stay latched until
close/recreate. Resize events and changed logical or drawable dimensions set
`resized`.

IME text input and heap-backed drop/extended-text events are disabled before the
window is created, since there is no consumer for them. The bounded drain also
frees any such SDL-owned payloads it encounters. Other events are ignored; mouse,
controller, text input and custom event transports are outside this host.

## Public API references

The narrow ABI uses SDL2's published event layout and functions:

- [SDL event union and fixed ABI size](https://wiki.libsdl.org/SDL2/SDL_Event)
- [SDL2 event declarations](https://github.com/libsdl-org/SDL/blob/SDL2/include/SDL_events.h)
- [Window, GL attribute and profile declarations](https://github.com/libsdl-org/SDL/blob/SDL2/include/SDL_video.h)
- [Scancode values](https://github.com/libsdl-org/SDL/blob/SDL2/include/SDL_scancode.h)
- [Bounded event retrieval](https://wiki.libsdl.org/SDL2/SDL_PeepEvents)

Actual GL version/profile are read with `glGetIntegerv`, because SDL2's
`SDL_GL_GetAttribute` version/profile cases return stored requests rather than
querying that context. See the
[SDL2 implementation](https://github.com/libsdl-org/SDL/blob/SDL2/src/video/SDL_video.c).

## Pending validation

The root integration task owns the shared hourly validation allowance. Required
future checks include library/display absence diagnostics; actual GL creation and
framebuffer requirements; quit/close/recreate; press/repeat/release and focus-loss
flows; resize/high-DPI/minimize/restore; saturated-event catch-up; renderer teardown
ordering; and real visible swap behavior. None has been executed for 50DQ here.
