# GC HOST steering preview: unbuilt and untested

This candidate addresses the observed first-bend HOST driver stops at AGC node 7. It has not been built or run. The previous 20:19 central attempt completed GC cold loading and capture, but all eight drivers stopped at node 7 and no GC lap passed. The next central run must establish visible first-bend progress and actual navigation beyond nodes 7/8; capture exit 0 alone is insufficient.

Only GC enables the new steering preview. Cold startup reads the authenticated original TUN AIDriving Lookahead value, 0.7 seconds, without substituting a default. The source direction planner at 0x172380 uses that horizon and a second 0.2-second sample, choosing the direction with the smaller alignment to current motion. The HOST implementation follows that idea through actual Next0 edges, with a bounded traversal and the existing low-speed floor. It remains a HOST input policy, with no claim to the original controller's complete branch, backward-traversal or arithmetic behavior.

Speed-preview traversal, authored pace and target computation remain unchanged. Native physics, canonical navigation, lap producers and all OffCourse, vertical-error, stuck and per-node safety stops are retained. TA leaves the new steering horizon disabled. Per-node and stop diagnostics add racer identity, actual position and speed, preview distances and steering observations.

The shared input callsite also supplies root.worldAngularVelocityA0.v[1] to the world-Y yaw API. The previous value was body-local angular velocity. The native root wrapper already produces the matching world-space observation; this changes the observation coordinate system, not damping parameters or native dynamics. This correction applies to every caller, including TA, and awaits regression coverage.

Gate/LostAI behavior remains separate. No raw original resource bytes, disassembly or instruction/data-word evidence is included here. See [GC integration status](gc-native-integration50ig.md) for current limits.
