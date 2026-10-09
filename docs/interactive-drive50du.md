# Interactive Linux vehicle development client 50DU

`tests/native/terrain_interactive_drive50du.cpp` connects the existing native
vehicle frame sequence to a live keyboard and an SDL2/desktop-GL window. It is a
development client, not a complete game. Its cold asset/state setup still reuses
the retained test fixtures and their instrumentation. Allocation counting is
explicitly thread-local in this client: cold checks describe the calling thread,
not whole-process allocations, driver workers, or GPU memory. The existing
`terrain_offscreen_drive50dh.cpp` remains a separate offscreen harness.

The coordinating task reports the saved 50EV host-restart client passed focused
cloud live throttle, differential steering, braking, zone crossing, restart and
quit checks. The new 50EU navigation join below is source-written and manually
inspected only; its author ran no build, test, executable or game launch for this
join. The coordinating task owns its focused integration checks. Earlier native
undertrack stops remain valid evidence of the still-open reposition path.

## Build and launch

The repository's `tools/prepare_interactive_build50dv.py` prepares a standalone
compiler argv JSON; it does not build or launch the client. The coordinating task
also provides the optional Linux CMake target `rr_interactive_vehicle50du` under
`RR_BUILD_INTERACTIVE_CLIENT=ON`. Build/run only when authorized under the shared
test budget.

The R addition requires `controller_actions50ec.cpp`, `event_dispatch48z.cpp`,
`player_events49a.cpp`, and `repair_authoring50a.cpp` alongside the existing
controller/frame dependencies. Live registration additionally requires
`live_zone_membership50en.cpp`, `portal_geometry50eo.cpp`, and
`vehicle_live_query50eq.cpp`. The navigation join additionally requires
`track_navigation50eu.cpp` and `undertrack_reposition50et.cpp`, with their existing
`source_acos49t.cpp`, `chariot_scene49u.cpp`, `track_start50ad.cpp`, portal and reset
leaf dependencies. The coordinating task owns those build lists.

After an authorized build, run the executable with the repository root as the
working directory and one argument:

```sh
/path/to/rr_interactive_vehicle50du \
  evidence/staticType2Producer49X/original-word/native-fixtures.json
```

The source-only compiler generator may name its output executable differently.
Window mode has no output-image argument or fixed frame limit. Escape or the
window close control ends the process. F5 explicitly restarts the whole session
from its original authored-grid cold setup without ending the process. Window
mode performs no screenshot/readback.

For the explicitly selected live-stdin/EGL host, add
`--headless-live CAPTURE_PPM`. Keep stdin open and enter newline-delimited
`down W`, `up W`, `down Up`, `up Up`, `down R`, `up R`, `release`, `capture`,
`restart`, or `quit` commands. The other declared keys have equivalent down/up
commands. A capture uses a committed renderer image. This mode has no window
swap; it never claims desktop presentation. See
`platform/linux/native/game-host50er.md` for the
bounded command parser and terminal-stream semantics.

Window mode requires a Linux graphical desktop, a runtime SDL2 library, and desktop OpenGL 3.2 or newer
with a core, double-buffered, non-MSAA drawable. The host explicitly
requests swap interval zero; host pacing is separate. Missing dependencies or
unsupported context requirements report an error instead of silently selecting
a headless renderer, installing anything, or displaying substitute geometry.

Keep the existing asset tree in place. Cold setup reads the retained corpus,
Pod01 meshes, source animation/physics data, TA archive, effect-control data, and
fixture/oracle support files through the same relative paths as 50DH. It is not
an independently packaged asset importer.

## Declared development controls

These are explicit keyboard mappings for this client, not recovered original
game bindings. Keys are SDL physical scancodes, so letter positions follow the
physical W and S locations on a US-layout keyboard.

- W: left-engine throttle, packet byte 7 = 0
- S: left-engine brake/aerodynamic axis, packet byte 7 = 255
- Up arrow: right-engine throttle, packet byte 5 = 0
- Down arrow: right-engine brake/aerodynamic axis, packet byte 5 = 255
- W + Up: throttle both engines
- S + Down: brake both engines
- Unequal left/right throttle or braking supplies differential steering
- Opposite keys on the same engine axis center that axis at 128
- Hold R: native repair-held input, selected preset `flag69C` (button code 4)
- F5: explicitly destroy and recreate the whole development session
- Escape: quit the host application

Packet bytes 4 and 6 remain centered. The client explicitly selects native
advanced preset 4 and routes the composed packet through 50CX/48Y/48X and the
existing native shaping/filter/binding path. A keyboard axis changes immediately
at its input boundary; native rate filtering still controls the resulting drive.

