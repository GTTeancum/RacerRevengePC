# Native gameplay recovery 48B

Date: 2026-10-03. The immediate target is native x86 PC gameplay; Xbox remains a
shared architecture constraint. This checkpoint extracts actual tuning/input
semantics and one small native function. It does not run a native vehicle or
claim gameplay/FPS improvement.

## Delivered evidence and reproduction

Run `tools/gameplay/recover48b.py` with the project's configured Python. It reads
only the exact supplied ELF, `UI/PODPHYS.RES`, selected original generated-source
files, and the accepted 47W VBlank-7000 RAM dump. It writes under
`evidence/gameplay48B/` and generates one test-only original-word header under
`tools/gameplay/`. It executes no game binary and changes no original resource.

The script produced 23 byte-preserved PHY files plus ordered text inventories,
10 instruction-verified original source excerpts, six proven joystick tuning
destinations, 4,396 original-word/formula substep comparisons, and a typed
identification of eight objects in the accepted stopped RAM snapshot. Its
manifest reopens and hashes every product. `test_recovery48b.py` exercises
truncation, malformed resource boundaries, out-of-range records, duplicate PHY
keys, unexpected text, conversion boundaries and original provenance.

Original inputs:

| Input | SHA-256 |
|---|---|
| `work/native/project/disc/SLUS_202.68` | `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30` |
| `work/native/project/disc/UI/PODPHYS.RES` | `8361cb484ea841e1772cc0ddec2223fa923e397c1cc9a89f06ab06b1cd7ca2c8` |
| accepted 47W V7000 `ee.bin` | `38682d69c2e2cc08ad1a52367a5198f88ec8624d232c2195fb267050928d3e48` |

The RAM source path is recorded in `snapshot-identification.json`; object
addresses below apply only to that dump. The script requires each exact input
hash and refuses silently substituting another game revision or snapshot.

## Original substep policy

The source string `PodVehicle::RunSimulation` resides at virtual `0x3B2CA0`,
file offset `0x2B2D20`. References at `0x219860/868` and `0x2198BC/8C0` occur
inside `FUN_00219810_0x219810.cpp` (lines 656–666 and 869–877). The routine
copies the entry object `a0` to `s3`, float `f12` to `f20`, and `a1` to `s2`.
Its true original return is `0x219FF8`, after calls to many component updates.

The caller belongs to discontiguous `FUN_001a7b40`; the relevant source is
`work/native/project/generated/entry_001a8060_0x1a8060.cpp`, not the earlier
function file. Original PCs `0x1A833C..0x1A83B0` establish:

1. A global scalar is initialized to binary32 60.0 at RAM `0x434188`.
2. `scaled = float32(elapsed * scale)`.
3. `count = CVT.W.S(scaled) + 1`.
4. `step = float32(elapsed / float32(count))`.
5. Iterate the eligible vehicles for each substep. The first-substep flag is 1,
   then becomes 0. Object field `+0x40` states 3 and 4 are skipped at this caller.

Original instructions at `0x1A83FC` and `0x1A8460` call `0x219810`; return
continuations are `0x1A8404` and `0x1A8468`. `0x1AF330` retrieves a vehicle
pointer from the racer's indexed pointer array at `+0x44`, with count use at
caller `+0x30`. The exact meanings of the skipped states are not yet recovered.

The runtime macro in `ps2xRuntime/include/ps2_runtime_macros.h:628` calls
`ee_cvt_w_s.h:9`: truncation toward zero with signed saturation. The supplied
PCSX2 `pcsx2/FPU.cpp:254` agrees about truncation/saturation for `CVT_W`.
Conversion does not use host round-to-nearest. Multiplication and division
rounding are separate concerns. In particular, binary32 `1/60` produces
`count=2`; its immediately preceding representable value produces `count=1`.
Replacing this with one update per VBlank or `ceil(elapsed*60)` changes behavior.

The accepted RAM snapshot contains scale 60.0 and reciprocal
0.01666666753590107. The source strongly suggests elapsed seconds, but no
recorded caller-entry trace proves the time domain or its clamp/pause policy.
The 4,396 fixtures are synthetic, finite nonnegative values near common
intervals and in 0..1, with nearest-even binary32 arithmetic. They are not an
observed gameplay domain or a complete PS2 FPU implementation. Preserve the
original policy while independently selecting presentation timing.

