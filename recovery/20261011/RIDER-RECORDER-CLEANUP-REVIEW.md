# Independent source-only cleanup evidence review

Reviewed on 2026-10-11 UTC. Result: no blocking source findings for this narrow recorder evidence-accounting change. This is not a runtime validation or authorization to launch.

## Exact materials

- Original recorder: `/workspace/shared/recovery-20261011/rider-capture-sequence/record_original_rider_30hz_recovery.py`
  - SHA-256: `0a71bc3ba46d63a88bb6f849bd8b5cdc26fb7dc31f71cb842cc41fdbf5963c83`
- Reviewed candidate: `files/tools/gameplay/record_original_rider_30hz_recovery.py`
  - SHA-256: `0aa1035a7577976561174164318a1a9f0121c2b3afd69b8a2c6b8f205e75c12c`
- Existing failed-run log, read as text only: `/workspace/shared/recovery-20261011/rider-capture-sequence/rider-validation-01/game.log`
  - SHA-256: `a4a1076df28e81722779b66529a634fd3fa21d08694d4c764195a2569b427e00`

## Findings

1. The original recorder conflated matched native image-commit receipts with fully validated and preserved images. Its failed trace contains native capture sequence 11 followed by a rejected source state, while only 10 images were preserved. Comparing native cleanup `captures=11` with preserved images incorrectly rejected otherwise distinct cleanup evidence.
2. The candidate separately counts received capture-prefix lines, structurally matched native ACKs, and preserved images. The matched count advances only after the existing request/sequence/target/dimension/audio/frame checks and the new contiguous-sequence check. Native cleanup compares its capture count with matched ACKs, and additionally cannot precede the last matched ACK's native frame.
3. Quit and closed receipt text, bounded decoded payloads, and counts are retained before sequence validation. Summary retention is bounded to the most recent two receipts of each kind, with 8192 text characters per receipt. The original bounded raw log remains the source for full receipt text. The wrapper records precise rejection status and error text when terminal processing raises; the first candidate's misleading `not_yet_validated` rejection state was corrected before this review was finalized.
4. Once the outcome has already failed, quit validation checks only explicit quit ownership, no-gameplay counters, real audio accounting, audio-clock remainder, and no queued physical-device PCM. It retains that observation with `sequence_validated=false` rather than applying successful-route expectations or claiming the requested route was observed. The successful path still runs the original complete `check_state` assertions on quit.
5. A successful result now explicitly requires received ACK lines, matched ACKs, requests, preserved images, and native cleanup capture count to agree, plus full requested-sequence validation of quit. Existing primary-failure, error, pending-capture, input/release, transition, checkpoint, round-trip, and WAV checks remain. Cleanup acceptance cannot turn a failed sequence into success.
6. No changes to game/native sources, requested controls, input barriers, capture preservation, successful route/profile assertions, signal/cleanup scheduling, or WAV header/length validation appear in the reviewed diff. The launch floor alone advances to 2026-10-11 06:21:12 UTC as specified by the parent.

## Expected evidence for the supplied failed trace

The following is a source-reading deduction, not a replay or produced result:

- Received capture ACK lines: 11.
- Matched native capture ACKs: 11.
- Preserved images: 10.
- Unpreserved acknowledged captures: 1.
- The initial Main Enter unavailable result remains the primary failure; the rejected capture state's pending ACK remains pending.
- The supplied quit state can be retained as terminal safety/audio evidence with full-sequence validation false.
- The supplied native close report can be retained with `cleanup_complete=true`, `captures=11`, `native_frames=799`, and `audio_captured_frames=511564`.
- The existing WAV inspection can label a file structurally finalized only if its actual RIFF header and length match the accepted close receipt. This review did not inspect or validate that WAV.
- The overall observation remains `partial_or_failed_observation`, regardless of cleanup or WAV finalization.

## Review boundaries

Only source/log text inspection, file listing, textual diffing, hashing, and writing this review were performed. No recorder execution or import, syntax check, parser invocation, log replay, test, build, decoder, sanitizer, or gameplay run occurred. No active repository or original recorder file was edited. The proposed recorder remains untested. Raw evidence retention is subject to the recorder's existing log, line, event, and summary bounds; it is not an unlimited retention guarantee. No audible-quality, playback, performance, or successful navigation claim follows from this review.
