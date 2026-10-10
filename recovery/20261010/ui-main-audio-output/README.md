# Original Main native audio output join

Status: staged source only. No build, syntax check, test, sanitizer, native
project parser/importer, decoder, game, or emulator was run. No active repository
file was changed and no commit was created. Parent owns the central validation
window (next stated slot: not before 2026-10-10 18:23:22 UTC).

## Deliverable

`candidate/` contains complete proposed files. `preimage/` contains the exact
five existing files copied before edits. `manifest.json` records SHA256 for each
preimage/candidate and identifies two newly added files. The observed checkout
HEAD was b7b6bcc4d2aac0ed8e9ccba94cc4b9966cc341f3. Apply `integration.patch` by
reviewing context against the parent's current files; do not blindly overwrite
later work, especially the large terrain interactive entry. `stage.py` is a
text-only construction record, not a test or an integration command. It is not
needed to integrate the candidate and should not be rerun against changed
preimages.

Changed files:

- platform/linux/native/audio_output50hd.{h,cpp}: typed PCM producer overload;
  original mixer overload remains and delegates using a local typed adapter.
- platform/linux/native/original_ui_main_audio50kg.h: implements the real typed
  producer contract through its existing pause-aware mix method and readiness.
  Its source PCM decoder, sound/event routing, music gain and fade are unchanged.
- tests/native/original_ui_main_entry50kg.h: explicit enabled/muted admission,
  audio asset root, real output ownership, session transport and truthful stats.
- tests/native/terrain_interactive_drive50du.cpp: no race output during Main;
  pause/resume, accepted-update PCM advancement, close and capture receipts.

New headers:

- platform/linux/native/audio_stereo_producer50hd.h: synchronous main-thread
  producer interface. AudioOutput does not retain its reference or invoke it
  from the SDL callback.
- tests/native/original_ui_main_output50kg.h: stable dedicated five-voice mixer,
  original Main transport and output owner. No race mixer/reference is accepted.

There is no new translation unit or linker dependency. Existing source builds
must already include original_ui_main_audio50kg.cpp and audio_output50hd.cpp.

## Runtime configuration

The frontend still requires RR_ORIGINAL_MAIN_MENU=1 and RR_ORIGINAL_UI_ROOT.
RR_ORIGINAL_MAIN_AUDIO must be explicitly `enabled` or `muted`.

Enabled requires RR_ORIGINAL_MAIN_AUDIO_ROOT pointing to AUDIO itself. This is
separate from race bank configuration RR_AUDIO_ROOT. No default, recursive
search, substitute sound, fallback bank or literal workspace path is compiled.

The retained prepared source root available in this task is:
/workspace/shared/recovery-20261010/asset-inputs/extracted/RacerRevenge-original-AUDIO/AUDIO

The existing OriginalUiMainAudio50KG authenticates exact UIEVENTS.INI,
UI_MUSIC.JUK, UI_MUSIC.JUB, MUSIC/UI/SCREENS/UI_MAIN.VAG and the four original
SELECT/LEFT/RIGHT/BADCALL effects before admitting the output. This stage reuses
that decoder and metadata unchanged; it does not claim a fresh decode result.
See ../ui-main-effects-reconstruction/SOURCE-EVIDENCE.md for original byte/source
findings and the existing native linear fade policy versus original IOP timing.

RR_AUDIO_DEVICE accepts 0 or 1 in enabled Main; default is a real device attempt
for a window and capture/mix only for headless. An unavailable device is reported
truthfully and does not cancel independently requested WAV output. SDL dummy
and disk remain rejected as genuine playback. Audible playback is never inferred.

RR_AUDIO_WAV optionally names a new PCM16 stereo WAV at 48 kHz. The existing
exclusive-create behavior refuses replacement and requires existing parents.
Muted mode continues to reject any RR_AUDIO_WAV setting. It creates no frontend
PCM transport/output and records the session's actual suppressed requests.
RR_AUDIO keeps its existing race cold-bank behavior; it is not an implicit
frontend mute switch. Existing mode1/dynamic-contact cold requirements remain.

## Ownership and pause

