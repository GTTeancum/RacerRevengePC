# Dense gameplay video capture

The original single-track workflow has focused runtime coverage; newer menu and
multitrack helpers have their own scoped status documents. The current encoder
successfully prepared a corrected silent artifact from an existing GC capture,
without a new game run. See [video export timing](video-export-timing.md).
Capture remains restricted to one authorized central runtime; encoding consumes
existing evidence and must use a new output directory. No validation take or
automatic retry is authorized by these tools.

## Deliverables

- `capture_gameplay.py`: one fresh runtime, default 20-second Racing-phase clip,
  requests at 20 fps, complete acknowledged native PPM frames, timestamp ledger,
  optional actual mixed WAV, one quit, and no automatic retry/restart.
- `encode_gameplay.py`: separate, explicitly requested encoding of captured
  evidence; VFR MP4 with measured timing plus a JPEG of one actual source frame.
- `menu_exercise.py`: optional one-process Main/start/pause/resume/final-Main
  state machine. It runs only when `--exercise-menus` is explicitly passed.

The scripts do not build a binary or select a track. Supply the intended reviewed
binary/fixture/cwd and any already-supported RR_ overrides. `--track-label` is a
label, not track-selection functionality or verification. Current inspected host
source has a fixed Podbox corpus case and does not establish other-track support.
Verify the new track implementation and its asset identity independently.

## Root-owned launch

Invoke only after any existing runtime has ended and the central slot is eligible.
Example placeholders below must be replaced with actual reviewed inputs:

```
python3 /workspace/shared/rr-gameplay-video-capture/capture_gameplay.py \
  /absolute/path/to/already-built/racerrevenge \
  /absolute/path/to/track-fixture.json \
  /workspace/shared/rr-track-name-slot/take \
  --root-authorized --source-commit <40-character-commit> \
  --track-label 'Verified track name' --seconds 20 --fps 20
```

The default input is the existing HOST authored-pace course driver. It records
real native runtime motion; it is neither a human-driving claim nor original AI.
Inherited RR_ settings are cleared. Deliberate supported overrides may be given
as `--env RR_RACE_MODE=1`, for example. The controlled WAV/device settings and
automatic wrong-way capture setting cannot be overridden. `--no-audio` explicitly
omits the WAV request if the intended track has no prepared audio; it never adds
replacement sound. The existing runtime may refuse startup if asked to record
audio when original samples are unavailable.

Admission uses `/workspace/shared/rr-last-central-test.json` and its `.lock` sibling.
It reads `started_utc`, `next_earliest_utc`, owner, status, and runtime-launch count.
It takes the same flock used by the prepared manual runner and holds it throughout
capture. It refuses a recent slot, any active slot, a missing/malformed ledger,
an ambiguous timestamp, or an existing output directory. A claim is atomically
published before Popen; even launch failure consumes the slot. Root should mark
the preceding slot terminal after its real process ends; stale `running` is not
silently ignored even if an hour has elapsed.

When root has already reserved this exact slot for a build plus its **first and
only** runtime, pass both:

```
--root-owns-active-slot --expected-slot-start '<exact started_utc from ledger>'
```

This is accepted only for an active, recent root-owned ledger entry that explicitly
has `runtime_launches: 0` and the same `source_commit`. Missing count, count 1,
mismatched start/commit, or another owner fails closed. This flag is not permission to attach to an existing process,
replace its observer, or launch a second runtime in the same slot. The currently
running 15:17 slot was not modified and cannot safely be reused for another take.

Without `--exercise-menus`, all commands in the running child are `capture` and
`quit`. Capture begins after
observed `race phase=2`, has a 75-second startup bound, and ends after the requested
15–30-second window. The runner stops on capture timeout, phase change, unexpected
session, or process exit, preserves evidence, and does not retry. Cleanup signals
can target only its own Popen child. The final actual captured frame marks the
endpoint and remains visible for one millisecond; all earlier durations use the
measured intervals. No image is duplicated to pad the ending.

## Optional menu exercise in the same sole runtime

Add `--exercise-menus` to that one capture invocation, never run a separate menu
validation process. This requires the staged menu implementation and its actual
`host_ui_state` JSON telemetry to be integrated in the reviewed binary. The flag
sets `RR_START_MENU=1`; without it, the generic capture behavior is unchanged.
It is incompatible with `--no-audio`, because real mixer-clock evidence is required.