The initial button bank is released. R is SDL physical scancode 21; the remaining
fifteen action-button slots stay explicitly `Released`. Escape and F5 never become
vehicle actions. Reposition (`action6AC`, button code 8), boost, menu, and other
unsupported native actions remain unmapped. The selected repair/action codes
are checked rather than inferred from a physical controller label.

Every active host poll samples through 50EC/50CX/48Y, even when no simulation
tick is due. Its complete transition batch is appended to a fixed 32-event
48Z queue, dispatched, then acknowledged before another sample. Dispatch uses
the ingress's live record bank and the actual 48Z joystick application bridge
with an explicit single enabled player child. Press reaches `handlePlayerPress49A`;
release reaches the native player release router. The player has no event-handling
descendants in this host. Category0 context/debug and category1 auxiliary input
sources genuinely do not exist here and have no fake callbacks. The player
event state is persistent supplemental storage for this existing player, with
field40 refreshed from its canonical frame owner and explicit source/action-row
indices. This is supplied development-host topology and cold state, not a
recovered full application/player constructor.

Repair is a held-state operation, not a 49A reposition effect. On an R press,
49A's separately configured action6AC query misses and its two absent-category0
queries miss normally. At the existing frame entry, 48X reads the same live bank,
uses the existing application/damage gates, and writes B9C bit1. The existing
49W `PostImpactConsumers` stage then calls unchanged `repairVehicleImpacts50A`
with canonical damage, timer, component presence, frame factor, and CF4. Holding
R on an undamaged pod does not force damage or repair activation. Any unexpectedly
reached 49A effect returns unavailable and stops the client with native route/
effect diagnostics; it is never reported as a successful no-op or replayed.

The old cold fixture left CF4 zero. The client now reads actual Pod01 repair
authoring and applies the retained pre-profile 215F3C..215F68 selection in the
already-explicit no-profile-overwrite context. Required min/max repair values
are .0055/.014; base/max ability endpoints are both 1. The six separate binary32
subtract/multiply/add operations initialize CF4 to word `0x3c656042` (about
0.014). Equal unit endpoints make the finite upgrade fraction irrelevant;
different endpoints or a profile-overwrite context reject. This does not select
the separate class A/B/C value .013, import a save profile, or change the 50A
repair formula. The retained source proof is in
`work/local50A/impact-aftermath/provenance.json` and `original-words.json`.

These are sampled-device semantics: every transition emitted by 48Y is routed,
but a complete physical press/release cycle between held-state samples need not
produce an edge. No raw SDL event replay or sub-poll edge-capture claim is made.

## Frame loop and presentation

- A monotonic host accumulator supplies 16 ms ticks with `.016f` native frame
  factor, at most four catch-up ticks per host turn. This is a diagnostic pacing
  choice, not a recovered original frame scheduler. Excess elapsed host time is
  discarded and counted rather than producing an unbounded catch-up burst.
- Each tick preserves the native reader, control binding, graphics/effect/base
  sequence, one-substep scheduling, candidate collection, main/secondary contact
  response, integration, and movement-query/frame-tail order from 50DH.
- The old 160-frame scripted throttle/brake workload, end-of-script behavioral
  assertions, per-frame stderr trace, timing array, and final PPM capture are
  absent. Runtime phase/admission/error checks remain.
- Focus loss, minimization, a zero-sized drawable, or an unconsumed event backlog
  pauses simulation and rendering. The host clears held keys on blur/minimize;
  the runner publishes and dispatches a release-all packet when entering pause
  and rebases the host clock. Quit also dispatches a final release-all sample
  before cleanup. There is no accumulated-time burst on return. Native
  filter/state history is retained, not reset or teleported; the reader updates
  B9C again at the next actual frame entry, and repair never advances while paused.
- The window starts at 640 x 480 logical pixels. Two renderer targets are cold
  allocated for at most 1280 x 720 drawable pixels. Oversized/high-DPI drawables
  pause with a terminal warning until resized within both limits. Aspect ratio
  follows actual drawable dimensions. Targets never grow during the loop.
- The actual retained chariot and both engine meshes are prepared by 50CO and
  submitted through 50DS. The newer source explicitly selects each material's
  authored pass 1 / slot 1, largest retained mip, with linear filtering and
  repeat wrapping. Five exact retained textures cover 26 dense material entries
  (622,592 RGBA bytes by retained metadata). This is a diagnostic display policy,
  not original multipass shader behavior. Required missing textures reject;
  no proxy vehicle primitives, collision-mesh substitutes or fallback pixels. Rendering uses a world-aligned fixed-offset follow camera, not a
  recovered game camera.
