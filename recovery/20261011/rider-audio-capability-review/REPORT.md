# Dormant Rider Song7 / event2 audio capability slice

2026-10-11. Source-only staging against repository commit `6dc2c8b1b3064f8fdeb5f12648fe4583119c1b9e`. The checkout was not edited by this worker. Four exact preimages, four candidates, and an integration patch are included. Independent review is recorded separately.

## Result

The candidate adds the concrete transport boundary from section 4 of `../presentation-audio-clock/INTERFACE-PLAN.md`:

- `canPlayRiderAdvance2()` and `playRiderAdvance2(RiderAdvanceCommandReceipt&)`, using the existing authenticated clip6/voice6 `playEffectClip(6)` implementation. This remains original SFX/UI/ADVANCE.VAG, 4900 mono frames at 22050 Hz, gain1, pitch1, no loop, centered routing, with the original stop-before-restart behavior.
- A privately issued historical command receipt with event/slot, exact transport/mixer/mode identities, actual control revisions before and after, actual mixer epoch/frame counter, and copied live voice observation. Ordinary Game Options event2 cannot mint a Rider receipt.
- `inspectSteadyRiderTransport(...) const`, with detailed rejection codes and copied actual voice/control/routing facts. It does not issue an application lease, pump time, call `update`, mutate bookkeeping, restart a sound, or certify an output/device/presentation.
- Transport control revisions, mixer control revisions, and per-slot playback revisions. The existing application sound-mode revision remains a distinct owner-supplied revision.

No session/input/movement call site, ledger bridge, output producer, build list, scheduler, capture path, or audio RNG was added or changed. The new Rider entry points have no active caller. Existing audio paths receive metadata bookkeeping only.

**Dormancy is limited to the new capability APIs.** Once integrated, the added mixer and transport revision bookkeeping executes on ordinary active command paths. Those metadata changes are source-reviewed but runtime-untested. Source inspection supports preservation of ordinary audio behavior; it is not a runtime validation result.

## Why the mixer addition is necessary

`activeMusicSong()==7` remains true through the existing fade and proves neither steady gain nor slot identity. `AudioVoice50HD` alone is insufficient: real original left/right route multipliers and group gains are retained separately by the mixer.

The narrow copied `AudioVoiceObservation50HD` includes the voice, last successful playback revision, group state, and actual cached routing factors. Read-only master/control revision accessors complete the observation. A same-slot identical foreign `play` receives a new playback identity and cannot pass the transport's retained owned-play identity check. The mixer retains its current public playback behavior and single-producer contract.

The application must continue exclusively owning its private seven-voice array and stable mixer. These observations do not make arbitrary writes through an externally held borrowed voice-array pointer safe or observable. The existing private `AudioOutput` owns that required lifetime and storage; future admission must use that actual owner.

## Steady check

The observer requires all of:

1. Successful Main and optional Rider preparation; unchanged mixer epoch, exact dedicated seven-slot layout, and unchanged prepared output rate.
2. Usable non-wrapped transport and mixer control identities, and unpaused transport.
3. The same non-null sound-mode owner captured from the concrete mixer at cold preparation, ready with valid source fields and supported centered selector0 routing. Its current identity and independent revision are returned for binding to the application's actual owner.
4. Unity master gain, no required group layer, and no per-slot group binding even at unity, including inactive slots.
5. Owned active Song7, no fade, gain0.4, and the zero elapsed/duration/retirement fields of a genuine settled start. It does not infer no fade from the song number.
6. The retained authenticated Song7 PCM owner/pointer/hash metadata, 619024 stereo frames at 22050 Hz, no embedded loop, exact active clip identity, owned playback revision, gain0.4/pitch1/pan0/loop, cached left/right unity, and a finite in-range source cursor.
7. Each active effect is a playback owned by this transport, with the matching authenticated clip/PCM metadata, owned playback revision, fixed gain1/pitch1/pan0/nonloop, cached unity routing, and a finite in-range cursor.

