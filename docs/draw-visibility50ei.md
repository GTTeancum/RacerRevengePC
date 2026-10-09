# Optional static visual draw visibility 50EI

Status: **source only, uncompiled and untested**. No build, test, runtime probe,
validation script or performance measurement was run for this addition. No
existing renderer, build registry, physics or registration source is changed.

50EI is an optional host-side rendering optimization. It is not recovered
original culling policy and does not establish original-game visibility parity,
playability or a console frame-rate result. It only filters an explicitly
supplied list of actual static visual draws.

## Caller contract

Add `src/game/draw_visibility50ei.cpp` to the chosen target when integrating.
`collectVisibleStaticDraws50EI(input, buffer, result, diagnostic)` consumes:

- A source-order array of `StaticDrawBounds50EI`, each carrying a caller draw ID
  and finite, ordered world-space `assets::Bounds48B`. IDs are opaque and can
  repeat. Bounds must enclose the actual submitted geometry after all placement
  and any shader deformation; collision proxies are not visual bounds.
- The exact current world-to-GL-clip matrix used for those draws. This is a
  row-vector transform in column-major storage, matching 50CZ/50DS and the
  explicit outputs of 50DG/50DX. For column `c`,
  `clip[c] = x*m[4*c] + y*m[4*c+1] + z*m[4*c+2] + m[4*c+3]`.
  There is no transpose, model transform, camera selection or depth remap here.
- An explicit `DrawClipDepth50EI::negativeOneToOne` or `zeroToOne` selection.
  The former admits `-W <= Z <= W`, the latter `0 <= Z <= W`. Both use
  `-W <= X,Y <= W`. The receiving renderer must actually use this convention;
  zero-to-one GL requires caller-established clip-control support/state.
- Caller-owned `uint32_t` output storage and its capacity in elements. Allocate
  once for the maximum source draw count to guarantee room for full retention.
  Smaller buffers are allowed but may reject a frame with `capacity`.

Use the current camera and current visual bounds. Do not reuse a list across
camera changes, substitute a stale producer result, or use bounds of unplaced
local geometry. The renderer must use standard homogeneous six-plane clipping;
depth clamp or other state that disables a plane invalidates this contract.
The helper does not touch GL state or execute draws.

## Conservative decision

All eight corners are transformed without perspective division. Each corner
receives a bit for every clip plane it is definitely outside. A draw is culled
only if one common bit survives across all eight corners. The signed plane
functions are affine over the world-space box, so a common strictly outside
plane excludes the entire enclosed geometry. Different corners outside
different planes are insufficient evidence. No positive-W shortcut is used:
boxes crossing the eye or a clip plane are handled by the same homogeneous rule.

Dot products use binary64 intermediates over supplied binary32 inputs. Each
coordinate carries `16 * float_epsilon * sum(abs(dot_terms)) + 16 * float_min`
as an outward uncertainty allowance. Plane checks combine the corresponding
coordinate allowances. This deliberately exceeds ordinary four-term binary32
dot-product roundoff and includes tiny intermediate/result flush-to-zero error.
Touching or roundoff-ambiguous boxes remain in the list. This is host rendering
math, not an attempt to reproduce PS2 instruction arithmetic.

The supported arithmetic contract is IEEE binary32/binary64, nearest-even host
rounding and no fast math. Non-nearest rounding and `__FAST_MATH__` builds reject
with `unsupportedArithmetic`. Other toolchains must also compile this source
without unsafe floating-point assumptions. Actual shader coordinates must be
produced by the supplied ordinary binary32 matrix transform, not lower-precision
or approximate replacements beyond the allowance.

Subnormal input bounds/matrix coefficients are treated as uncertain rather than
assuming whether a CPU/GPU flushes operands. A nonfinite transformed coordinate
or sum of absolute terms too large to rule out binary32 intermediate overflow
also makes the whole draw uncertain. Even apparently finite cancellation cannot
justify a cull in that case. `DrawUncertainPolicy50EI::retain` includes the draw
and increments `uncertainRetained`; `reject` returns `uncertainTransform` at its
input ordinal. Ordinary boundary overlap is retained under either policy.
Nonfinite matrix inputs, nonfinite or reversed bounds, missing storage and
unknown enum tags are explicit errors under both policies.

## Ordering, capacity and failure

Successful output contains precisely the retained draw IDs in source order,
including duplicates. `result.retained + result.culled` equals the input count;
`uncertainRetained` is a subset of retained. Entries beyond the returned count
are untouched. A fully culled list can succeed with null storage and zero
capacity; an empty input can use a null draw pointer but still requires a valid
matrix and explicit depth/policy. Nonzero output capacity requires storage.

On failure, the result descriptor is unchanged and the diagnostic identifies
the error and input ordinal (or `UINT32_MAX` for call-wide errors). Output array
bytes already written may have changed. Never submit that partial prefix or a
stale result count: use an inactive staging array and publish only after true.
The caller may instead elect to submit the full original list after a failure;
the helper never silently publishes that fallback. Input arrays, matrix, output
array, result and diagnostic must have valid, stable, mutually disjoint storage.

The implementation is one source-order pass with eight corner transforms per
ordinary draw, constant stack space, no heap allocation, callbacks, sorting or
persistent cache. It does not extract triangles or re-upload static resources.
Integrate immediately before static visual draw submission and use returned IDs
to reference existing draw records; preserve their material and geometry data.
Keep animated vehicle rendering and all physical queries/registration outside
this optional filter. No collision hit/miss, world membership or race-state
result may be inferred from its retained or culled lists.

The next authorized verification should cover each of six outside planes,
both depth conventions, exact plane contact, crossing/eye-straddling boxes,
different outside planes with no common rejection, source order/duplicate IDs,
empty/all-culled lists, capacity failure after a written prefix, malformed bounds
and both uncertain-arithmetic policies. These checks have not been run here.
