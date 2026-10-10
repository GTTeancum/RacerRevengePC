# GC HOST steering preview: partial runtime coverage

The 21:19 central slot built and ran source e9ece06f227d4834c1591d235e92b1856d944756. The player advanced beyond the previous node-7 stop to node 28, but opponent slots 2 and 3 stopped at nodes 7/8. The 20-second capture and actual-audio export completed; no GC lap or complete-field pass is claimed. This result supports a focused improvement, not full-course or native-AI equivalence. See [current validation](current-validation.md).

Only GC enables the new steering preview. Cold startup reads the authenticated original TUN AIDriving Lookahead value, 0.7 seconds, without substituting a default. The source direction planner at 0x172380 uses that horizon and a second 0.2-second sample, choosing the direction with the smaller alignment to current motion. The HOST implementation follows that idea through actual Next0 edges, with a bounded traversal and the existing low-speed floor. It remains a HOST input policy, with no claim to the original controller's complete branch, backward-traversal or arithmetic behavior.

Speed-preview traversal, authored pace and target computation remain unchanged. Native physics, canonical navigation, lap producers and all OffCourse, vertical-error, stuck and per-node safety stops are retained. TA leaves the new steering horizon disabled. Per-node and stop diagnostics add racer identity, actual position and speed, preview distances and steering observations.

The shared input callsite also supplies root.worldAngularVelocityA0.v[1] to the world-Y yaw API. The previous value was body-local angular velocity. The native root wrapper already produces the matching world-space observation; this changes the observation coordinate system, not damping parameters or native dynamics. This correction applies to every caller, including TA. The focused TA/menu and GC sequence passed; broader regression coverage remains incomplete.

Gate/LostAI behavior remains separate. No raw original resource bytes, disassembly or instruction/data-word evidence is included here. See [GC integration status](gc-native-integration50ig.md) for current limits.