The exact sequence, entirely outside the gameplay-video interval:

1. Wait for initial `host_ui_state` reason `ready`, Main page 1. Capture the
   prepared frontend as `menu-front.ppm`.
2. Pulse `down Enter` and `up Enter` with no simultaneous navigation. Wait for
   Closed page 0 and then a new actual Racing telemetry sample. Start is activated
   only once. No restart, checkpoint, or second race launch is permitted.
3. Send `pause`; require Pause page 2. Capture `menu-pause-first.ppm`, hold for at
   least two measured wall seconds, and capture `menu-pause-held.ppm`.
4. Require identical native tick, mixed-frame count, actual WAV sample count,
   session number, and audio-bank address across those paused observations.
5. Send `resume`; require Closed page 0 with the exact frozen clocks and same
   identity. After at least one second, capture `menu-resumed.ppm` and require both
   native tick and mixed count to advance in the same session/audio bank.
6. Only now begin the ordinary 15–30-second dense gameplay video. Menu screenshots,
   pause time, and pre-roll never enter its frame list, duration, or audio range.
7. After the final video frame and its UI-state acknowledgement, send `menu`.
   The supported implementation holds the same prepared bank on Main page 1;
   it does not rebuild until another Start. Capture `menu-final-front.ppm`, verify
   clocks stay held between transition and capture, then send the one normal quit.

Required JSON prefix is `host_ui_state ` with fields `reason`, `menu`, `page`,
`session`, `audio_bank_id`, `tick`, and `mixed_frames`. Reasons are `ready`,
`transition`, and `capture`. The bank ID is the actual bank object's address;
combined with the session counter and reviewed source it demonstrates retained
ownership, but it is not a hash of all bank contents. No ownership or clock claim
is inferred merely from absence of logs. Unexpected transitions, missing telemetry,
bank/session changes, clock movement while paused, or failure to advance on resume
fail the take without retry. The result is written to `menu-exercise.json` and
embedded in `capture-receipt.json`; raw log lines and command times are retained.

Menu exercise has bounded waits (75 seconds cold startup, 15 seconds per later
stage, 2 seconds per capture acknowledgement). It is not a complete menu acceptance
test: Controls, confirmation Cancel/Confirm, restart, resizing, and manual input
usability are outside this particular one-take sequence.

For provenance, repeat `--source-file relative/or/absolute/path` for each reviewed
integrated source file. The runner always hashes itself; menu mode also hashes its
helper. It records before/after SHA-256 values for these sources, binary, and
fixture and rejects changes during the take. Source commit remains a supplied
review label; the runner does not build, mutate git, or claim to prove compilation
from that commit. Suggested menu sources are the integrated interactive drive,
headless input host, UI state reducer, and menu renderer files listed in the menu
worker's integration manifest.

### Actual Clegg selection, still one process

Add `--select-racer3` together with `--exercise-menus` to exercise the integrated
Anakin/Clegg menu choice. This never sets a racer-selection environment variable.
The generic invocation and ordinary Anakin menu flow remain unchanged.

From the genuine default Main frontend (`loaded_racer=1`, `selected_racer=1`),
the helper pulses Down to row 1, waits for that transition, then pulses Enter.
It requires selected identity 3 while loaded identity remains 1 and captures
`menu-racer-choice.ppm`. It separately pulses Up to return to Start, waits for
the transition, then pulses Enter once. Direction and activation are never sent
as the same host input cycle.

The existing application deliberately cold-rebuilds when the selected racer
differs from the prepared one. This option admits exactly one such pregame
native-session change, after that one explicit Start, within the same Popen
process and central slot. It requires one `Starting a fresh authored-grid session.`
marker, one additional HOST RACE header, and one new `ready` whose native session
counter increments exactly once and whose loaded/selected IDs are both 3.
The internal zero-based session index becomes 1; telemetry reports session 2.
No second Start or later restart is admitted.

The new bank/session identity is established only after verifying actual cold
installation logs for all eight slots, including:

- `ORIGINAL RACER 0: Clegg Holdfast [POD03] ...`
- `ORIGINAL RACER 1: Anakin Skywalker (ep1) [POD01] ...`

