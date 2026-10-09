# Native original-roster rigs (50HW)

Status: implemented and read-reviewed only. No build, syntax check, gameplay,
benchmark, sanitizer or test was run for this change. It awaits the coordinated
hourly validation slot and full client integration.

## Ownership and integration

`tests/native/native_racer_rig50hw_fixture.h` provides a noncopyable/nonmovable
`native_racer50HW::Rig(repository, podJson)`. Keep one stable owner per roster
entry. Its PSG, MCF, VUT and MOT views borrow immutable bytes held by that owner;
member byte counts and SHA-256 are checked against retained import provenance.
Do not register or byte-copy the Rig object in the serial snapshot bank.

The source component order is left engine, right engine, chariot, matching the
existing startup asset array. `roots`, `sourceNodes`, `hierarchy`, `rows` and
`localRows` preserve the actual selected source topology. `rebaseModels` uses
the native source order chariot, right engine, left engine. `geometry` contains
the source-derived cold radius, secondary mass/inertia, target and joint inputs.

For an opponent's untouched cold snapshot:

1. Call `rig.install(state, bindings, handRecords)`.
2. Bind its own PHY, geometry, collision/probe and handling owners.
3. Place its physical/displayed roots using its own secondary target.
4. Rebase `rig.rebaseModels`, and bind effect names with `rig.buildEffectNames`.
5. Bind renderer assets/palettes and cable endpoints from its own source mapping.
6. Restore/save every mutable binding and parameter owner along with the frame.

The existing Anakin startup remains a valid 53-node default and keeps its
original indices and first 53 graph words. The new Rig can also read Anakin,
but replacing the existing startup policy is not required to support opponents.

## Capacity and active forest

`maxSceneNodes49P` and fixed frame storage now have capacity 80. The largest
selected forest, Pod08, contains 61 original PSG nodes plus seven native
owner/root nodes, for 68 active nodes. `VehicleFrameState49W::sceneNodeCount`
and `VehicleFrameBindings49W::sceneNodeCount` agree and default to 53. Native
frame, rendering, cables, query and recovery graph views consume the active
count. Individual PSG/MOT binding rows and mesh palettes remain capped at 64.
Spare cold storage is detached; it is not silently treated as active geometry.

`EffectNames50BL` keeps legacy low membership words plus high words and uses
modulo-64 bit positions. Call `effectModelRoot50BL` to check root membership;
do not shift a 64-bit word directly by a source scene index.

## Exact resource and motion selection

PHY component names select the three PSG/MCF models. MCF `NAME` identifies the
motion and `VUE` selects its `.mot` member; array positions never determine
Forward, Back or brake roles. This matters for Pod04, Pod07 and Pod11's different
MCF ordering. The six opponents with `NumberOfMotions=0` engine MCFs keep null
engine motion pointers and no fabricated VUT/MOT data. Pod09 uses its own engine
motion records. Procedural native engine-root response still runs for all pods.

IK chains follow source `handlt`/`handrt` lookup, then the wrist's parent and
grandparent, with `l_hand_dummy` and `r_hand_dummy` targets. Cables resolve their
own PSG names. The original hierarchy is never collapsed into one rigid pod.

## Pod08 Mid source closure

The retained source files establish two different ordered uses:

- `evidence/authoredGeometry49O/motion/selection-source-disassembly.txt`:
  213830 samples Forward/Back at (.5, 0), then 213838..213854 samples optional
  Chariot Mid at .5 before the cold bounds/radius calculation.
- `evidence/chariotAnimation49T/flow/source/animation.txt`: 21A1B0..21A218 samples
  Chariot Mid at clamp(.5 * (1 + filtered steering B6C)), after the two brake
  samples and before engine samples. Engine Mid calls are 21A388 and 21A3F8.

The optional cold `FreshPodGeometrySources49O::mid` uses a true single-motion
numeric sampler. It does not emulate a single sample by blending a motion with
itself. Runtime nullable Mid bindings use the existing exact scene sampler;
Anakin's null Mid pointers preserve its previous skipped branches. The active
frame supplies the actual filtered control B6C, not a time or velocity proxy.

`evidence/engineAnimation49Q/flow/source/engine_motion_selection_and_initial_sample.txt`
also retains the right-then-left cold engine order: primary motion fraction 0,
identity root orientation, then optional Mid fraction .5 (21385C..213A20).

These additions preserve the native strict-FP and allocation-free hot-path
contracts. They are not a claim of new execution validation or complete original
constructor/race-loop equivalence.

## Cable and flame cold owners

`native_racer_cables50hw_fixture.h` exposes `Cables(rig, state, bindings)` with
stable `bank()` and `bindings()` accessors. Endpoints use the selected PSG's
`drawcable*` names when present, otherwise its actual `chariotcable*` nodes;
engine endpoints use its actual `enginecable*` nodes. The selected RES's own
cable PSM/PST members are loaded and authenticated. Their eight original hashes
are identical, so the existing exact-source kind0 decoder is reused without an
Anakin-archive fallback or guessed texture.

`buildColdFlames(rig, drawAssets, bindings)` decodes that racer's named original
PSMs and calls the generic `buildPodFlameOwners50W` overload. The material table
is aligned with each scene's complete original material-slot order. POD09 has
nine slots per engine, with its extra static `09_2` reference at slot7 and its
fourth actual flame reference at slot8. Fixed reference storage now holds32
slots (native sixteen-per-model admission); static material slots remain outside
the flame-only projection. Four authored flame references per side retain the
existing eight callback writes. Anakin's old overload still requires its exact
original eight-slot material layout. All cold owner arrays are copied by the
existing per-racer snapshot bank; decoded source containers are never copied as
raw mutable snapshot data.
