# SA canonical racer ownership

This change extends the existing original-Main canonical racer route to SA.
All selected racers construct owners at their actual Rig installation, complete
their original model resources and material references, and contribute their
PSM/map closure before the same Application material domain is populated once.
The existing source-provider conflict checks, context ownership, model binding,
camera/entity bundle and cleanup order remain enforced.

The track adapter now distinguishes canonical racer material initialization
from canonical world targets. TA retains both exact pinned world targets and
their original resource assertions and submitted-span accounting. SA supplies
no canonical world targets: its independently authenticated, full authored-pass
world path remains intact. The proposed SA `tred01` world target is not admitted.

The authenticated SA SCB/PRT cold particle adapter runs at the existing
post-population phase with the same Application table. This does not populate
particle rendering resources or claim rendered particle parity. Persistent
provider conflicts remain hard failures; no first-match override, substitute
texture, alternate material table or silent Application reset is introduced.

The centralized Windows original-Main Occo/SA build and smoke passed at
commit `804d274` on October 14, 2026, ending at 00:47:43 UTC. The single
GPU-completed swap baseline was 7.4919 FPS. This is historical Windows evidence;
no fresh Linux result is claimed.
No full-race, cross-track restart, performance or visual-parity result is claimed.
