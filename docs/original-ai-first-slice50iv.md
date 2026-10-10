# Original native AI: first joined driving slice

## Status

This feature originated on GC source `53d44876`. Guarded source `93201627` compiled, linked and passed one bounded direct-GC first-slice observation on 2026-10-10, with Clegg as the sole original-AI racer. See [current validation](current-validation.md) for the measured state/brake milestones and limits. The optional path remains disabled unless `RR_ORIGINAL_AI_FIRST_SLICE=1` is supplied; all-eight original AI is subsequent requested work.

The feature targets the actual original racer in native bank slot 1. With the default human that is POD03/Clegg, whose original profile supplies `StartRaceNoCombatTime=9`, `CollisionRadius=141` and `MyMinSpeed=200`. Every other actual racer remains present and observed; their existing HOST drivers are unchanged. This is a bounded original-AI slice, not a claim of complete original AI or original race-root scheduling.

## What is connected

1. **Cold identity and authoring.** The chosen racer's real RES `.ai` member, PHY, model geometry, and the actual MAP/AI splines and track TUN are read. AI handling is selected before binding, retaining full authored steering and original engine positions. Source construction supplies `58=1`, `FE8=0`, the original timer/list initialization and route-choice sentinels. GC's AI creation class is C via track ID/order 10; this is independent of the PHY's track-name class policy. FE4 compares against the fresh original GC course-record pod 17, never the selected human pod. Loaded original save/profile overrides are not represented.
2. **One process RNG/noise owner.** The original C-runtime initial raw state is 1. The entire shared noise context is excluded from per-racer snapshots, including its tables and readiness. Existing consumers and AI boost use the same live owner; per-racer XOR seeds are suppressed in this optional path. The application owner survives native session restarts. The sequence is the order of represented native consumers; original whole-program chronology is not claimed.
3. **Real navigation and neighbors.** `16FFE0` advances AI and map gates from actual prior CA0/current local scene coordinates. It writes the real current/previous node words, map lap edge, actual primary-zone ordinal, route-choice history and the SAME boost cooldown reset on entering `BoostScale>1`. `170B50` observes all actual racers in manager order, with genuine node, position and cached-speed fields. No empty-list stand-in, nearest-waypoint substitution or distance sorting is used.
4. **Original state and decisions.** Orientation/stall, startup state, packing and repair run in original dispatcher order. Cold state 0 becomes 4. State 4 uses the fixed 200-unit planner, genuine neighbor-following target and avoidance adjustments. Only genuine CB8 greater than 5 changes it to state 2. Ordinary state-2 direction uses the original two lookahead targets and signed cached speed; original target selection retains node speed, live packing, conditional pace, minimum and full-speed override.
5. **Original boost and braking.** The real native `173180` provider runs when its caller requires it, preserving source early-return behavior, cooldown and shared RNG. A genuine decline reaches `173770`. Raw steering and brake action B80 remain independent, then the actual non-player binder bypasses only player nonlinear shaping. It retains smoothing, rate filtering and model binding. Live X rate B94 is set to 4 at each AI entry. This permits original simultaneous steering and global braking, which the HOST preset-4 axis mixer cannot represent in every combination.
6. **Actual physics and rendering.** Distinct AI frame read/bind entry points avoid the player packet/reader. The same terrain probes, engine/aero/body integration, scene graph, animation and rendering continue. AI physical/virtual chariot selection and static environment-impact scaling are joined from their actual owners. During cold grid setup the shared started byte is 0 for the entire fleet; the original outer impact skip now reaches the genuine plain/base continuation. The declared HOST GO edge publishes started byte 2 to the fleet before any racer update.
7. **Real manager-derived inputs.** Native `1AF9C0/1AFF50/1B0400` numeric cuts supply CB0/CB8 and all-racer remaining CC4/rank CCC. They consume real map C88, authored node distances/gates, physical scene positions, source lap words and flags. The human's original packing-reference update uses those ranks before AI control consumes FF8. HOST scoreboard place and centerline progress never feed original AI.

## Scheduling boundary

