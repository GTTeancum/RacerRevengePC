# Native PCM mixer and output, 50HD

## Scope and honest output status

`platform/linux/native/audio_mixer50hd.{h,cpp}` mixes decoded original PCM;
`audio_output50hd.{h,cpp}` sends those mixed buffers to a Linux SDL2 playback
device and/or a stereo PCM16 WAV. Neither implementation invents effects,
voices, an engine oscillator, or substitute speech. Original clip decoding and
source-based gameplay event selection are separate layers, 50HC and 50HE.

The cloud executor has the SDL2, ALSA, and PulseAudio runtime libraries, but no
development headers and no `/dev/snd`. The genuine default-device probe on
2026-10-09 reported `device_status=unavailable`, `driver=alsa`, and
`ALSA: Couldn't open audio device: No such file or directory`. PulseAudio also
could not connect in this environment. This is not proof of audible playback.
Headless WAV capture remains usable. Explicit SDL `dummy` or `disk` drivers are
rejected as `rejected_non_playback`; they are never reported as a real device.
Even a successful real backend open proves only that SDL accepted a playback
device, not that a person heard it or that an audio server's final sink is a
physical speaker.

## Stable integration API

All types are in `rr::platform`. Allocate a fixed array of `AudioVoice50HD`
(up to 128 slots), then call `AudioMixer50HD::initialize(voices, count, 48000)`.
The mixer borrows this storage. It also borrows every `AudioClip50HD::samples`
pointer; complete clip-bank loading before playback and keep its PCM vectors
stable for the entire lifetime of their voices.

`AudioClip50HD` is signed 16-bit mono or interleaved stereo with a frame count,
original source sample rate, and optional exclusive-end loop range. A 50HC
decoded clip maps directly as `{pcm.samples.data(), pcm.frames, pcm.sampleRate,
pcm.channels, pcm.loopStart, pcm.loopEnd}`. Both loop markers zero means a
requested looping voice repeats the whole clip. Embedded markers are ignored
when the voice is not looping, so one-shots can play the complete decoded clip.
Do not submit a missing, invalid, or unsupported asset as a clip.

- `play(slot, clip, gain, pitch, loop, pan)` replaces only that voice and starts
  its source coordinate at zero. It does not steal a different voice.
- `controls(slot, gain, pitch, pan)` adjusts the existing coordinate without
  restarting it. Gain is 0–8, pitch 0.125–8, and pan −1–1; all must be finite.
- `setLoop(slot, bool)`, `stop(slot)`, and `stopAll()` are allocation-free.
- `availableVoice(first, count)` returns the total slot count if its requested
  range is exhausted. The caller chooses any priority/voice-stealing policy.
- `mixStereo16(output, frames)` uses linear source-rate interpolation, mixes
  stereo with mono duplication, and saturates to PCM16. `clippedSamples()`
  exposes saturation. Its work is bounded by frame count × configured slots;
  a call is limited to one output-rate second. No application allocation occurs.

The caller must not concurrently mutate the borrowed voice array or PCM.
Mixer controls and all public output methods belong to one producer thread.
The SDL callback never reads voices or decoded assets.

Create `AudioOutput50HD` after `WindowHost50DQ::create`, and close/destroy audio
before the window host invokes process-global `SDL_Quit`. Output initializes
and releases only its SDL audio-subsystem reference; it never calls SDL_Quit,
changes SDL environment variables, creates a window, or touches the event queue.
In headless mode no window host is needed.

`AudioOutputRequest50HD` defaults to trying a real device, 48 kHz PCM16 stereo,
a 1,024-frame callback request, and 1,536 mixed frames of startup buffering.
Set `wavPath` to a new writable filename for capture; existing files are
rejected rather than truncated. Set `tryDevice=false` for explicit capture-only
work. Parent directories must already exist.

`open(request, diagnostic)` succeeds when mixing/capture can proceed even if
the device is unavailable. Inspect `info().deviceStatus` and `deviceDetail` to
report the real result. Capture-open failures are actual errors, not a silent
switch to another path. The dynamic loader uses the installed SDL2 runtime,
with no package installation or emulated guest CPU.

For each successfully committed simulation step, update the event/voice state
and call `advanceMicroseconds(mixer, 16000, diagnostic)`. The output uses one
cold-allocated state containing a 32,768-frame SPSC ring and a 1,024-frame mix
buffer. Per-buffer work allocates nothing in this implementation. The callback
does only zero-fill, up to two PCM copies, and lock-free counter operations.
It performs no file I/O, mixing, mutex locking, or allocation. SDL/backend
internals and synchronous capture writes are not claimed to be hard real-time.

