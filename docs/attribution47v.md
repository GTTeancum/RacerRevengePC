# 47V: full native caller attribution accepted

2026-09-30. Goal active/incomplete. This is diagnostic evidence, not a new speed
measurement. The verified 47T matched-race gain remains 1.740681x (5.774172 to
10.050989 emulated VBlanks/s); see checkpoint47U and its dependency chain.

47U's full instrumented run reached VBlank7000 with exact saved EE/IOP memory,
reported terminal state, audio, final image and unchanged cards matching the
accepted reference. Run: work/native-run47u-1-1-1-0-1-1-1-1-20260930-175329-326.
Executable SHA-256: d3f84de0d6befd1c985dff048a96c18a7c581fd1b5f1b0f0ac07dfc882bb5556.
The report validates the matching binary and linker map. Per-thread counts
have zero overflow. These are whole-run counts including startup and racing;
they do not measure exclusive time or isolate the 6000–6900 interval.

The 68,347,425 recorded lookups include:

| Setting | Calls |
| --- | ---: |
| RR_TRACE_VIF_UNPACK | 29,202,212 |
| RR_TRACE_IPU_STALL | 13,368,503 |
| RR_CAPTURE_VU | 6,444,644 |
| RR_CAPTURE_VU_ARM | 6,444,644 |
| RR_CAPTURE_VU_RESUME | 6,444,644 |
| RR_CAPTURE_IPU_DMA | 3,283,605 |
| RR_CAPTURE_IPU_ERROR | 1,409,304 |
| RR_CAPTURE_VU_COUNT | 821,113 |

The largest source maps to processVIF1Data; IPU stall/DMA settings map to
serviceIpu25; VU settings map to the capture constructor/options. No values
were logged. DLL-internal getenv calls are not intercepted. The shim and
instrumented run must not be used as a performance benchmark.

47V explicitly disables VU capture by removing RR_CAPTURE_VU, while keeping
the frozen 47T executable and all game behavior. Its VBlank500 startup passes
the saved-state comparison; the full run is active and excluded here.
Evidence: evidence/captureOff47V-20260930-180114-129. Zero capture count retains
its original meaning (up to 64 unique programs); it was not redefined.

47W is prepared and queued, not built/accepted by this checkpoint. It targets
the measured diagnostics in isolated copies of memory/VIF implementation,
IPU helper files and the VU capture header, recompiling the VU implementation
unchanged. Only literal RR_TRACE_/RR_CAPTURE_ getenv sites use launch-fixed,
owned optional strings. Off mode preserves original live lookup. Three runtime
translation units must replace their originals in the final link; inspect the
map and require both off/on native replay checks before accepting the candidate.
The proposed tests preserve present values and absence across CRT environment
storage changes, then compare startup and full-race state/media in both modes.

Restore checkpoint47U and its chain, then overlay this Source archive.
The complete attribution run, reports and accepted capture-off startup are
included. Revalidate live 47V and queued 47W processes before rerunning. Remaining
work includes much larger gains, compact whole-game code, 64MiB fit, full P3/
x86-32 execution, NV2A graphics, rendering correctness and stable playable laps.
No Xbox boot or FPS is claimed.
