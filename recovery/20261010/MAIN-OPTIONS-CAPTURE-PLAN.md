# Original Main ↔ Options recording candidate

Status: staged source only. This worker has not built, syntax-checked, imported,
executed, or run the recorder or any project test. Root owns integration and the
single shared software-test gate. The prior Main-only recorders are untouched.

Candidate:
`files/tools/gameplay/record_original_ui_navigation_30hz_recovery.py`

Source reviewed: root entry wiring commit `31d1f88`, with active repository HEAD
`18b4b01cb5d77022c492bd4623faa5824979ca1a` at the time of review. These facts do
not certify any binary; root must separately review the chosen executable and
its build provenance. The recorder snapshots source-file and executable hashes.

## Actual coverage intended

1. Capture the original Main screen 4/page 1 from its real first rendered frame
   onward. Wait for source lifecycle idle (0), aperture open (0), input container
   shown, and aperture input enabled. Fresh source selection/profile slot is 1.
2. Send three distinct Down presses, checking real selections 2, 3, and 4 after
   their individual capture acknowledgments. Saved Main slot remains 1 here.
3. Send Enter only at open Main slot 4. Require source saved Main slot 4, then
   actual Options screen 12/page 1 with `switches=1` and fresh Options slot 1.
4. Wait for open Options input; send one Down and require real selected slot 2.
   Saved Options slot remains 1 until Back is admitted.
5. Send Escape only at open Options slot 2. Require source saved Options slot 2,
   then actual Main screen 4/page 1 with `switches=2`, restored Main saved slot 4,
   and fully open input. A fresh Main bank can briefly expose its original
   initial slot 1 before openingAC restores slot 4; this is admitted only while
   input is not open. No menu input is sent in that state.
6. Keep collecting genuine source frames through the 20-second observation
   window, then explicitly quit and verify process exit plus native cleanup.

The complete outcome additionally requires captured closing Main, opening
Options, closing Options, and reopening Main states. A round trip with a missed
sample is reported as partial coverage, not substituted with an invented frame.
The summary keeps each observed milestone and points to its preserved frame.

No Options subpage (including GameOptions), Rider, Course, Gallery, gameplay,
original boot/save parity, or audio recording is covered. Audio is deliberately
muted. Every actual state/cleanup receipt must have zero world ticks and no
unavailable-destination count. The environment retains
`RR_ORIGINAL_AI_ALL_RACERS=1`, `RR_AUTODRIVE=0`, and original UI ownership; no
temporary course driver, gameplay launch, manual driving packet, restart,
checkpoint, host-menu request, pause, or resume is available to this recorder.

## Host grammar and serialization

`GameHost50ER::consumeCommand` accepts newline-delimited ASCII `down KEY` and
`up KEY`, case-insensitively. Used commands are exactly:

- `down Down` / `up Down`
- `down Enter` / `up Enter`
- `down Escape` / `up Escape`
- `capture`
- `quit`

A fresh down edge plus its following `capture` are one bounded stdin write.
The next write cannot happen until the native capture ACK, following source
state, and complete PPM have been validated and preserved. A separate up edge
plus `capture` then receives its own complete barrier before another press.
This avoids merging repeated down/up pairs in one host poll. The grammar and
entry ordering prove the release has been consumed before its capture; the
current source does not expose a separate key-up receipt, and the recorder does
not claim otherwise.

Exact source locations:

- `platform/linux/native/game_host50er.cpp`: command parser, scancode mapping,
  one 1,024-byte poll read, pressed-on-fresh-down behavior.
- `tests/native/terrain_interactive_drive50du.cpp`: original UI branch owns
  input before gameplay, processes pressed keys before update/render/readback,
  then commits deferred screen changes only after capture.
- `tests/native/original_ui_main_entry50kg.h`: actual active route/selection,
  saved slots, lifecycle, aperture, counts, RNG, audio and world state reports.
- `src/game/original_ui_navigation50kg.cpp`: nonempty navigation groups bind at
  slot 1; real directional moves select source controls.