## PHY data and loader evidence

The existing v3 UI-container specification applies to PODPHYS.RES: bounded
24,576-byte zlib blocks and indexed logical extents. All 23 stored PHY records
decode to ASCII section/key/value source text. Pod01 is Anakin Skywalker,
6,584 original bytes. It contains physical properties, repulsor, engine,
aerodynamics, collision, joystick, camera, repair/cooling, audio and tuning.

The extracted JSON preserves order, duplicate keys, spelling and raw values.
It deliberately does not select a default on duplicate keys or interpret
the original reader's missing-key behavior. Original bytes remain alongside it.
Output names are synthetic ordinals; consult the index to map each to the
original pod identifier. In particular, ordinal 5 is not pod05.

Original reader `0x158A70` preserves destination `a2` in `s0`, invokes the string
reader `0x158240`, converts a successful result through `0x302E10/0x310440`, then
writes `f0` to the destination at `0x158AAC`. Failure bypasses the write. Thus
unread keys must not automatically become zero. Full parser/number-conversion
semantics remain in the original helpers.

The Joystick section selection at `0x215A78` is followed by six exact float-reader
calls. The recovery script checks every five-instruction call sequence and the
string bytes against the original ELF before reporting this map:

| PHY key | Original destination relative to loader entry object | Pod01 text value |
|---|---|---|
| SteerSpeed1 | `+0xB84` | 250 |
| SteerScale1 | `+0xB88` | 1 |
| SteerSpeed2 | `+0xB8C` | 450 |
| SteerScale2 | `+0xB90` | 1 |
| XAxisRateLimit | `+0xB94` | 4 |
| YAxisRateLimit | `+0xB98` | 3 |

Source: `generated/entry_00214f6c_0x214f6c.cpp:4909`, continuation of
`FUN_002146c0`; caller object is preserved in `s5` at original `0x2146E8`.
The earlier loader reads Gravity into `+0x378` and AirborneGravity into `+0x37C`,
then multiplies each by float bits `0x4200CCCD` (32.20000076293945). Pod01 text
values 6 and 10 become 193.20001220703125 and 322 in the snapshot. This numeric
transformation is proven; interpreting the engine's world units remains separate.

MaxThrust A/B/C and TheoreticalTopSpeed A/B/C feed common stack slots through
conditional tuning branches, while another branch interpolates min/max values
and upgrade fractions. Do not choose a field merely because it is named
ActualTopSpeed or feed the text numbers directly into a new physics model.

## Input path and one native function

48V continuation: [native input shaping](input-shaping48v.md) now recovers the
preceding nonlinear/quantization interval and composes it with the corrected
limiter. The historical 48B extraction below and its evidence remain preserved.


`FUN_001a0570` has the original PodPlayerVehicle::Create joystick failure string.
It binds an input object at player `+0xFF0` and input index `+0xFFC`. Its four
created input components occupy `+0xFE0/FE4/FE8/FEC`. `FUN_001a1a20` copies each
component's `+0x24` float into player `+0xB5C/B60/B64/B68`, then performs virtual
button queries and some overrides. Which physical controls/action names map to
these channels is unresolved; they must not yet be named throttle/brake.

`FUN_00217de0` performs nonlinear shaping and quantization before the extracted
interval. At original `0x2180AC..0x2181C4`, the four channel values are limited
relative to prior outputs `+0xB6C/B70/B74/B78`. Rate order is X,Y,X,Y, multiplied
by the entry `f12` factor. Each channel follows the original ordered operations:

```
limit = float32(rate * elapsedFactor)
delta = float32(target - previous)
change = min(max(delta, -limit), limit)
next = float32(previous + change)
```

`src/game/control_filter48b.h/.cpp` implements these operations directly over
four native float channels. It has no guest registers, guest addresses,
instruction dispatch, MMIO, PS2 runtime or renderer dependency. This is one
small recovered native input function, not a vehicle simulation.

Its API explicitly accepts finite values and nonnegative rates/elapsed factor,
rejects overflowing intermediates, and preserves output on failure. Those are
native API requirements, not assertions that original gameplay validated those
inputs. Compile with strict floating-point semantics. Explicit binary32 stores
preserve original operation rounding. As corrected in48O, scalar MIN.S/MAX.S
use signed-word ordering, with -0 below +0; the historical generated comparator
was not an adequate signed-zero reference.

