# Shared host-announcer ducking integration

The 2026-10-09 Linux build and one native race/finish-continuation check passed
on source `79ece3b97e9e184a8e34c1c79b73f7137749c4c2`. Shared host-Dax
completion, ducking and restoration were observed. Pause, preemption and
restart remain unverified here. Mode2 and source type12 admission are unchanged.

## Observed focused result

Validation began at14:17:23 UTC: three units compiled,190 reused, link exit0.
One runtime launched at14:18:20 UTC and exited normally. The player finished
three laps in first place at160.928 seconds; all finish-continuation criteria
passed, including retained native activity, sealed results and post-finish
input rejection.

Five actual Dax starts produced five begins, ends and completions. Preemptions,
rejections and group failures were zero. Periodic telemetry observed the five
ducked groups at29/27/41/45/29, with Dax80 and auxiliary100, then the restored
levels45/42/64/70/45/80/100. Restoration followed real voice completion;
this run did not exercise pause, forced preemption or restart.

The mix finalized to174 seconds of PCM (SHA-256
`60a6445b3de9a97a2b42a80cece6b5f31441b7d854e41f83663f7c8cb146834a`).
Completed-render FPS ranged from51.3866 to62.6776. Binary SHA-256:
`6411a8673f85428c1ec7366a5fe7bbf020e91f16bebd2afd4ce74db62846ca81`.
The recording and counters do not establish listening quality, source type12
admission or complete original announcer behavior.

## What this joins

The live audio owner initializes one shared configuration from the recovered
fresh original reset: effects50, music100, character80, Dax80. This is an
explicit cold configuration choice for the current client, which has no loaded
user audio preferences. It is not a claim that existing saved preferences were
read. Initial source group baselines become45/42/64/70/45/80/100 in enum order.
This means source baseline attenuation applies even outside an announcement;
ducking further lowers the five affected groups with the recovered integer
truncation, and Dax stays at its configured baseline80.

All current native providers share this retained group-gain layer. The authored
49 catalog entries, four lap voices,16 material voices, four countdown voices,
14 opponent engine/binder voices and actual Dax voice84 are bound using the
recovered constructor roles. Ordinary per-voice gain/pitch/pan updates remain
unchanged. The mixer applies exactly one independent group multiplier.

One `AnnouncerEpisode50IC` lives with the mixer and group owner. The existing
host race subscriber borrows it and routes every actual Dax play through it.
Countdown/GO remains an effects-group path. Host50HQ still chooses the same
authored variants, five-second place cooldown, busy suppression and lap/finish
preemption policy. These are explicitly host choices, not original complete
announcer chance, priority, queue or delay reconstruction.

## Playback and lifetime ordering

- Missing PCM returns unavailable before callbacks or an episode start.
- Busy non-preempting host speech leaves the existing episode untouched.
- Preemption stops voice84, invokes recovered end/restore, then invokes begin
  before starting the replacement on the same voice84.
- A normal start invokes begin before actual mixer play. Playback rejection
  attempts immediate restore rather than leaving the groups ducked.
- The live audio owner observes real voice84 activity immediately after its
  common committed-step mix and restores once it is inactive. Completion is
  checked even if an output write fails after that mix. Restoration occurs at
  this native mix-block boundary, not a guessed clip duration or wall timer.
- Pause retains source-frame position, group levels and episode state. No
  mixed frames or extra begin/end callbacks are generated during a paused
  interval. Resume continues the same episode. Host requests while paused are
  rejected by the adapter.
- Race reset, live voice reset and close explicitly stop/restore. The session
  close path resets race audio before closing the mixer; repeated close/reset
  does not apply the gain transform twice. A restart creates fresh ownership.

Raw playback on voice84 outside an owned episode is detected. Replacing that
voice during an owned episode is indistinguishable and remains prohibited by
the explicit single-writer contract. Attaching a second episode adapter to an
already-bound slot is rejected. Unmapped mixer slots reject starts and controls
in strict group mode. This is not a second free-running speech owner on spare
slots.

## Deliberate boundaries

Race mode11258 remains the current host2. No lap/timer/record configuration
changes appear in this patch. Type12 remains unjoined and its existing counter
is unchanged; neither its selector nor authoring fixture is added to this
integration. Mode1/event6 work must join a legitimate common original admission
owner rather than call the host-preemption adapter as an assumed substitute.

The original complete27-racer/generic announcer emitter topology, pending
queue, priority, delay, RNG and source category admission are still separate
work. The present episode adapter owns exactly the existing native Dax voice84
and labels its request policy host. It must be extended or replaced coherently
before another Dax route is enabled.

Unknown group owners remain explicit: the11 catalog debris/wreck/track-intro
entries are not mapped. Unused85..95 and110..127 remain unbound in this current
active snapshot. In particular, independently staged opponent boost110..116
needs its own proven EngineBinderBoost range binding when that separate patch
is joined; this integration does not pre-authorize unused capacity. Music and
world group state is retained, but no missing music/ambience provider is made
live. Native gain normalization is still the documented volume/100 convention,
not a claim of bit-identical original SPU output.

## Changed paths

Existing files:

- `platform/linux/native/audio_mixer50hd.h`
- `platform/linux/native/audio_mixer50hd.cpp`
- `tests/native/live_audio50hf_fixture.h`
- `tests/native/race_audio50hq_fixture.h`
- `tests/native/terrain_interactive_drive50du.cpp`

New files:

- `src/game/announcer_callbacks50ic.h`
- `platform/linux/native/race_audio_groups50ic.h`
- `platform/linux/native/announcer_episode50ic.h`
- `tests/native/race_audio_groups50ic_fixture.h`

The header-only adapter required no build configuration changes for the
observed Linux integration build. Broader lifecycle and platform coverage remain
pending; the focused result above is the complete runtime claim.

Periodic shared-Dax telemetry includes actual integer group levels so the focused run can distinguish ducked and restored state.