AudioOutput owns five private voices, a private mixer, a shared stable-address
OriginalUiMainAudio50KG transport and a device/WAV output. Session receives only
the transport. The output calls its actual pause-aware mixStereo16 gate through
the typed interface, never a raw mixer bypass or function-pointer cast.

All calls remain main-thread. The existing SDL callback only reads copied PCM
from its owned queue. WAV bytes and queued device bytes share the actual mixed
buffer. No callback reads source PCM, mutates a voice or invokes the producer.

The Main inactive branch explicitly pauses the transport and flushes device
queues. It neither advances source samples nor emits WAV silence to fill wall
time. Resume unpauses before input effects are accepted, and the first resumed
update receives zero elapsed time, as before. The fractional microsecond carry
is retained across pause. Ordinary gameplay's AudioMixer overload preserves
its original admission, PCM processing, sample clock and output path.

The event loop skips race output opening during Main even if cold race audio
was prepared. Cleanup first drops the Main session, closes/finalizes its device
and WAV, resets only its own voices, then closes retained race owners and the
renderer/host. Reverse member destruction preserves the same safe lifetime on
exceptions. Main currently has no admitted race launch; any future transition
must close this frontend output before starting a race output.

## Honest timing receipts

Once a source update succeeds, Main passes exactly its measured active elapsed
nanoseconds to the frontend output owner. The update gets float seconds derived
from that same interval; output carries sub-microsecond remainder and uses the
existing integer sample clock. A stall greater than one second fails instead
of being clamped or silently skipped. Capture requests do not mix additional
frames. Actual update-to-audio then render ordering is retained in the code.

New Main status fields include source mixer frames, output mixed/captured frames,
rate, accepted active nanoseconds, output microseconds, carried nanoseconds,
submitted/source-suppressed/failed source events and real device counters.
`mixed_frames` remains the existing race mixer counter; it should stay zero.
Capture receipts associate those actual audio counters with that actual native
frame/capture sequence. Closure reports finalized counts only after successful
WAV/device/GPU cleanup. No video timestamp, sampling-rate conversion, AV offset,
wall-clock padding or audiovisual-sync claim is manufactured.

Expected identities for a successful enabled capture (claims to verify later):
- audio_clock_microseconds = floor(audio_accepted_nanoseconds / 1000)
- audio_mixed_frames = floor(audio_clock_microseconds * audio_rate / 1000000)
- audio_source_mixed_frames = audio_mixed_frames during this active-only host path
- audio_captured_frames = audio_mixed_frames when WAV was requested from startup
- PCM payload length = 4 * audio_captured_frames; finalized RIFF length = 44 + payload
- world_ticks and the existing race mixed_frames remain zero

These formulas describe the implemented accounting, not measured results.
Audio startup includes actual zero-delta source lifecycle updates. Device
prebuffer, callback underruns, overflow and OS scheduling remain independently
reported. A real WAV is not evidence that physical speakers played it, and
sample counters alone are not proof of audiovisual synchronization.

## Parent's pending centralized validation

Only the parent may decide which checks fit its authorized aggregate slot.
No standalone command or helper validation is requested here.

Review/build all candidate joins together. Preserve ordinary gameplay behavior
through the retained mixer overload. For the enabled Main observation, use a new
WAV path, the verified explicit AUDIO root and real submitted Main frames.
Inspect actual source-event receipts, captured WAV header/payload/nonzero PCM,
clock/count identities, finalization, unchanged zero world/race ticks, and any
device availability result. Pause/resume checks should show unchanged source
and WAV counters during pause, then continuation without wall-time catch-up.
Check mute separately under the same centralized policy when permitted; it must
reject WAV and keep audio source/output counts zero. Missing/corrupt source or an
existing WAV path must fail, not turn into silent proof audio.

The existing record_original_main_recovery.py hardcodes muted and rejects
audio_muted=false. It is untouched here. An enabled observation needs an explicit
recorder change to record the configured root/WAV, validate the new fields and
inspect the resulting PCM. Its current muted path remains compatible. Do not
attach unrelated audio to the visual recording or claim a matched AV result
without actual timing evidence.
