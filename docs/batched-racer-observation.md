# Batched racer observation

Status: source-reviewed and centrally integrated; uncompiled and untested. This change replaces redundant camera-slot restores between existing serial observation callbacks with one bounded bank lease. It is based on the all-eight original-AI/lighting/route source that passed the preceding short GC run. Performance has not been measured after this change.

`Bank::visitMany` preserves the supplied callback order; `visitAll` supplies ordinal order. The same full-fleet readiness, dynamic-peer, dynamic-actor and original-AI observation sweeps remain. Callbacks copy current owners into external observations and retain no arena pointers. False stops immediately, exceptions propagate, and the scope guard saves the current slot and restores camera slot zero. Nested visits remain disallowed during the lease.

Native stepping, AI dispatch, collision responses, manager timing/ranking, route/RNG operations, sound generation, fixed-step duration and host catch-up policy are unchanged by source inspection. No stale fleet cache substitutes for observations immediately before each racer frame.

For eight racers, each ordinary sweep changes from 28 full snapshot-copy passes to 16. The observed snapshot size was 311,358 bytes; across 44 retained steady-world sweeps, static accounting removes 528 passes, or 164,397,024 bytes of copying per world step. This is a source-derived work reduction, not measured speedup or proof of real-time performance.

The next authorized central run must retain actual all-eight AI ownership and source-boundary checks, then compare physics CPU cost and wall/native progression. False/exception cleanup, camera restoration and nonidentity manager order require runtime coverage. The preceding test result and incomplete capture/audio limits remain in [current validation](current-validation.md). No software checks were performed while preserving this source.
