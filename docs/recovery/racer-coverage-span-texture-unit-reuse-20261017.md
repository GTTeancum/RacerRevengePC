# Coverage span texture-unit reuse

Status: source-reviewed optimization; UNTESTED. No build, syntax check, test,
gameplay run or benchmark accompanies this change. Performance benefit is
unmeasured.

## Exact scope

In `drawSpecularSpan50LA`, the four-sample coverage route omits two repeated
three-call texture-unit transitions: the later-group base unbind, and the
second-group bind of texture 0 on unit 1. Both formerly selected unit 1, bound
`GL_TEXTURE_2D` 0, and returned to unit 0.

The single-sample route retains those transitions and its original
`sourcePairColors50LA[bank]` binding. Span entry and final cleanup remain
unconditional. All existing setup, draw, group and restoration error checks
remain in their original positions. No source pass, draw, framebuffer, sampler,
shader, blend/depth operation or coverage rectangle changes.

## Entry and successive groups

Both actual callers first accept `applyPodMaterial`. A paired material sets
`hasSpecularPairs50LA` during its complete cold copy, so the material and span
entry calls to `unbindSpecularFeedback50LA` really establish unit 1's 2D binding
as zero, sampler object 0, and active unit 0. The retained first base setup check
audits span entry before its draw.

The base setup and draw leave those texture-unit bindings unchanged. Texture 0
therefore remains current for both the existing base-image bind and the
subsequent map-2-image bind. No callback or external state owner runs between
groups, and `coverageDemand` is fixed during cold creation.

On the coverage route, the composition program temporarily uses unit 1's 2D
binding for private raster depth. Every successful bridge restores unit 1's 2D
binding to zero, returns to unit 0 and restores the source map-2 image, then
checks the complete tail. The bridge also retains its separate unit 2
multisample-target unbind. This change does not treat different texture targets
or sampler-object bindings as interchangeable.

Consequently a successful group leaves exactly the state needed by the next
base setup. The source image on unit 0 still changes between base and map 2;
both of those required image binds remain.

## Empty groups and failures

A group whose conservative regions are all empty performs CPU lookahead only
and preserves entry bindings. A partly visible group receives the same checked
tail after every nonempty bridge; skipped empty regions do not modify GL state.

An error in material/span setup, a base draw, second setup, bridge or bridge
restoration prevents another dependent group. The existing span tail still
unbinds feedback and restores the source texture, uniforms and depth mask on
success or rejection. No failed tail authorizes reuse. Existing frame abort and
publication boundaries remain unchanged.

## Incidence and limits

For a map-2-enabled coverage span containing n >= 1 original MSCNT groups, the
omission removes 3(n-1) later-base calls and 3n second-setup calls: 6n-3 GL calls.
The existing source-colored racer coverage route uses this loop. Distance-skipped
spans and the single-sample route retain their previous behavior.

This removes no raster draw, destination snapshot or per-run error check, and
does not expand batching. Its effect on driver submission time and frame rate
is unknown.

The patch starts from vehicle renderer blob
`7b8acea8079d329e90b1c3c1329d320c9f892ca7`, including the separately reviewed
normal-upload change. The bridge proof was checked against coverage source blob
`8ce5eb5cb2c6c01dd2ab8919edad6e91bd08663e`.
