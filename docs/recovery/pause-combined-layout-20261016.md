# Original Pause combined CPU layout preparation

Base c1ed29a. Static source only; no builds/tests/syntax checks or execution.
Source interval1BB010..1BD7BC retained in
/workspace/shared/pause-1bb010-1bd7c0.words.txt.

OriginalPauseLayout50LA is a narrow private leaf, invoked only through the
existing actual Application/camera/track/profile observation facet. The facet
weak-locks those actual owners, revalidates original material/RA identity and
camera generation, applies source mode1F0==0 and opening30<=0 admission, then
refreshes genuine title/settings strings and regenerates actual owned geometry.
It does not offer a selection, controller, text or elapsed-time setter.

Source positions/colors:
-title+2D0 at320,82; source call2E0170 at1BB31C.
-eleven rows+3B0+i*70 at320,112+20*i, loop ordinal compared to actual880.
 Original constructor words+170=CCCCCC select the current row; +16C=E5D96C
 color the others. Original setTextColor retains source half-range packetRGB.
 Each row uses genuine2E0170 regeneration at source call1BD5D0.
-only selected rows2..8 enable arrows. Width is genuine2E16F0 from actual
 selected CEL/texture dimensions and scale/CVT.W.S. The selected origin is
 regenerated source+20/+24, not a guessed centered screen coordinate.
-left arrow+210 x=originX-left.width (+0 at1BD63C), y=originY+2.
-right arrow+270 x=6+(originX+float(width)), y=originY+2. The +6 is required:
 1BD5EC sets f21=6,1BD6CC adds width,1BD6D0 adds f21. An earlier informal
 recovery summary omitted this spacing; this implementation includes it.
-arrow original2DF550 CPU projection preserves STQ/RGBA/depth/W.
-prompt+340 is regenerated last at source call1BD768.

The bounded15-entry Commands result records the exact source submission order:
title, eleven list rows, left/right arrows if eligible, prompt. It records only
kind/object/sourcePC, has no owner pointer or ready flag, and does not submit
anything. Object numbering matches integrated leaves: text1 title,2 prompt,
3..13 rows; texture4/5 arrows. On failure, the published command list is empty;
already performed source CPU mutations are not rolled back. The same retained
owner must be held and commands consumed synchronously by future lowering.
Commands are not a retained frame, GPU lifetime lease or authorization token.

The observer refreshes text strings in one serial CPU preparation phase before
geometry regeneration. This is equivalent for the immutable observed profile
and text data only; it does not claim interleaving with yet-unimplemented source
material/renderer callbacks. Future complete1BB010 dispatch must preserve those
callback boundaries, not just replay an asynchronously cached list.

Remaining gates: actual last-camera/parent admission before captureLayout,
complete1B9220 draw-tail order including backdrops/icon/polygons, original Entity
input/open/close callbacks, canonical FINAL lowering and checked GPU retirement.
The new method remains uncalled; the existing driver observation-header include
compiles the added header transitively without a runtime activation change.
