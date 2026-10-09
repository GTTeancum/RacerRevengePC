# Source-backed gameplay audio events and native sample owner (50HE)

## Result and boundary

50HE supplies allocation-free source-linked engine controls, boost/repair cues,
warning-loop transitions, a 60-entry authored cue registry, real native mixer
start/stop/control dispatch, and the concrete lap callback's genuine PCM-backed
sound method. The client join belongs to the integrating change.

This is not a complete original `D40` object or a full-game sound implementation.
`advancePlayerBoost50FK` must retain `d40Present=false`: its positive-owner guard
is not bypassed. Except for the specifically implemented lap selection/sound
method, this is a **native host-audio subscriber** to existing gameplay fields
and source boundaries. It does not fabricate gameplay returns, race state,
opponents, speech events, contact-material identities, or sample bytes.

The currently retained originals identify 17 sound-effect samples and 43 voices
(40 Pod01 voices and 3 TA introductions). Their VAG bytes were missing when the
focused checks ran. Registration and emitted events are not audible playback.
The check's positive PCM is explicitly synthetic test data; it is never shipped
as a replacement game sound or written as an audio deliverable.

## Files and integration

- `src/game/gameplay_audio50he.h/.cpp`: platform-independent bounded event and
  numeric projection; add the `.cpp` to the native target.
- `tests/native/gameplay_audio_provider50he_fixture.h`: `gameplay_audio50HE::Provider`.
  Prepare once with the immutable `AudioBank50HC`, initialized `AudioMixer50HD`,
  a first voice slot, and an error string. Exactly 60 slots are reserved, one
  per cue. `apply(command)` returns actual `Started`, `Stopped`, `Controlled`,
  `Inactive`, or `Unavailable`; other results are explicit errors. It performs
  no decoding, I/O, lookup-string construction, allocation, or synthesis.
- `tests/native/audio_cue_catalog50he.inc`: all 60 exact bank/key/name mappings.
  `cueFilename(cue)` resolves without allocation. Catalog admission checks the
  bank's actual authored loop/stream flags. Engine loop status is a source
  runtime choice; the PHY filenames themselves do not declare a loop flag.
- `tests/native/pod_audio_owner50he_fixture.h`: `pod_audio50HE::Owner`,
  `prepareFromPod01(owner,mixer,bank,firstVoice,voiceCount,error,archivePath)`,
  and `dispatchLap(owner,key,flags,argA3)`.
- `tests/native/gameplay_audio50he.cpp`: focused source-boundary/provider checks.

An example allocation is a 128-voice mixer, generic provider slots 0–59, and lap
owner slots 60–63. These ranges must be disjoint and exclusive. The bank's PCM
vectors must remain immutable, alive, and unmoved until all owners/voices stop.
The generic provider deliberately uses one fixed slot per cue, restarting that
cue on another Start; this is bounded native allocation policy, not recovered
original polyphony or voice-priority arbitration.

### Per-tick order

1. At the original boost boundary, capture previous `vehicle64`, actual
   `heldBoost=(B9C&1)&&!(B9C&2)`, and source heating `88*(engine70+engine74)`.
   `updatePodBoostAudio50HE` emits a boost onset only on the actual positive
   heating/previous-state-zero path. Do not infer it from the post-heat state,
   because that same tick can already be overheated. Its retained pitch flag
   clears on the original release/repair path, while the onset clip is not
   prematurely stopped. This subscriber does not replace the 50FK D40 guard.
2. After actual repair, call `repairCompletionAudio50HE` with current B9C,
   vehicle3C, and damage in ascending `[7D0,7D4,7D8,7DC]` order. This implements
   the sound tail of player override `1A23F0`, not its unrelated player-field
   housekeeping. It must not be called for an invented repair completion.
3. Call `updatePodWarnings50HE` with actual post-owner state. Execute its
   commands in returned order before the next update. Up to five commands are
   possible. It emits transitions only; unchanged warning loops do not restart.
4. Call `podEngineControls50HE` with retained speedRatio70 and retained boost
   pitch state. Start engine and binder once through real provider Start
   commands with loop=true, then Controls preserve the running sample phase.
   Start/stop lifecycle and host mute/pause policy belong to the client.
5. A reset/restart must stop the corresponding actual voices and initialize
   subscriber state coherently. Never restart a progression cursor to hide an
   unavailable sample or pending effect.

