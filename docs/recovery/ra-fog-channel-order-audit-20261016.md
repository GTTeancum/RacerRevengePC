# RA fog packed-color boundary correction

Source-only patch against010e1dc. No build, test, syntax/sanitizer check,
gameplay, screenshot inspection or decoder check performed. Original SLUS
was read only as source data. Central checkout was untouched.

Authenticated RA SCB field colorrgb is180,180,165. The actual RA fog owner
validates that exact field and retains color30=0xffb4b4a5, the game's packed
ARGB convention. Original13E990..13E99C copies fog+30 to renderer+28 without
repacking. Existing original133F20 channel extraction also confirms the game
packed-color convention: bits16..23 are red,8..15 green,0..7 blue, before its
separate screen RGB-halving operation.

bindRaWorldMaterial incorrectly sent low-byte first to the native shader,
turning authored RGB180,180,165 into165,180,180. The patch changes only that
native uniform boundary to high-red/middle-green/low-blue. Fog byte production,
alpha/material state, geometry, visibility and clipping are untouched.

Root independently reported foreground RGB164,179,179. That exactly matches
fully fogged floor((165,180,180)*255/256), so the mismatch explains its channel
color, not the presence of geometry. Correct fully fogged color is179,179,164.
No claim is made that this removes the foreground triangle or visual hold.

Audit found the accepted fog arithmetic internally consistent with recovered
source: slope255/(start-end); near F255, far F0; independent material FGE;
fragment RGB weightsF/256 and(255-F)/256, alpha unchanged. A large triangle can
cover the near viewport while all its original vertices carry far fog values.
That alone does not justify disabling fog or suppressing original geometry.
The separate placement/raw-quantization audit remains in progress.
