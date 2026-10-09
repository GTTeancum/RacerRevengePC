# Renderer call profile, 47O

The isolated native GS backend measures nine existing DLL calls. The PCSX2
bridge DLL, packets, transfers, FIFO handling and all synchronous fences are
unchanged. The wrapper stays under the original recursive backend lock and
records cumulative calls/nanoseconds, total packet bytes and packet-size bins.
Every 100 backend VSync calls it writes a snapshot; destruction writes a final
snapshot after final captures. A missing requested output file is an explicit
error, not a silently empty profile. With the environment variable absent the
observer creates no file and does not call the clock.

The measurement is host elapsed time spent inside each DLL call. That includes
producer-side work, GS-thread execution, waiting and preemption. It cannot
separate exclusive CPU cost, GPU execution or queue latency. Counter collection
also has overhead: this is a diagnostic run, not a throughput benchmark.

The analyzer checks monotonic cumulative values, all nine metrics per snapshot,
packet-count/histogram agreement, VSync counts and that disjoint DLL-call time
does not exceed the host interval. The final backend count must equal the actual
fixed scheduler stop before assigning game-VBlank meaning to the 6000–6900
window. Startup on/off both match the 47N combined runtime at VBlank 500 in full
saved RAM, PCM, image and reported state. The observed count is exactly 500.

Startup takes 31.578 seconds in this measured run. DLL calls total 6.761% of
that, with one renderer initialization call accounting for 6.148%. The remaining
measured calls total approximately 0.613%; VSync accounts for 0.240%, flushes
0.226%. This startup evidence does not establish race behavior or justify
removing synchronization. The full-race profile is queued after 47N validation.

The 47M compact-leaf full race has now completed and matches the accepted 47L
mode-1 run at VBlank 7000: full EE/IOP RAM, PCM, final image, reported state and
exact stop. It executes 1,954,337 leaf calls with zero fallbacks. No compact-leaf
performance improvement or complete lap follows from that saved boundary.

Preparation and build are reproducible via prepare_gs_profile47o.py and
Build-GsProfile47O.ps1. The latter can wait until an existing race has captured
VBlank 6950, avoiding its timing window. Run-GsProfileStartup47O.ps1 preserves
the off/on startup pair and validates its CSV. Run-GsProfileRace47O.ps1 freezes
the binary, waits for the existing registry race and checks both state and
profile counters before recording success. The reference renderer source is
not changed, and the backend's original SHA-256 is verified by the generator.
