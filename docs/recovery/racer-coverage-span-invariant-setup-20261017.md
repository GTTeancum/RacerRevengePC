# Invariant coverage setup within one source-pair span

Status: source-reviewed optimization; UNTESTED. No build, syntax check, test,
gameplay run or benchmark accompanies this change. Performance benefit is
unmeasured.

## Two separate scopes

`drawSpecularSpan50LA` now owns a local `firstCoverageSetup` flag, initially true.
It passes the flag by reference through `drawCoveragePairGroup` and by value to
each bridge. Only a successful nonempty bridge clears it. The flag expires when
the span returns, including every failure path.

The existing `firstRun` flag remains local to each MSCNT group. It still controls
the group's initial source-program bind, payload unit/image setup and depth
mask. Those operations retain their current frequency. Source base/map-2 mode,
fog, alpha-test and depth transitions are unchanged.

The new span flag controls only setup whose values are invariant between groups:
- Five disabled sample policies: alpha-to-coverage, alpha-to-one, sample
  coverage, sample mask and sample shading.
- Disabled framebuffer sRGB and blend.
- The frame viewport and all-enabled color mask.
- The private coverage-composition mode 0.
- Sampler object 0 on units 0, 1 and 2.

The initial `coverageAa0` call also disables MULTISAMPLE. Composition still
enables it for every run, and the existing checked successful tail disables it
for every run. Every rejected bridge retains the original full
`coverageAa0` cleanup retry.

## Writer inventory

Between groups, the sole caller submits the original base group and its checked
map-2 setup. These change source textures, source-program uniforms and depth
state, all of which retain their existing setup. They do not write the proposed
invariant state. No callback, nested RA compositor, resize, program relink or
external state owner runs inside the span. `check` only queries GL error and
records a diagnostic.

Within bridges, the private compositor uses the same mode and sampler objects.
It changes texture bindings, programs, VAOs, FBOs, scissor, depth and MULTISAMPLE;
all of those required transitions and their existing restorations remain.
Successful tails and CPU region lookahead do not change the invariant values.
The other writer of coverage-composition mode is the separate RA world path,
which cannot run within a racer span.

This change does not defer group or per-run restoration. Every original
restoration statement and error check remains. It only extends the scope of the
listed immutable setup, while keeping mutable setup group-local.

## Empty and failure paths

Zero-count groups do not enter the loop and leave the span flag untouched.
All-empty groups likewise issue no bridge and retain its prior value. If the
first groups are empty, the first later nonempty bridge still performs complete
initial setup.

The group rejects empty regions before calling the bridge. Its union always
contains the admitted nonempty initial rectangle. The span flag therefore
advances only after a known nonempty bridge returns `accepted && restored`.
Errors before the bridge, or in setup, raster, copy, composition or restoration,
return before the flag update and prevent another group. A later group-level
error also terminates the span; no success flag survives to a future span.

An unpaired or distance-skipped span never calls a coverage group. The
single-sample path does not use the new flag. Original group/triangle ordering,
conservative clipping, disjoint rectangle admission, thresholds, scratch cache
scope, draw counts and all publication/abort boundaries are unchanged.

## Cost and limitations

For each additional nonempty MSCNT group after the first in a span, the new
scope avoids eleven mandatory GL state/uniform writes and up to three sampler
binds. It adds one stack-local Boolean and passes that bounded scope explicitly.
No state is reused across spans, materials, components, callbacks or frames.

No raster draw, destination copy or error check is removed. Driver-time and
frame-rate effects are unknown. This must not be presented as measured speedup.

The draft is based on vehicle renderer blob
`b383fb68a0cd484113744bdb933c3b63bd344f8d` and coverage helper blob
`8ce5eb5cb2c6c01dd2ab8919edad6e91bd08663e`.
The span's feedback comment is also corrected: only the single-sample path
repeats the later-base unbind; coverage retains its checked bridge-tail unbind.
