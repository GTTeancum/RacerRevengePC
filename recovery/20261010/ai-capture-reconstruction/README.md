# Source-only original-AI recorder candidate

Prepared on 2026-10-10 for root review. **No execution, syntax check, test,
build, importer, gameplay, or PC work was performed for this deliverable.**
Only source inspection and these new staging files were used. The repository
was not edited. This is a candidate for the next root-owned check, not proof
that a recording or original-AI run has succeeded.

The existing hourly slot is consumed until **2026-10-10 15:18 UTC**. The script
refuses to launch earlier. Reaching that time does not authorize a check:
root must review the candidate, identify the new executable, and decide whether
the one permitted check is available. The guard flag is an accident-prevention
measure, not permission. It is not a shared scheduler and cannot detect checks
run by another process.

## Intended invocation, after review and permission

Supply the new native ELF executable and its independently reviewed SHA-256.
Use a new output directory directly below this directory. The command below
is illustrative and has not been run; replace both placeholders and the new
directory name. Do not execute it before review or while the slot is occupied.

```sh
python3 /workspace/shared/recovery-20261010/ai-capture-reconstruction/record_original_ai.py \
  /ABSOLUTE/PATH/TO/NEW_NATIVE_EXECUTABLE \
  --expected-exe-sha256 REVIEWED_64_HEX_DIGIT_SHA256 \
  --output /workspace/shared/recovery-20261010/ai-capture-reconstruction/NEW_UNIQUE_RUN \
  --root-reviewed-one-check
```

There is exactly one child-process launch attempt and no automatic retry. The
script does not build or discover an executable, run an importer, use a shell
to launch the game, or change repository files. Current source file hashes and
the executable hash are recorded separately: matching a checkout is not assumed
to establish executable build provenance. Root should retain its build evidence.

## Fixed experiment

Working directory:

`/workspace/shared/recovery-20261010/native-assets-prepared-01`

Child arguments:

`NEW_EXECUTABLE evidence/staticType2Producer49X/original-word/native-fixtures.json --headless-live NEW_UNIQUE_RUN/live.ppm`

The capture path is absolute in the actual command. The child environment is
constructed anew, not copied from the parent. In addition to basic PATH,
locale, timezone, and fresh per-run home/tmp/cache locations, it contains only:

```text
RR_TRACK=GC
RR_GC_INCOMPLETE_WORLD=1
RR_RACE=1
RR_RACE_MODE=2
RR_START_MENU=0
RR_ORIGINAL_AI_ALL_RACERS=1
RR_AUTODRIVE=0
RR_DISABLE_RACER_CONTACTS=0
RR_AUDIO=1
RR_AUDIO_DEVICE=0
RR_AUTHORED_WORLD_FOG=0
RR_AUDIO_WAV=NEW_UNIQUE_RUN/audio.wav
```

No inherited RR settings, preload/library paths, display variables, GL overrides,
or wrapper settings are retained. If that clean environment cannot start the
reviewed binary, the attempt ends with evidence; it does not silently add a
fallback or launch a second check. A new directory prevents overwriting a prior
WAV, live PPM, frame sequence, or log.

Only `capture\n` and `quit\n` may be written to child stdin. No driving,
key-release, menu, pause, checkpoint, restart, or resume input is sent. Process
termination signals are used only for bounded cleanup of the owned process group.

## Readiness and wall-clock window

The 30-second monotonic window begins only when the recorder has observed:

1. A WAV-enabled output with device `not_requested` and a positive sample rate.
2. `host_ui_state` with reason `ready`, session 1, menu closed, loaded/selected
   GC, incomplete-world opt-in, mode-2 admission, and audio enabled.
3. `original_ai_all ... reason=ready` with enabled=1, racer_count=8,
   human_count=0, ai_count=8, camera_slot=0, camera_racer60=1, and zero host-driver,
   controller-sample, and keyboard-packet counters.
