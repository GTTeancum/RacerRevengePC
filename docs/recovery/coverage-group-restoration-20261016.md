# Native coverage restoration owned by the actual MSCNT group

Base6027d02. Source-only change; no compilation, syntax checking, tests,
gameplay, profiler or execution-based performance claim.

## Cost and bounded scope

The existing4700cce capture's last frame reports5174 bridge runs for5253
surviving map-2 triangles. Each bridge currently restores the source program,
pass2, source VAO, scene FBO, AA0 and depth/scissor state. The next bridge
immediately installs its own payload program/pass3, raster FBO and no-depth
state. No callback or external GL consumer runs between these owned bridge
calls in drawCoveragePairGroup. CPU rectangle lookahead reads only frame data.

This change moves the redundant restoration state to the actual group boundary.
The source program is still rebound and checked per run before any subsequent
source uniform write; the redundant payload-entry program bind is removed.
The existing max14 disjoint-rectangle admission, triangle order, MSCNT boundary,
material, byte arithmetic, payload/depth writes, destination snapshot and
sample-specific composition are unchanged. Every payload explicitly binds the
actual group-entry VAO, so it cannot inherit the preceding composition VAO.

The removed tail has14 GL calls: framebuffer, program, pass uniform, VAO,
six coverage disables, three depth calls and scissor disable. The source
program bind remains once per run, replacing the old identical
payload-entry bind. One VAO bind is retained at each payload entry and the
full14-call restoration occurs once per nonempty group. For R runs across G reached groups the net call reduction
is13R-14G, excluding error polls. One additional program-bind poll per group
prevents writing a source uniform into a wrong program after a failed restore.
This is an API-count statement, not a measured speedup. Singleton
groups gain one explicit VAO bind; multiple-run groups avoid redundant state.

## Failure and empty paths

All payload-bind, draw, blit, composition-bind and composition-draw checks remain.
Per-run unit2 and unit1 feedback unbinds remain, followed by their own check,
before another private raster attachment becomes writable. Active unit0 and
the actual specular image are restored per run as before.

After any reached bridge rejection, the group exits its loop and executes the
same final source state restoration before returning false. The final restore
also has its own check, regardless of the prior failure. The caller retains
its existing material reset and unpublished-frame abort behavior. No failing
bridge increments accepted-run diagnostics.

An entirely clipped/empty group never changes GL state and returns without
restoration. An initial VAO-query failure also returns before any mutation.
After the first reached bridge there are no early returns bypassing restoration.
If remaining triangles are empty, the final restore still runs.

## Rejected alternative

Exact shared-edge max-two batching remains unimplemented. OpenGL3.2 core
section3.6.1 supports unique center-sample ownership across identical shared
endpoints, but the current GLSL150 shader admission does not establish a
portable ULP bound for CPU prediction of transformed/snap-side orientation.
Existing heuristic projection margins are not promoted into such a proof.
This patch makes no new geometric or numeric approximation.
