# Racer Revenge PC: native source checkpoint

## Current status: recovered 50HV work in progress (2026-10-09)

This private repository preserves the recovered native Windows/Linux source and
shared code intended for a future original-Xbox port. It is a development
checkpoint, not a complete or release-validated game.

- The current source includes the eight-racer native owner bank, host race
  scheduling, and the reconstructed 50HT contact helper joined into the 50HV
  interactive main.
- Fresh post-recovery validation reported 161 passing focused native contact
  checks and a successful main-file syntax check. The focused eight-body case
  exercised 56 directed pairs, with 10 responses, 8 reciprocal suppressions,
  and 86 generated records. These are focused checks, not a full game run.
- No full game link or contact-integrated gameplay run has been completed for
  these recovered bytes. Earlier four-racer and pre-rollback normal/sanitizer
  results are historical and do not certify the current eight-racer integration.
  ASan/UBSan has not been rerun after recovery.
- Original-roster preparation/import tools are included under
  `tools/assets/original_roster/`. Their presence does not mean the original
  eight distinct visual identities are connected to the live race yet.
- No additional build, syntax check, or software test was run while preparing
  this repository snapshot. Read [contact recovery status](docs/native-racer-contact50ht.md)
  for the exact admitted behavior and limitations.

## Source-only contents and local inputs

The repository includes native source, build recipes, test/oracle source,
technical documentation, and the required vendored source with its existing
license notices. It contains no original disc assets, converted graphics/audio,
executables, build products, memory dumps, or evidence captures. Unreleased 49C
material and separate held changes are excluded; unchanged baseline dependencies
remain.

Some historic source-comparison tests require local original-data fixtures and
unpublished evidence paths. Their generated instruction-word/geometry headers
are intentionally omitted. Keep required original inputs locally and use the
existing generators only for an authorized task. See
[source-only exclusions](docs/source-only-exclusions.md). References to evidence
in older milestone documents describe local historical material, not files in
this repository.

The preserved milestone notes below explain the project's earlier development.
Their dated results must be read as historical, rather than current validation.

---

# Racer Revenge: shared Windows / original Xbox runtime

Current priority (2026-10-06): recover native gameplay in cloud/Linux with
shared code designed for an eventual original-Xbox port. Single-core execution,
bounded reusable storage, and low memory are the development baseline. The
Windows renderer and target remain supported; Linux validation is headless.
Historical target: a native x86-family PC game. The [native PC implementation
plan](docs/native-pc-plan48a.md) supersedes the earlier compatibility-runtime
direction. Completion requires stable full races and progression at a measured
60 presented FPS at 1280x720, with original gameplay and a built-in converter.
The [recovery map](docs/native-recovery-map48a.md) records concrete asset and
simulation entry points, and what remains unproven.

The native implementation builds on Windows x64 and x86. It draws decoded
original track/pod geometry and authored static worlds, has a built-in converter for an
original ISO or extracted directory, and includes independently checked native
control, rigid-body, engine, gravity, aerodynamic and terrain-following functions.
The [root-body update](docs/body-wrapper48h.md) and [terrain-query recovery](docs/terrain-query48h.md)
extend that work in 48H. [Compact collision data](docs/native-collision48i.md) and
[ordered native queries](docs/terrain-query48i.md) continue that work in 48I.
[Terrain filtering and contact state](docs/terrain-query48j.md) and
[spring friction](docs/spring-contact48j.md) extend the native collision path in48J;
[48K corrects the saved impact replay binding](docs/impact-replay-correction48k.md).
[48G](docs/native-progress48g.md) records the preceding converter and capture evidence. The built-in
converter now preserves rigid scene hierarchies, materials, normals and all LODs
in native scene objects, with 5,558 geometry entries, 3,850 textures and 6,107
authored materials accepted across the disc. Cached scene surfaces now bind
their original base textures; twelve original world layouts now have native caches,
and TA renders its authored static placements. Native collision trees and their
distinct original placements are now being connected to the physics path.
Complete shaders, collision queries and gameplay remain unfinished.

The historical [49W-StagedAdvancingFrame milestone](docs/staged-vehicle-frame49w.md)
connects native input, animation, graphics and two-body advancement through
explicit supplied stage boundaries. One persistent state retains real scene
children, source cache ordering, histories and shared noise; actual Pod01 shape
bounds follow the original late transform. Two144-frame source-compared cases
preserve both a labeled synthetic-support trajectory and a no-support fallthrough
control. Evidence is in `evidence/frameBridge49W/`. Authentic spawn, contact
production and complete frame transport remain open;49C remains unreleased and
is not included.

