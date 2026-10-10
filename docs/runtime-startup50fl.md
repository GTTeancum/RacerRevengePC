# Native development startup 50FL

`tests/native/runtime_startup50fl_fixture.h` supplies a native-only cold owner
and authored parameter binder for the interactive development client. It does
not include an exporter, source-word interpreter, reference Machine, or saved
runtime state. The original test/oracle files remain separate and unchanged.

## Join

In the interactive executable's main translation unit:

- Include `runtime_startup50fl_fixture.h` and `runtime_allocations50fl.h`.
- Remove the `frameImpact49Y/impact_join49y_fixture.h` and
  `frameBridge49W/connected/authored_inputs49w.h` evidence includes.
- Use `namespace f = runtime_startup50fl`; `f::Fixture x(0)`, `f::project`, and
  `f::cut` keep their existing call shapes.
- Replace `c::e::readBytes` with `runtime_startup50fl::readBytes`; the old `c`
  namespace alias can be removed.
- Replace `frame49w_authored::bind` with `runtime_startup50fl::bind`. The return
  value is directly `rr::game::AuthoredBinding49N`, so use `authored.selectedClass`
  and `authored.usedAuthoredClassValues`, or pass `authored` to the cooling binder.
- Do not also include the old fixture's allocation instrumentation. The new
  independent header preserves ordinary C++ allocation counting on the calling
  thread only; it does not describe whole-process, driver, or GPU allocation.

The header does not require a new production `.cpp` module. Its native leaf
closure is already used by the preceding client: PSG/MOT/VUT readers, scene
projection, `initializeChariotIk49T`, animation filter initialization, visual and
Chariot-root authoring, fresh pod geometry, authored physics projection,
secondary activation globals, PodSphere initialization/setup, and pod-shape
COL decoding. In particular, retain `chariot_ik49t.cpp`, `source_acos49t.cpp`,
`model_geometry49o.cpp`, `model_bindings49o.cpp`, `model_motion49o.cpp`,
`model_bounds49o.cpp`, `pod_model_geometry49o.cpp`, `authored_geometry49o.cpp`,
`authored_parameters49n.cpp`, `pod_physics49n.cpp`,
`vehicle_visual_authoring49r.cpp`, `chariot_root_authoring49u.cpp`,
`secondary_activation49v.cpp`, `secondary_body49m.cpp`,
`pod_sphere_constraint49l.cpp`, `scene_graph49p.cpp`, and the existing shape/
collision-decoder dependencies. Native mathematical source-order functions
such as `sourceAcos49T` are algorithms, not CPU or device interpreters.

## Construction and asset provenance

The same three PSG hierarchies, three VUT maps, six MOT assets, Pod01 PHY, and
podboxf COL are read through the existing checked native parsers. Model topology
is assembled from PSG parent records. IK coefficients are computed from that
fresh authored graph by `initializeChariotIk49T`; they are not loaded from a
captured graph or coefficient snapshot. Native calls then compute the visual,
root, physical, gravity, secondary geometry and joint parameters.

The owner preserves the previous client's explicit supplied development body,
cache, flag, model-ID, random-seed and lazy-record inputs. Temporary supplied
poses are still replaced by the existing authored-grid/ground initialization
before gameplay. This change does not claim to recover the full application,
player or vehicle creation graph, nor replace those remaining supplied inputs.
It adds no new geometry, constants substituted for computed results, or replay.

The previous `frame49w_test::Fixture` retained an animation reference Machine
for the entire session. Its two primary vectors alone reserve 5,652,480 bytes
(5.390625 MiB), before instruction blocks, read maps and trace storage. The
native owner retains no such memory. It also removes unused duplicate visual,
animation and lifecycle test owners. Exact peak/process memory and launch time
are not inferred from this structural saving.

## Focused validation

`tests/native/runtime_startup50fl_check.cpp` defaults to native-only construction
and binding. Defining `RR_STARTUP50FL_COMPARE_BASELINE` explicitly adds the old
retained constructor and authored reference binder to that separate executable.
The comparison covers fresh state, parameters, IK records, topology and bindings
(normalizing owner addresses), motion maps, selected authored binding, and all
resulting canonical state/parameter/gravity/secondary records. No frames are
advanced, no game or graphics host is launched, and no baseline files are
changed. This reference-enabled executable must never be linked into the
interactive client.

### Result

The focused retained-baseline check passed all 21 checks on 2026-10-08. It
compared all constructor and bound canonical records bit-for-bit, plus computed
IK records and address-normalized bindings. One retained binder execution
accounted for 2,578 reference instructions, now absent from runtime startup.
No broad regression or source-word sweep was run.

In that single diagnostic run, native construction took 1,986 microseconds and
the retained reference-backed constructor took 14,064 microseconds. This is a
single observation, not a launch-time benchmark. `sizeof` the new owner is
107,904 bytes, excluding its actual authored asset buffers. The native header's
compiler dependency closure contains no evidence, oracle or exporter headers.
The standalone native-only build also passed its 7 focused checks, and its
linked symbol audit found zero original/reference/oracle/Machine symbols.
The coordinating integration task owns compiling and validating the final
interactive join.
