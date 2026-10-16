# Original ALL8 radar rival markers

Incremental source-only patch after radar native lowering. No builds, tests,
syntax checks, sanitizers, gameplay or decoder checks were performed.

1927F0..192A4C visits actual manager order, excludes the current view slot and
skips nonzero state40. The adapter now reads these actual states and visits each
eligible bank slot to refresh/read its body288 world transform. The current
view0 pose/heading still comes only from slot0, whose original AI is unchanged.
The synchronous publication and same-owner checks are preserved.

Original projection1922B0 is reused, then each rival's texture index117-slot
receives (projectedX-3)+layoutX and (projectedY-3)+layoutY. The packet alpha lanes
receive original alpha/4; packet RGB receives107,9,23 from source00D6122E.
Metadata alpha48/color44 is updated with the same setter-change guards. Changed
positions set original dirty bits. Alpha0 markers remain in source draw order.

Resources expose the private friend-only radarMarkerIndices_ and radarMarkerCount_
for the authentic HUD assembler. Slot1..7 yields texture116..110 in that order.
The existing texture lowering must update positions and preserve these exact
packet colors/STQ. The assembled order is radar route, rival markers, fixed6
center icon, then later original HUD phases.

Subtype3 has a separate manager+40 entity and flags29 gate at192A60 onward.
That identity is not established by the bounded ALL8 route. It explicitly fails
at192A60 rather than inventing a self marker or silently claiming completeness.
No simulation, driver selection, rank or lap fields are changed.

Source words are already preserved in position-draw-source-20261015.txt.
