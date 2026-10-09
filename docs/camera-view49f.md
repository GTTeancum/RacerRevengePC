# 49F: genuine native TV-camera view and projection

## Delivered boundary

This milestone completes a numeric camera subsystem rather than stopping at
camera selection. Given an already-selected authentic camera record, resolved
player pose, full ordered scene samples, original random state and scene
projection parameters, it produces:

- body-attached TV camera position, target or authored Look/Up, and filtered FOV;
- source-faithful camera orientation, including equal-target and vertical cases;
- original core 4×4 view, perspective projection and combined matrices;
- updated TV pose/filter/shake state, scene orientation/FOV and shared RNG state.

Production files:
`src/game/tv_camera_update49f.h/.cpp`, `camera_view49f.h/.cpp`, and
`camera_frame49f.h/.cpp`. All are serial, typed, caller-owned and allocation-free.
No instruction interpreter, guest memory, hardware or opaque world callbacks run
in production. The retained 49E loader provides all 19 authentic TA camera records
and 20 ordered links. No 49C source or graph projection was copied.

The supplied scene list must be complete and in original manager order, including
inactive entries; array index is the remembered source index. Its local cached
positions and the player's resolved/global position are deliberately distinct
inputs. No visibility, collision, respawn or world-coordinate equivalence is invented.

## Why these are real game view matrices

Exact original ELF/caller evidence is retained under
`evidence/cameraView49F/{tv-source,consumer-source,follow-source}`.

Game function175D70 calls controller115CB0 at176DBC. Controller115CB0 first
uses scene getter115060 to seed position, Look, Up, roll and FOV, then invokes
the current camera's virtual+14. For TV vtable3D5160 that resolves to2D3400.
It passes the resulting vectors/FOV into scene setter114F90 and, in target mode,
target-orientation virtual1150C0. Scene update114AE0 calls builder115350.

The actual game scene overrides those virtuals. With global application+228
bit2 clear,2363B0 bypasses its input-driven pose-adjustment branch and reaches
base update114AE0. With scene+29 bit2 clear, that base invokes its matrix builder;
236DD0 calls base builder115350. `CameraViewContext49F` supplies both raw flags,
and the composed API rejects either unsupported bit2-set context before updates.
Other bits are not reinterpreted; the default zero context declares this ordinary
build-enabled scope, not an inferred global initialization. Thus the recovered matrices are genuinely
used by the game: view at scene+C0, projection at+80 and combined at+100.
Later PS2 viewport/screen matrices at+140/+250 and adjusted projections at
+290/+2D0 are separate rendering boundaries, not silently substituted here.
The scene's motion history, diagnostic scalars, counters and render stamp are
also outside this explicit numeric output. After core matrix construction the
base can perform optional world-registration at114C44→280D40 and later object
update141ED0; these callees are not entered or emulated. The result is the complete
numeric view/projection output before those world/rendering operations, not a
claim that every scene-update side effect or renderer invocation is recovered.

The top-level `updateTVCameraView49F` reproduces the numeric order: seed carried
Look/Up from prior scene, full TV update, then the real scene/matrix consumer.
It stages scalar state and commits only on complete supported success. That
atomic failure contract is native admission, not original partial-error behavior.

## Keep selection and mode choice separate

There is deliberately no implicit 49E selector call in 49F. The original selector
caller uses28B180's local player-position XYZ. The TV view base instead uses
28B1A0's resolved/global player-position XYZ. Parent transforms can make these
different. The game also consumes a separate RNG draw while choosing a camera
type between selection and the view update. Calling selection with the view's
pose and skipping that draw would be an invented full-game scheduler.

Supply already-selected49E state and the shared RNG at actual TV-update entry.
Tests explicitly select cameras separately and distinguish local selection input
from resolved view input. A supplied TV frame is a concrete subsystem invocation,
not a claim that every authored preference chooses TV rather than follow/other modes.

## TV pose, FOV and state ordering

The admitted complete branch is body-attached TV (`camera+20 != 0`), established
by original setter2D2090. Player getter28B1A0(node,0,...) writes XYZ into+60,
preserving W; body velocity128 is copied into+70. The non-body previous-position/
sentinel/displacement-over-time branch remains outside this adapter.

Base2D2170 computes target2D2430 (resolved player position plus targetYOffset in
Y), selected position2D3890 and raw FOV2D24D0. It stores unshaken position+10
and unfiltered FOV+04. Stationary nonzero outputs mode0 and authored Look/Up,
preserving the live target lane. Otherwise mode1 outputs target and preserves
live Look/Up. W lanes are copied where original instructions copy them.

Raw FOV first compares MinFOV and MaxFOV; equality returns that value without
consuming distance. Stationary returns MaxFOV. Otherwise it uses horizontal
XZ distance only, computes
MinFOV + (MaxFOV-MinFOV) * ((MaxFOVDistance-distance) /
(MaxFOVDistance-MinFOVDistance)), then applies the original sequential Min/Max
comparisons. Reversed authored ranges are never sorted; the original arithmetic
and clamp ordering determine their result.

After nearby-vehicle shake and shake expiry, the original filter always computes
blend = MIN.S(dt,tau)/tau, then current += blend*(rawFOV-current). The proven
initial tau is bits3ECCCCCD (~0.4), current0, blend1, limits±FLT_MAX; the update
does not consume those limits. Negative finite time/tau are not silently clamped.
The two scalar MIN.S sites reuse the independently verified finite signed-zero
order helper, with no collision/query dependency. Output position/FOV contain
shake/filter results; stored+10/+04 remain unshaken/unfiltered.

## Nearby-vehicle shake is closed numerically

