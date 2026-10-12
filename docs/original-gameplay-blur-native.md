# Native original gameplay blur and completed display history

Source integration based on `673e5bb`, October 12, 2026. This work is unbuilt and
unexecuted. It adds native CPU/GPU rendering, not original-instruction execution,
GS/VU emulation, a capture-only effect, or an alternate diagnostic gameplay path.

## Real producer and render phase

`VehicleWindowRequest50DS::enableOriginalGameplayBlur` cold-allocates the owner.
A real camera owner lends its `GameplayBlurPending` plus exact original video and
camera rectangle through `GameplayBlurFrame50DS`. This join admits logical and
physical view 0 only. Split-view queues reject explicitly. There is no speed,
boost, host elapsed-time, force-on or inferred positive-strength API.

At the existing native scene tail, after racers, transparent world/pod effects,
Binder and the currently enabled host HUD, the renderer invokes the recovered
`consumeGameplayBlur` leaf. First-wins requests use its original signed clamp and
0..128 FIX conversion. No request retains the previous video strength/rectangle.
The existing host HUD is part of the current native scene tail; this integration
does not assert that it recreates the original HUD or original RenderPost3D.
Later original display-fade composition remains later.

A zero coefficient does no effect draw. A positive coefficient without a prior
actually completed native display rejects; it never substitutes the current
scene or a fabricated initial image. Queue consumption remains a Render3D event,
separate from successful display publication. A later rendering failure can
leave the consumed video state persistent while displayed history stays intact.

## Native GPU implementation

Three cold RGBA8 texture/FBO pairs are added at the explicit maximum native
width/height: a current-scene snapshot and two transactional history images.
An empty VAO and GLSL150 program are also cold-created, bounded and retained by
ordinary renderer cleanup. The existing inactive race renderbuffer is the blur
output. Neither sampled texture is attached to the draw target.

The pass reads the last accepted displayed composition, including any prior blur,
menu overlay or later display fade that actually completed. It applies the
recovered neutral `(Cs-Cd)*FIX/128+Cd` RGB byte arithmetic with signed-floor
rounding, in the original rectangle with its source one-logical-pixel UV inset.
It does not apply radial zoom, velocity vectors or host-strength heuristics.

Native nearest pixel-center sampling and preservation of current native alpha
are explicit host policies. This does not claim chip-rasterization/FBA/mask parity
or closure for a future consumer of original framebuffer alpha. Both sampling
and destination coordinates use valid viewport extents rather than cold texture
allocation size. Resize within the cold ceiling needs no hot allocation; prior
history is sampled over its retained valid extent in normalized logical space.
A resize beyond the ceiling is rejected by the existing frame bounds.

## Actual display ownership

`submit` still commits only a private image and optionally copies it to BACK.
It does not advance displayed history. `finishSubmittedWork` and `readbackRgba`
also do not advance it. This permits the first Attached underlay to be prepared
and completed without flashing it between the original OUT and IN effects.

The recovered route calls `completeGameplayDisplay` in place of its ordinary
window swap, or as its explicit headless display completion. It checks the exact
renderer GameHost/context and a new successful race/host-menu publication. It
copies the final image into the inactive history surface and completes GPU work
before calling the concrete GameHost window swap. Headless completes the same
native composition without a swap. Only after success does a nonfallible CPU
step accept history. No GL call or other fallible operation follows that swap.
Repeated completion without a new publication rejects.

The concrete GameHost bindings are in optional
`original_gameplay_blur_display.cpp`; add it to the interactive build alongside
the original display-fade backend. Renderer-only targets do not acquire a new
window/EGL linker dependency. Window completion means the actual native swap
call was submitted, not proof of physical scanout or compositor presentation.

The display-fade candidate's `gameplay-history-bridge.patch` calls
`stageGameplayDisplayedHistory(effectTarget,width,height)` before completion and
`acceptGameplayDisplayedHistory()` immediately after its real swap/headless
completion. Every accepted fade sample therefore becomes the next gameplay
history. The fade does not run a second gameplay blur pass. The optional hooks
are no-ops when the cold gameplay-blur owner was not requested.

## Failure and lifetime rules

- Incomplete rendering, failed BACK copies, failed history copies, failed GPU
  completion and unsubmitted swaps preserve the prior accepted history.
- The staged history bank is always the inactive bank. Cancellation leaves it
  replaceable. A subsequent retry/new frame never overwrites accepted history.
- A successful display-fade sample retires any ordinary unpublished display
  completion; do not call `completeGameplayDisplay` again for that same frame.
- Native pause menus can call `completeGameplayDisplay` after `submitHostMenu`.
  Their real final image enters history without consuming a gameplay request.
- Source VideoFX-manager teardown calls `clearGameplayBlurStrengths`, clearing
  the four video strength bytes while retaining rectangles and real history.
  The root separately owns the borrowed pending queue/session teardown.
- Close invalidates CPU image state, completes outstanding resource work,
  unbinds both texture units and deletes programs, shaders, VAO, FBOs and textures.
  Failed deletion retains names under the renderer's existing retry contract.

## Deferred central validation

No build, test, syntax/import check, renderer execution, decoder, gameplay,
sanitation tool or user-PC access was performed for this patch. Root retains the
single hourly basic-smoke slot. A later authorized check should cover zero and
positive source requests, feedback history, canceled/failed publication, host
menu and fade history, first-frame rejection, resized valid extents and cleanup.
Only genuine recovered requests may activate the gameplay effect in live footage.
