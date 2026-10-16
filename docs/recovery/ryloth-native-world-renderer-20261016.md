# Retained native Ryloth world renderer

Source implementation only. No build, compiler, test, decoder, sanitizer,
shader validation, gameplay run or visual comparison has been performed by
this worker. Root owns all execution/validation.

## Entry and lifetime

VehicleWindowRequest50DS::raStaticWorld retains the actual cold aggregate.
raStaticWorldTextures must be the same world's complete createRaStaticWorld
49-image GPU publication. Leave the legacy track empty. Existing sky, racer,
UI, particle and blur inputs remain independent, genuine source owners.

Creation acquires a private renderer publication on that actual world and
checks the exact image owner, context, thread and publication. It allocates
one complete source-index GPU buffer, one current-operand vertex buffer and
two dedicated native world shader programs. It uses the existing common
four-sample scene target and AA0 source-pair bridge, not legacy track shading.
Partial creation is retained until checked GPU cleanup. Cleanup drains GPU
work and retires all names before releasing the world renderer publication;
the existing image/aggregate close gates prevent early source retirement.
No source-only token or retail allocation/FIFO sequence grants readiness.

For each current published camera, call the actual aggregate's
prepareNativeStaticFogObservations, prepareNativeStaticLighting and
prepareNativeStaticDiffuseStream before submit. Fog preparation rebuilds the
camera projection, so obtain the submitted world matrix AFTER it. The renderer
compares all sixteen converted camera matrix words and verifies the same
camera generation/publication/frame and fog revision across the entire world.
Black/DiffuseCompressed reads only the actual per-vertex static diffuse binding;
prelit surfaces require their original source colors. Missing/stale operands
reject the frame, with no fallback light, constant color or fog value.

The renderer invokes actual crowd model service and current-map selection for
each source surface in order and resolves the canonical current child to the
retained GPU bank. It does not advance time, manufacture frame ordinals, or
substitute the first crowd image.

## Native raster semantics

All actual placements and source triangles retain their order. Opaque surfaces
can issue one indexed draw. Blended surfaces issue ordered triangles and copy
the preceding destination samples before each triangle. Alpha coefficients
above 128 remain unclamped until RGB saturation, using signed-floor
(Cs-Cd)*As/128+Cd. Output alpha independently stores source alpha. GREATER0 or
GREATER120 is evaluated after RGBA byte MODULATE and before depth/color writes;
ATE-off preserves zero-alpha fragments. All admitted profiles use inclusive
native depth with writes. Source ALPHA/depth/coverage fields come from the
world's authenticated seven-profile inventory, not caller blend enums.

Original UVs use native perspective interpolation and existing per-image
base-linear/repeat or vertical-clamp sampling. The bank already owns TEXA
expansion; the shader does not apply it twice. Native sampled texels and
screen-linear Gouraud values are floored to byte operands before MODULATE.
Fog is screen-linear from source-backed per-vertex packed bytes. The real
placement bound selects the compute-fog branch; raw integer positions,
original packet transform/scale and actual camera/end/slope feed the existing
native binary32 radial leaf. FGE controls fragment fog independently. Its
RGB operation is floor((Cs*F+Cf*(255-F))/256), preserving F255 attenuation.

AA1 crowd uses four-sample coverage with per-sample current destination.
AA0 blended material uses the existing center-raster payload followed by
per-sample composition so previous sample-specific values survive. Opaque AA0
uses normal native depth-tested triangles with multisampling disabled. World,
racers and effects share the same scene color/depth before the existing final
resolve, HUD, blur and display transaction. No diagnostic render is renamed
as gameplay.

## Explicit boundaries / remaining integration

This is native GPU sampling, clipping, depth and four-sample coverage, not
PS2 raster/EFU rounding emulation. Static diffuse uses the existing explicit
selectedMatrixWithoutModelOverrides policy. Unsupported fill/material,
nonrecoverable raw positions, stale camera/fog or absent capabilities reject.
No source vtable/allocation chronology or guest instruction execution is added.

The renderer does not construct a playable Session, input loop, Lost AI,
original sky publication or runtime UI. Those remain the real session owner's
integration obligations. It also does not synthesize bounded FireP0t
CompletedDrawReceipt values for this distinct full-world owner. A consumer
requiring that specific receipt needs a genuine completion join. Build and
visual validation plus performance measurement are still outstanding; ordered
blended triangle snapshots deliberately favor semantic correctness over speed.

## Independent source review after environment recovery

Reviewed draw attachment/sampler transitions, source arithmetic and order,
current-camera scope, and partial creation/deletion ownership. The snapshot
texture is never the active draw color attachment while sampled; AA0 payload
and depth are sampled only after switching back to the common scene target.
Each blended triangle gets its own preceding-destination sample copy. The
RGBA16F payload can represent every integer source byte exactly, with negative
alpha reserved for untouched pixels; only surviving payload pixels consume
raster depth. Existing cleanup uses context/thread checks, GPU finish and
checked name deletion before retiring renderer publication. A failed deletion
keeps both the resource name and world lease available for retry.

Added explicit draw-range and signed GL count admission, rejection of the
unimplemented fill-color program flag for every static profile (including
prelit), and a fresh actual camera/fog lease read immediately before drawing.
The latter closes a stale-operand window between CPU frame preparation and
intervening frame callbacks. No compiler, shader validator, test, decoder,
sanitizer or gameplay process was run during this review. Runtime GL errors,
visual correctness, native sampling edge behavior, and performance still need
root-owned validation.