`tests/native/control_filter48b.cpp` uses a test-only decoder of the **exact 70
original instructions**, generated into `tools/gameplay/control_filter_words48b.h`
from the hash-checked ELF. The oracle retains original registers, memory offsets,
loads/stores and instruction order; it does not call the extracted helper to
generate expected outputs. Tests cover independent random four-channel cases,
four host rounding modes, multi-step trajectories, signed-zero ties, aliasing
and rejected-input/no-mutation behavior. The historical48B implementation and
its then-used float-comparison oracle passed130,349 checks in both x64 and x86
builds. Those artifacts predate the48O signed-zero correction and are preserved
as historical evidence, not validation of the revised source. The checks ran
with `/fp:strict`; logs were read back from
`evidence/nativePc48B-build-20261003-122453-299/`. The 12 offline recovery tests
also passed (`evidence/gameplay48B/tests.log`). No full reference build or live
game run is performed by this recovery workflow. The helper is not yet wired
into the native viewer or vehicle gameplay.

The active instruction oracle uses host IEEE arithmetic and independent signed-word
scalar MIN/MAX selection.48O adds explicit zero-sign Cartesian cases in all four
host rounding modes; see `docs/minmax-audit48o.md` for the correction and current
Linux validation. Windows/x86 hardware were not rerun for this change. Results
cover the tested finite domain and host rounding modes; they do **not** prove all PS2
denormal/overflow/exception modes, the surrounding shaping function, or vehicle
physics. The binary32 stores in the helper are not a replacement PS2 FPU.

| Historical48B helper executable (before48O correction) | SHA-256 |
|---|---|
| x64 | `938843c25a49e5f5ce7c549d08b7d6898b668e9bd24cb0efc460f21f1d9196e4` |
| x86 | `e6e24a27a251abba178aa3b26e5877ac7cc84dd218cf873e8252b916b7b7caff` |

## Stopped-state identification and remaining unknowns

Searching the exact six joystick floats in the accepted RAM dump yields eight
candidate objects. Each passes an original-ELF vtable/type-name check: seven
`PodAIVehicle` objects (vtable `0x3CE1E0`) and one `PodPlayerVehicle` (vtable
`0x3CED50`). Player address is `0xC5B0B0`; its input object is `0x581450`, index 0,
and body-component pointer at `+0x340` is `0xD110A0`. This makes a subsequent
trace bounded and concrete. It does not establish allocation size or a stable
address in other runs.

RunSimulation labels the update of component `+0x340` with the literal PodBody.
Its scalar `+0x34` is used in steering interpolation, consistent with speed.
Elsewhere object `+0xC70` is copied to that component's `+0x70`, and its three
components' magnitude is stored into `+0x34`. These are strong motion leads,
but the final snapshot shows the two vectors differ: capture the writers and
temporal roles before declaring both the current velocity. The body scalar in
the snapshot is 13.774285316467285. The associated object's `+0x100` and `+0xC0`
64-byte blocks are transform candidates; convention and meaning are unresolved.

Remaining blockers: actual elapsed-factor traces, action mapping and analog
dead zones, complete body/position/orientation/angular-state layout, collision
object formats/query closure and response ordering, random-state ownership,
camera update identity, and race-event semantics. None is replaced by invented
constants or a new generic vehicle physics model.

## Next capture contract

An isolated reference trace should observe player input update `0x1A1A20`,
input shaping `0x217DE0`, caller substep calculation and `0x219810` entry/true
return. Include thread ID, original SP/RA, call/substep ID, float bit patterns,
component pointers, virtual targets, and bounded before/after relevant fields.
Pair actual guest returns across continuations: a generated C++ return may be
a dispatch yield and must never be treated as a completed simulation step.
Matching saved RA alone is also insufficient without original SP/thread/call
identity. Preserve interrupted/incomplete records explicitly.

Capture neutral, accelerate, brake, steer, reset and wall/ground contact cases
after actions have been mapped. Begin with the identified native rate-limit
function, then close the first body substep's read/write dependencies. Native
acceptance compares typed state and observable events; reference cycle/register
identity remains a separate compatibility-runtime test. Require native queries
before calling a replay-backed collision fixture playable gameplay.
