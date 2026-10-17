# Reuse RA composition mode within a selected draw

UNTESTED source-only change. No execution checks or measured performance gain.

In drawRaWorld's blended-AA0 branch, write coverageRaCompositionUniform=1 only
when firstPrimitive is true. Keep the coverage program bind and draw unconditional.

The flag is local to a selectedDraw, starts true and becomes false only after
the existing primitive error gate succeeds. Clipped or empty primitives skip
without advancing it. A failure stops the loop. Blend and AA mode come from the
same immutable selected raster, so that draw cannot alternate into another path.

Each later AA0 primitive switches between the RA payload program and the private
coverage program. Uniform values belong to their program and survive those
bindings. bindRaWorldMaterial writes only RA program uniforms. The only other
coverage composition-mode writer is the racer bridge, which cannot run inside
this synchronous selected-draw loop. No relink, extent swap or callback occurs.

New selected draws still initialize mode1 at their first admitted primitive;
racer spans still initialize mode0 independently. Every draw, error gate and
cleanup remains. One uniform write is removed per later admitted blended-AA0
primitive in a selected draw. A successor may validate alternating RA/racer work,
clipped leading primitives and rejection handling before measuring impact.