The preserved [49V-ChariotModeLifecycle milestone](docs/chariot-lifecycle49v.md)
closes the ordinary low-speed reverse transition, actual secondary-body reset and
persistent basis blend after internal animation and engine response. A240-frame
original/native cycle uses canonical body histories and shared phase/noise; six
mode changes and both concrete reverse effects reach inspected diagnostic views.
The separately identified later scene-getter observation does not complete the
physical frame bridge. Original proofs, normal/sanitizer checks and review are in
`evidence/chariotLifecycle49V/`. Authentic spawn, contacts and full physics remain
open;49C remains unreleased and is not included.

The preserved [49U-ChariotRootResponse milestone](docs/chariot-root49u.md)
adds the complete admitted original Chariot-root update after internal animation
and engine response. Its high-speed physical-to-virtual transition seeds the
persistent response from current supplied physical scenes; subsequent source
response produces the displayed Chariot root. Exact phase, shared clock/noise,
authored coefficients and mode limitations are recorded under
`evidence/chariotRoot49U/`. Within49U, reverse transition remains outside admission;
49V closes that ordinary path. Physical/contact integration and authentic race
spawn remain open.

The preserved [49T-ChariotAnimation milestone](docs/chariot-animation49t.md)
closes the original internal Chariot motion and both hand IK chains using actual
Pod01 authoring. A 96-frame source-compared sequence feeds all three current
model palettes into the fixed TA diagnostic, with explicit supplied root/body
state and two controls isolating Chariot motion and IK. Original source proofs,
normal/sanitizer regressions, independent review and rendered pixels are retained
under `evidence/chariotAnimation49T/`. Full Chariot root updates, authentic spawn,
contacts, complete materials and gameplay remain open.

The preserved [49S-AnimatedEngineView milestone](docs/animated-engine-view49s.md)
shows the real Pod01 brake/fin animation and engine-root response in the fixed TA
inspection view. Original current-node palettes drive semantic local vertices;
the static Chariot, camera, texture associations and draw order stay explicit.
Four native samples and two frozen-motion controls have actual inspected pixels.
Source proofs, renderer evidence and bounded host measurements are retained under
`evidence/modelPalette49S/`. Authentic spawn, Chariot IK, contacts and original
material/VU rendering remain open.

The preserved [49R-EngineRootResponse milestone](docs/vehicle-visual49r.md)
produces the original normal-mode graphics container and both engine-root locals
from actual Pod01 authoring and persistent response/noise state. It follows the
real brake/fin motion producer in a 576-frame connected diagnostic. Complete
original-call comparisons, authored loading, independent review and host memory/
cost measurements are retained under `evidence/vehicleVisual49R/`. Chariot IK,
renderer consumption, authentic spawn and contacts remain explicit dependencies.

The preserved [49Q-EngineAnimation milestone](docs/engine-animation49q.md)
adds the original control-driven Pod01 engine-node motion producer. It preserves
persistent filters, source gates, actual MOT interpolation, ordered track writes
and lazy scene caches. The real engine brakes and fins now move from recovered
control outputs. Chariot IK, later engine-root response and authentic spawn remain
explicit open dependencies. The source proofs and recovery validation are under
`evidence/engineAnimation49Q/`.

The preserved [49P-SceneProjection milestone](docs/scene-projection49p.md)
adds fixed-node scene setters, exact dirty/lazy world-cache behavior, and the
source-phased secondary-scene-to-Chariot copy before later physics. All 41
focused suites pass normally and with ASan/UBSan. The persistent projection
replay keeps animation/container state and physical poses explicit; full engine
visuals, scene observers and authentic race spawn remain open. Existing body
admission gates are unchanged.

The preserved [49O-AuthoredPodGeometry milestone](docs/authored-geometry49o.md)
executes actual Pod01 model motion and live bounds to initialize secondary-body
mass/inertia, joint geometry and original tuning in the persistent two-body
diagnostic. Original source-word comparisons cover the full numeric path;
authored geometry changes affect the secondary trajectory while supplied initial
poses and external effects remain explicit. The fresh-model context, table
rounding assumptions and unresolved full vehicle/world dependencies are recorded.

The preserved [49N-AuthoredPodParameters milestone](docs/authored-initialization49n.md)
loads the actual Pod01 PHY and applies source-ordered main mass/inertia,
engine/aero, gravity and joystick tuning to the native two-body diagnostic.
All30 focused suites pass normally and with ASan/UBSan. Its11 supplied-state
scenarios preserve original preset/profile ordering and demonstrate real tuning
sensitivity. This parameter projection does not initialize the complete vehicle;
secondary geometry, initial poses, contacts and other loader effects remain open.

