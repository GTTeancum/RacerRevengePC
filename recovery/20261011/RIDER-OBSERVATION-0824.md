# Rider rendering-cost observation — 2026-10-11 08:24 UTC

Fresh Linux source: af822880dd1b5a64ab5b7a601f54b45333928000.

The one centralized build/capture attempt started08:24:16.130719UTC; next permitted slot after09:25:16.130719UTC. No driving or additional launch.

- Build passed:3compiled,274dependency/object-hash-verified reused, link exit0.
- Executable SHA256: bc444122cac6526c5446fa9fd79067d81f9bd9acad25f46a2e0e4a09571cf4db.
- Genuine Rider19 opening again rendered, with attached original selection and Song7 active. Last accepted frame had16140source quad slots,15688triangles and6821native draw calls. Previous run's different opening state had15690triangles and15690draws; this is not a controlled benchmark or proof of identical images.
- The last Rider capture request-to-commit-ACK latency was approximately1.3486seconds, versus4.7058seconds in the previous run. Source state and timing differ; no precise overall speedup or steady frame rate is established.
- Native execution again stopped at `original UI active update stall exceeds one second`, before the menu fully opened. 28validated images preserved; no idle, directional-rejection or Back sequence completed.
- Native exit1 followed the ordinary error-handler message. The previous unresolved-bank cleanup/terminate message and abort-6 did not recur. There is still no successful explicit-quit/closed receipt; do not infer complete cleanup or WAV verification from that absence alone.
- Last accepted state had79005source/mixed/captured PCM frames at48000Hz and world_ticks0. Recorder result remains partial/failed with exit2.

Build evidence: /workspace/shared/recovery-20261011/rider-validation-04.
Capture evidence: /workspace/shared/recovery-20261011/rider-capture-sequence/rider-validation-04.

Next: source-only work on remaining feedback rendering overhead, preserving original primitive/material ordering, all source geometry, source clocks, and the existing stall gate. Runtime speed and complete menu navigation remain unverified.
