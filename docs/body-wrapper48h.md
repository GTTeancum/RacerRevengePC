# Native root-body update, 48H

`updateRootBody48H` connects the recovered rigid-body integrator to original scene readback, quaternion reconstruction, rotation/position corrections, velocity caches, clearing and the observed final body callback. Production is portable C++17 over typed native structures. No PS2 instruction execution is linked into the production function.

The entry is original `167C40..16836C`. The numerical root path and every helper execute as original instruction words in the independent test oracle. The snapshot body at `D110A0` has no component children (`+18=0`, `+24=0`); its scene at `CFD8F0` has no parent (`+180=0`) and no spatial observer (`+19C=0`). The scene does have a render child notification graph at `+184`; tests suppress that graph while retaining the actual root numerical setters and cache refresh instructions. Native scene propagation remains the caller's responsibility. This is not the complete vehicle scheduler, collision response or renderer notification system.

## Preserved order

1. The original base component update precedes the numerical phase. The native caller must update any attached native components first.
2. A missing scene (`body+288`) or disabled body (`+204`) jumps directly to clearing and the final callback. It does **not** preserve accumulated loads.
3. Save velocity `+70` into previous velocity `+90`. Transform center offset `+1E0` by the current scene into position `+60`. Reconstruct quaternion `+1F0` from the scene matrix; any prior body's position/orientation fields are overwritten here.
4. Run the recovered original integration with its Adams–Bashforth histories, axis locks, limits and quaternion normalization.
5. Convert the integrated quaternion to a matrix and assign the **whole** root scene matrix through `289B20`. This resets its translation to zero. Keeping the previous scene translation here would move the pod incorrectly.
6. If correction `+1B0` is nonzero, transform it by the new orientation, compute its length, normalize, and call the original axis rotation operation. This uses the recovered `13BAD0`/`13BBA0` polynomial sine/cosine functions, not standard-library trigonometry. `112B20` normalizes the axis again using VU sqrt-then-divide behavior, constructs the rotation matrix, and composes it in the original order.
7. Transform the center offset again. Set stored position to integrated position plus `(correction1A0 - transformedCenter)`, then use the original relative translation operation. Because step 5 cleared translation, this establishes the corrected scene position. The body's stored quaternion intentionally remains the pre-correction integration output; the next update reconstructs it from the scene.
8. Refresh local velocity `+80` from world velocity using the updated orientation and world angular velocity `+A0` from local angular velocity `+B0`. The original final zero-add and signed-zero behavior are preserved.
9. Clear force `+180`, torque `+190`, positional/rotational corrections `+1A0/+1B0` and accumulators `+1C0/+1D0`. The captured body vtable `3CE000` slot `9C` targets `1654B0`, whose actual operation clears field `+238`. This callback is included explicitly, not assumed to do nothing.

The API exposes meaningful XYZ values and all 16 matrix words. It does not expose guest padding lanes, pointers or cache-valid bits. `body.position` after return is the original post-correction stored position; it must not be treated as the next call's unconditionally valid world center. Failure due to nonfinite consumed data or arithmetic overflow leaves output unchanged, including aliasing. Disabled/missing-scene calls clear the original fields without attempting integration.

## Verification

`tools/gameplay/body_wrapper_recover48h.py` verifies the original ELF and stopped RAM SHA-256, extracts exact instruction blocks, validates generated instruction comments against ELF words, and records source hashes. `evidence/bodyWrapper48H/recovery.json` contains addresses, hashes and actual saved graph/callback identities. The new oracle extends the frozen previous opcode interpreters; it executes actual numerical helpers and the final callback to the original return sentinel.

`tests/native/body_wrapper48h.cpp` compares all 80 exposed 32-bit words, tests output aliasing, the actual stopped-state fixture, randomized loads and histories, axis locks, zero mass, acceleration/velocity caps, disabled/missing scenes, zero elapsed time, four host rounding modes, nonorthogonal scene input and large rotation corrections. A 180-call chain retains original memory, register and scene-cache state between calls; only external force/torque inputs are refreshed. Original state is not reset from native output. Eight consequential branch gates must execute.

Preserved initial failures exposed the disabled-state clearing branch, the final direction zero-add and rotation-matrix signed-zero lanes under downward rounding. Those failures were fixed against original operations; comparisons remain bit exact. The final checkpoint records actual cross-architecture counts and build hashes.

Finite ordinary arithmetic and explicitly exercised zero/underflow cases are covered. This is not proof of every PS2 floating-point exception, denormal or VU precision mode. The original game scene graph, second body, spring impulses/friction, physical action timing and full vehicle substep still require integration and verification. No native racing FPS or Xbox performance is inferred from these component tests.
