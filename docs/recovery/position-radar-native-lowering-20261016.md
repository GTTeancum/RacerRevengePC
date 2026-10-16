# Native original radar type5 lowering

Incremental source candidate against the traversal result based on4c56471.
No builds, tests, syntax checks, sanitizers, gameplay or decoder checks run.
Actual original geometry is lowered with host C++ arithmetic; no guest packet
execution, VU interpreter or guest allocation emulation is implemented.

## Actual geometry and counts

finishRadarElement192624 now also performs2E26A0..2E27A0's active-count effects:
ceil(count08/72) active packets, min(remaining,72) vertices per active packet.
Original allocated packet first/count and overlap aliases are preserved, including
unused trailing packets. No inferred ideal full-strip coverage replaces the
source's packet counts. count48 is allocation lifetime; capacity4C is now active
primitive count after finalization, as in source. Both teardown drains remain.

lowerRadarElement2E2ED0 admits the actual radar constructor state scale1,
rotation0, depth1 and width3, and actual Video maximum0xffffff. For each even
source vertex it uses the existing roundOriginalUiTextMetric50KG native EE
CVT.W.S contract, writes authored Z and copies source/color into the odd pair.
The existing shared conversion's truncation contract is used consistently with
other native UI paths; the earlier held note's nearest-rounding suggestion
was not an implementation or established source contract.

Consecutive quantized centers produce a native normalized direction; zero-length
segments retain zero XY normal. The source screen perpendicular (-ny,nx) times3
is added/subtracted into the actual packet STQ aliases. Segment0 writes its
starting pair, and each segment writes the next pair. Interior joins use the
preceding segment's normal, without an invented averaged/miter join. Source
STQ.z receives VideoMaximum-1 plus/minus3 and remains preserved. Source authored
XY, packet XYZ and unrelated fields are not replaced with guessed GPU positions.
All copies at overlapping packet boundaries receive the same STQ and RGBA.

Preparation automatically invokes this lowering after real route/pose traversal.
Native failure preserves previously committed source writes; sourcePC identifies
the failing lowerer. dirty3C clears only after its whole element succeeds.

## Native draw join

OriginalPositionStatsHud50LA is now a friend of the resources and primitive
owners. The independent HUD assembler can synchronously borrow completed
radarRoute_.completedElements; for each element it visits capacity4C active
packets, activeCount source vertices and its actual primitives30[p]->material.
Each quad orders source [odd0,even0,even1,odd1], preserving the original strip
shared diagonal under native indices[0,1,2,0,2,3]. No triangle is invented from
the source's unsubmitted trailing packet data.

The material worker's actual canonical FINAL pass proves depth ALWAYS and no
Z-write before converting screen pixels/rawRGBA to native vertices with inertZ0.
This is depth-independent native rasterization, not a claim that STQ.z equals
packet XYZ.z or that it can be clamped blindly. The raw expanded STQ remains
owned unchanged. Fractional expanded XY is supplied in screen-pixel units;
no guessed additional ScreenObject offset or12.4 snapping is introduced.
The full HUD/renderer owner must retain real canonical pass state, alpha storage
and reverse resource retirement through actual submission.

No activation call is added here; the parallel HUD assembler/renderer work owns
the live full-frame join. Remaining original work includes racer-marker textures
and any unsupported source material/shader state. No runtime success claimed.
