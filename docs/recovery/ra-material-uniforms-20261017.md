RA material uniform reuse: untested source-only patch

Base: 7dee8b3a38c3191c7b37528bac8de3580a8aae50
Only changed file: platform/linux/native/ra_static_world_gl_storage.inl

Scope and proof:
Each selectedDraw fixes its RasterInventory, blend/AA choice, image, native
camera matrix, original source matrix, fog color/FGE, alpha threshold and
payload/destination mode. The selected draw loop is synchronous and has no
external callbacks. Its clip and rectangle helpers are CPU-only.

bindRaWorldMaterial still calls gl.useProgram on every submitted primitive.
It uploads the same six uniforms only for the first nonempty primitive:
two matrices, image unit, fog vector, alpha threshold, payload/destination.
A local firstPrimitive flag resets for every selectedDraw and changes only
after the existing per-primitive GL check succeeds. Clipped primitives do
not advance it. Errors end the loop through the existing cleanup path.

There are exactly two helper call sites, the AA0 blended payload path and
the opaque/AA1 path. Their program and payload choices remain unchanged and
are constant within selectedDraw. RA-program uniforms have no other writers
within this loop. The AA0 coverage compositor is a separate program; its
mode=1 write remains on every primitive. No value is cached across selected
draws, frames, program links or callbacks.

The original snapshot and per-primitive error polls, source payload draws,
per-sample compositions, all texture operations, scissor/clip tests, final
restoration and publication are unchanged. There is no new heap allocation.
Static saving: six uniform uploads per additional surviving primitive in a
selected draw. No measured performance improvement is claimed.

This change does not modify clip-region arithmetic.

Independent source review completed with no identified blocker.
No builds, tests, syntax checks, gameplay or benchmarks ran.