Before pause, actual Racing telemetry must show eight unique names with
`racer1[Clegg Holdfast]` and `racer2[Anakin Skywalker (ep1)]`. The reviewed source
defines native bank slot 0 / race tuple 1 as the human and slots 1–7 as HOST
opponents. The receipt states this role contract rather than pretending the menu
label alone proves gameplay identity. All subsequent pause, resumed, video, and
final-Main observations must retain that new session/audio bank and loaded ID 3.

The native writer closes the first WAV and opens `actual-mix-session-2.wav` for
the rebuilt session. The runner switches only after matching the real `Audio
recording session=2 path=...` log and accepted ready event, retains both WAVs and
their hashes, and uses only the actual selected-session WAV for video anchors.
The encoder reads that verified filename from the capture receipt. Menu/pre-roll
audio is not spliced into gameplay. The total pre-video bound is 150 seconds for
this two-cold-setup option; the expected second setup has its own 75-second bound.

## Encoding, only after root requests it

Do not execute this step merely to check its syntax or codec support.

```
python3 /path/to/RacerRevengePC/tools/capture/encode_gameplay.py \
  /path/to/take --root-requested-encode --output /path/to/new-export
```

Use `--require-audio` if absence of trustworthy synchronized sound should block
delivery. The artifact preparation exercised the available ffmpeg/ffprobe path;
that result does not establish every platform or codec combination. `--output`
is required and must name a new directory. If encoding fails, preserve the take
and diagnose without another gameplay run.

Outputs in the requested directory are `gameplay.mp4`, `preview.jpg`, and `encode-receipt.json`.
The encoder verifies source frame hashes, dimensions, timestamp order, native
tick advancement, MP4 frame timestamps/count/duration, and audio presence. Root
must additionally inspect the JPEG and video content before describing the track
as visibly correct. A successful encode is not full gameplay validation.

## Truthfulness and timing

- Each frame is copied only after `live capture committed at tick ...`. Existing
  host source closes the PPM before this acknowledgement; only one request is
  outstanding, so the file cannot be overwritten during copying.
- Capture uses observer monotonic time. Request/ack times and latencies are
  retained; the exact GPU presentation timestamp is not exposed by this host.
  This is dense headless framebuffer recording, not desktop/window screen capture.
- Video uses the actual interval between acknowledgement observations, with
  millisecond encoding timebase. No fixed-rate retiming, interpolated movement,
  screenshot animation, hidden gap removal, or sped-up race is permitted.
- Encoding refuses less than 12 measured fps, any inter-frame gap over 250 ms,
  less than 80% distinct pixel frames, or a window outside 15–30.5 seconds. These
  are honest quality gates, not a reason to rerun without a new authorized slot.
- Capturing may itself affect performance. Retain raw runtime performance logs;
  do not call the target capture rate the uncaptured game performance.
- Actual mixed WAV advances 768 stereo samples per committed 16 ms simulation
  step, including countdown. It is not inherently a wall-time microphone track.
  The runner records its real file sample count at each frame acknowledgement.
  Encoder trims only the anchor-to-anchor sample range and checks wall-time skew
  at every anchor (80 ms maximum, 50 ms final). It does not synthesize, stretch,
  normalize, or resample sound. AAC is the only delivery-format transformation.
  Missing, silent, damaged, or out-of-sync PCM is retained separately and clearly
  omitted from the MP4 (or blocks `--require-audio`). No game audio availability
  or device playback should be claimed solely from a video file existing.
- Approximately 400 PPM frames at 640×480 occupy 369 MB for a 20-second/20-fps
  take, before WAV and MP4. Keep evidence and never automatically delete a take.

## Source contracts inspected

- `tests/native/terrain_interactive_drive50du.cpp`: `captureCommitted`, PPM close
  before acknowledgement, `--headless-live`, HOST race telemetry, shared WAV hook.
- `tests/native/live_audio50hf_fixture.h`: exactly one 768-frame mix per committed
  16 ms step; no fabricated clock for unavailable samples.
- `platform/linux/native/audio_output50hd.cpp`: 44-byte PCM header, unbuffered
  appended frames, header finalization on close.
- `/workspace/shared/rr-manual-slot1116/capture_manual.py`: central ledger/flock
  admission convention.
- Earlier full-race observers retained a few sparse screenshots. Those existing
  takes cannot be truthfully repackaged into this dense gameplay proof.