The preserved [49M-SecondaryBodyAdvancement milestone](docs/secondary-wrapper49m.md)
connects original ordinary/noisy secondary forces, the one-way constraint, and
secondary-before-main rigid-body integration in an explicitly supplied-state
pipeline. Its120-frame two-body trace preserves both bodies' frame caches,
shared noise/RNG state and late feedback. All26 focused suites pass normally
and with ASan/UBSan. Contact generation, authored initialization and full scene
assembly remain open; the trace is a bounded diagnostic, not a playable race.

The preserved [49L-PodSphereConstraint milestone](docs/pod-sphere-constraint49l.md)
implements original two-body constraint setup, acceleration correction and prepared
force application. It preserves the game's one-way load writes to the chariot body,
full vector/matrix lanes, setup caches and suppression rule. All 22 focused suites
pass normally and with ASan/UBSan. Bodies and ready scenes are explicit inputs;
secondary contacts, integration and complete rendered-vehicle assembly remain open.
The verified 49K views and their synthetic-state limitations are preserved.

The preserved [49K-BodyAdvancement milestone](docs/body-advancement49k.md)
connects original frame-only input/cache updates, substep selectors, engine/gravity/
aero and root-body scene integration into persistent motion. Its 180-frame trace
is compared with original instructions and uses explicit synthetic post-player,
world/contact and no-hit-query states. It is a bounded main-body diagnostic, not
an original free-flight vehicle or a playable race. Two same-camera TA/Pod01 views
show the verified pose evolution with unchanged49H texture/shading limits. The
new API admits nearest-even arithmetic; a pre-existing historical48H downward-
rounding GCC zero-sign issue is retained and documented, not silently repaired.
49C remains excluded and unreleased; collision/reset boundaries remain unresolved.

The preserved [49J-ClockQuantization milestone](docs/clock-quantization49j.md)
closes the numeric clock producer from supplied timer samples through the original
wrapped accumulator, software-double/sticky conversion, quantized timestamp and
49I frame timing. It preserves the original precision and saturation behavior,
including a verified double-rounding quirk. Connected original-input and authored
TV-camera diagnostics demonstrate the effects across stalls, wraps and long
counter sequences. Hardware acquisition and IRQ atomicity remain external.

The preserved [49I-FrameTiming milestone](docs/frame-timing49i.md) supplies the
missing native post-clock elapsed-time producer: original timestamp bootstrap,
unsigned delta conversion, uncapped history, both upper caps and staged frame
state. Its 28-byte caller-owned state is allocation-free. Original words now close
the static elapsed route through the registered GameObjects to PodRaceEvent,
without substituting an OS clock or inventing pause/catch-up behavior. Quantized
clock production, live topology and opaque callbacks remain explicit boundaries.

The preserved [49H-TexturedDiagnostic milestone](docs/textured-diagnostic49h.md)
extends the same TA/Pod01 view with cached first-pass/slot-one textures and authored
UVs. It preserves the 49G camera, geometry and explicit stationary placement. The
diagnostic samples 26 original cached images on 286 surfaces; 74 unsupported TA
surfaces retain the declared neutral fallback. Original material programs remain
unexecuted. The separate 49G unlit images and all inherited production are preserved.

The preserved [49G-VisualDiagnostic milestone](docs/visual-diagnostic49g.md) renders
an inspected Linux frame from the authentic TA static geometry, three Pod01 bind
parts, and the verified49F camera matrices. The repeatable headless Mesa driver
uses two unlit diagnostic colors, an explicit stationary Pod placement, and a
separate wire overlay. It draws 73,409 triangles at 640x480 with one CPU affinity.
The result closes a real asset-to-camera-to-pixel path; original material behavior,
visibility, animation, integrated gameplay and target performance remain unproven.
Only scoped original cache assets were imported; no historical source was overlaid.

The preserved [49F-CameraView milestone](docs/camera-view49f.md) produces actual
TV-camera position/orientation/FOV and original core view, projection and combined
matrices for the ordinary build-enabled caller context. It includes source-proven FOV smoothing and nearby-vehicle shake with
explicit resolved player pose and ordered local scene samples. Selection and
camera-type scheduling stay separate; PS2 viewport/rendering adjustments and
follow-camera height/collision providers remain outside this numeric subsystem.
It starts from verified49E and keeps the separate49C work unreleased.

The preserved [49E-TVCamera milestone](docs/tv-camera 49e.md) loads the authentic TA
TV-camera configuration into caller-owned immutable records and ordered links.
The recovered native selector chooses camera positions from supplied player
positions, preserves the original delayed-switch behavior, and uses the closed
original random generator. Full camera transforms/world-dependent updates remain
outside this stage. It builds on verified 49D without incorporating 49C code;
the separate 49C work remains unreleased.

