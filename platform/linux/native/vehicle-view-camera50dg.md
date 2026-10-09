# Explicit diagnostic-view camera 50DG

Status: **source-only, uncompiled, untested and unexecuted**. No build, compiler,
test, executable, device/context probe or validation script was run. No existing
source, build registry or test was edited. This establishes no rendered pixels,
original-game camera parity, acceleration, presentation or FPS evidence.

## Scope and evidence

`vehicle_view_camera50dg.{h,cpp}` adds one allocation-free numeric adapter for
the Linux 50DD offscreen compatibility viewer. It follows the inspection-viewer
conventions read in `platform/windows/native/main48b.cpp`: `XMMatrixLookAtRH`,
`XMMatrixPerspectiveFovRH`, row-vector `view * projection`, and the HLSL
`mul(float4(input.position, 1.0), worldViewProjection)` shader. The Windows
`XMMatrixTranspose` before `XMStoreFloat4x4` supplies column-major constant-buffer
packing; it does not change that row-vector convention.

50CZ uses `vec4(rrPosition, 1.0) * worldViewProjection`; 50DD uploads the provided
matrix with `glUniformMatrix4fv(..., 1, GL_FALSE, ...)`. This adapter produces the
complete matching row-vector world-to-clip matrix in column-major storage. Its
output goes directly into `VehicleOffscreenFrame50DD::worldViewProjection`.
Do not transpose it again or append another depth-conversion matrix.

This is explicitly **diagnostic viewer math**, not the recovered original 49F
game camera. `src/game/camera_view49f.{h,cpp}` retains its separate source-ordered
binary32 numeric leaves, original scene/look/up/FOV and aspect contracts, and
pre-PS2-viewport output. 50DG neither calls nor changes 49F and makes no recovered
camera, original viewport, controller-selection or vehicle-follow claim.

## Explicit input and exact conventions

Fill every `VehicleViewCamera50DG` field: finite three-component eye, target and
up, vertical FOV in radians, width/height aspect, and positive finite near/far
distances with far greater than near. FOV must be strictly between zero and pi.
No valid defaults, viewport-derived aspect, bounds-derived clipping, orbit,
vehicle-follow, axis fallback or hidden root/model transform are supplied.

The right-handed view basis is:

- Backward Z = normalized `(eye - target)`; view-forward is negative Z
- Right X = normalized `up cross (eye - target)`
- Corrected Y = normalized `backward cross right`
- View translation = `(-dot(right, eye), -dot(corrected Y, eye), -dot(backward, eye))`

The input up vector need not already be normalized or orthogonal to view. Its
perpendicular component fixes the diagnostic roll orientation. A zero up or
zero cross product rejects, rather than choosing an arbitrary replacement axis.
No angle epsilon or near-parallel fallback is introduced.

For row vectors, projection has `P[0][0] = cot(FOV/2)/aspect`,
`P[1][1] = cot(FOV/2)`, `P[2][2] = -(far+near)/(far-near)`,
`P[2][3] = -1`, `P[3][2] = -2*near*far/(far-near)`, with other entries zero.
Thus GL clip depth is `-W <= Z <= +W`: view Z `-near` maps to NDC Z `-1` and
`-far` maps to `+1`. This is the algebraic D3D-to-GL conversion
`clipZ_GL = 2*clipZ_D3D - clipW_D3D`, with X, Y and W unchanged. These are analytic
conventions, not an executed endpoint test. There is no Y flip, reversed depth,
infinite-far mode or PS2 screen mapping. 50DD retains its GL bottom-row-first
readback orientation; image encoding or flipping remains the caller's decision.

The result is `M = V * P`, stored at `worldViewProjection[column * 4 + row]`.
50CV's positions are already world-space, so applying the vehicle root again
would be incorrect. The caller chooses an aspect appropriate to the actual
50DD viewport; this function never reads a renderer or its dimensions.

## Failure, ownership and integration

`composeVehicleViewCamera50DG(input, output, error)` returns false for nonfinite
input, invalid projection, coincident eye/target, zero/parallel up, or a matrix
coefficient that is nonfinite, exceeds float range, or is nonzero but rounds to
zero. The output remains unchanged on every failure. The small error enum resets
for each call and distinguishes these categories; `matrixRange` covers numeric
admission after basis construction. Near-parallel finite inputs are not silently
reoriented. Standard double intermediates reduce range loss; they do not emulate
DirectX bit patterns or original PS2 instruction/rounding behavior.

The function has no heap allocation, mutable global state, retained pointer,
context, resource ownership or lifetime hooks. Input, output and error must be
separate live objects. Calls with disjoint objects are independent. After a
successful call, set the 50DD frame's matrix pointer to
`output.worldViewProjection`, provide the separate explicit clear color, and
keep those values alive and unchanged through `submit`. 50DD copies the sixteen
matrix floats before submission begins, so the input camera is not borrowed
and the output need not survive a completed submission call.

A future authorized caller must compile the new `.cpp` alongside its chosen
50DD client; the camera itself needs only the C++ standard math/limits headers
and has no GL, EGL or DirectX dependency. Existing build/launch paths are
unchanged. Root-owned verification/integration remains pending; this source
addition consumed no test slot. Protected 49W/48U/49C work, `ee.bin`, game assets
and the user's PC were not accessed.
