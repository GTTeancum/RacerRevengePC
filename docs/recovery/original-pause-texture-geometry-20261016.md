# Pause ScreenTexture CPU projection

Isolated source-only increment from ef17dda. No build/test/syntax checks,
project decoder or gameplay execution. Header is inactive and separately
included by the actual driver solely for the next root-owned central build.

OriginalPauseTextureGeometry50LA::regenerate2DF550 updates the real existing
Pause packet and returns a value snapshot with its retained source-reference
ordinal. Objects0..2 map to+A0,+100,+40 (references0/1/2);object3 to icon890
(reference7);objects4/5 to arrows210/270 (references34/35). It never chooses
which objects are visible or their submission order.

Retained source rr-clock-full-static-fields.txt:2DF684/688 reads dirty bit0;
2DF68C bypasses geometry when clear.2DF694..6C0 multiplies dimensions by30
then adds position.2DF6A0/6AC extracts rotation bit1, branching2DF6BC to the
unrotated writes2DF80C..848. These write X/Y in strip order TL/TR/BL/BR.
2DF84C..860 clears only bit0. The bounded adapter admits the actual current
Pause scale1, rotation0/bit1clear, depth1 route. Unsupported rotation rejects.

The snapshot preserves every original RGBA/STQ lane, depth and W. In
particular center+40's direct0..0.1 ST values survive, and degenerate heights
from the real opening animation remain valid. It performs no atlas decode,
UV normalization, alpha conversion or host replacement geometry. Native
validation precedes mutation and failure leaves output/packet unchanged.

The original2DF634/63C and2DF67C/680 packet header viewport values are not
modeled as vertices or invented here. Their eventual native equivalent must
come from actual viewport submission state.246760 material dispatch, source
phase assertions, viewport scopes, visibility, GPU preparation/retirement and
original callback sequencing remain outside this CPU-only leaf. The returned
snapshot is not a material/image or draw-readiness receipt.