- Only a complete, accepted renderer batch is followed by a host swap call.
  CPU-bank ownership is published after renderer acceptance. A failed submit
  stops without swapping its possibly partial back buffer. A successful swap
  call is not proof of display/compositor presentation or GPU completion.
- Application telemetry uses fixed storage and saturating counters, with status
  no more often than every five monotonic seconds while active, plus startup, resize-boundary,
  failure, and shutdown messages. The existing status line includes main/secondary
  primary zone refs, saturating cumulative portal crossings from committed 50EQ
  prefix results, and completed ticks where the existing B9C-bit1 repair-active
  indicator is true. The same counts are included at session shutdown. These are
  observations only and do not change native state. No frame history accumulates. The loop sleeps
  in short bounded requests when idle; native/driver/OS work is not latency-bound.
- Cleanup closes the renderer while its context is current, then the borrowed
  GPU texture owner, then the host. Exception paths preserve that order and report cleanup errors.

## Explicit host restart and temporary undertrack pause

F5 in window mode or the stdin `restart` command is a development-host command,
not source `2255B0` reposition or a recovered original respawn. It works during
ordinary play, a host pause, or the specific undertrack pause below. The runner
first composes and fully dispatches/acknowledges the existing 50EC release-all
sample. It closes the renderer, GPU texture bank, then host in that order. A
small outer loop recreates the complete session, including all native state,
portal owners, input owners, assets, renderer and clock, with the exact existing
authored-grid cold initialization and the same command-line arguments. The app
process stays alive; the window/context is recreated. No position clamp, partial
state reset or automatic restart is used. Cleanup or setup failures remain errors.

Only failure from `advanceVehicleBaseThroughEngine50X` with
`VehicleFrameError49W::ExternalCut` and source stage `0x228BBC` is retained as a
recoverable host pause. The client reports that original undertrack reposition
is still pending, stops all frame advancement, releases held input through 50EC,
and keeps polling for restart, capture, or quit. It does not rewrite the partial
native phase to Ready or resume that source call. All other source-cut, native,
input, renderer, or cleanup failures retain their error behavior.

While recovery-paused, no camera, draw batch or renderer submission is prepared
from the interrupted source state. `capture` can read only the last already
committed private renderer target and reports that target's completed tick. If
no image was committed, the request remains pending; no substitute is produced.
Restart discards the old session's pending capture. Escape/window-close/quit/EOF
still quit, including when a restart request shares the same input snapshot.

## Live spring-query membership

The client retains all nine authored World48F zones and their original portal
fields. The cold 50EP bridge parses four vertices with the existing source
binary32 parser and preserves authored vertex/portal order; 50EN prepares the
source reverse-order directional links through 50EO. No spatial sorting or
nearest-zone inference is used.

Terrain object refs 1..37 are unchanged. Main query ref 38 and secondary query
ref 39 extend every collector state array and lifecycle registration capacity.
Each query owns independent links/entries, sized to one primary plus the authored
portal count. A single caller-owned shadow workspace is reused serially. The
queries attach to `Second`, initial zone 1, with overlap radius zero, matching
the retained grid-start and collision-object source paths described in
`live-zone-membership50en.md`. Their initial bounds come from the already
initialized main190 and physical19C cold scenes through 48O; the live secondary
getter is not moved ahead of its source call site.

The source spring constructor mask is preserved for both filters:
`(flagsF8 & ~0x0e) | 2`. Their collection-object type tags are each query's actual
collision type tag, separate from shape tag 2. Both query owners are registered
without becoming static targets. Unknown collected identities still fail handle
resolution; there is no special list of owners to discard or invented filter.

Each substep retains the outer method2/method1 collection and merge before the
main spring prefix. It uses main's prior accepted query matrix/cached sphere
and E0 chain, including its primary and secondary zone memberships, rather than
the global first-zone link. Source 1223E4 calls 1202E0 directly; the type2 bounds
path consumes the existing query shape matrix, and 126EB0 consumes its cached
sphere. Fresh scene/sphere preparation is not moved ahead of this collection.
50EQ then prepares the actual current scene matrix/sphere, executes
120230/1202E0/280D40, validates its matching object/resource/matrix/revision/cache
receipt, and runs the already collected indexed query. The secondary follows its
source scene-getter position after main's query, executes its own fresh prefix,
and reuses the same candidate list. Its additive registered scene getter retains
the original 50J refresh/body-cache projection and scene gates while admitting
the matching nonnull membership; legacy zero-E0 guards remain unchanged.
Membership transfer never triggers a second collection. Accepted prefix/cache
changes remain committed if a later narrowphase stage rejects; the client stops
and reports the failed stage.

