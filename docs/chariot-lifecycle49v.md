# 49V: ordinary Chariot mode lifecycle

49V closes the low-speed reverse transition, concrete secondary-body callback and
persistent basis blend in the original `21A660` Chariot-root update. It adds the
complete ordinary lifecycle alongside preserved49U, using canonical49K/49M body
records, current animated scenes, shared phase and shared noise. A240-frame
original/native replay now passes through physical, virtual and blend modes and
produces visible current Chariot geometry in the fixed TA diagnostic.

The admitted context is a present Chariot, vehicle58=40=0, finite nonnegative
frame factor, positive finite duration848, and one of the three nonempty mode
pairs `(838,83C)`: physical, virtual, or both. The pair(0,0) is rejected because
it has no producer for the stack260 matrix read by the source blend. Nonzero58
application mode and unavailable external effects remain outside this API.
This is the whole original call through its real return for that admission,
not a general vehicle tick.

## Original state and order

The base caller runs internal animation21A000 at2287BC, engine/container response
21C120 at2287F0 and Chariot root21A660 at228824, loading application23C separately
for each call. Later body cache work and physical substeps follow. The49V API
therefore consumes current49T animation and49R roots before later physics.

Physical-only calls copy physical scene19C's local matrix to displayed model1B4.
At high cached speed, the existing original tail seeds hidden198 and persistent
response, clears838 and sets83C. Virtual calls update response with the exact
shared phase864 and six ordered noise calls, including zero-amplitude calls.
The consumed cached speed34, local velocity80, local angularB4 and probe10C/1D0
come directly from immutable canonical main-body fields. They are not derived
from a later body pose or substituted world-angular cache.

When a virtual call reaches low speed, it first completes response publication.
The reverse tail then executes these effects in order:

1. Copy displayed1B4.local to physical19C.local using the original scene setter.
   Its cached world remains stale and dirty at this point.
2. Call actual secondary35C vtable slotAC:167A50 calls slotA8:167AD0.
3. Copy full main70 and mainB0 vectors, including their W words, into secondary.
4. Set elapsed84C to+0 and physical838 to1. Virtual83C stays enabled.

The callback is query-independent. It touches exactly236 source bytes: eight
full vectors at180,190,C0,D0,70,80,90,B0 from the separate433B80 zero vector,
cached speed34, and the original derivative/history region2B8..31F using six
12-byte clears and two quaternion copies from separate382B60. These are distinct
initialized source globals. The callback preserves position60, orientation1F0,
center1E0, world-angularA0, cached anglesE0, corrections1A0/1B0, accumulators1C0/1D0,
parameters, joint records, scene, phase864 and other unmodified body state.
After main velocity copies, cached34 is still zero and local80/previous90 remain
reset. Refreshing them here would change the original call.

A52-byte supplemental fixed record holds only fields missing from canonical49M:
four W words and current three-component buffers2B8,2D0,308. Canonical velocity,
history and phase fields are never duplicated. Caller-owned838 can be the actual
`CoupledBodyState49M.vehicle838`;864 is solely `secondary.time864`. The main body
and its W-lane record may be disjoint subobjects of that same owner and remain
unchanged by49V.

## Blend and expiry

While both flags are nonzero, response still runs. The blend uses old84C/848 and
physical19C's **cached world100**, without refreshing it. It processes rows3,2,1
with the original scalar clamp and arithmetic order. This is not a matrix or
quaternion interpolation shortcut: the source rebuilds a basis using cross
products, separately normalizes up and forward, then reconstructs right.
Basis W words become zero and translation W becomes1, even at endpoint alpha.
Endpoint delay-slot arithmetic remains part of numeric admission.

Only after publishing the displayed matrix does the source advance elapsed by
the frame factor. Virtual83C clears only when updated elapsed is strictly greater
than duration. Equality retains blend. A speed rise does not cancel an active
blend. If speed is high when expiry finally clears83C, the ordinary tail can
immediately seed virtual response again in that same call. Error history85C
persists across transitions; it is not reset with elapsed.

The implementation uses fixed caller workspace and transactional publication.
A rejected input, nonfinite consumed result, unsupported normalization, graph
failure or noise failure preserves caller graph/state/body/noise. Copy-only
lanes preserve arbitrary bit patterns. Consumed arithmetic uses separate binary32
operations under FE_TONEAREST, with retained VU zero-norm behavior and explicit
positive-subnormal norm limits. Finite surviving source VU scratch is a declared
oracle context, not an assertion about all possible machine states.

## Scene boundary and connected replay

The graph separately owns localC0 and cached world100. The canonical secondary
scene projection must match physical19C cached world bit-for-bit and its low flag
byte. Upper flag bytes and unrelated body fields survive. The complete49V return
preserves the dirty/stale scene produced by reverse. It performs no eager getter
or body integration.

The connected diagnostic uses240 calls over a repeated16,17,17ms application
cadence, exactly4,000 integer milliseconds. Its supplied speed is coherently80 or
200 across main cached/local/world fields; retained96-row numeric controls repeat
at frame%96 while filter/animation state continues. Supplied current-state body
sentinels make actual sparse reset/preservation observable. This is not authentic
spawn or a fresh-constructor state. The main pose stays at the retained inspection
anchor; the initial secondary scene is supplied, and later secondary-local
changes are produced by the actual reverse transition.

The source/native modes change at16,64,125,144,160,221. There are122 blend calls
and two real reverse transitions. Each exact root return is recorded before
later observation, including stale world, dirty flags, zero speed cache and
copied nonzero velocity. Two discarded source/native forks from those exact
returns use explicitly supplied elapsed=.5 to detect erroneous local-versus-world
blend input. Those forks are not additional frames of the main sequence.