- `platform/linux/native/original_ui_frontend50kg.cpp`: Main slot 4 is the only
  admitted outward route; Options Back commits its real saved selection and
  returns to a freshly prepared Main owner with the shared profile retained.
- `src/game/original_ui_aperture_owner50kg.h` and
  `src/game/original_ui_transition_owner50kg.h`: numeric state definitions.

The recorder never sets source selection, source clocks, RNG, aperture, route,
manager state, or scene content. Its six-action plan is a bounded menu test
input sequence, not a scripted rendering or replacement UI state machine.

## Provenance and failure evidence

Each preserved 640×448 P6 PPM has an atomic native commit receipt, matching
native frame number and `screen/page/switches` in the immediately associated
source state, image SHA-256, executable SHA-256, request/ACK/state/copy host
timestamps, and its exact key-edge/gating frame provenance in `frames.jsonl`.
The raw native output and recorder events are retained separately.

The 30 Hz setting is a request target/ceiling. The recorder serializes work and
never catches up with a burst, interpolates, resamples, manufactures duplicate
frames, or changes the source timer to meet it. Source-produced duplicate
pixels are retained. The summary reports measured host capture-ACK cadence and
maximum gap; it does not call this native display FPS. No video encoder or mux
is launched. A later video must preserve these real timings and provenance.

On failure, retain the evidence prefix. `summary.json` includes the immutable
first problem, first/last rejected receipt, the last 32 source-state diagnostic
projections (recorded before strict validation, including a rejected quit row),
last valid state, terminal native failure lines, pending capture, active action,
timeouts, and signal/exit/cleanup outcome. A terminal diagnostic cannot replace
the original stop reason or first problem. The raw log preserves complete
receipt text up to its byte limit. If storage itself fails, a summary may be
unavailable; the existing evidence prefix and stderr explain that condition.

## Bounds and ownership

- One native ELF process and one launch attempt, never a retry/relaunch
- 20 seconds from actual native readiness; startup default 180 seconds, max 300
- Capture ACK default 5 seconds, max 10; next navigation gate default 8 seconds,
  max 20; quit default 10 seconds, max 15
- At most 620 PPMs, 860,175 bytes each, plus native live/pending image files
- Raw log ≤32 MiB, events ≤64 MiB, frame ledger ≤16 MiB, each JSON manifest or
  summary ≤2 MiB, and total command stream ≤8,192 bytes
- No new input after stop; pending capture gets only its original ACK deadline
- Explicit quit releases host keys. After its timeout, only this owned process
  group receives SIGTERM, then SIGKILL after two more seconds if necessary
- New private output directory only; no overwriting a prior run

Budget roughly 750 MiB of free storage for the maximum retained evidence.
The recorder-controlled file/command limits do not claim to bound all native
heap/GPU allocation or external filesystem behavior.

## Root-only launch requirements

Earliest available gate reported by root:
`2026-10-10T18:56:57.403928+00:00`.

The recorder rejects a gate earlier than this and refuses to wait or launch
before the supplied gate. This check does not implement the shared hourly
ledger; root must check the ledger and authorize exactly one central test.
Do not execute this recipe as an extra probe, syntax check, smoke test, or retry.

After root has integrated/reviewed the final source and built a matching native
ELF in its authorized central slot, its one selected invocation has this shape:

```text
python3 /workspace/shared/recovery-20261010/ui-navigation-recording/files/tools/gameplay/record_original_ui_navigation_30hz_recovery.py \
  /ABSOLUTE/PATH/TO/ROOT_REVIEWED_NATIVE_ELF \
  --expected-exe-sha256 ACTUAL_REVIEWED_64_HEX_SHA256 \
  --output /workspace/shared/recovery-20261010/ui-navigation-recording/NEW_UNIQUE_RUN_DIRECTORY \
  --root-reviewed-one-check \
  --not-before 2026-10-10T18:56:57.403928+00:00 \
  --capture-hz 30
```

The original UI assets, authenticated corpus, reviewed source files, and EGL/
native dependencies must already be available at the fixed stage/repository
locations declared in the candidate. Root must not select an older Main-only
binary: missing route fields, unsupported Options, bad state, or timeout all
fail closed. No test success is claimed for this staged source.
