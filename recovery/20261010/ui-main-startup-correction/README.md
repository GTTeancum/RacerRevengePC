# Main-only cold startup correction

Status: staged source correction against f60d163. No build, syntax check, parser,
importer, test, or gameplay execution was performed. The active checkout is
unchanged. Root reviews/applies and owns the next eligible test gate,
2026-10-10T18:23:22.417511Z.

## Observed failure

The parent's one permitted native execution reached real original Main bind,
render and GPU submission. Its recorder correctly refused `world_ticks=1` and
quit before any capture. Existing log:
`/workspace/shared/recovery-20261010/ui-main-recording-reconstruction/main-ui-validation-01/game.log`.

The cause is the complete native race setup tick before host creation, not a
telemetry offset. `initializeGraphics` calls `advanceFrame` once for every racer;
it also composes/samples a neutral driving packet in human/mixed configurations.
The subsequent source rank, host field commit and mode1 initialization consume
that activated world. Resetting `completedFrames` would falsify actual state.

## Correction

Only `tests/native/terrain_interactive_drive50du.cpp` changes.

1. Main-only startup skips the entire existing race activation block, starting
   at `raceSetupFrame=true` and ending after initial race/rank/admission logging.
   Ordinary gameplay executes the unchanged enclosed body.
2. Main does not initialize source ranks, host race fields/countdown, initial
   standings, or mode1 manager. It does not pretend to have an empty completed
   race. Those owners remain genuinely uninitialized.
3. Initial `raceAudio->observe(race)` is likewise skipped for Main, since that
   API correctly rejects an uninitialized/uncommitted race. Original PCM assets
   remain loaded; no countdown/playback event is seeded from the dormant world.
4. Driving packet composition, controller sampling, host course-controller
   invocation and actual `advanceFrame` reject Main mode before any side effect.
   These are safety gates, not counter substitutions or no-op successes.
5. Main explicitly inspects each actual cold racer slot before host creation:
   completedFrames0, frameFactor0, native frame/action-call count0, source phase
   Ready from its cold constructor, original-AI dispatchedFrames0, and no
   controller transitions. Shared driving-input counters must also be0. Source
   observed/rank fields and host sampled/observed fields must remain unbound.
   No assertion changes a count, phase, field, rank or controller packet.

All real asset preparation before activation is retained: immutable original
rigs/hierarchies, handling constants, original-AI cold authoring, source grid and
native ground placement, model bounds, terrain/query/zone bindings, contact
owners, visual/cable/flame construction, dynamic actor registry, and opponent
draw storage. All later track/sky/original Main loading, exact CPU/GPU texture
banks, player mesh and racer archetype renderer bindings are unchanged.

## Downstream dependency review

- `native_racer_grid50hn.inc` / `placeColdGrid` operate only on unadvanced states
  and explicitly require Ready/frameFactor0/zero body velocities. They position
  real source geometry and refresh bounds; they do not execute a vehicle frame.
- `native_racer50HN::DrawOwner::initialize` only provisions actual mesh-sized CPU
  buffers and records immutable `VehicleDrawBatchInput50CO` descriptors. Its
  later `prepare`/batch path is not invoked by Main.
- `VehicleWindowGl50DS::create` requires loaded genuine `VehicleDrawAssets50CR`,
  textures/materials/cables and mesh descriptors. It never receives a vehicle
  state, transformed `VehicleDrawBatch`, rank or race observation. Therefore its
  real player/archetype ownership does not depend on the neutral setup tick.
- Original Main uses `submitOriginalUi`, not the renderer's gameplay `submit`.
  Thus no race HUD, native vehicle graphics bank or standings need publishing.
- `rankOriginalAiFleet`, host `sampleRaceField`/`commitFrame`, and mode1 admission
  are all inside the skipped race activation block. The live Main loop returns
  or continues before their later gameplay uses, and cannot start a race route.
- The retained `requireOriginalAiFleetReady` pre-loop admission only reads active,
  Ready, vehicle58, AI-owner-bound, initialized and manager-index fields. These
  are provided by the preserved cold constructors/grid binding; it does not
  require rankBound, a completed tick or observation publication.
- `advanceSourceAudioWorld` reads source rank only after a genuine world mix.
  Main never calls it and does not mix. Existing close paths reset providers
  independently and do not dereference the absent mode1 manager.
- `HostSessionMenu50IF` construction copies selected identities/settings; it
  does not borrow the race object. Main intercepts all events before its poll.
- The original Main application RNG and fire serial remain separate genuine
  Main-only process ownership, unchanged by this correction. No gameplay route
  or runtime consumer is newly admitted.

The Main recorder protocol and strict zero-world/input/mix requirements are
unchanged. No new build/run has established the corrected runtime outcome.
