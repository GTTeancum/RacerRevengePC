# Original core camera to Linux GL clip matrix 50DX

Status: **source-only, uncompiled, untested and unexecuted**. No build, compiler,
test, validation script, runtime/device probe or rendered capture was run for
this addition. Only the new 50DX header, implementation and this document are
owned by this change. Existing camera logic, runner, build wiring and evidence
are unchanged. Root-owned integration and authorized verification remain pending.

## Delivered boundary

`platform/linux/native/native_camera_view50dx.{h,cpp}` consumes an already
successful `game::CameraViewOutput49F`. It supplies the exact matrix convention
required by `vehicle_shader_gl50cz`: a row-vector world-to-clip transform stored
column-major for `glUniformMatrix4fv(..., 1, GL_FALSE, ...)`.

The original perspective projection **is recovered**. No external replacement
projection, diagnostic look-at, fixed vehicle offset, host tangent, aspect guess
or view-times-projection recomputation is needed here. The adapter uses 49F's
already rounded `combined` matrix. Its sole numeric change, when requested, is
the explicit GL clip-depth conversion after that original computation.

The function does not produce a camera frame, choose TV/follow mode, select an
authored camera, consume RNG, resolve a player scene, create ordered vehicle
samples, or run a full-frame scheduler. Existing 50DG diagnostic camera behavior
is not silently replaced in any runner.

## Source chain

- `src/assets/tv_camera49e.{h,cpp}` loads caller-owned authored camera records and
  ordered links. `docs/tv-camera49e.md` and the retained independent
  `evidence/tvCamera49E/loader-source/authored_projection49e.h` document the actual
  TA configuration: 19 records, 20 links and source-derived numeric words. Those
  evidence tables are not compiled into 50DX or used as runtime camera defaults.
- `src/game/tv_camera_update49f.{h,cpp}` consumes already-selected body-attached
  TV state, resolved player pose, the complete original manager-order scene
  samples and shared RNG at TV-update entry. It produces camera pose and FOV.
- `src/game/camera_frame49f.{h,cpp}` carries prior Look/Up, calls that TV update,
  then the source-recovered matrix consumer. It rejects the unsupported root
  pose-override and scene matrix-build-suppressed bit-2 contexts before updating.
- `src/game/camera_view49f.{h,cpp}` implements original view `14DC80`, roll
  `14E070`, projection `14DB90`, and source-order `V * P`. Details and retained
  source references are in `docs/camera-view49f.md` and
  `evidence/cameraView49F/consumer-source/consumer-audit.md`.
- `platform/linux/native/vehicle_shader_gl50cz.cpp` evaluates
  `vec4(rrPosition, 1.0) * worldViewProjection`. Both 50DD and 50DS upload sixteen
  caller floats with `GL_FALSE`. Thus 49F's flat row-major bytes must be repacked
  for this shader's row-vector multiply.

This differs from the earlier 49G Python renderer, which uploaded row-major
native bytes unchanged and used a **column-vector** GLSL multiply. That paired
transpose convention cannot be copied into 50CZ. The old path also explicitly
selected GL zero-to-one clip control; the current 50DD/50DS renderers do not.

## Explicit provenance and output contract

Call `convertNativeCameraView50DX(source, contract, output, error)` only after
a successful `composeCameraView49F` or `updateTVCameraView49F` call has produced
the current source. The contract requires all three:

1. `provenance = NativeCameraViewProvenance50DX::currentCore49FForOutput`.
   This explicitly attests that the complete current source matrices came from
   that successful producer, before PS2 viewport/screen adjustment, using
   source-supplied scene projection state for the declared output. A composed
   game-frame caller must also establish 49F's ordinary/build-enabled gates.
2. Positive `width` and `height` in actual drawable pixels. Logical SDL window
   units are insufficient. These dimensions identify the output for which the
   caller supplied the projection; they do not generate or alter aspect.
3. `clipDepth = negativeOneToOne` or `zeroToOne`, explicitly matching the
   receiving GL context. Existing 50DD/50DS clients use `negativeOneToOne`.
   `zeroToOne` is only appropriate for a caller that has separately established
   the required GL clip-control support and state.

The default provenance, zero dimensions and unspecified clip depth all reject.
Unknown enum values reject too. Tags are caller attestations, not authentication
or proof that missing producers, scheduler operations or queries ran. Structural
checks reject incompatible core matrix layouts, including a default all-zero
49F output; they do not prove consistency between arbitrarily supplied arrays.

Crucially, `CameraSceneState49F::aspect1F8 = 1` is a native placeholder, not a
recovered viewport initializer. The original `114960` relation is:

    aspect1F8 = (viewportHeight / viewportWidth)
              / (scene1DC * (fullHeight / fullWidth))

