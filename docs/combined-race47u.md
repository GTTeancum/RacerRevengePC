# 47U: accepted 1.74068x combined race comparison; caller attribution begins

2026-09-30. Goal remains active/incomplete. No Xbox performance or completed-lap
acceptance. The current Windows test uses the original game's fixed input and
the borrowed PCSX2 renderer/VU reference.

## Verified result

47T used one frozen executable with scheduler gate and diagnostic line buffering
off/off versus on/on. IOP diagnostic cache stayed enabled. The executable hash
was 53762c35cb847cb30d7c5b2e6abc0f1636959db859652a4e9f39702b650231cb in both.
Both stopped at VBlank 7000 / EE cycle 34407093000. Saved full EE/IOP RAM, audio,
final image, reported terminal state and input match the accepted reference;
memory cards remain unchanged. These observations do not cover all hidden state
or establish a completed lap.

The same 6000–6900 interpolated race window recorded 5.774172 versus 10.050989
emulated VBlanks per host second: 1.740681x, approximately 74% faster. Capture
bracket bounds are [5.283333, 5.986391] and [8.985264, 11.227685]. These are
temporal bracket bounds, not statistical confidence intervals. One controlled
pair supports this result; it is not distinct rendered FPS or an Xbox forecast.
The pair combines two independently tested mechanisms and does not identify
their individual share of the gain. No build/archive/search workload ran in
either fixed race timing window.

Full-run scheduler counts: 849972144 fast / 2008568 full. Whole-process IO in
capture-bounded race subsets falls from 1353.23 to 106.29 write operations/s.
The subsets have different approximate boundaries, and these counts do not
identify every destination or measure exclusive write cost. The observer's
generic 47Q-contamination caveat refers to the earlier named 47Q control, not
this 47T pair. The earlier contamination remains preserved in its own evidence.

Evidence: evidence/combined-race47T-20260930-173405-793 plus observer47T-cpu-0/1.
Runs: native-run47t-0-0-1-0-1-1-1-1-20260930-173406-455 and
native-run47t-1-1-1-0-1-1-1-1-20260930-174506-186 under work/.

## Attribution, not another speed claim

47U adds a Windows-only executable getenv import shim to unchanged 47T behavior.
It forwards the actual CRT return pointer and records caller RVAs and setting
names in bounded per-thread tables. It does not record environment values or
observe DLL-internal imports. This is diagnostic instrumentation, not Xbox code
and not a timing candidate. It uses Microsoft's documented _ReturnAddress
intrinsic: https://learn.microsoft.com/en-us/cpp/intrinsics/returnaddress

The standalone test preserves results for 2000 present/absent lookups across
two threads; recorded counts are exact, with zero overflow. Native startup at
VBlank 500 matches 47T saved state. The exact diagnostic executable hash is
d3f84de0d6befd1c985dff048a96c18a7c581fd1b5f1b0f0ac07dfc882bb5556.
The matching linker map and source/provenance are preserved.

Startup recorded 2435582 lookups, led by RR_TRACE_IPU_STALL (1771238),
RR_CAPTURE_IPU_DMA (361792), RR_CAPTURE_IPU_ERROR (205120), and
RR_TRACE_MPEG_STATE (66559). The first two map to serviceIpu25. Those sites are
in ipu/rr_ipu_memory25.inc, missed by earlier searches limited to .cpp/.h.
Startup includes movie decoding; do not assume these counts describe racing.
The full instrumented VBlank-7000 run is active and excluded from this archive.

## Next experiments and recovery

47V is queued behind the full attribution run. It uses the frozen 47T executable
and explicitly unsets RR_CAPTURE_VU, then compares startup/full saved state and
the race window against 47T-on. It is not yet accepted. The existing count=0
semantics select up to 64 unique VU programs; they do not disable capture, and
the original constructor still hashes code on later invocations. The evidence
mode stays available. No capture helper or original source was modified.

docs/polling-loop47u.md records a longer-term lead: repeated timer conversion
inside a guest VBlank polling loop, including the memory/GS side effects a
safe acceleration would have to preserve. No shortcut has been implemented.

Restore 47T and its dependency chain, then overlay this Source archive.
Completed 47T runs/observers, 47U build/unit tests and 47U startup are included.
Inspect evidence/47U/native.exit and captureOff47V status before rerunning queued
work. The global goal is incomplete: large further gains, whole-game compact
code and 64 MiB fit, x86-32/SSE1 execution, NV2A rendering, sky/surface validation,
stable racing, and measured stock-Xbox execution remain outstanding.
