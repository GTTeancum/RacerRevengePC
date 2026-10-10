# 49U: complete admitted Chariot-root response

49U closes the original `21A660` call for its high-speed physical-to-virtual
transition and persistent virtual response. It produces the hidden response
scene `198`, displayed Chariot model root `1B4`, response/mode records and probe
feedback through the real return. The earlier selected physical-copy helper in
49P remains unchanged and is not treated as the entire function.

The admitted path requires a present Chariot, vehicle fields `58=0` and `40=0`,
and the original cached-speed gate against the effective shared `434750` value.
It accepts physical `838!=0,83C=0` or virtual `838=0,83C!=0` mode. Other contexts,
including the lower-speed reverse transition's secondary-body virtual effect,
remain outside this API. Rejection does not silently replace those effects.

## Phase and persistent state

The original base caller reloads application `23C` independently for animation
`21A000` at `2287BC`, engine/container response `21C120` at `2287F0`, then Chariot
root response `21A660` at `228824`. Later body caches and physical substeps follow.
The new root update therefore consumes existing animated nodes and current
engine-root output, before any later physical pose is available.

The physical entry copies the supplied current secondary scene's complete local
matrix to the displayed Chariot. Its actual transition tail then copies that
local into scene198, derives the persistent response position and quaternion,
zeros the response velocities using source shared434630, and switches to virtual
mode. Subsequent virtual calls integrate and publish the source response. There
is no invented root pose copy after physics.

The position seed uses `28B1A0(1B4,194)`, relative to container194's world matrix.
The rotation seed follows `28B550(1B4,194)` through **194's parent**, main190,
before `14F950`. Replacing both with a conventional inverse194 transform would
change the original result. Scene198 is an independent generic unparented scene,
initialized by the source constructor with identity local, zero cached world,
flags80 and no scene links. It has no new authored PSG offset.

Phase864 is the same vehicle field used by secondary49M, passed as a reference
to caller-owned canonical state. It is not a separate Chariot clock. Noise and
RNG are the same caller-owned context used by49R and later physical consumers.
Every admitted virtual call executes six source noise evaluations in order:
translationX/Y and quaternionX/Y/Z/W, even when an amplitude is zero. Both lazy
shared words, scene caches/dirty flags, mode outputs and `probe10C -> vehicle840`
are retained.

## Body and authored inputs

The body fields have distinct source meanings:

- Cached34 is the older frame-cache speed consumed at this base phase. It is not
  recomputed from a later physical state.
- Body80 is cached body-local velocity. The original root function normalizes it;
  the caller must not substitute an already normalized direction.
- BodyB4 is the Y component of local angular velocity. World angular cacheA4 is
  a different input and cannot replace it.
- BodyC0 is current world linear acceleration consumed by preceding49T internal
  animation, not directly by this selected root response.

The real cable endpoints are Chariot PSG nodes4/5, named
`chariotcableleft1` and `chariotcableright1`, beneath nullcg0. They are distinct
from the hand IK targets16/17. Relative getters consume their current animated
scene records and the current primary engine roots.

The authoring adapter projects the original21 Chariot/Cable scalar loads from
Pod01 PHY, preserving the original degree conversion and numeric parser. A90 is
an authored load effect that this root consumer does not read; its optional
provenance output is separate from consumed parameters. The authored text's
`ChariotOrientationGain=100` is not loaded by the original code. EffectiveA94
comes from the source constructor word for-300. Substituting the authored100
would invent a mapping.

Rest length854 comes from the already recovered fresh49O geometry result using
actual Pod01 authoring. Source constructors establish initial85C and864 as+0;
these are fresh-record facts, not permission to repair or reset live histories.
The physical-to-virtual transition produces response seeds from current scenes,
so a connected replay can progress without supplying a new hidden198 matrix or
invented response quaternion.

## Supplied boundaries and diagnostic meaning

Current main and secondary physical poses, current body/component/probe values,
initial incoming scene state and earlier base work remain explicit inputs. An
initial Chariot root is supplied only until a complete source root call has
actually produced it. This does not establish an authentic starting grid or
execute the complete constructor, first-base217440, contacts or physical frame.

The original main scene has graphics child194. Existing49M body-domain guards
remain unchanged; this graphical stage does not silently claim a direct complete
49M frame bridge. Its838 output and shared864 value must be carried into the
proper later physical boundary by a separately proved composition.

The connected renderer retains the TA geometry, fixed49H camera, texture/UV/color
and index associations. Source current-node palettes drive semantic local
vertices using the existing diagnostic arithmetic. It does not introduce an
original VU vertex program, complete lighting/visibility or a gameplay FPS claim.
The source-reference equivalence admits finite surviving VU scratch used in
self-cancel operations. Targeted VF9.W/body80.W poison checks establish that those
specific opaque lanes do not affect represented outputs; they do not establish
arbitrary NaN-register independence or universal EE hardware behavior.

The reverse virtual-to-physical transition, authentic spawn, contact production
and a complete playable game remain open.