The exact source field must be supplied to 49F with its own provenance. Output
width/height alone cannot recover `scene1DC`, the original full-output ratio,
or the original sequencing and rounding of their production. 50DX never
substitutes desktop width/height or height/width for that field. The historical
49G fixture's explicit aspect 0.75 and its explicit camera/player placement are
diagnostic inputs, not a default for the new runner.

A failed 49F call leaves its previous output intact. Such a retained matrix
must not be tagged current-successful for the attempted frame. Likewise,
resize requires the caller to re-establish the projection/output contract;
50DX never stretches or silently refits an earlier camera.

## Matrix and depth conventions

49F stores each row contiguously. Its core view uses positive view Z for the
Look direction and forms right as `Up cross Look`; 50DX preserves the complete
source basis, translation and roll without re-normalization or handedness change.

With the original tangent-derived scale `h` and separately rounded
`z = 1 / (1 - near/far)`, the nonzero projection entries are:

    P[0,0] = h
    P[1,1] = h * (1/aspect1F8)
    P[2,2] = z
    P[2,3] = 1
    P[3,2] = -z * near

All others are zero. This yields `clipW = viewZ` and the native zero-to-one
depth convention for ordinary positive near/far inputs. The existing 49F
finite arithmetic domain also permits unusual FOV, near/far and basis inputs;
50DX does not introduce new clamps, epsilon tests or physical-camera repairs.

Let `C` be the already-rounded native `combined` matrix. For zero-to-one GL:

    output[column * 4 + row] = C[row * 4 + column]

Each coefficient is copied bit-for-bit, including signed zeros. For default
negative-one-to-one GL, only mathematical column 2 changes:

    twiceZ = round_binary32(2 * C[row,2])
    output[2 * 4 + row] = round_binary32(twiceZ - C[row,3])

This is the host clip-space adaptation `Z_GL = 2*Z_native - W_native`. X, Y and
W keep their exact native bits. There is no additional Y flip, PS2 screen map,
root/model transform, or depth remap. Algebraically near/far change from 0/1 to
-1/+1; finite coefficient rounding is explicitly retained, so this is not a
claim of bit-identical per-vertex arithmetic or exact numerical endpoints.
Converting P first and multiplying V again would alter original rounding and
is deliberately avoided.

## Failure and integration

Errors distinguish missing provenance, invalid output size/depth, non-nearest
rounding, nonfinite matrices, incompatible core layout and conversion overflow.
The source view must be affine in its last column, with bottom-right 1, and
projection must retain the original zero pattern and `P[2,3] = 1`. All 48
source matrix coefficients must be finite. These checks are native admission,
not newly claimed branches in original game code.

Negative-one-to-one conversion separately checks both the doubled coefficient
and the subtraction result. An overflowing intermediate rejects even if a
double-precision alternative could later produce a finite value. IEEE binary32,
nearest-even rounding, no fast math and no contraction are required. Build the
new source with the existing strict flags (`-fno-fast-math -frounding-math
-ffp-contract=off`). No source trigonometry or V*P product runs twice.

The implementation has fixed loops over sixteen coefficients, one fixed local
result, no heap allocation, no global mutable state and no GL dependency. The
four arguments must be distinct live objects. Source and contract remain
unchanged; output remains unchanged on failure, while error describes that call.

On success, use `output.worldViewProjection` directly for the 50DD/50DS frame
matrix, and use its width/height for that frame's viewport. Ensure the context's
depth convention matches `output.clipDepth`. Keep the result alive and unchanged
through submission; those renderers copy the matrix before drawing. Vertices
already contain world transforms, so do not append the vehicle root again.

A future integration must add this `.cpp` to the chosen Linux target and connect
only a genuinely successful current native camera producer. The conversion adds
no out-of-line dependency on 49F by itself; its typed source is included from
the existing camera header. No build registry was modified in this task.

## Remaining blockers and verification limits

The projection formula is available; **live original camera scheduling and
viewport provenance remain caller boundaries**. In particular, selector input
uses local player position, TV update uses resolved world position, and the
game may consume an intervening camera-type RNG draw. Calling these in an
invented order or using a fake empty scene-manager list would not be a recovered
full frame. The separate follow-camera height/collision providers remain open
as documented in 49F. This adapter closes none of those dependencies by inference.

Source inspection establishes the intended packing and algebra only. Compilation,
bitwise transport/depth checks, rejection/atomicity tests, authored-camera runner
integration and rendered pixels remain unverified. Existing 49F evidence is
inherited evidence for its unchanged producer, not a new 50DX test pass. No test
slot was consumed. No protected 49C/48U source, `ee.bin`, game asset extraction,
user PC, install or network retrieval was used for this addition.