Source2D38C0 reads the first eligible vehicle in exact manager order. It skips
inactive40!=0, then consumes velocityXYZ and local cached positionXYZ. Eligibility:
squared distance from selector positionA0 <=3025, speed>=170, and index differs
from rememberedD0. The remembered-index check follows the numeric reads.

Intensity is MIN.S(1,((speed-170)/170)*(1-distance²/3025)). It writes duration
0.45*intensity, elapsed0, enabled1, amplitude14*intensity and remembered index.
Zero intensity still activates. It is not silently skipped: the restore phase
can remain finite, while the random phase reaches elapsed/duration=0/0 and is
outside the finite native arithmetic domain.

When enabled, a nonzero stored shake X restores positionA0 and clears only X.
Otherwise exactly three shared original RNG draws build a positive XYZ vector,
which is normalized, multiplied by (1-elapsed/duration) and amplitude, stored
as shakeB0 and added to output position. Time is then added; not-less-than-duration
expires/reset fields in source order. Zero or three draws occur, with no hidden
extra random draw in the composed view consumer.

Resolved scene getters and manager lookup are explicit supplied-input boundaries.
Their original successful numeric contracts are source-proven; no live world
lookup or placeholder callback is substituted into production. Optional debug
console formatting is a presentation boundary and is not a numeric input/effect.

## Orientation and matrices

Mode0 normalizes supplied Look and Up independently; it does not orthogonalize
them. Other modes normalize carried prior Up before the target operation. Exact
target==position preserves prior Look and that already-normalized Up. Otherwise:
forward = normalize(target-position); right = normalize((0,1,0)×forward), with
exact vertical target using right=(1,0,0); corrected Up=forward×right.
No epsilon or invented fallback direction is used.

Normalization's source VADDx.w performs finite W + positive zero; -0 W becomes
+0. Nonfinite W rejects only when that arithmetic is reached. True copy-only
and unused W lanes remain bit-preserved. Original zero-norm reciprocal saturation
is modeled explicitly; a degenerate finite authored basis is not repaired into
a new orientation.

View builder14DC80 takes right=Up×forward without further normalization. Flat
storage is rows [Rx,Ux,Fx,0], [Ry,Uy,Fy,0], [Rz,Uz,Fz,0],
[-dot(R,position),-dot(U,position),-dot(F,position),1]. Nonzero roll applies
original14E070 with negative roll and original sincos13BFE0.

FOV input zero (either sign) retains prior scene FOV. Scene builder115350 converts
FOV degrees by exact bits3C8EFA34. Projection14DB90 uses the original tan13BCA0
polynomial, not host trigonometry. If t=tan(0.5*convertedFOV), h=1/t,
z=1/(1-near/far), matrix entries are [0]=h,[5]=h*(1/aspect1F8),[10]=z,
[11]=1,[14]=-z*near; other entries are zero. Combined is view×projection in the
original row-vector storage/order, with separate multiplication/addition rounding.

Aspect1F8 is an exact original scene field, not a guessed desktop aspect ratio:
(viewportHeight/viewportWidth)/(scene1DC*(fullHeight/fullWidth)). Its native1
placeholder must be replaced by the supplied source value when integrating.
Other scene defaults have constructor provenance; field initialization does not
claim recovery of renderer registration or full platform setup.

## Proof and domain

The only original code source is the retained ELF SHA256
c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
Source-only extraction, independently decoded word tables, two original-word
oracles and all original-domain exclusions are preserved. TV oracle boundaries
are resolved player-position input and valid ordered manager lookup; local position
and RNG leaves execute original instructions. The matrix oracle executes original
view/projection/roll/polynomial/normalization/multiply words without native formulas.

Five retained tangent exact-cosine-zero saturation words remain source-reviewed
but unexecuted by the numeric fixtures. A bounded FOV-neighbor scan found no
witness; no exhaustive unreachability claim is made. Dead duplicate/padding and
excluded non-body/debug/invalid-current paths are listed separately in coverage.

All new APIs explicitly admit nearest-even rounding only, strict binary32,
no contraction/fast math, finite reached arithmetic and valid typed backing.
Reached nonfinite values, zero divisors and unsupported domains reject without
committing caller state. Copied or branch-unused numeric lanes are not globally
validated. Host instruction-step VU Q/MADD/square-root behavior is qualified;
this is not universal PS2 silicon denormal/exception/micro-arithmetic conformance.

## Follow-camera blocker and preservation

Follow update17A050 reaches real unresolved providers: virtual179CA0 can call
height provider2E83D0;17A870 reaches11C1F0,11FF60,11CA80 and121920 query boundaries.
Those callees were not opened. No restricted absent-provider branch is presented
as a complete follow-camera implementation. The exact evidence/stops are retained.

This checkpoint starts from verified 49E ZIP SHA256
1125caad601a597342b8a995dca1d9acb87f141422031a28da4759613124d23e.
Only inherited CMakeLists.txt and README.md change;49F files are additive.
49C remains separately frozen and unreleased; collision/action2255B0 blockers
remain unresolved. No PC/RAM access, new assets, installs or pushes occurred.
No Windows/Xbox build, rendered frame, full race scheduler or gameplay FPS is claimed.

Run `python3 tools/test_native_camera_view49f.py --output-dir /new/external/path`;
add `--sanitizers --no-leak-check --stack-usage` for this host. CMake option
`RR_NATIVE_CAMERA_VIEW_ONLY` is supplied but unexecuted because CMake is absent.
LeakSanitizer is explicitly excluded due the host tracing limitation.
Final tests, independent semantic review, sensitivity and footprint measurements
are recorded in `evidence/cameraView49F/validation.json` and the checkpoint note.
