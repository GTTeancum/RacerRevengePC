# PositionStats texture dirty-bit polarity

The active PositionStats resource writer had inverted the original test for
ScreenTexture rotation `field3C`. Position/dimension changes always set bit0,
but set bit1 (mask2) only when the rotation differs from zero.

Retained source words in `rr-clock-full-static-fields.txt` establish:

- Width: `190360` loads rotation, `190364` compares equal to zero, and
  `19036C BC1T` skips the mask2 store at `190388`.
- Height: the corresponding comparison at `1903D0` and branch `1903D8`
  have the same polarity.
- Fixed layout: `191358` loads rotation, `19135C` compares, and `191364`
  skips the mask2 store at `191380` when equal. The other fixed objects
  repeat this sequence.
- Rival marker movement: `1928EC` loads rotation, `1928F0` compares, and
  `1928F8` skips the mask2 store at `192914` when equal.

All four native conditions now match this source branch. The bounded native
ScreenTexture renderer already admits only rotation0/scale1 and now also
rejects a genuinely outstanding mask2 regeneration request rather than silently
ignoring it. Fresh texture flags start at zero, and the admitted zero-rotation
path sets only bit0, which the existing XY regeneration clears.

No layout, text, texture, numeric observations, source order, or renderer
submission is replaced. This is a source-state correction in the active HUD;
it does not claim new visible elements or broader rotation support.
Source-only review; no build or execution was performed.