## Evidence and visible controls

The focused root proof executes432 complete calls, including physical entry,
persistent response, interleaved shared-noise contexts, zero delta, source gates
and rejected contexts. It reaches every1,316 admitted root word;394 additional
root words belong to excluded blend/reverse/nonzero58 routes. Broader retained
helper coverage is reported separately and is not presented as full-path proof.
All19 core semantic controls detect actual mismatches. Relative scene/math proof
covers523 fixtures and5,994 operations, with4,548 ordered flag stores in its
instrumented form and11 detected controls. Authoring proof covers443 original
words and nine semantic controls. Ordinary aggregate objects omit tracing hooks.

Independent checks include analytic reference frames, updated-velocity response,
landing/impact behavior, shared clock/noise semantics and late-failure atomicity.
A separate full-source test poisons finite surviving VU scratch and targeted
unused VF9.W/body80.W inputs while comparing represented outputs. General NaN
scratch equivalence is explicitly excluded rather than hidden by zero registers.

The connected96-frame sequence compares complete49T,49R and49U original calls,
all53 graph nodes and46 palette entries. It retains one actual physical entry,
95 persistent virtual calls and one shared clock/noise context. All552,480
per-frame vertex positions are independently reconstructed, with only four
full-frame meshes and two control meshes retained. The float32-first local-input
negative control is insensitive at this particular large-anchor pose; that zero
is reported honestly, alongside exact binary64-local reconstruction and the
retained earlier rigid-transform sensitivity evidence.

Frame95 controls restore the actual source-produced frame31 Chariot root or copy
the held supplied secondary local over the full response output. Each changes
all2,884 Chariot vertices and zero engine vertices. They change12,755 and12,275
raw framebuffer pixels respectively. The counterfactual copies are display
controls, not alternate recovered runtime modes. Both render passes reproduce
all raw and captioned PNG bytes exactly.

Delivered compositions live in `frames/presentation-final`: four unscaled source
samples, a lossless four-sample loop with declared600ms presentation cadence,
and a2x nearest-pixel comparison using crop(288,224,608,444). The initial inherited
crop is retained in `frames/presentation` as provisional evidence; it clips the
newly moving Chariot and is not the delivered comparison. No camera or mesh data
was changed to improve the crop.

Reproduction uses the direct compiler runners:

```
python3 -B tools/gameplay/chariot_scene_recover49u.py
python3 -B tools/gameplay/chariot_root_recover49u.py
python3 -B tools/test_native_chariot_root_authoring49u.py
python3 -B tools/test_native_chariot_root49u.py
python3 -B tools/test_native_chariot_root49u.py --sanitizers --no-leak-check
python3 -B tools/visual/run_chariot_root49u.py --output-dir /tmp/rr49u-reproduction
python3 -B tools/visual/render_chariot_root49u.py --self-test
```

Use a fresh output directory. Normal and ASan/UBSan checks are retained; leak
checking is explicitly disabled in this tracing environment. The focused CMake
option is `RR_NATIVE_CHARIOT_ROOT_ONLY`; CMake is unavailable here, so no CMake
configure/build is claimed. Host measurements are isolated subsystem costs,
not gameplay FPS or original-Xbox results. The saved49T archive is the exact
baseline; only inherited README/CMake references change.49C remains excluded
and unreleased, and the held historical diagnostics are not resumed.


## Native cost and preserved validation

The final aggregate comprises60 suites and53 production units. Normal and
ASan/UBSan runs each perform176,376,609 checks over1,265 pinned inputs. All56
inherited suite outputs are preserved byte-for-byte. Focused ordered-effect
proofs remain separate from the aggregate's ordinary uninstrumented objects.

The new49U caller payload is11,876 bytes, including the11,392-byte root workspace
and the added scene198 record. The full connected harness has361,671 bytes of
caller payload plus15,624 immutable motion bytes. This includes previous stages
and the existing secondary-state owner; it is not the incremental49U cost.
The three new objects add30,809 text bytes,920 ordinary read-only bytes and504
relocated read-only bytes from the authoring lookup table. Mutable data and BSS
are zero. The largest new runtime compiler frame is1,072 bytes; the authoring
frame is4,000 bytes. These are compiler estimates, not a measured complete stack
high-water mark.

Fifteen CPU0 samples per variant give medians of13.848 microseconds for49U alone,
77.528 microseconds for49T→49R→49U plus three palettes, and241.398 microseconds
including all5,755 rigid vertices. Fixture I/O, original instruction oracles and
correctness hashes stay outside timing; surrounding untimed stages maintain
source history where needed. All45 samples and successful-call ordinary-new
probes are retained, with zero counted allocations. These measurements exclude
rendering and are not gameplay FPS or target-console measurements.

The unchanged displayed scene submits360 draws and73,409 triangles with7,604,084
bytes of GL geometry/texture payload. The Python/Mesa process footprint is
separate from native caller storage and is retained in each render report.