The native session still uses the existing HOST accepted-world scheduler. It performs real controller/physics work serially, a bounded dynamic-contact pass, then native manager timing and ranking before the HOST standings observation. The player's packing-reference writes are transported at the serial-arena boundary after that player's update and before the selected AI update; the projection reads the same prior manager rank and other-racer flags. Original application root-update ordering, complete player-update semantics for the other HOST racers, complete original roster creation and complete original event delivery are not claimed.

Prepared time is the same native frame input used by the existing physics path. CB8 is the native manager float result, not elapsed wall time, HOST double race time, frame-count multiplication or a forced startup-state transition. Source fresh-profile low-five-bit lap count is 3 (`17E000/17E060`). Source startup clears gate `433CB0`; the optional fresh application owner retains it, and a nonzero value skips AI dispatch. Static initialization reaches `3C9410` and writes fallback `433C70=(0,0,0,1)`.

## Exact stops and limits

- Eligible combat target search stops at `1705C8`. Default Clegg therefore reaches a genuine boundary at CB8 >= 9 unless an earlier original guard suppresses combat. Combat hover/attack is not replaced by ordinary driving.
- A new multi-successor branch choice stops at `1719B0`; a genuine existing choice can still be honored.
- State 5 with stall timer below 1.5 keeps state 5 and stops at reset dispatcher `16EA94`. The branch at `16EDA8` skips BOTH the clamp and state-2 store. The earlier isolated prefix checkpoint's unconditional transition was corrected in the cohesive copy.
- Any actual dynamic contact involving `58=1` is detected by the unchanged pair calculation and rejected before any pair result commits. Canonical body20C/contact-list and D34 partner/dynamic-damage ownership remain unjoined. Empty/no-hit pair passes are retained; collisions are not disabled or reported as misses. This prevents an AI racer impact from being silently processed later as a static wall impact.
- A lap/finish/retirement continuation, unsupported vehicle state, missing live owner, malformed route or unsupported numeric domain fails explicitly. Existing undertrack/reset, GC LostAI triggers and incomplete-world boundaries remain. No safety reset or fake pose is supplied.
- Original full-race AI behavior, post-crash recovery, all opponents running original AI, full-track success and performance parity remain unproven.

## Bounded acceptance for the parent-owned later test

Use a separate integrated candidate, actual GC assets, the existing explicit incomplete-GC admission and mode 2, with the new option enabled. Do not treat a source-only backup as an accepted build.

The smallest useful run must show the selected real `58=1` racer entering state 4, taking real terrain-supported physics/render steps, observing every actual racer, and transitioning to state 2 only after native CB8 exceeds 5. Observe raw steering, B7C/B80, rate B94=4, desired speed, source CC4/CCC, FF8/FFC and list counts. A brake command must be bound into the native model simultaneously with steering when the source decision requires it; merely calculating a standalone boolean is insufficient.

The `original_ai` telemetry reports these owner values and command counts. It distinguishes dispatches/commands from successfully completed physical frames. The run may terminate at a genuine unimplemented boundary before this goal; that is a failed acceptance with a concrete source PC, not permission to bypass the boundary. Longer course or post-crash claims require the stopped owner to be implemented and independently accepted.

## Source anchors

- AI constructor/create/profile: `16E260`, `16E450`, `16E630`; frame/dispatcher/state: `16E8C0`, `16E9C0`, `16EB00`
- Navigation/crossing/list/choice: `16FFE0`, `170230`, `170380`, `170B50`, `171A90`
- Packing reference and scale: `1A0CA0`, `1A15EC`, `171B70`, `171C60`
- Desired position/direction/startup/steering: `171FE0`, `172380`, `1726F0`, `172E60`
- Boost/repair/ordinary brake/orientation: `173180`, `1734C0`, `173770`, `1739D0`
- Manager timing/ranking: `1AF9C0`, `1AFF50`, `1B0400`
- Control/lifecycle/static impact: `217DE0`, `2181C4`, `21A660`, `223030`, `222770`

All recovered source evidence is retained separately from publishable native source. No guest execution, bytecode/VU emulation, captured trajectory playback, ee.bin input or held49C/48U path is used.