After root capture and mesh projection,157 separately identified observations
execute only the later source218C60 call to28B1A0(19C,190), with two dirty-world
refreshes. The diagnostic mirrors only the resulting graph world and low flag
byte into the canonical secondary scene projection. Body histories, velocities,
W words, cached34/E0, phase and modes remain unchanged. This isolated getter is
an explicit diagnostic seam. Intervening cache/force/physics/observer stages do
not run, and an observer-free context is supplied rather than inferred.

A full49M frame bridge is still unavailable: actual main190 has child194 whereas
the existing wrapper admission is childless, and49V's reverse return can be
dirty whereas that wrapper requires ready scene caches. These guards remain
unchanged. The work does not establish FrameReady, authentic startgrid, contacts,
reset-path resource completion or the unreleased49C contract.

## Proof and visible output

The focused whole-call proof compares560 original returns, including16 concrete
reverse calls and84 blends, with1,681,153 traced checks and913,920 body bytes.
All1,637 admitted root instruction words and72 callback words are reached.
Forty-four lifecycle mutations are detected. Separate callback proof covers144
cases and117,504 whole-object bytes with10 detected mutations. Basis-blend proof
covers2,612 cases, endpoint signed-zero effects and28 detected mutations.
Normal, ordinary production and ASan/UBSan paths pass. The final64-suite
aggregate passes200,823,605 checks in each mode across56 production units and
1,362 unchanged inputs. All60 inherited suite outputs are identical;368 durable
process logs per mode and exact connected replays are preserved. LeakSanitizer
is disabled because the execution environment is traced.

Independent review adds240 full source returns,273 analytical assertions,
960 same-owner comparisons,10 mutations and a3,556-assertion connected trace
audit. The initial diagnostic body-pointer snapshot was corrected before the
connected freeze; production/source scene binding was already correct.

Every connected frame runs original/native49T,49R,49V and all46 palette entries,
then projects all5,755 semantic vertices. All1,381,200 positions are independently
rebuilt. Six frozen meshes feed the same fixed-camera, first-pass textured TA
renderer. Four full samples are15,47,94,135; frame94 controls display only the
current virtual root or only the current physical cached-world root. Each control
changes2,884 Chariot vertices and no engine vertices. They change9,121 and9,010
framebuffer pixels respectively. Two complete rendering passes have identical
raw and caption pixels, each360 draws,73,409 triangles and7,604,084 bytes of GPU
payload. The authoritative sheet, controls and lossless four-sample loop are in
`evidence/chariotLifecycle49V/frames/presentation-final/`. The600ms loop cadence
is presentation timing, not game frame rate.

The textures, diagnostic vertex arithmetic, supplied body inputs and camera are
explicit. There is no claim of original VU rendering, full material/lighting,
visibility, physical driving, race progression or gameplay performance.

## Caller memory and host measurement

The three new production units add22,784 executable-text bytes (23,736 GNU text
including read-only data), with no mutable data or BSS. Matched API records take
12,040 bytes versus11,572 in49U, a468-byte increase. The older11,876-byte figure
also included retained32-byte shared state,92-byte parameters,36-byte bindings
and the144-byte scene198 node; those records are not newly charged here.

The complete connected caller payload is363,367 bytes, including the full
canonical coupled-body owner and separate getter scratch. Relative to49U's
361,671 bytes this is468 additional API bytes,1,208 bytes for replacing a
secondary-only record with the actual coupled owner, and20 bytes of later-getter
output/context. Authored MOT bytes15,624 remain separate. These are measured
record/payload sums, not a peak application-memory or original-Xbox fit claim.
The budget retains per-function stack estimates, with no total-live-stack claim.

On this cloud CPU,15 affinity-pinned samples per mode measure median12.634µs
per lifecycle call over the actual240-frame history. The separate later getter
costs0.730µs amortized per frame, or1.116µs per active observation. Full internal
animation/IK, engine response, lifecycle and palettes take77.251µs excluding
that getter and77.829µs including it; adding5,755 rigid vertices takes244.700µs.
The histories preserve actual reverse outputs before getter observation. Timed
binaries first pass exact snapshot/palette/vertex preflight; ordinary-new probes
report zero allocations in every mode. These are bounded native component costs
on this host, excluding rendering/loading and source-oracle execution. They are
not a game frame-time or original-Xbox performance result.

## Reproduction

From this source root, using the installed compiler without downloads:

```
python3 -B tools/test_native_chariot_lifecycle49v.py --output-dir /tmp/chariot49v-normal --stack-usage
python3 -B tools/test_native_chariot_lifecycle49v.py --output-dir /tmp/chariot49v-sanitized --sanitizers --no-leak-check --stack-usage
python3 -B tools/visual/run_chariot_lifecycle49v.py --output-dir /tmp/chariot49v-capture --jobs 4
python3 -B tools/visual/render_chariot_lifecycle49v.py --self-test
```

Each output directory must be fresh and outside this source tree. The aggregate
compiles56 production units and runs64 related suites serially. Equivalent CMake
configuration is `RR_NATIVE_CHARIOT_LIFECYCLE_ONLY=ON`; CMake itself is unavailable
in this cloud image, so direct compiler results are authoritative. The native
exporter with no arguments compares frozen artifacts without writing; its explicit
capture-directory form writes new evidence. Final regressions, budget, independent
review and recovery validation are retained under `evidence/chariotLifecycle49V/`.
