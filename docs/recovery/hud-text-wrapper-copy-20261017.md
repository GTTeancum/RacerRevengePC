# Remove the redundant pixel-backed HUD text layout temporary

UNTESTED. No build, syntax check, smoke, gameplay or benchmark executed.

layoutOriginalUiText50KG validates the genuine font/atlas and calls
layoutOriginalUiTextDimensions50LA. The dimensions helper independently builds
its fixed 255-quad layout and assigns output only at its final success statement.
The wrapper formerly supplied its own full temporary and copied that layout again.
It now passes the caller output directly to the helper and assigns the genuine
atlas pointer after success.

All wrapper checks, helper checks, glyph arithmetic and diagnostic updates remain
unchanged. Every helper failure returns before output assignment. The final atlas
pointer assignment cannot fail, allocate or invoke callbacks. The documented API
requires distinct input/output/diagnostic storage. Text bytes are all read before
publication, including if a caller's byte view refers to previous output storage.
Unused quads and sentinel members still come from the helper's initialized local.
This preserves member values, not a new portable padding-byte guarantee.

TimeStats and SpeedBoost dirty text paths reach this wrapper, including a second
layout on right-justification updates. The source removes one extra full layout
initialization/copy per wrapper call; compiler elision and runtime benefit have
not been measured. Two independent source-only reviews accepted the design.
Successor should validate valid/invalid/empty runs, original atlas admission,
right justification and unchanged output on all failures before timing it.
