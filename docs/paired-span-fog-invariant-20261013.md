# Paired span equal-fog invariant

Base eb45d2b4bb513a0c744db79215e856a00106e17c. Isolated source change only; canonical main is untouched. One renderer file; three small hunks in drawSpecularSpan50LA.

## Source evidence and change

The original paired group loop sets sourcePairFog50LA before each base and map-2 pass from two immutable material policy booleans. When the booleans are equal, all these writes set the same value. The loop does not switch shader programs or contain any other writer of the uniform. vehicle_shader_gl50cz.cpp:79–86 consumes that same uniform for both passes; its numeric fog equation is unchanged.

The candidate computes the equality once. It keeps the original first-base write and its following existing check, skips subsequent base and map-2 fog writes only when policies are equal, and preserves every existing write when policies differ. Final restoration to zero is unchanged. This is a local invariant, not persistent state caching.

For a successful equal-policy span with G groups and map 2 enabled, fog setup calls inside the group loop go from 2G to 1; cleanup still performs its original reset. The source distance-skip path already wrote once and remains unchanged. This command-count observation is not profiling or a measured FPS improvement.

## Preservation proof

- Existing span/group/resource validation is unchanged, and a nonzero group count guarantees the first-base establishment occurs before any draw.
- The first base always sets its original fog value. For equal policies, that value is exactly the required second value, and remains unchanged for all later groups. For differing policies, both original per-pass assignments still execute in their original position.
- All check calls and placement remain unchanged, including base bind, complete group draw, second bind, bounded ordered second-group draws and final cleanup. Errors still reject before publication.
- Whole-group base draws followed by original ordered map-2 triangle draws are untouched. Every texture barrier remains immediately before its corresponding triangle draw.
- Texture/sampler binding, destination-alpha feedback, depth function/writes, color mask, alpha test, coverage, sourcePairPass selection, shader math and final restore/unbind are untouched.
- No new GPU capability, framework, batching, arithmetic rearrangement, shader change, allocation, persistent state or owner change is added.

Root has a separate lower-engine admission packet that also touches this renderer file. Integrate these span-only hunks; do not replace a newer whole renderer with this base-derived postimage.

Validation: source reads and manual diff review only. No builds, tests, syntax checks, scanners, profiling, asset decoders, runtime execution or user-PC work. No bottleneck or regression-causality claim. Independent source review completed with no blocking findings; see INDEPENDENT-REVIEW.md.

# Independent source review

No blocking findings.

- The pre file matches vehicle_window_gl50ds.cpp at eb45d2b4bb513a0c744db79215e856a00106e17c. The entire diff contains only the proposed three changes inside drawSpecularSpan50LA.
- First-group establishment is sound: the first base enters the bind block and writes sourcePairFog50LA before the existing bind check and draw. For equal policies, nothing inside the remaining group loop changes that uniform.
- Different-policy paired draws retain every original base and second-pass fog write. The distance-skip/base-only path retains its existing first-base write and behavior.
- Every error check, failure rejection, group ordering, per-triangle texture barrier, destination-alpha feedback, depth write/test, color mask, alpha test, and draw coverage remains unchanged.
- Cleanup still runs after loop failure, unbinds feedback, restores the base texture, resets pair/fog/alpha-test uniforms and depth writes, and returns accepted && restored.
- The unchanged shader applies the same fog calculation, source-alpha test, destination-alpha RGB accumulation, and independent map-2 alpha replacement.

For an equal-policy paired span with G groups, the rendering portion drops from 2G fog writes to one; cleanup remains unchanged. No builds, tests, syntax checks, scanners, profiling, runtime execution, asset decoding, or repository modifications were performed.
