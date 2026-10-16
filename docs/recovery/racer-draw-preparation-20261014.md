# Native racer draw preparation: retain the checked position result

## Scope and evidence

The supplied Windows instrumented intervals were 7.644 ms for seven-opponent
preparation, approximately 0.97 ms for player projection, 13.254 ms total render,
and 4.572 ms driver submission. All included countdown. These identify native
draw preparation as a worthwhile target; they do not measure this change's gain.

Only `vehicle_draw50cm.cpp` and its contract comment in `vehicle_draw50cm.h`
change. No AI, scene producer, packet producer, material program, normal
transform, shader, driver, original guest instruction, or public rigid projector
changes. No build, test, syntax check, sanitizer, gameplay run, executable run,
or commit was performed for this change. Integration and verification remain
with the owner of the central checkpoint.

## Removed work

`projectRigidVertices49S` in `src/game/rigid_vertices49s.cpp` first validates
every matrix, every vertex and every projected coordinate. Its first vertex
pass calculates a complete `RigidPosition49S checked`, then discards it. A
second vertex pass repeats the same arithmetic to publish positions, preserving
that public function's promise to leave its output untouched on failure.

`buildVehicleDraw50CM` previously passed `workspace.positions` to that public
transactional projector. The draw's own public contract already allows workspace
mutation on failure. Its real destinations and submission descriptor are not
published until all positions, colors, optional normals and optional specular
geometry have succeeded.

The new anonymous-namespace `projectDrawPositions` retains each complete checked
position directly in that disposable scratch. Successful preparation performs
one checked projection per vertex instead of two. Per vertex, the removed
second projection contains nine binary64 multiplies, twelve binary64 adds,
three binary64-to-binary32 conversions, and their intermediate/range checks.
All checks that determine admission remain in the first and only pass. There
is no new allocation, cache, cross-frame readiness or storage requirement.

## Numeric and rejection equivalence

The local implementation follows the public projector's first pass in the same
order:

1. Initialize the nested rigid diagnostic to `none, 0, 0, 0`.
2. Admit pointers, input/palette/output limits, output capacity and FE_TONEAREST.
3. For every palette entry in order, check all sixteen lanes for finiteness,
   then affine lanes 3, 7, 11 and 15. Unused palette entries remain validated.
4. For each vertex in order, admit its palette index and finite local XYZ.
5. For axes 0, 1, 2, start double `sum = 0`, form each product for k = 0, 1, 2,
   check it, add it separately to sum, and check the sum. Promote the same
   binary32 matrix coefficients to double. Add the same translation separately,
   check finiteness and +/-FLT_MAX, then convert once to float.
6. Publish only a complete three-axis result into scratch and continue.

The error enum, failed vertex, matrix and lane remain identical to the old
first pass. A failure returns immediately through the unchanged outer
`VehicleDrawError50CM::vertices` path. Signed-zero behavior and intermediate
rounding follow the same operations. IEEE-width assertions and the existing
strict floating-point build requirements remain necessary; reassociation or
contraction is not an allowed optimization.

The old successful second pass consumed the same stable, distinct input and
palette, with no callback or intervening FP-mode change. Consequently its
position result equals the retained first-pass result. Neither path clears FP
exception flags; repeating identical successful operations does not add a new
kind of sticky exception flag. There is no support added for trapping FP modes.

## Transaction and alias proof

The header explicitly requires the input descriptors/backing arrays, destination
vertices, output, workspace and diagnostic to be distinct live storage with
stable lifetimes; serial access is required. Therefore writing an earlier
`workspace.positions[i]` cannot change a later local vertex or matrix. A caller
that aliases those records is outside both the old and new contract.

On position rejection, some earlier scratch positions may now differ; scratch
mutation was already permitted. Destination vertex bytes, the optional
destination normal stream and the prior submission stay unchanged. All later
fallible stages remain before the first destination write. A retry on the same
workspace computes every active position again before consumption; a rejected
prefix is never accepted as ready. No pointer to positions is retained in the
completed submission: positions are copied into the destination vertices only
after every fallible stage succeeds.

## Palette snapshot semantics

Both existing scene snapshots are deliberately retained. Draw preparation
copies `state.nodes` into `workspace.nodes` for the admitted active scene count.
`projectModelPalette49S` validates that graph, clones it into
`paletteWorkspace.nodes`, and runs lazy world getters there. On success it
copies refreshed worlds/flags back to the supplied graph, which is the draw's
private snapshot, then publishes its palette.

Removing the outer snapshot would let successful palette preparation modify
canonical frame caches. Removing the inner snapshot would violate the palette
projector's own failure transaction. Ready-world cache semantics, dirty-root
refresh, every stored-order palette getter including repeated IDs, and test
instrumentation callbacks remain unchanged. Current packet matrix selection,
root/node transforms and specular-bound inputs retain the same producer order.

## Verification requested at integration

Compare successful destination vertices and bounds byte-for-byte with the
unchanged public49S projector under strict FP for real roster meshes and for
finite affine scale/shear/translation matrices, including signed zeros. Compare
nested diagnostics and unchanged public outputs on invalid palette indices,
nonfinite matrices/locals, nonaffine matrices, overflowing products/sums and
out-of-float-range results, including failures after an accepted prefix.
Retain scene-cache immutability, source color/alpha, normals, packet receipts,
specular geometry and callback-order coverage. Reprofile the same Windows
countdown-inclusive cut before claiming a timing improvement.

The local numeric implementation is intentionally confined to the assigned
files. Its maintenance obligation is to stay aligned with the public49S
projector's admission and arithmetic; a future shared internal checked kernel
could remove this duplication if changes to the rigid projector files are
authorized and verified centrally.