The preserved [49D-ControlPresets milestone](docs/control-presets49d.md) adds original
controller-preset initialization, selection and stateful preview. Two fixed slots
supply the actual action codes and independent configuration mode to the existing
ready-packet/held-input/engine-aero path, plus caller-owned player-event action
rows. This branch starts from verified49B. The separate 49C work is not included
and remains unreleased. Hardware transport and a complete playable tick remain
unimplemented.

The preserved [49B-WorldIndex milestone](docs/world-index49b.md) retains compact
immutable authored-world metadata: exact placements/resource references, direct
zone ranges and ordered incident portals. It uses twelve real retained worlds
and supplies allocation-free typed views/caller-owned copies. Provenance remains
separate from resident records. This adds metadata retention, not a visibility,
respawn or collision policy. [Action2255B0 remains blocked](docs/reposition49b-blocker.md)
by missing spline graphs and unresolved initialization/reset effects.

The preserved [49A-PlayerEvents milestone](docs/player-events49a.md) adds concrete
manager-held source selection and the original player handler's query, gate,
state-write and callback ordering. An explicit player child binding now handles
both press and release. Required action helpers remain synchronous external
bindings: missing effects report unavailable before dependent writes, rather
than claiming their larger gameplay behavior is recovered.

The preserved [48Z-EventDispatch milestone](docs/event-dispatch48z.md) adds the
original supplied-event manager loop, ordered child routing and the normal
category2 application bridge. Mutable callbacks retain original ordering and
partial effects; device transitions can now enter caller-owned native dispatch
storage. Concrete child handlers, transport/update/rebuild and full gameplay
scheduling remain explicit missing bindings.

The preserved [48Y-DeviceHeld milestone](docs/device-held48y.md) closes the
player held-query boundary for supported numeric codes with concrete native
packet-produced state. It preserves digital/pressure thresholds, exact record
copies, readiness/failure distinctions and ordered transition outputs. Transport,
queue dispatch and full gameplay scheduling remain outside this bounded step.

The preserved [48X-PlayerInput milestone](docs/player-input48x.md) adds original
player-axis deadband conditioning, the proven DualShock packet-axis inversion,
ordered action queries and overrides, and a connected supplied-input path into
engine/aero bindings. Physical button names, device transport and the complete
gameplay scheduler remain unresolved.

The preserved [48W-ControlBinding milestone](docs/control-binding48w.md) connects
the recovered controls to the existing engine/aero input fields, preserving
original mode/state branches and prior fields. It adds a typed adapter and
complete shaping/filter/binding composition. Linux normal and ASan/UBSan checks
pass; physical controls and the full vehicle scheduler remain unresolved.

The preserved [48V-InputShaping milestone](docs/input-shaping48v.md) recovers the
original nonlinear input polynomial, pre-quantization state and asymmetric
quantization, composed with the corrected rate limiter. It is allocation-free
and verified against exact original instructions on Linux. Physical control
bindings, engine/aero mapping and full scheduling remain unresolved. The separate
collision buffer-capacity contract remains unresolved and unchanged.

The preserved cloud 48U milestone adds the [grouped limited-list type3 query](docs/group-query48u-linux.md),
including source-order shared shape updates, member identity in known query
callbacks, mixed ordinary/group visits and transactional failure handling.
The complete retained release and ASan/UBSan sweep passes 6,711,303 checks per mode.
The [48T shape/validation work](docs/group-shape48t-linux.md),
[48S collector](docs/candidate-collection48s-linux.md),
[48R handler](docs/vehicle-contacts48r-linux.md) and
[48Q detector](docs/spring-detector-pipeline48q.md) remain preserved and revalidated.
Runtime storage is caller-owned, serial and allocation-free. The grouped query
entry is not yet wired through the detector/vehicle composition; live ownership
and target performance remain unverified. Current Windows/Xbox builds are untested.

The new runtime is **not yet a playable game port**.
The existing Windows game and PCSX2 reference remain under `work/` unchanged.
Checkpoint46G in `outputs/` describes that reference and its recovery chain.

Historical compatibility-runtime result: [47U](docs/combined-race47u.md) records the
combined scheduler/diagnostic change at 5.77 to 10.05 emulated VBlanks per second
in one matched race window, with identical saved RAM, audio and final picture.
This is not native gameplay FPS; the native full-race benchmark remains pending.
The [47T checkpoint](outputs/RacerRevenge-Checkpoint47T-CombinedSchedulerStartup.md)
and later checkpoint manifests preserve exact recoverable source/evidence hashes.