## Engine controls, not a held-key tone

`178EA0` loads Pod01 PHY `[Pod Audio]` into the actual engine emitter. The
retained values are engine `sfx\race\as_ngn.vag`, binder
`sfx\race\as_beam.vag`, idle pitch `0.32`, range multiplier `4`, binder
idle/range `1/1`, and authored volumes `100/90`.

`178FE0` computes base pitch and delta from genuine sound metadata. `19F5B0`
uses vehicle70, the speed ratio already calculated by the boost controller,
not a keyboard throttle fraction. The source sine polynomial at `13BAD0` and the scalar square-root at
`19F5FC` (word46000204, SQRT.S, not ABS.S) are used, with the original reflection and separately rounded binary32 operations:

- `amount = sqrt(abs(sourceSin(pi/2 * speedRatio70)))`
- `engine = max(idle, idle + (idle*range-idle)*amount)`
- Binder uses its own identical expression and authored parameters
- Truncate each pitch word, then apply source 1.15 boost multiplier and truncate
- Engine alone is multiplied by explicit current damage-dip scale484
- Normalize pitch words by the actual native provider base-pitch units

The native provider uses normalized pitch units4096. This is an explicit native
representation, not a claimed recovered PS2 sample-bank metadata value. The
start methods `19FDB0` and `19FE80` use `0.45*authoredVolume`, yielding normalized
gains 0.45 and 0.405. Dynamic original spatial attenuation is not reproduced by
this fixed-listener native provider.

The full randomized damage-dip timer owner `19F6CC..19FB84` is not joined.
`enginePitchScale484=1` is explicitly the constructor/reset and undamaged
baseline, not evidence of complete damaged-engine audio. The input range
[-2,2] is admitted for the single-reflection polynomial. Invalid inputs leave
the output unchanged; the API must not silently clamp authoritative gameplay
state to hide a failure.

## Reachable source-linked effects

| Cue | Actual source trigger | 50HE status |
| --- | --- | --- |
| PodEngine1 `as_ngn.vag` | Engine start/stop and retained speed-ratio pitch | Real provider and continuous control available; full damage dip/spatial owner unjoined |
| PodEngine2 `as_beam.vag` | Binder start/stop, own authored pitch | Real provider and continuous control available |
| PodEngine3 `boost.vag` | `217760 ->19FF10 ->1A0120`, positive heating, previous64=0 | Exact onset predicate and persistent boost-pitch flag available |
| PodEvent1 `lapcount.vag` | `1A2BB4 ->292360(1,0,0)` | Genuine six-row owner and positive/negative sound method implemented |
| PodEvent2 `boostmax.vag` | `1A24D8`, held repair with all four damage fields<=0 and vehicle3C=0 | Exact repair-completion sound tail implemented; **not** a boost-onset substitute |
| PodEvent3 `overheat.vag` | `19F07C/19F0BC`, vehicle64==2 transitions | Ordered loop start/stop implemented |
| PodEvent4 `damage.vag` | `19EFF8/19F038`, source2234E0 result>=0.7 | Ordered loop start/stop implemented; source max reads7D0/7D4/7DC, not7D8 |
| PodEvent5 `wrongway.vag` | `19F100/19F140`, actual flags10CC bit1 | Ordered loop start/stop implemented |
| PodEvent6 `wspray.vag` | `19F184/19F1C4`, actual probe348 field118 | Ordered loop and source speed-ratio gain command implemented; do not fabricate missing field118 |

The warning block is suppressed by emitter22C bit2 or manager1B0 bit4. Those
suppressions preserve prior loop state. VehicleCD8 bit0 separately stops all
four warning loops in source damage/overheat/wrongway/spray order. A host subscriber
without a represented probe118 must set `sprayInputKnown=false`; the admitted
partial block preserves the existing spray bit and emits no spray start or
control, rather than inventing a dry surface. CD8 still stops all loops. Capacity or
input rejection leaves previous state, output storage, and count unchanged.

Repair does not have an authored continuous repair hum in this retained set.
The actual repair lowers damage, clears boosting through existing gameplay,
and can trigger the source repair-completion cue. A synthetic hum is not used.
Flame geometry/material animation has no separate authored sound in this set;
engine/boost source sounds are not restarted for every flame draw/update.

## Genuine lap owner contract

