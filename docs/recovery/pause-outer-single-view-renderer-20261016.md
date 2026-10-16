# Original Pause ordinary single-view ordered renderer extension

Base5530547. Source-only implementation; no build, syntax check, test, decoder
or application execution. No driver call or scheduler activation is added.

## Genuine outer order

The new observation `captureOuterLayout` reads the SAME Application profile's
playerBits1125C. It never derives this field from ALL8 manager humanCount34.
Count0 produces no draw commands or derived-layout mutation. Count1 admits the
ordinary mode1F0==0 path and retains this exact ordered command sequence:

1. Derived virtual78: icon890 iff opening30<=0, at1BAFA4.
2. Backdrops+A0,+100,+40 iff hidden160==0 or transition164>=0,
   at1BAFE0,1BAFE8,1BAFF0.
3. Genuine private derived1BB010 commands, including their existing visibility.
4. Bottom polygon+38 at1B9EF8.
5. Top polygon+3C iff hidden160!=0 or transition164>=0, at1B9F30.

Icon precedes the backdrops in the actual1BAF70 implementation. It is not appended
after them. Existing derived capture now records its original owner identity even
when its visibility gate yields no commands, allowing the opening-animation outer
geometry to remain distinct from the hidden derived text.

`OriginalPauseNativeDraw50LA::prepareOuter` lowers the complete admitted sequence
into one batch. Existing GL binding/frame hooks remain named PauseDerived for API
compatibility; the frame records outerTail=true. It is one private target, one
initial depth clear, and one source-ordered pass stream for all submitted objects.
No polygon/background component causes a separate clear, compositing pass or
opaque-alpha normalization between components. The existing checked GPU cleanup
and strong canonical owner lifetime remain unchanged.

## Kind1 polygon source projection

Retained static source is rr-clock-full-static-fields.txt.2E2398..23B4 selects
kind1 and calls2E2DD0 then2E2ED0 before the original246760 primitive loop.
2E2944..2978 installs PRIM0xC: Gouraud triangle strip. Source vertices remain
0,1,2,3, lowered to host perimeter1,0,2,3 to preserve the original strip diagonal.

2E2FE0 tests dirty3C;2E3010..3020 derives depth from actual Video60-field18.
Rotation0 skips the pivot transform. The non-type5 branch2E3714..3898 rounds each
scaled authored X and Y through CVT.W.S then converts back to float, writes the
authored depth and copies projected coordinates to the genuine STQ alias. The
new bounded adapter admits only the actual four-vertex kind1 constructor state,
scale1, rotation0, depth1, flags3 and exact primitive/packet/alias identities.
It reuses the existing source-compatible rounding kernel. Packet XYZ and RGBA
are not fabricated or substituted. Native untextured raster vertices consume
projected STQ XY/Z and retain actual packet RGBA. The existing fourth STQ lane
is preserved and is not consumed by this native TME0 path; no unproven global
scratch-W value is introduced. Textured polygon policies reject.
No type5 radar expansion, guest packet execution or VU execution occurs.

## Explicit remaining gates

This prepares the DRAW TAIL from current actual storage; it does not call the
geometry timer segment or establish last-camera/Entity admission. The genuine
1B9324 geometry phase must precede this tail at its eventual scheduler boundary.
Commands remain synchronous borrows, not immutable historical frame snapshots.

Count>=2 rejects at1B9DB0: actual115A60 six-field viewport save/override/restore
ownership is not joined. Mode1F0!=0 rejects at1B9C24: authentic device-disconnection
mode1F4 producer is absent. Therefore optional baseText localization87/88 cannot
be emitted, and no replacement text or default mode1F4 is introduced. The existing
native text emitter already supports object0 once that original producer and
outer mode branch are genuinely supplied.

The ordinary source profile can retain count1 even when every racer uses AI.
No racer ownership/control policy is modified. Full Pause open/close, last-camera
scheduler and inverse Entity retirement remain required before activation.
