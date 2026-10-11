# Direct-GC original-AI capture

Unexecuted source-only harness for one centrally reserved native runtime. None of these modules has been imported, syntax-checked or run for this backup. Existing TA-to-GC capture tools remain separate.

## Required runtime and local inputs

Use the guarded original-AI source on `wip/native-ai-first-slice`, equivalent to local commit `93201627ee0b2afd834cb2ab51885fc55f749c6d` / remote commit `4292c484f8ee2ecafcdbc06e679e6543b79167a2`. Required main SHA-256: `c9cdc1fc76f934dd2e48da1eb79f524c2b9d2b818598bb8fc1fb081ee6053933`. This helper branch retains an older source baseline; its own runtime tree does not supply that feature.

The modules preserve their reviewed cloud-workspace paths. They require the existing central reservation ledger, frozen 27-file source manifest and its source files, separate guard source, an independently built matching executable, a user-supplied collision fixture and original game data. These local inputs and generated evidence are excluded from Git. A clone alone is not a portable execution bundle. The runner verifies source identities before and after capture; the central build receipt must establish the actual executable-to-source link.

`capture_native_ai.py` owns capture, `native_ai_evidence.py` accounts for telemetry, `capture_clock_contract.py` pairs image/native clocks, and `read_clock_gate.py` only reads reservation eligibility.

## One reservation, one process

Only an existing active reservation owned by the central coordinator, with matching source commit and exact start time and zero prior runtime launches, is admitted. A shared nonblocking lock and compare-before-write ledger transaction retain ownership; the launch is charged before Popen, including launch failures. No retry or replacement reservation is created. The frozen candidate additionally rejects a start earlier than 2026-10-10 00:20:36 UTC; this floor never substitutes for the rolling-hour policy.

Invocation requires the existing binary, fixture and a fresh output directory; `--root-authorized`, `--root-owns-active-slot`, `--source-commit`, `--expected-slot-start`, `--cwd`, `--seconds 12`, and `--fps 20` describe the already authorized central attempt. Do not run help, parser, dry-run or syntax checks as an extra software attempt.

## Fixed observation scope

Inherited RR settings are cleared. The harness selects actual GC/mode2, the incomplete-world opt-in, eight racers, original AI in slot 1, `RR_START_MENU=0`, actual contacts and actual mixed audio. Existing human and other-racer HOST controls remain in use. Only the actual audio-root path may be overridden. Commands are restricted to capture and quit; no held inputs, synthetic states, restart, checkpoint or menu actions are sent.

The observation window is 12 seconds after real ready, including countdown, paced at 20 requested captures per second. It bounds requests and milestone evidence, not hard process lifetime. Cold loading has a separate bound; quit allows finalization and then termination of that same child if needed. No late observation can satisfy an earlier milestone. Complete native image acknowledgements, image state, native UI tick and actual PCM anchors must pair; retained images never count as fresh gameplay.

## Acceptance and retained limits

The report separately records direct-GC single-session admission, eight distinct active racers and advancing native counters, genuine slot-1 `vehicle58=1`, sampled startup state 4 then state 2 with native CB8 greater than 5, real brake commands and native-bound simultaneous brake/steer, and fresh render progress. Five-second telemetry can miss states. Planner and binding counters do not prove completion of that exact physics frame; cold state 0 and AI-specific terrain support are not directly exposed.

Every actual source, contact, incomplete-world, recovery or session boundary is retained with its original log prefix and stops acceptance. Earlier observed milestones remain visible even when an eventual boundary makes the overall attempt fail. Clegg combat or actual dynamic contact may legitimately stop the prefix. No completed lap, full original AI, complete GC world or performance parity is claimed.

Capture and acceptance receipts, append-only logs, acknowledged images and original PCM are local evidence. There is no encoding, audio repair or synthesis. This short-prefix schema must not be sent to the ordinary 20/60-second encoder as a completed take.