The existing compatibility-component checks target single-core execution,
32-bit pointers and the stock Xbox's constraints. The planned native PC game
uses x64 for desktop development and Win32 checks for shared-code portability.
Xbox implementation and packaging follow the validated PC path. No Xbox frame
rate has been measured. [47W completion](docs/reference47w-completion.md) records
the finished reference checks without claiming a new performance ratio.

- `src/core`: historical compatibility components and reference helpers.
- `src/assets`, `src/game`, `tests/native`: native data, recovered game logic and their checks.
- `platform/windows/native`: independent native asset viewer.
- `tests/core_suite.cpp`: the same behavioral checks on Windows and Xbox.
- `platform/windows`: console runner; both x86 and x64 builds.
- `platform/xbox`: nxdk diagnostic XBE; no fabricated game behavior.
- `tools`: reproducible build, inspection, and recovery tools.
- `docs/xbox-migration.md`: architecture, acceptance gates, and unresolved risks.

Build the Windows checks with `tools/Build-Windows.ps1`. The Xbox build uses a
pinned, project-local nxdk checkout; see `tools/Build-Xbox.ps1` when provisioned.
Build logs and binary inspection results are evidence, not gameplay acceptance.

Build the independent native application and its checks with
`tools/Build-NativePc48O.ps1 -Architecture both`. Launch `racer_native_pc.exe`
without arguments for the built-in importer. Inspect a converted mesh using
`tools/Run-NativePc48E.ps1 -Mesh evidence/worldScene48E/ta-static-placed-neutral.rrmesh`.
Use `-Scene -Mesh <converted.rrscene>` to inspect semantic scene data.
Use the application's `--cache <path> --container <original-container> --member
<original-member>` selection to inspect native material/texture bindings.
The runner creates a bounded offscreen capture and a fresh evidence directory.

## Focused cloud/Linux spring checks (48K)

The generated-contact coordinator is now integrated through an allocation-free,
serial API with caller-owned bounded scratch/output storage. Read
[the headless coordinator milestone](docs/spring-coordinator48k-linux.md) for
scope, evidence and the original-Xbox-oriented memory contract. This is a
numerical gameplay component, not a complete vehicle update or a playable race.

Run `python3 tools/test_native_spring48k.py --output-dir ../spring48k-check`
from this source tree. The output directory must be new/empty and outside it.
The Python runner needs an existing GCC/Clang-compatible C++17 compiler; it
downloads nothing and runs five focused tests serially. The equivalent opt-in
CMake path is `-DRR_NATIVE_SPRING_ONLY=ON`; the Windows/D3D11 target is unchanged.
Missing frozen evidence is a blocked test, never a silent skip.

[48L](docs/spring-responses48l-linux.md) adds point contacts, original spring
force and secondary velocity response, with independently tested mixed-list
ordering and zero runtime allocations. Live query production and full vehicle
scheduling remain future work.

[48M](docs/terrain-workspace48m.md) adds bounded numeric terrain preparation
and a no-allocation query path over the real native collision meshes. Run
`python3 tools/test_native_terrain48m.py --output-dir ../terrain48m-check` for
the focused Linux suite. World registration/candidate production and full
vehicle scheduling are still outside this validated boundary.

[48N](docs/spring-query48n.md) recovers generated-record append/reset and static
spring callbacks, composed with an explicit ordered one-pair query. Every hit
runs the callback with cumulative nearest-pair data; record saturation and stale
fields follow the original. Run `python3 tools/test_native_spring_producer48n.py
--output-dir ../producer48n-check`. Frozen original instructions and explicit
synthetic inputs provide differential evidence; saved live detector binding,
world registration, multi-pair scheduling and a full vehicle tick are not verified.

[48O](docs/spring-detector-map48o.md) recovers bounded detector point/scene-cache
and AABB preparation, conditional first endpoints, and type3 world-vector
staging. A separate type2 sphere-preparation helper stops before registration.
These are distinct source branches; the detector's type3 data is not connected
to48N's type2 query by assumption. Run `python3 tools/test_native_detector48o.py
--output-dir ../detector48o-check`. The [proof](docs/spring-detector-proof48o.md)
records the corrected scalar MIN/MAX signed-zero ordering and explicit limits.

The [48O signed-zero audit](docs/minmax-audit48o.md) also corrects the earlier
control filter, spring force clamps and secondary velocity response, along with
the active original-word executor. Historical checkpoints remain unchanged;
current tests include explicit original-word output-sensitive zero-sign cases.