4. Ordered, unique rows for slots 0 through 7, each source_index matching its
   slot, vehicle58=1, initialized=1, owners_bound=1, active=1, phase=0 (Ready),
   and zero reported source_boundary. Their exact racer60 identities must be
   `[1, 3, 4, 7, 8, 9, 10, 11]`, with loaded UI racer 1.

Startup has a separate default 180-second watchdog. It does not count toward
the 30-second observation. CLI overrides remain bounded: startup at most 300
seconds, capture acknowledgments at most 10 seconds, quit at most 15 seconds,
and capture frequency at most 5 Hz. Defaults are 180 seconds, 5 seconds,
10 seconds, and 5 Hz respectively.

The first capture is requested after readiness. Further requests are separated
by at least 200 ms at the default frequency, and only one request is outstanding.
There is no catch-up burst. New requests stop when the 30-second deadline is
observed. A request already in flight may finish within its original bounded
acknowledgment timeout; any acknowledgment beyond the deadline is marked.
No extra end-of-window capture is requested.

## Three-part acknowledgment and frame preservation

Each requested image must produce these ordered lines before any numbered frame
is copied:

```text
live capture committed at tick N
capture_state wrong_way=... tick=N host_menu=...
host_ui_state {"reason":"capture", ..., "tick":N, "mixed_frames":M}
```

The source closes and validates the entire PPM before writing the first line.
The recorder waits for the following state and UI lines, verifies their clocks
and owner identity, checks the exact 640x480 P6 file length, and copies bytes to
`frames/frame-NNNNNN.ppm` through a temporary file and rename. It writes the
corresponding durable `frames.jsonl` entry before allowing another request.
The live file is not a frame manifest and must not be used on its own as proof.

Each frame entry includes request, acknowledgment, and copy monotonic times;
elapsed time since readiness; committed/UI tick; mixed audio frame count;
sample-clock seconds; exact PPM SHA-256; reviewed executable SHA-256; and the
latest complete fleet snapshot index. Duplicate images or ticks are retained
honestly rather than resampled, interpolated, or replaced. A complete capture
with a menu/retained-image/clock violation is retained with the violation and
ends the observation. An incomplete acknowledgment never creates an accepted
numbered frame. An interrupted atomic write can leave a `.part` or orphaned PPM;
only a complete `frames.jsonl` row identifies an acknowledged preserved frame.

## Evidence and honest limits

- `launch.json`: exact argv, environment, corpus, recorder, source-file, and
  reviewed executable identities; timing bounds and policy.
- `game.log`: combined raw child stdout/stderr, written before parsing.
- `events.jsonl`: timestamped log lines with raw-log offsets, actual commands,
  protocol problems, process status, and cleanup actions.
- `fleets.jsonl`: complete ready/status groups, including all eight original-AI
  owner rows and the zero-input counters.
- `frames.jsonl` and `frames/`: acknowledged numbered raw frames and timestamps.
- `audio.wav`: the game's unmodified source WAV. Header, actual file length,
  sample rate, format, and final capture-clock coverage are checked after exit.
- `summary.json`: checks, termination reason, pending acknowledgment/partial
  fleet, complete prefix, source boundary lines, and observed per-slot growth.
- `live.ppm`: last live file, which can be incomplete or unacknowledged after a
  failure and is never automatically promoted to a numbered frame.

All logged fleet headers must keep host_driver_calls/controller_samples/
keyboard_packets at zero. All complete fleet rows must remain initialized,
owners-bound, active, Ready, and source-boundary-free. Every slot must have
observed native_ticks and dispatched growth for a complete recorder observation.
The exact racer60 roster and camera identity are checked in every fleet report.
A partial final fleet group prevents a complete observation result. Malformed
counter headers remain in the evidence and make the zero-counter check false;
they cannot raise an exception while that summary check is being produced.
However, source `native_ticks` is the shared `completedFrames` field; it is not
an independent per-racer timer. `dispatched` is the separate per-racer evidence.

