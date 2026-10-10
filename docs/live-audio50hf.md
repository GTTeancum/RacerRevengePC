# Live native audio bridge (50HF)

## Scope and actual availability

`tests/native/live_audio50hf_fixture.h` owns the existing 50HC decoded bank,
50HD mixer/output, 50HE generic cue provider, and genuine bounded Pod01 lap
sound owner at one stable address. It is a client-side host subscriber to
source-backed fields and phases. It does not supply a complete original D40
owner; `d40Present=false` and the core source guard remain unchanged.

The retained assets available when this bridge was implemented have the
original catalog and authoring but no original sample bytes. Cold preparation
therefore succeeds with 60 explicit missing records and zero loaded PCM clips.
That is useful negative availability information, not audible audio.
`openOutput` refuses an all-unavailable bank before opening either the backend
or a WAV. No synthesized replacement, silent placeholder, fake handle, or
purported original-audio recording is generated.

If original sample files later become available under an explicitly supplied
AUDIO directory, 50HC admits only its documented VAG subset and preserves
missing, unsupported, invalid, and ready states separately. File loading and
decoding remain cold operations. Device availability is separate: a real
backend failure may leave a requested WAV capture working, and the client must
report `info().deviceStatus` without claiming that capture was audible output.
A ready decoded clip or emitted command alone is never proof of playback.

## Stable ownership and API

`live_audio50HF::Owner` is neither copyable nor movable. It holds 128 fixed,
caller-owned mixer voices. Generic provider slots 0–59 and the genuine lap
owner's slots 60–63 are exclusive; slots 64–127 remain unused. The PCM bank is
immutable after preparation, and only const views are exposed. The output
callback consumes the output's fixed PCM ring, never game owners or files.

Client lifecycle:

1. Construct the owner at its permanent address. Call
   `prepare(discRoot, audioRoot, error)` once. This authenticates the retained
   original archives/executable and all 60 request records, loads the explicit
   AUDIO root, prepares the generic registry, then authenticates the six-event
   Pod01 lap owner. A failed preparation requires a new owner.
2. Create the native window host, then call `openOutput(request, diagnostic)`
   only when a usable bank exists. The bridge requires 48000 Hz. Device and WAV
   settings are the existing `AudioOutputRequest50HD`; file creation retains
   its exclusive-create protection. Missing-bank refusal creates no file.
3. Supply actual gameplay inputs at the phases below. Event methods return an
   `EventReceipt`: `okay()` means projection/dispatch completed without an
   invalid input or provider failure. Its fixed `commands` and `results` arrays
   separately distinguish actual Started/Stopped/Controlled, Inactive, and
   Unavailable outcomes. Unavailable is an honest completed negative result.
4. After each successfully committed 16 ms simulation step, call
   `committedStep(diagnostic)` exactly once when output is open. It mixes 768
   stereo frames at 48 kHz through the shared capture/device buffer. Render
   frequency, paused frames, and failed or blocked simulation steps do not
   advance the sample clock.
5. `pause(true)` flushes stale device data without changing capture time or
   live voices. Resume with `pause(false)`. Event and committed-step methods
   reject a paused owner. `resetVoices()` is an explicit host reset policy: it
   stops all voices and clears the warning/boost subscriber states, while
   keeping decoded PCM, registrations, and the cumulative output clock.
6. Call `close(diagnostic)` before host destruction and SDL_Quit. Close is
   idempotent; statistics remain readable. The destructor is only a fallback.

All event, lap-dispatch, reset, and mixing paths use bounded existing storage
and perform no application heap allocation or asset decoding. WAV writes are
main-thread I/O, not audio-callback I/O. Preparation, backend opening, and
closing are cold lifecycle operations.

## Source phases, without manufactured triggers

- `boostAtBoundary(heldBoost, heating, previous64)` is called at the original
  boost boundary with PRE-update vehicle64, actual B9C bits, and source heating
  `88*(70+74)`. The same tick can subsequently become overheated. Its retained
  `boostPitch()` feeds the engine input; boost release does not cut off the
  onset clip. This subscription does not relax the 50FK guard.
- `repairCompletion(flagsB9C, vehicle3C, damage)` is called after the real
  player repair path. Damage order is `[7D0,7D4,7D8,7DC]`. It emits only the
  original completion tail, not an invented repair loop or boostmax onset.
- `warnings(input)` consumes actual post-owner damage, overheat, wrong-way,
  water-probe, and suppression fields. It preserves source ordering and emits
  only transitions plus actual spray gain. An unjoined probe118 is not guessed
  from other contact data.
- `engine(parameters,input)` uses retained speedRatio70 and source pitch law,
  starts each real available loop, then controls its phase-preserving mixer
  voice. The caller supplies `input.boostPitch=owner.boostPitch()` and the real
  damage pitch scale when available. Explicit scale484=1 is the documented
  undamaged/reset baseline, not full randomized damaged-engine modulation.
- `dispatchLap(key,flags,a3)` is the specifically admitted real
  lap292360→233410 path. Its `DispatchReceipt` independently reports completed,
  actual started/unavailable/no-channel, and actual sound `originalV0`.
  Acknowledging the void 50GG effect uses `originalV0=0`, even if this nested
  sound method returned 1. An unprepared/unsupported/invalid receipt must not
  acknowledge completion.
- `applySourceCommand(command)` lets a separately recovered source event owner
  submit an actual `AudioCommand50HE` through the same bounded admission,
  provider, receipts, and counters. It supplies no trigger, random choice,
  cooldown, or owner state itself; sourceSite is provenance, not dispatch.

See [50HE](audio-events50he.md) for exact source predicates and numeric law,
[50HC](audio-assets50hc.md) for catalog authentication/codec boundaries, and
[50HD](native-audio50hd.md) for output timing and backend diagnostics.

## Loaded resources are not unsupported event implementations

The bridge knows nine cue identities with bounded source-linked paths: six
PodEvent cues and three PodEngine cues. `hasJoinedTrigger(cue)` describes that
identity-level boundary. Actual client availability still depends on each
required source owner/input and on the cue's PCM availability.

The other 51 retained identities are still registered and cold-loaded if
present: 43 speech clips and eight debris/wreck effects. They have no invented
automatic trigger here. `unsupportedTriggerCues()` reports 51 and
`loadedUnsupportedTriggerCues()` reports how many have PCM but no joined
trigger in this bridge. This count describes this bridge only; a separate,
explicitly joined original speech/event owner may add supported paths later.
There are no generic timers, lap-based speech substitutions, rumble-to-speech
conversions, guessed collision materials, or invented opponent/race events.
This implementation must not be described as all game effects or voices.

## Diagnostics and bounded verification

The bank exposes ready/missing/unsupported/invalid clip counts and per-clip
source details. Provider counters and fixed receipts expose real outcomes;
`lapOwner()` exposes the separate real lap start/unavailable/channel counters.
`counters()`, `mixedFrames()`, `clippedSamples()`, `activeVoices()`, and `info()`
are read-only observations. Generated commands, loaded clips, mixed silence,
and device consumption must remain distinct in reports.

The standalone C++20 header compilation passed with `-Wall -Wextra -Wpedantic`.
A focused check uses the actual authenticated retained catalog with an explicit
missing AUDIO root to verify honest negative dispatch, refusal to create a
WAV, subscriber transitions, pause/reset/close behavior, and allocation-free
hot negative paths. Evidence is preserved under the task's `solo-audio50hf`
directory. No original audible check or original-audio recording is possible
until actual sample bytes are supplied. These focused checks are not a full
regression, device listening test, or native race-completion proof.
