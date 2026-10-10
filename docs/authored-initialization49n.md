# 49N: authentic Pod01 parameter binding

## Delivered behavior

The actual Pod01 PHY now drives source-ordered main-body mass/inertia,
engine/aerodynamic parameters, gravity and joystick tuning in the existing
native two-body diagnostic. New code uses fixed caller-owned records, serial
execution and no production allocations or guest instruction interpreter.

This checkpoint begins from exact verified49M ZIP SHA256
`c272ffc3dcd2098b88dd715a4107c0bbd5928b9e7b111be805679ecfa6d0d1b3`.
All historical production remains unchanged. The main code lives in
`src/assets/pod_physics49n.{h,cpp}` and
`src/game/authored_parameters49n.{h,cpp}`. The [parameter contract](authored-parameters49n.md)
contains each mapped field and source-order rule.

## Original bytes and numeric conversion

No further PC transfer was needed. The already verified48F archive contained
original `PODS/POD01/POD01.RES`,349,110 bytes, SHA256
`08b4d7655cb796da8a29d9b9a5318c0364561c659794ac9de6c9dfed7416eca3`.
Its96 members include `Pods\Pod01\Pod01.phy` at logical offset1,130,496,
6,584 bytes, SHA256
`47fcfbaab278d10506b302520adc51af4e3388ceb601daa3dbd588aea1f39a1b`.
The complete original container is preserved in the source tree, and extraction
uses the retained bounded RES decoder. This is asset data, not an old-source overlay.

The text reader retains original byte offsets for61 mapped scalars, two engine
vectors and optional Tuning fields. Its lexical domain is deliberately narrower
than the original file service: finite simple decimals, at most nine digits and
six fractional places, complete mapped fields, bounded ASCII text, no duplicates.
Unsupported syntax fails atomically. Original missing scalar lookups skip writes;
the native reader's required-key rejection is not an invented source default.

For admitted numbers, production uses integer arithmetic to reproduce original
nearest-even binary64 division and software binary32 narrowing. Original negative
zero becomes positive zero. The source-word proof executes the original scanner
and software conversion instructions. Independent review additionally checks
exact-rational reference words and four fully initialized synthetic stack patterns.
Whole-file original file-service/lookup execution is not claimed: its semantics
are source-recovered, while numeric conversion is independently executed.

## Values and selection boundaries

Pod01 authors Weight1000, Gravity6 and AirborneGravity10. The original source
unit constant yields main mass31.05590057373047, gravity193.20001220703125 and
airborne gravity322. Inertia uses the exact ordered formula and original full
matrix inversion, including signed-zero lanes; it is not a reciprocal-diagonal
approximation. Joystick rates are4 and3, with steering breakpoints250/450 and
scales1/1.

The caller supplies the track string/class, preset mode and selected profile
abilities at their source read boundaries. No live profile or save state is
inferred. Initial class values may be replaced by tuning rules, the preset branch,
and later profile interpolation. The source order matters: preset selection can
halve steering and replace engine torque vectors; later profile selection can
change steering again without rebuilding those vectors. Engine5C uses the final
modeled engine58 minus converted ActualTopSpeed.

This is a **selected parameter-write projection**, not execution of all2146C0
loader side effects or proof of its complete final state. In particular, the
optional auxiliary virtual call215FD4 through vehicleD40 is not executed. Effects
of that callback and other unmapped loading stages are outside this API and are
not silently represented by a fake callback. The omitted216350..216688 profile
suffix was independently audited: its checked reader calls and stores do not
rewrite the represented body/engine/aero parameters under the supplied-reader
contract. Neither that audit nor the diagnostic closes the whole loader.

## Connected evidence

All30 focused suites pass normally and with ASan/UBSan:

- 32,644,848 checks in each build;177 tracked inputs unchanged.
- Per-test stdout, parameter coverage, reader-boundary traces and persistent
 body traces are identical between normal and sanitized builds.
- 2,585 exact-original decimal cases, six admitted file variants and232
 malformed/domain rejections in the primary reader suite.
- 297 numeric adapter cases, all1,381 selected source words accounted for,
 and122 explicit parsed-reader contracts across12,293 observed boundary calls.
- 11 connected scenarios:1,320 frames,2,970 substeps,2,750 secondary advances,
 220 live838 skips and28,897 allocation-free native calls.
- 5,165,776 connected checks when writing the trace, including20,323,828
 original-word instructions; all150,007 traced float words are finite.
- Five real-text tuning changes alter both final body poses. Rebuilding torque
 vectors from overwritten steering is detected. Changing unselected ground
 gravity leaves this supplied trajectory unchanged, a retained negative control.

Independent review adds347,103 checks per normal/sanitized run,4,804 decimal
cases under four host rounding modes,56 source routes and10,976 demanded-input
probes. Its19,216 fully initialized original conversions execute22,372,496
instructions. Five independent mutations are detected, in addition to five
reader and11 parameter mutations. One-CPU/64MiB normal checks also pass.

The original49M pose/history, secondary mass/inertia/joint geometry, support state,
contact effects and no-hit query contract remain explicit synthetic inputs.
Actual authoring does not turn those supplied conditions into an authentic spawn,
contact implementation or complete playable race. Damping130 and other unproved
constructor fields remain preserved inputs.

## Footprint and build route

Measured strict-C++17 host records:

- Authored input564B; selected binding484B; selection context20B.
- Optional provenance816B; diagnostic16B; source text remains caller-owned.
- New objects22,448B text,1,480B constant relocation-backed schema data,0B BSS.
- Largest individual compiler-reported bounded frame5,232B; nested calls add
 to the live stack. Original RES decompression is a separate load stage.

A one-CPU native-only benchmark repeatedly parses the actual text, binds selected
parameters and projects into49M, with zero allocations and a64MiB address-space
cap. It uses an explicit synthetic selection/profile context. Its host timing
and launch-inclusive RSS are retained separately; they do not measure gameplay
FPS, target-console performance or full resource loading.

Installed-compiler normal/sanitizer runners were executed. CMake wiring is supplied,
but CMake is unavailable in this environment. LeakSanitizer is disabled because
executor ptrace prevents it running; ASan and UBSan pass. Physical EE exceptional
modes and target-console builds remain outside these host proofs.

## Separate secondary initialization assessment

Read-only records in `evidence/authoredInitialization49N/geometry-assessment/`
are preserved for the next milestone and have no production consumer. The
pre-animation nullcg-derived rest arithmetic is checked against43 original words:
854 bits41A64687 and858 bits418E4687. Existing49L applies its own original clamp.

Live secondary inertia remains unclosed. Its actual graphics-model bounds
virtual target is26F4B0, and initial authored motions run before the radius read.
The stored render radius and bind-only candidates must not be substituted.
The exact model/motion inputs are already in POD01.RES; no PC transfer is needed.

49C remains excluded and unreleased. Collision-capacity, ownership and reset
work remains unresolved and was not reopened by this milestone.