The source emits all-AI status approximately once per second, but **does not
emit a final fleet snapshot on quit**. The result explicitly identifies the
last logged evidence. It does not claim to inspect counter values during the
unobserved suffix or cleanup. Logs containing `ordinary` counts remain evidence
for review, not an ordinary-driving or lap-completion acceptance decision.
No lap acceptance is inferred from captures, time elapsed, or an exit code.

The run is explicitly incomplete-world GC in mode 2. The native source's opening
warning documents the unjoined zone11 Gate trigger, LostAI branch triggers, and
Gate sound. A recorder result cannot turn this into complete-track support.

## Boundaries, shutdown, and timing

A genuine `ORIGINAL AI ... boundary:` line immediately stops scheduling new
captures. Generic game errors, protocol violations, counter violations, owner
changes, timeout, EOF, and interruption also stop new scheduling. Existing raw
logs, complete frames, timestamps, and WAV bytes remain in place. The recorder
does not invent continuation, clear native state, reset racers, substitute
host driving, or launch another session.

An outstanding capture gets only its original acknowledgment deadline. The
recorder then sends at most one quit command and waits the bounded quit grace.
If necessary it sends SIGTERM to its own process group, waits two seconds, sends
SIGKILL, and waits at most two more seconds. Forced shutdown is reported and can
leave an unfinalized WAV, whose bytes are retained unchanged. A source-boundary
exit can be useful evidence even when the 30-second window is not reached.

Default gameplay-loop budget is at most approximately 180 + 30 + 5 + 10 + 2 + 2
seconds, plus bounded log draining and ordinary OS/process/file I/O overhead.
These watchdogs are monotonic loop deadlines, not a guarantee against a stalled
kernel, process creation, hashing, storage, or filesystem operation.

Exit 0 means only that the bounded observation and listed recording checks
completed. Exit 3 means a source boundary was observed; exit 2 covers other
partial/failing observations. Prelaunch argument/preflight errors do not launch
the game. Inspect `summary.json` and source logs rather than treating any exit
code as race, original-AI ordinary-loop, or lap acceptance.

If evidence storage itself fails (for example, a full disk), existing files are
left in place and the owned process still gets bounded cleanup. The final
summary may be unavailable in that case; inspect the reported directory and
terminal error without interpreting missing evidence as success.

No video is encoded. There is no audio mux, stretch, duplication, synthetic
frame timing, or silent replacement. Capture timestamps are the recorder's
host-observed times, while mixed_frames/WAV samples use the native simulation
clock. Root receives both clocks and the raw artifacts, including any unmatched
audio prefix/suffix, for a later explicitly reviewed presentation decision.

## Source references used during preparation

Read in `/workspace/shared/rr-recovery-git`:

- `tests/native/terrain_interactive_drive50du.cpp`: configuration and all-AI
  controls at approximately 195–246; zero-input early returns at 1349 onward;
  fleet validation/reporting at 1989–2072; audio startup at 2658–2685; capture
  and UI protocol at 2971–3020; startup reports at 3048–3051; all-AI status cadence
  at 3793–3796; quit cleanup/reporting at 4025 onward.
- `platform/linux/native/game_host50er.cpp`: stdin command parsing at 159–207.
- `src/game/vehicle_frame49w.h`: Ready is enum value 0.
- `platform/linux/native/audio_output50hd.cpp`: fixed 44-byte stereo PCM16 WAV
  header and close-time frame-length finalization.

Line numbers describe the inspected source and may move. The script hashes the
current versions at launch without asserting that they are the binary's source.

## Root integration amendment

The repository copy at `tools/gameplay/record_original_ai_recovery.py` accepts
`--ready-window-seconds` (default 30, maximum 180). The recorded requested window
and elapsed-window check use that exact value. Root may select 180 seconds for
one focused original-AI observation around the first braking/impact sequence;
all capture serialization, source-boundary stops and one-launch limits remain.
This amendment is source-reviewed only and has not been executed.
