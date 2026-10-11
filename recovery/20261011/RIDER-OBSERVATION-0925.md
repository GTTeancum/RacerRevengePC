# Instrumented Rider opening — 2026-10-11 09:25 UTC

Fresh Linux source: 59a4a6bdabefed0c2d3cb8dd4a3ca3580e2d8fba.

One centralized build/capture attempt started at 09:25:39.022009 UTC, with optional host stage observations enabled. Next permitted slot: after 10:26:39.022009 UTC. No driving.

- Build passed: 2 compiled, 275 dependency/object-hash-verified reused, link exit 0.
- Executable SHA256: 53d2715686aaeed1081113f97689c631dfc5593b9022efc6449c7f02f9b67bb2.
- 27 actual frames preserved. Rider19 opening rendered with the original selection attached and Song 7 active; its fully open checkpoint and Back sequence were not reached.
- Last Rider frame: 16,140 source quad slots, 15,684 retained source triangles, 6,422 native draw calls. Different source time/state prevents treating the changing counts across runs as a controlled benchmark.
- Native exit 1 again reported `original UI active update stall exceeds one second`. Recorder exit 2. No explicit quit/closed receipt, complete cleanup verification, or WAV finalization verification.

## Actual stage observations for accepted update 52

| Stage | Host elapsed |
| --- | ---: |
| Source update | 1.849648 ms |
| PCM advancement | 0.446833 ms |
| Source draw construction | 7.402038 ms |
| Native submission | 1363.064340 ms |
| Capture readback | 0.888291 ms |
| File publication | 2.607863 ms |
| Deferred commit | 0.003755 ms |

These instrumented host intervals isolate the dominant observed cost in native submission. Native submission includes its preflight, GL commands and internal completion/copy behavior; it is not a separately measured pure GPU execution time. Logs add real overhead and are not subtracted from accepted clocks. The existing stall gate is unchanged.

Build evidence: /workspace/shared/recovery-20261011/rider-validation-05.
Capture evidence: /workspace/shared/recovery-20261011/rider-capture-sequence/rider-validation-05.

Next work: rigorous native reduction of ordered feedback submission overhead. Unproven rectangle batching remains excluded; do not replace source output or clocks to manufacture smooth footage.
