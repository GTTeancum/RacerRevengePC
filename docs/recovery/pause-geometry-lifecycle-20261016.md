# Pause geometry/lifecycle segment, source only

Base:03dc5ad. Source: authenticated SLUS_202.68, VA file offset VA-0xFFF80.
Static annotated words retained at /workspace/shared/pause-1b9220-1ba130.words.txt.
No build, test, syntax check, guest execution or runtime validation performed.

## Implemented private segment

OriginalPauseStorage50LA now owns the actual constructor defaults +30=.25
(1BA1C4), +160=0 (1BA1DC), +164=0 (1BA1E4). No public timer or selection setter.
advanceGeometry1B9324 consumes the existing actual Application clock capability,
which must expose an active begin/commit update250 publication. It does not accept
an arbitrary elapsed frame. It is deliberately private and not yet called.

1B9324..1B976C: while +30>=0, progress=1-(+30/.25), extent=min(128,128*progress),
center backdrop+40 has x0,y196-extent,height2*extent and depth1, upper+A0 is
x0,y196-extent-16, lower+100 is x0,y196+extent. Actual center height is computed
as bottom-top, preserving the source subtraction. Bottom polygon+38 vertices0/1
move to y448-65*progress; vertices2/3 retain their authored bottom coordinates.

1B9770..1B9BF0: once +30<0 and while +164>=0, progress is +164/.25 when +160
is nonzero, otherwise 1-(+164/.25). The same three backdrop changes run. Top
polygon+3C vertices2/3 move to y65*(1-progress); vertices0/1 retain their authored
top coordinates. Only concrete source kind1 four-vertex storage is admitted.
Both paths preserve X/Z/W and copy the whole authored vector to its actual STQ
alias; they do not substitute native XYZ for ScreenObject processing.

Geometry precedes the countdown write. A positive timer subtracts actual250 and
clamps at zero; a timer already zero becomes -1 on the following callback. The
other timer is untouched. Both polygons receive field18=1 and dirty3C=1 at
1B9BF4..1B9C20, including when neither animation phase runs. Texture dirty bits
retain the already corrected field3C!=0 polarity. No extra lower clamp or guessed
frame duration is introduced.

Exact private reveal1BA120 stores +164=.25 then +160=0. The direct caller1BA0C4
is the shared controller-disconnection update: mode1F0 is nonzero and +160==1.
This is not proof of a complete Pause open/close input contract. The +160=1 store
at1C994C belongs to a different derived owner; it must not be transplanted here.

## Remaining callback integration gates

1B9308..1B931C admits only actual Application+0C == Application+264-1. There is
no actual multi-camera scheduler observation published in the current adapter,
so the private segment must remain uncalled until this admission is implemented.
The clock capability alone does not establish camera admission or parent identity.

The rest of1B9220 is mixed mutation and DRAW, not merely an update callback:
-1B9C24..1B9D80 refreshes baseText to localization88 or87 from real mode1F4 when
 mode1F0 is set. Actual device-disconnection authority still needs binding.
-Player count is the original two-bit field (race1125C>>5)&3, extracted by the
 source DSLL32/DSRL32 pair. For count>=2 it saves the actual viewport, selects
 full640x448, draws bottom polygon, virtual+78, virtual+74, optional baseText,
 then top polygon iff +160!=0 OR +164>=0; it restores the saved viewport.
-For count==1, virtual+78 then virtual+74 precede optional baseText, bottom
 polygon, and the same conditional top polygon. Entity141D70 runs afterward.
-Count0 skips these draws and reaches Entity141D70.
-Virtual+78 is the genuine backdrop path (derived1BAF70 also conditionally
 submits icon); virtual+74 is derived combined layout/text1BB010. Backdrops
 draw in source+A0,+100,+40 order, admitted when +160==0 OR +164>=0.

Do not claim the new method completes1B9220, call it independently of its draw
scheduler boundary, or mark Pause ready. Remaining work is actual parent/input
binding, mode1F0/1F4 observation, derived layout/glyph regeneration, canonical
ScreenObject material submission, viewport scope, Entity traversal and checked
GPU retirement. The current cold-only destructor is unchanged and cannot be
used to retire a future in-flight native GPU owner.