Inactive effect slots are valid regardless of stale `ownsEffect_` housekeeping. This deliberately accepts an effect that naturally ended before inspection or during future genuine PCM service. It never requires event2 to still be active to preserve the historical command fact.

## Revision semantics

- Every actual mixer initialize/bind/play/restart/control/routing/loop/master/group/stop change changes its checked control revision. Successful plays also stamp the target slot with that revision. An exact no-change setter does not falsely invalidate a steady lease.
- Transport starts/restarts, successful effect commands, fade starts/stops/progression, successful sound-mode commands, actual pause changes, preparation, and actual reset changes update the transport control revision. The existing linear fade arithmetic and retirement boundary are unchanged.
- `mixStereo16` and `AudioSampleClock50HD` are unchanged. PCM source-frame progression, loop wrap, and natural one-shot completion do not update command revisions. Nonfading `update` and its natural inactive-effect bookkeeping do not update them either. Repeating the same pause value or reset after reset has no false revision change.
- A future lease must bind and compare all three revisions: transport, mixer, and application sound mode. Independently applying the shared mode changes its own revision even when the command did not travel through this transport.
- Counters never wrap. If an ordinary existing operation exhausts revision metadata, ordinary audio behavior remains unchanged but the identity becomes permanently unusable for capabilities. The new Rider command preflights one transport increment and both possible mixer stop/play increments before mutation. The poisoned terminal number can never be reused as a valid capability.

## Preserved ordinary behavior

Existing method signatures and return behavior remain intact apart from the added dormant APIs and read-only accessors. Audio data, routing calculations, gain arithmetic, source float fade arithmetic, source-frame/sample timing, event allowlists, effect voice allocation, Main/Options/Rider Back calls, mixer callback ownership, and sound-mode field projection are unchanged. There is no new runtime loading, decoding, allocation, output call, fade clock, or play from a presentation checkpoint.

The actual new Rider command is intentionally stricter than older ordinary menu methods: it requires a currently steady original Rider transport and full command identity capacity. It is not yet reachable from any page.

## Joins still required before live admission

- The real direction owner must preflight this capability together with all resource/video/pose/material/deferred4EC gates before destructive direction mutation, then play event2 exactly once at its genuine source sound point and retain the successful command receipt. It must not call the command at pump entry or on blend samples.
- After the real third page update and incoming source completion, the application must inspect again and bind the final lease to the actual live output/transport/mode owner, transport/mixer/mode revisions, clock, direction transaction, immutable source pair, and scheduler/context lifetime.
- Every genuine PCM checkpoint must revalidate current owner facts and all revisions. Natural effect cursor/end changes are allowed; a new control command, fade, pause, mode change, reset, foreign playback, stale epoch, or gain/routing mismatch invalidates the lease.
- Output enabled/closed/faulted/paused state, capture success, explicit mute policy, actual host presentation, and irreversible late-failure handling remain the separate output/application owners' responsibility. This transport-only observation cannot certify them.
- The receipt is a historical command fact tied to the stable transport/mixer lifetime, not proof of current audible sound, event duration, direction completion, original IOP timing parity, or publication. Retain the actual owner rather than using its raw diagnostic address after destruction or reusing an old receipt for another movement.

## Verification boundary

Only source reading, editing staged text, diff inspection, and hashing were performed. No builds, tests, syntax-only/compiler checks, sanitizers, gameplay/native launch, GL probes, project parsers/importers/decoders, or guest emulation were run. No retry, dependency change, publication, or hold workaround occurred. The centralized test/publication hold remains pending user approval.

Future review cases are specified, not executed: steady Song7; live fade with song7; wrong/missing music; unbound/changed mode owner; foreign identical slot restart; unexpected controls/master/group/routing; exact no-change update/pause; natural effect completion before and during a lease; owned repeated event2; reset/epoch/rate invalidation; checked revision exhaustion; unchanged ordinary Main/Options/Back behavior. These remain runtime-unverified.
