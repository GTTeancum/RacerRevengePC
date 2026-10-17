# Ordered racer projection quotient reuse

Status: source-reviewed and untested. No build, syntax check, gameplay run, test or benchmark was performed for this change. Performance benefit is unmeasured.

Base: 61b289e3bc34bbab7a505553be5a937a2e97854c. Base RA source blob: ae731c9fc8b2c9ee7ccd065d0b9f5c6d6aae2536.

The existing synchronous original MSCNT-group cache retains transformed vertex operands. This change also retains the eight finite scalar projection quotients for a matching indexed vertex. It does not cache merged extrema, a triangle rectangle or a failed projection.

The original common-plane rejection remains before projection lookup or arithmetic. Each corner retains the original W guard before lookup. An eligible cache slot must belong to an active group, be occupied and finite, and have the exact current vertex-index tag. The three operand records are still copied into local triangle storage first. A collision during those copies can overwrite a slot, so the projection helper reacquires and checks the slot at the moment each corner is consumed. A tag mismatch uses the authoritative local operand copy and retains no projection. No cache-slot pointer survives across corners or calls.

The miss expression remains exactly (c[axis]+a*e[axis])/(c[3]+w*e[3]). The axis, numerator-sign and denominator-sign iteration order is unchanged. Hits return the corresponding stored double value. Every value still passes the same finite check and the same ordered std::min/std::max operations. This preserves signed-zero tie behavior and the triangle's original extrema sequence. Width/height conversion, floor/ceil, margins, clamping, conservative fallback and all grouping thresholds remain unchanged.

A ready flag is cleared before every operand recomputation, including an uncached call. It becomes true only after all eight quotient slots have received finite values. A W failure or nonfinite quotient never marks that record ready. If a later corner causes full-region fallback, earlier fully completed vertex projections remain valid for the same group. Repeated indices may reuse the same completed values; direct-map collisions can reduce reuse but cannot associate a different vertex's values.

The caller passes the current three-index slice only after the existing active-group, slot, component and index-range checks. The pointer remains local to this synchronous call and is not retained. Matrix, source indices, vertex data and floating-point control remain stable across the callback-free group, as required by the existing operand reuse. Projection quotients do not depend on viewport extent; current extent conversion still runs per triangle. Beginning each group clears occupancy, ending it clears the active flag, and the next operand replacement clears readiness. Generic RA projection calls keep the default-null cache argument.

Potential work avoided is eight divisions for each eligible repeated vertex. The cost is fixed storage for 32 sets of eight doubles plus a ready flag and padding, one readiness-reset store per operand miss, lookups, quotient indexing, conditional branches, stores on first projection and loads on reuse. Fully clipped triangles perform no projection lookup or quotient calculation; a preceding operand miss still performs the readiness-reset store. The uncached path retains its arithmetic and may incur small bookkeeping overhead. Net speedup has not been measured.

The change does not modify shaders, geometry, source vertex attributes, clipping policy, draw order, GL calls, publication gates, error checks, physics or callbacks.