`pauseDevice(true)` pauses and flushes stale queued device audio without
rewinding voices or the WAV. Stop simulation-clock advancement while paused.
On resume, fresh samples satisfy the prebuffer again. `close(diagnostic)`
finalizes WAV sizes and closes the device; it does not wait for queued tails.
Check explicit close errors instead of relying only on destructor cleanup.
Final counters remain readable after close. Plain RIFF WAV's 4 GiB limit is
checked; capture does not wrap its size fields.

## Audio clock versus video clock

Audio is generated on the committed simulation timeline, not from render
frame count or rounded wall-clock ticks. `AudioSampleClock50HD` carries an
integer fractional-frame remainder:

    total frames = floor(total simulation microseconds × output rate / 1,000,000)

At 48 kHz, every 16 ms step is exactly 768 sample frames. At 44.1 kHz, 16 ms
steps alternate the needed 705/706 frames without cumulative rounding drift.
The output reports `simulationMicroseconds`, `mixedFrames`, and
`capturedFrames`; a video frame recorded after a step can be labeled with
`mixedFrames / sampleRate` for a simulation-time movie. Keep the existing
wall-clock capture timestamp as separate evidence when recording it.

The existing fixed-step host can discard wall time after stalls. Therefore:

1. Do not assume that a WAV generated from N native steps is N/60 seconds.
   Native 16 ms stepping is 62.5 simulation steps per second.
2. Do not simply mux simulation WAV into a longer wall-clock video and claim
   synchronization. Choose simulation-time video timestamps or explicitly
   account for known gaps in the export layer. This layer neither time-stretches
   original clips nor fabricates engine activity through missing steps.
3. Every WAV payload byte comes from the same mixed PCM block submitted to the
   device ring. Device gaps are a separate fact: the callback fills a starved
   output with silence and increments `deviceUnderrunFrames`. Those hardware-
   timeline zeros are not secretly inserted into the simulation-time WAV.
4. A producer that outruns the real device can fill the bounded ring. It then
   counts `deviceOverflowFrames` and per-call `diagnostic.droppedDeviceFrames`;
   the independently captured WAV is preserved. No unbounded device queue is
   grown and dropped playback is never claimed as heard output.
5. `deviceQueuedFrames` and `deviceConsumedFrames` describe the ring, not proof
   of speaker audibility. A default-device disconnect is reported, while
   already enabled WAV capture continues.

For near-1:1 gameplay, a simulation-clock movie can use original recorded image
frames at these sample-clock times and the unchanged WAV. If wall-clock video
is required, handle the actual missing intervals explicitly in that exporter.

## Focused verification and limits

Only the new mixer/output implementation and its focused fixture were built.
No broad regression suite, package install, guest CPU, or desktop test was run.

`solo-audio50hd/evidence/native_audio50hd_checks.cpp` uses short artificial PCM
number arrays solely to verify software arithmetic. Its generated
`unit-pcm-not-game-audio.wav` is not an original asset or a game demo.

Verified cases:

- 16-to-48 kHz interpolation and one-shot final sample/end handling
- Original-rate fractional playback with pitch, mono/stereo, pan and gain
- Embedded loop intro/range, loop disable, stop and silence
- Saturation reporting, invalid indices/formats/ranges/nonfinite controls
- Exact 768,000 frames for 1,000 × 16 ms steps at 48 kHz
- Fractional carry producing 88,200 frames for 125 × 16 ms steps at 44.1 kHz
- No C++ heap allocations in repeated mixing/capture after initialization
- WAV header/sample-count correctness and byte-for-byte equality against an
  independently mixed reference using the same clip and controls
- Existing recording paths are rejected without replacement
- Actual default device unavailable and explicit dummy driver rejected

The code compiles with C++20 and `-Wall -Wextra -Wpedantic -Wconversion -Werror`.
The focused executable links `audio_mixer50hd.cpp`, `audio_output50hd.cpp`,
`-ldl`, and `-pthread`. The parent live-client integration owns adding these new
translation units to its build. A focused address/undefined-sanitizer attempt
was not a full pass: after fixing the fixture's custom-allocation overload,
LeakSanitizer could not run under this executor's ptrace environment. Both
attempt logs are retained; no sanitizer-clean claim is made. The ordinary
focused arithmetic/capture checks passed. Actual original-game sound, event coverage,
device callback behavior on physical hardware, and audible quality still need
the genuine audio asset bank and integrated tests; numeric fixtures do not
establish any of those.

SDL ABI references: [SDL_AudioSpec](https://wiki.libsdl.org/SDL2/SDL_AudioSpec),
[SDL_OpenAudioDevice](https://wiki.libsdl.org/SDL2/SDL_OpenAudioDevice), and
[SDL_InitSubSystem](https://wiki.libsdl.org/SDL2/SDL_InitSubSystem).
