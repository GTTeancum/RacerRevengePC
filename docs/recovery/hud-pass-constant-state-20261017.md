# Reuse pass-constant original HUD state across material transitions

UNTESTED. No execution checks, benchmarks or measured FPS improvement.
Base publication: 81d27c302282f600c3ba95e5f8b659c4cca2b84c.

TimeStats and SpeedBoost set depth-write enabled and source-alpha blend mode1
only at packet0. Their first/material-change texture-binding guards remain.
PositionStats sets blend mode1 only at packet0, retaining its original statement
position inside the material guard; material-dependent depth, radar/texturing
policy and texture binding still update on every material transition.

All packets were already admitted with positive counts, so a later iteration
follows a successful draw error check in either coherent-fetch or barrier mode.
No body operation changes these constants, invokes callbacks or relinks/switches
the UI program. Each pass starts fresh with packet0, and failures terminate the
loop before later reuse. All draw, barrier, error and cleanup operations remain.

Damage is intentionally unchanged: its only admitted material is0, so its prior
adjacent-material guard already wrote these constants once. Splitting that guard
would add source complexity without eliminating another state call.

For T material transitions after packet0, this removes 2T state calls in TimeStats
and SpeedBoost and T uniform writes in PositionStats. Source owners can reach
multiple materials, but actual transition counts and performance are unmeasured.
A successor may validate mixed-material frames, radar transitions, failure cleanup
and both fetch paths before measuring any benefit.
