# Source-only export boundary

This checkpoint intentionally omits original disc files, converted asset caches,
images/audio/video, executable and object files, build products, memory dumps,
work directories, evidence outputs, and unrelated Cars Tools reference material.
Vendored native JSON, zlib, and PCSX2 reference source retain their license notices.
The native runtime numeric table `src/game/model_motion49o_tables.inc` is retained
because it is a required runtime lookup table.

The following generated test headers contain original instruction words or
collision geometry and are omitted even though their filenames look like source:

- `tools/assets/collision_placement_fixtures48i.h`
- `tools/assets/collision_runtime_words48h.h`
- `tools/gameplay/aero_words48e.h`
- `tools/gameplay/body_scene_words48g.h`
- `tools/gameplay/body_wrapper_words48h.h`
- `tools/gameplay/contact_words48g.h`
- `tools/gameplay/control_binding_words48w.h`
- `tools/gameplay/control_filter_words48b.h`
- `tools/gameplay/control_presets_words49d.h`
- `tools/gameplay/device_held_words48y.h`
- `tools/gameplay/event_dispatch_words48z.h`
- `tools/gameplay/forces_words48d.h`
- `tools/gameplay/input_shaping_words48v.h`
- `tools/gameplay/physics_words48c.h`
- `tools/gameplay/player_event_words49a.h`
- `tools/gameplay/player_input_words48x.h`
- `tools/gameplay/query_fixture48i.h`
- `tools/gameplay/query_fixtures48h.h`
- `tools/gameplay/terrain_query_fixtures48j.h`

The source for the associated generators, decoders, and test harnesses is retained.
These files are not synthetic replacements. Source-comparison tests that include
them need the local generated headers and the corresponding original inputs.
Existing scripts document their expected input paths and output names; some also
require historical evidence manifests that are not part of this export. A plain
checkout therefore does not provide all inputs needed for every historical check.

Do not commit generated original-data headers or the inputs used to create them.
The `.gitignore` includes explicit protections for these known paths. No fixture
regeneration or software tests were performed during publication.