`UnsupportedLinkRemoval` remains an explicit reported stop, as do exhausted
fixed storage, malformed topology, unsupported scene gates, or rejected numeric
geometry. There is no fabricated removal policy, zero-E0 admission bypass,
collision miss substituted for a rejected update, or enlarged terrain set.
The late movement detector keeps its existing separate ordered 37-owner path.

## Live source navigation progression

The cold session now extracts both actual `tracks\mta.spl` and `tracks\ata.spl`
from TA.RES and retains separate 50EU owners. The complete ordered graphs,
source-prepared normal/widened gates, authored heights/widths/branch flags, and
`1ACF70` portal-derived node `+78` indices are loaded before the live query
memberships attach. The gate preparation shares the existing
`animationShared.libm` owner with 49T. Archive bytes and parser scratch are
released after cold setup. Node selection uses the source gate tests and
ordered scans; it has no nearest-waypoint or nearest-zone substitute.

The persistent progression words begin with exact full-initialization values
`C80=C88=1`, `C84=C8C=0`. The new persistent `CA0` vector begins at the original
constructor value `(0,0,0,1)`, so its first XZ test is the source's explicit miss.
It is not initialized from the grid, reset position, or C50 history.

Each live tick joins `1A0DA0` immediately after the existing `1A1A20` reader,
before control binding and graphics. It uses the actual `applicationByte58`
gate (`(byte & 3)==2`) or canonical `impactFlagsCD8` bit 0, source main
`probe348.field104`, the prior CA0, and current `lifecycle.main190.localC0`
translation. Successful progression persists even if a later native stage stops.
Then the source `228724..228730` capture copies only local XYZ from the actual
`animation.bodyScene288` binding into CA0, without refreshing a world cache,
before the existing `217DE0` binding call. This preserves old/current endpoint
order; graphics and simulation cannot replace the prior endpoint prematurely.
The existing preserving cut continues to cover unrelated intervening effects.

Five-second status includes `C80/C84/C88/C8C`, the C8C anchor's source zero-based
`node+78` index, and cumulative map/AI sequential and scan advances. Shutdown additionally reports admitted navigation updates,
gate skips, and CA0 copies. These fixed, saturating counters can include the
accepted prefix of a frame that later reached the retained undertrack pause;
they are not all completed-frame counts. No navigation advances occur during
host or recovery pause. F5/restart destroys these owners and begins a fresh
source-initialized navigation session along with every existing session owner.

This join owns progression/index data only. It does not execute lap callbacks,
claim full player-update recovery, create application visibility/collision
reset-table owners, or call `2255B0`. Its scene/phase/numeric admission failures
report the source PC and node and stop; there is no fallback to a guessed node.
The exact `0x228BBC` guard, release-all recovery pause, retained committed image,
explicit full-session restart and quit paths are unchanged. See
`track-navigation50eu.md` for source slices and the full 50EU contracts.

## Scope limits

The collision scene remains the retained 37 actual authored TA collider owners
with explicit supplied cold terrain registration order, nine authored zone
buckets, and live portal memberships for the two spring queries. This is not
full-world collision admission, recovered race-object registration, or broader
track coverage. Leaving that diagnostic collision scope can lose support; no
invented floor, collision fallback, teleport, or automatic reset is added.

The next source increment renders the actual retained49G TA world visual mesh
before the vehicle, in explicit neutral untextured mode. Its284 drawable
placements are broader than the37-collider physics scope;34 unresolved exact PSG
references remain undrawn. Visual appearance never grants collision support.
World GPU buffers load once; CPU track staging is released after successful
renderer creation. Terminal status includes position, engine drive values,
repair held/active state, and left/right damage; shutdown counts dispatched
controller press/release callbacks.
Native effect-control updates do not provide visible particle rendering. There
are no races, opponents, lap logic, audio, menus, original materials, save data,
live controller selection, or recovered original reset/respawn flow. The explicit
full-session host restart above is temporary usability support. Unsupported
native states or data errors stop with a diagnostic; only the precisely declared
undertrack boundary preserves the input loop for an explicit restart or quit.

## Host driving view update
The live client now requests sourceVertexColors for the actual TA mesh with explicit host exposure2 and geometric face lighting. This is an honest presentation policy, not recovered original track material/texture rendering. The host chase view uses actual main190 world heading with a52-unit rear offset,12-unit elevation and38-unit lookahead; near-vertical heading retains the last valid horizontal heading. The change does not touch native physics, navigation or original camera owners. Runtime verification is pending for these view changes.
