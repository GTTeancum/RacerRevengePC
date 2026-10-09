# Staged opponent boost-event connection

Status: source-reviewed staging only, 2026-10-09. Active source and the frozen central test candidate were not modified. No build, syntax check, test, gameplay, event injection, candidate importer, guest execution, or new source recovery was run.

## Important gameplay boundary

The current HOST opponent driver does not press boost. It writes only two throttle-axis bytes after `composePacket(true,noKeys)`. Consequently this staged subscriber remains dormant during ordinary authored-pace opponent races. It fixes an actual missing source-state/audio connection, but does not make opponents start boosting or establish audible opponent boosts. No driver, AI, handling, input policy, or RNG change is included.

Source anchors in the active baseline:
- `src/game/host_course_driver50gk.h:63–83`: output is left/right throttle only; no synthesized buttons.
- `src/game/host_course_driver50gk.cpp:210–216`: only packet bytes 7 and 5 are written.
- `tests/native/terrain_interactive_drive50du.cpp:2717–2726`: each opponent composes the no-keys packet before applying course input.

## Deliverable

- `opponent-event-audio.patch`: root-relative unified patch for three existing source files.
- `before/`: exact source preimages used for staging.
- `after/`: corresponding staged source files, without unrelated dependencies or assets.
- `source-sha256.txt`: preimage and staged file identities.

No active source replacement or patch application was performed. Root should reconcile the patch against its current source rather than overwrite a newer full main file from `after/`.

## Actual gap and source closure

The baseline skips the boost subscriber for `opponentFrame`, and `sampleRaceField()` explicitly supplies `boostPitch=false` for all opponents. The source-backed helper already exists:

- `src/game/gameplay_audio50he.cpp:100–113`: `updatePodBoostAudio50HE` uses held/repaired flags, real pre-update `vehicle64`, and positive authored heating. It emits PodEngine key 3 and retains the exact pitch-boost state. Release/repair clears pitch through the original condition without stopping the onset sound's natural tail.
- `src/game/gameplay_audio50he.cpp:68–98`: existing original pitch law and 1.15 boost multiplier remain unchanged.
- `tests/native/terrain_interactive_drive50du.cpp:1486–1499`: all required native fields are available at the existing pre-heat boost boundary during every real opponent bank step.
- `src/assets/audio_assets50hc.cpp:266–273`: original executable identity and exact global boost literal at file offset 0x2A8210 are already authenticated by the cold catalog loader.
- `tests/native/selected_player_audio50ij_fixture.h:27–42`: every supported selected player preserves this global PodEngine3 request. This is not character-specific POD01 speech.
- `tests/native/audio_cue_catalog50he.inc`: existing semantic PodEngine3 maps to the authenticated `sfx\\race\\boost.vag` record.

The retained static source excerpts `pod_emitter_methods-static.txt` (19FF10/19FF60) and `player_emitter_channel-static.txt` (1A0120/1A01F4) were read from the already-retained audio evidence. No executable, memory dump, or excluded investigation was opened or executed.

## Changes

1. Add one trivial `BoostState` to each racer snapshot. It contains original pitch state, the last emitted original command, and a monotonic event serial. Only opponent frame calls update this owner. The human retains its existing live-audio owner unchanged.
2. Evaluate the already-recovered helper at the same existing pre-heat boundary using that bank's authored heating and prior vehicle64. No mixer action occurs in the bank visit.
3. Publish those small fields during the existing `sampleRaceField()` visit after a completed native frame. There are no extra full-state bank visits.
4. Extend the existing opponent mixer owner with seven one-shot slots, 110–116. Engine/binder slots remain 96–109; music 117 and material/ambient slots 118 onward are untouched. Bind all 21 owned slots to the existing source EngineBinderBoost group.
5. Borrow the already-authenticated immutable global boost PCM from the live bank. No new decode, hardcoded sample mapping, player voice reuse, or asset substitution is added. Unavailable boost PCM remains an unavailable result; source pitch state still progresses.
6. Apply one start per new observed serial. Repeated observations and natural clip completion never restart an old onset. Serial regression, skipped events, or malformed cue controls reject before any mixer mutation. A genuine later onset restarts that opponent's dedicated one-shot, matching the existing native fixed-slot playback policy.
7. Reuse the existing HOST distance attenuation and opponent gain normalization. Its 0.45 ceiling now covers all currently active ready opponent loops and boost tails together. This presentation policy is not recovered PS2 spatial or polyphony behavior. Baseline no-boost gains are unchanged.
8. Emit counters for original boost onsets, successful starts, and unavailable starts. These counters do not assert heard output.

## Timing and lifetime review

- Countdown, menu and pause paths do not call the opponent boundary helper. Their existing simulation and mixer gates remain unchanged.
- Failed opponent snapshots are excluded from publication. Failed player frames do not advance opponents or reach the shared mix.
- Engine/binder pitch uses that opponent's source-owned boost flag; no selected-player state is copied into it.
- Playing boost tails use current committed distance/gain controls and finish naturally after release. Inactive native slots stop all their owned sounds at the accepted update.
- The existing single world mix remains the only mix; the patch adds none.
- Explicit close stops 96–116 before closing the live mixer. Construction/destruction order also keeps the borrowed live-bank PCM alive until the opponent owner is closed/destroyed.
- Restart creates fresh source state, serials, audio owners and mixer. An owner reset retains consumed serials so stale observations do not replay completed onsets.
- Core physics, native boost progression, original engine/binder authoring, engine-damage RNG and all source parameters are unchanged.

## Unverified

All claims above are from source reading and staged diff review. Compilation, positive source event playback, missing-PCM behavior, repeated observation, pause/resume, tail completion, restart, current group ducking, allocation assertions and listening quality remain unexecuted for this change. The ordinary current HOST race cannot provide positive opponent-boost coverage because its driver does not press boost. No positive event injection was added or run.