Cold preparation reuses the authenticated Pod01 archive/member parser from
50GH. It verifies the same archive SHA and six actual EventN records, then
resolves each name against the real 50HC bank. A ready sample must have actual
bounded PCM storage and pass the mixer's clip validator. Unavailable samples
retain -1; absence of a registry row is a preparation error.

Hot dispatch admits only key1/flags0/a3=0. It validates six native row/object
identities, performs the selected-owner store before the sound method, refreshes
an actual negative sample result when necessary, and calls real mixer playback
for a positive sample. It preserves source channel/publication ordering:

- Negative sample: return0 before channel2C/24/7C or playback changes
- Positive sample: call actual playback, publish actual channel or -1, copy
  current pitch/volume/pan to their cached fields
- Actual nonnegative channel: clear24, set7C=1, return1
- No available native voice: completed actual negative channel return0

No fake callback is used. The root must acknowledge `50GG` only after
`receipt.completed()` is true. `LapCompletionReturn50GG.originalV0` is **0 for
this void effect acknowledgement**, even when the nested sound method's actual
receipt records originalV0=1. 50GG's effect-return token intentionally does not
carry unused nested function results. Queries still require genuine query
returns. The existing disabled 50GH provider is preserved as a separate,
explicitly silent fallback; it is not relabeled successful.

## Registered voices and effects still lacking game-event owners

| Authored group | Registered sample count | Why not currently emitted automatically |
| --- | ---: | --- |
| Pod01 low-damage curses, keys1–4 | 4 | Player speech event manager, source category selection/cooldown unjoined |
| Pod01 wall-hit curses, keys5–8 | 4 | Source collision/speech event owner unjoined; rumble is not speech |
| Pod01 passed curses, keys9–14 | 6 | Opponents and ranking/pass events unjoined |
| Pod01 attacked curses, keys15–19 | 5 | Attacker/partner events unjoined |
| Pod01 passing taunts, keys1–6 | 6 | Opponent pass-event owner unjoined |
| Pod01 attacking taunts, keys7–12 | 6 | Attacking/opponent owner unjoined |
| Pod01 death specials, keys1–4 | 4 | Destructive vehicle transition/respawn owner unjoined |
| Pod01 win specials, keys5–7 | 3 | Race-finish/rank owner unjoined |
| Pod01 lose specials, keys8–9 | 2 | Race-finish/rank owner unjoined |
| TA Dax intro, keys1–3 | 3 | Authored intro-camera/script-line event owner unjoined |
| PodDebris keys1–4: debris2,1,3,4 | 4 | Original wreck/debris scheduling and shared RNG owner unjoined |
| PodWreck keys1–4: podexpl5..8 | 4 | Actual destructive transition owner unjoined |

All 43 registered speech clips remain separate authored identities. None is
scheduled on a generic timer, substituted for another character, or played on
an unrelated lap event. The native provider can play a genuinely requested cue
when the real future event owner exists; this does not make those events live.

Original material collision/drive-on sound owners are also missing. Existing
49Z impact events are controller rumble. 50B `ChannelA8` is an actual source
boundary: timer830>0 selects argument2 at224130, otherwise3 at224148. It reaches
`1A0120 ->179070`, which needs real contact material354->130->A9 and a D44
material sound owner. TA.aub retains eight collision, eight drive-on, and52
soul-tree authoring rows, but these are not the 60-clip Pod01/TA event catalog
and no default material identifier or generic collision sound is invented.

Other racers, other tracks, race countdown/finish announcers, UI sounds, music,
and the remaining full-game audio resources/triggers are not supplied by this
60-entry catalog. This change must never be reported as “all game voices”.

## Verification and evidence

`solo-audio50he/` preserves static immutable ELF disassembly excerpts and their
source manifest. Original SLUS SHA-256 is
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
No guest CPU, runtime memory image, excluded49C/48U, or emulated audio hardware
is used. Static source sites are provenance data, not executable dispatch keys.

Focused normal check passes for exact pitch words at idle/full/boost,
warning ordering/capacity atomicity/skip gates, boost onset/release, repair
completion, all60 authored mappings, real missing-provider results,
positive native mixing/loop/control/stop with synthetic-only PCM, positive and
capacity-failed genuine lap calls, mode2 continuation, and zero hot allocations.
The sanitizer result is saved separately. These focused checks are not a
full-game regression, proof of original audio assets being available, a human
listening test, or full native race completion.
