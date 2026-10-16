# Pause original text CPU geometry and selected width

Source-only isolated increment based03dc5ad. No build, test, syntax check,
project decoder or gameplay execution performed. No runtime activation.

OriginalPauseTextGeometry50LA operates on the already owned original Pause
texts. Object0 is base180;1 is title2D0;2 is prompt340;3..13 are the eleven
list texts beginning3B0. selectedWidth2E16F0 uses the private original880
selection and this exact list. It does not accept a replacement selection.

The existing original_ui_text50kg glyph arithmetic is factored into a CPU-only
entry taking actual281F90 atlas dimensions. All existing pixel-backed callers
retain their original font/atlas/rgba-shape validation and atlas output pointer.
The dimensions entry publishes a null atlas pointer: it provides no decoded
pixel or GPU admission receipt. Pause supplies dimensions from its actual
retained Application font Entry texture, not a fabricated Texture48B.

The adapter reuses the original glyph arithmetic, width/first-height metrics,
and source-scaled CVT.W.S conversion. It implements justification0..4 using
the same2E13A0 arithmetic as original_ui_text_position50kg. Original dirty
bit0 recomputes position, bits0/2 request inverse scale, and the final zero-
rotation transform multiplies scale. Sprite corners are the existing glyph
kernel's opposite corners0/2. Source authored coordinates and packet XYZ/ST
are updated only after all validation; RGBA and STQ Q lanes are preserved.
Active batches are ceil(length/36); activeVertices exposes the per-batch
length without overwriting source allocation capacity. Bits0..2 clear while
unrelated flags, including original bit3, survive.

Admission remains bounded to existing40/60-character allocations, two source
packets, zero rotation, positive finite scale, original depth field48=1 and
video maximumFFFFFF. Mixed metric runs/missing glyphs retain existing strict
kernel rejection. Empty strings produce zero active batches. This is neither
full2E0170 virtual dispatch nor a complete Pause draw callback. It does not
choose visibility, lay out menus, mutate selection, schedule timers, populate
GPU resources or append renderer packets. The caller must invoke individual
leaves in the actual lifecycle order and retain Pause throughout each call.

The storage friend/forward declaration is exported separately so the parallel
lifecycle patch can integrate without replacing its storage edits.
