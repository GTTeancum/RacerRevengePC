# Bounded host chase framing 50FN

`tests/native/host_chase_camera50fn_fixture.h` adds an optional development-host
presentation correction for steep or airborne poses that leave the cockpit or
an engine outside the existing chase camera. It is **not original game-camera
recovery** and makes no original-source parity claim.

## Input and behavior

The helper reads `VehicleDrawBatch50CO::component[0..2].bounds` from the current
successfully prepared draw batch. Those are world-space AABBs computed from the
actual cockpit and both engines' projected vertices by `buildVehicleDraw50CM`.
They include graphics-root response and animation. They are not physical body
positions, scene-local translations, collision spheres, or potentially stale
canonical world caches. In particular, the dormant secondary body's position
cannot be substituted for the rendered cockpit.

- When all 24 component-bound corners already pass the real GL clip matrix,
  every baseline camera field is copied unchanged and correction state resets.
- Otherwise, the camera keeps its bearing, pitch, up vector, FOV, aspect, and
  clipping planes. Eye and target translate together in the camera plane; they
  retreat together only as much as the framing inequalities require.
- Corrected framing targets NDC horizontal +/-0.90 and vertical +/-0.84,
  reserving a little edge/HUD space. These margins apply only after baseline
  framing fails; they do not zoom out an already framed normal view.
- Plane shift is capped independently at +/-96 world units along camera right
  and up. Retreat is capped at 192 world units and further constrained by the
  supplied far plane. These are explicit host choices, not recovered constants.
- The inequalities for the 24 corners are solved directly with fixed loops.
  No heap storage, original execution/emulation, scene mutation, vertex rewrite,
  callback, or frame-rate-dependent smoothing is added. State is three floats
  reporting the last successful correction; the next result is always computed
  from that frame's uncorrected baseline, so offsets cannot accumulate.

`framePod` returns false for an invalid camera, missing/invalid component bounds,
or an unrepresentable result, leaving output and state unchanged. On true,
`Diagnostic::framed` reports the final actual clip-matrix check; `limited` reports
that the bounds could not reach the requested fit within the fixed budget or
clipping planes. A limited result remains a valid bounded camera, not a promise
of complete visibility. Terrain occlusion, camera/terrain collision, transparent
effects, cable billboards, and HUD overlap are outside this helper's guarantee.
Component AABBs are conservative, so the correction can exceed the minimum
needed for the actual triangles. Immediate correction/release can produce a
camera step; preserving the normal view and avoiding lag take priority here.

## Interactive-client integration

1. Include `host_chase_camera50fn_fixture.h` and keep one `State` beside the
   existing host heading state, reset with each fresh session.
2. Move current CPU-bank selection, `drawAssets.batchInput`, and
   `buildVehicleDrawBatch50CO` above camera composition. Do not publish that
   bank early: renderer acceptance remains the publication boundary.
3. Construct the existing normal chase camera with its existing heading policy.
   Keep a separate baseline and corrected output object. Do not feed last
   frame's corrected output back as the next baseline. The framing decision
   must use the new batch, not `presenter.published` or physical-body spacing.
4. Call `framePod(baseline, next, state, camera, diagnostic)`. Reject false with
   the host's existing error path; optionally report `limited` and the three
   state offsets as host-only diagnostics. Do not treat true alone as proof
   that all components fit.
5. Compose `camera` with the existing 50DG composer. Generate cable billboards
   using this corrected camera's backward vector, then submit the already
   prepared batch and publish only after renderer acceptance.

This ordering does not change the simulation/graphics cut: draw preparation
copies the canonical graph and leaves its physics, scene caches, and RNG alone.
No native production module or linker source-list addition is needed; 50DG is
already linked by the interactive client. The helper itself reads no body or
scene pose and does not claim that the baseline's supplied follow policy is the
original controller.

## Verification scope

The integrating client completed one focused 22-second VM check, reporting
43.9–62.3 FPS, 430 adjusted frames, and zero limited samples. These are results
from that short run, not a sustained-performance or all-pose guarantee.

The `solo-camera50fn` JPEG captures show the cockpit in the normal view. At
boost 2 seconds and 5 seconds, terrain still occludes the cockpit even though
its bounds are framed; at boost 8 seconds the cockpit is visible sideways.
Thus the bounded frustum correction works in the checked run, but does **not**
solve terrain occlusion or the vehicle's attitude. `Diagnostic::framed` means
the submitted bounds pass the clip volume, not that their pixels are visible.

No additional tests were run for the helper-only change. Suggested synthetic
coverage remains an unchanged normal frame, a cockpit below the viewport, an
engine outside either side, near-plane straddling, a limit-exceeding separation,
invalid bounds/camera atomic rejection, and a second normal frame resetting
offsets. The short VM check does not establish that coverage.
